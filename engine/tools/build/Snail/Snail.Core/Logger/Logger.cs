using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Threading;

namespace Snail.Core.Logger
{
    public enum LogLevel
    {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warn = 3,
        Error = 4,
        Fatal = 5
    }

    public interface LogCategory
    {
        string GetName();
    }

    public class LogCategorySystem : LogCategory
    {
        public string GetName()
        {
            return "System";
        }

        public static LogCategorySystem Default = new LogCategorySystem();
    }

    /// <summary>
    /// Base class for JSON-serializable log data. Public properties and fields form the content identity.
    /// </summary>
    public class LogData
    {
        public string Message { get; set; } = string.Empty;

        public LogData(string message)
        {
            Message = message;
        }

        public static LogData Create(string message)
        {
            return new LogData(message);
        }
    }

    public sealed record LogEntry
    {
        public LogEntry(
            DateTimeOffset timestamp,
            LogLevel level,
            LogCategory category,
            LogData data,
            Exception? exception = null)
        {
            ArgumentNullException.ThrowIfNull(category);
            ArgumentNullException.ThrowIfNull(data);

            Timestamp = timestamp;
            Level = level;
            Category = category;
            CategoryName = category.GetName();
            Data = data;
            Text = data.Message ?? string.Empty;
            Exception = exception;
            _dataContent = JsonSerializer.Serialize(data, data.GetType(), _serializationOptions);
            _exceptionText = exception?.ToString();
            ContentHash = HashCode.Combine(Level, CategoryName, Data.GetType(), _dataContent, _exceptionText);
        }

        public DateTimeOffset Timestamp { get; }
        public LogLevel Level { get; }
        public LogCategory Category { get; }
        public string CategoryName { get; }
        public LogData Data { get; }
        public string Text { get; }
        public Exception? Exception { get; }

        /// <summary>
        /// An in-process content hash that excludes Timestamp. Payload content is captured when the entry is created.
        /// Hash collisions are checked against the captured content, independently of payload object identity.
        /// </summary>
        public int ContentHash { get; }

        internal bool HasSameContent(LogEntry other)
        {
            return Level == other.Level &&
                string.Equals(CategoryName, other.CategoryName, StringComparison.Ordinal) &&
                Data.GetType() == other.Data.GetType() &&
                string.Equals(_dataContent, other._dataContent, StringComparison.Ordinal) &&
                string.Equals(_exceptionText, other._exceptionText, StringComparison.Ordinal);
        }

        private static readonly JsonSerializerOptions _serializationOptions = new JsonSerializerOptions
        {
            IncludeFields = true
        };

        private readonly string _dataContent;
        private readonly string? _exceptionText;
    }

    public sealed record LogQuery(
        LogLevel? MinimumLevel = null,
        LogLevel? Level = null,
        LogCategory? Category = null,
        string? Text = null);

    public interface ILogSink
    {
        void Write(LogEntry entry);
    }

    public interface ILogSource
    {
        IDisposable Subscribe(Action<LogEntry> callback);
    }

    public sealed class Logger
    {
        public Logger(params ILogSink[] sinks)
        {
            ArgumentNullException.ThrowIfNull(sinks);

            _sinks = (ILogSink[])sinks.Clone();
            foreach (ILogSink sink in _sinks)
            {
                ArgumentNullException.ThrowIfNull(sink);
            }
        }

        public LogLevel MinimumLevel
        {
            get => (LogLevel)Volatile.Read(ref _minimumLevel);
            set => Volatile.Write(ref _minimumLevel, (int)value);
        }

        public void Log<TData>(LogLevel level, LogCategory category, TData data, Exception? exception = null)
            where TData : LogData
        {
            if (level < MinimumLevel)
            {
                return;
            }

            var entry = new LogEntry(DateTimeOffset.UtcNow, level, category, data, exception);
            foreach (ILogSink sink in _sinks)
            {
                sink.Write(entry);
            }
        }

        public void Trace<TData>(LogCategory category, TData data) where TData : LogData
        {
            Log(LogLevel.Trace, category, data);
        }

        public void Debug<TData>(LogCategory category, TData data) where TData : LogData
        {
            Log(LogLevel.Debug, category, data);
        }

        public void Info<TData>(LogCategory category, TData data) where TData : LogData
        {
            Log(LogLevel.Info, category, data);
        }

        public void Warn<TData>(LogCategory category, TData data) where TData : LogData
        {
            Log(LogLevel.Warn, category, data);
        }

        public void Error<TData>(LogCategory category, TData data, Exception? exception = null) where TData : LogData
        {
            Log(LogLevel.Error, category, data, exception);
        }

        public void Fatal<TData>(LogCategory category, TData data, Exception? exception = null) where TData : LogData
        {
            Log(LogLevel.Fatal, category, data, exception);
        }

        private readonly ILogSink[] _sinks;
        private int _minimumLevel = (int)LogLevel.Debug;
    }

    public sealed class MemoryLogStore : ILogSink, ILogSource
    {
        public MemoryLogStore(int capacity = 10_000)
        {
            if (capacity <= 0)
            {
                throw new ArgumentOutOfRangeException(
                    nameof(capacity), capacity, "Capacity must be greater than zero.");
            }

            _capacity = capacity;
            _entries = new Queue<LogEntry>(capacity);
        }

        public int Count
        {
            get
            {
                lock (_syncRoot)
                {
                    return _entries.Count;
                }
            }
        }

        public void Write(LogEntry entry)
        {
            ArgumentNullException.ThrowIfNull(entry);

            Action<LogEntry>[] callbacks;
            lock (_syncRoot)
            {
                if (!_uniqueEntries.Add(entry))
                {
                    return;
                }

                if (_entries.Count == _capacity)
                {
                    _uniqueEntries.Remove(_entries.Dequeue());
                }

                _entries.Enqueue(entry);
                callbacks = _subscribers.Values.ToArray();
            }

            // A copied callback can still run after its subscription is disposed.
            // Exceptions propagate to the writer and stop notification of subsequent subscribers.
            foreach (Action<LogEntry> callback in callbacks)
            {
                callback(entry);
            }
        }

        public IReadOnlyList<LogEntry> Snapshot()
        {
            lock (_syncRoot)
            {
                return Array.AsReadOnly(_entries.ToArray());
            }
        }

        public IReadOnlyList<LogEntry> Query(LogQuery query)
        {
            ArgumentNullException.ThrowIfNull(query);

            IEnumerable<LogEntry> entries = Snapshot();
            if (query.MinimumLevel.HasValue)
            {
                entries = entries.Where(entry => entry.Level >= query.MinimumLevel.Value);
            }

            if (query.Level.HasValue)
            {
                entries = entries.Where(entry => entry.Level == query.Level.Value);
            }

            if (query.Category != null)
            {
                string categoryName = query.Category.GetName();
                entries = entries.Where(entry =>
                    string.Equals(entry.CategoryName, categoryName, StringComparison.Ordinal));
            }

            if (query.Text != null)
            {
                entries = entries.Where(entry =>
                    entry.Text.Contains(query.Text, StringComparison.OrdinalIgnoreCase));
            }

            return Array.AsReadOnly(entries.ToArray());
        }

        public void Clear()
        {
            lock (_syncRoot)
            {
                _entries.Clear();
                _uniqueEntries.Clear();
            }
        }

        public IDisposable Subscribe(Action<LogEntry> callback)
        {
            ArgumentNullException.ThrowIfNull(callback);

            long id = Interlocked.Increment(ref _nextSubscriptionId);
            var subscription = new Subscription(this, id);
            lock (_syncRoot)
            {
                _subscribers.Add(id, callback);
            }

            return subscription;
        }

        private void Unsubscribe(long id)
        {
            lock (_syncRoot)
            {
                _subscribers.Remove(id);
            }
        }

        private sealed class Subscription : IDisposable
        {
            public Subscription(MemoryLogStore owner, long id)
            {
                _owner = owner;
                _id = id;
            }

            public void Dispose()
            {
                MemoryLogStore? owner = Interlocked.Exchange(ref _owner, null);
                owner?.Unsubscribe(_id);
            }

            private MemoryLogStore? _owner;
            private readonly long _id;
        }

        private sealed class ContentComparer : IEqualityComparer<LogEntry>
        {
            public bool Equals(LogEntry? left, LogEntry? right)
            {
                return ReferenceEquals(left, right) ||
                    (left != null && right != null && left.HasSameContent(right));
            }

            public int GetHashCode(LogEntry entry)
            {
                return entry.ContentHash;
            }
        }

        private readonly object _syncRoot = new object();
        private readonly int _capacity;
        private readonly Queue<LogEntry> _entries;
        private readonly HashSet<LogEntry> _uniqueEntries = new HashSet<LogEntry>(new ContentComparer());
        private readonly Dictionary<long, Action<LogEntry>> _subscribers = new Dictionary<long, Action<LogEntry>>();
        private long _nextSubscriptionId;
    }

    public static class GlobalLogger
    {
        public static void Trace<TData>(LogCategory category, TData data) where TData : LogData
        {
            LogInternal(new EventArgs(LogLevel.Trace, category, data));
        }

        public static void Debug<TData>(LogCategory category, TData data) where TData : LogData
        {
            LogInternal(new EventArgs(LogLevel.Debug, category, data));
        }

        public static void Info<TData>(LogCategory category, TData data) where TData : LogData
        {
            LogInternal(new EventArgs(LogLevel.Info, category, data));
        }

        public static void Warn<TData>(LogCategory category, TData data) where TData : LogData
        {
            LogInternal(new EventArgs(LogLevel.Warn, category, data));
        }

        public static void Error<TData>(LogCategory category, TData data, Exception? exception = null) where TData : LogData
        {
            LogInternal(new EventArgs(LogLevel.Error, category, data, exception));
        }

        public static void Fatal<TData>(LogCategory category, TData data, Exception? exception = null) where TData : LogData
        {
            LogInternal(new EventArgs(LogLevel.Fatal, category, data));
        }

        private static void LogInternal(EventArgs args)
        {
            if (LogCallback != null)
            {
                LogCallback.Invoke(args);
            }
            else
            {
                Console.WriteLine($"[{args.LogLevel.ToString()}][{args.LogCategory.GetName()}]{args.LogData.Message}");
            }
        }

        public struct EventArgs
        {
            public LogLevel LogLevel { get; set; }
            public LogCategory LogCategory { get; set; }
            public LogData LogData { get; set; }
            public Exception? Exception { get; set; }

            public EventArgs(LogLevel logLevel, LogCategory logCategory, LogData logData, Exception? exception = null)
            {
                LogLevel = logLevel;
                LogCategory = logCategory;
                LogData = logData;
                Exception = exception;
            }
        }

        public static Action<EventArgs>? LogCallback { get; set; } = null;
    }
}
