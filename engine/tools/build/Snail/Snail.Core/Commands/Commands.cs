using System;
using System.Collections.Generic;
using System.Reflection;
using System.Text;

using Snail.Core.Logger;

namespace Snail.Core.Commands
{
    public class CommandResult
    {
        public enum StatusType
        {
            Success,
            Failure
        }

        public StatusType Status { get; set; } = StatusType.Success;
        public string Message { get; set; } = string.Empty;
    }

    public interface ICommandOptions
    {
        public string CommandName { get; }

        public bool ParseArguments(string[] args);
    }

    public interface ICommands
    {
        public Task<CommandResult> ExecuteAsync(ICommandOptions options);

        public Type CommandOptionsType { get; }

        public string GetCommandName();
    }

    public abstract class CommandBase<T> : ICommands where T : ICommandOptions, new()
    {
        public Type CommandOptionsType => typeof(T);

        public abstract Task<CommandResult> ExecuteAsync(T options);

        public string GetCommandName()
        {
            return new T().CommandName;
        }

        public Task<CommandResult> ExecuteAsync(ICommandOptions options)
        {
            if (options is T typedOptions)
            {
                return ExecuteAsync(typedOptions);
            }
            else
            {
                throw new ArgumentException($"Invalid options type. Expected {typeof(T).Name}, but got {options.GetType().Name}.");
            }
        }
    }

    [Service.ServiceAttribute(typeof(CommandService), true)]
    public class CommandService : Service.IService , IDisposable
    {
        private readonly Dictionary<string, ICommands> _commands = new Dictionary<string, ICommands>();
        private readonly Assembly.AssemblyService _assemblyService;
        private bool _disposed = false;

        public CommandService(Assembly.AssemblyService assemblyService)
        {
            _assemblyService = assemblyService;
            _assemblyService.RefleshEvent += RefreshChangedEvent;
            RefreshCommands(_assemblyService.Assemblies);
        }

        public void Dispose()
        {
            if (!_disposed)
            {
                _assemblyService.RefleshEvent -= RefreshChangedEvent;
                _disposed = true;
            }
        }

        private void RefreshChangedEvent(object? sender, Assembly.ReflecshEventArgs args)
        {
            RefreshCommands(args.Assemblies);
        }

        public void RefreshCommands(IEnumerable<System.Reflection.Assembly> assemblies)
        {
            foreach (var assembly in assemblies)
            {
                RegisterCommand(assembly);
            }
        }

        public void RegisterCommand(System.Reflection.Assembly assembly)
        {
            var commandTypes = assembly.GetTypes().Where(t => typeof(ICommands).IsAssignableFrom(t) && !t.IsAbstract);
            foreach (var commandType in commandTypes)
            {
                var commandInstance = Activator.CreateInstance(commandType) as ICommands;
                if (commandInstance != null)
                {
                    var commandName = commandInstance.GetCommandName();
                    try
                    {
                        RegisterCommand(commandName, commandInstance);
                    }
                    catch (Exception ex)
                    {
                        GlobalLogger.Warn(LogCategorySystem.Default, LogData.Create(ex.Message));
                    }
                }
            }
        }

        public void RegisterCommand(string commandName, ICommands command)
        {
            if (!_commands.ContainsKey(commandName))
            {
                _commands.Add(commandName, command);
            }
            else
            {
                throw new Exception($"Command already registered {commandName}");
            }
        }

        public async Task<CommandResult> ExecuteCommandAsync(ICommandOptions options)
        {
            if (_commands.TryGetValue(options.CommandName, out var command))
            {
                return await command.ExecuteAsync(options);
            }
            else
            {
                return new CommandResult
                {
                    Status = CommandResult.StatusType.Failure,
                    Message = $"Command '{options.CommandName}' not found."
                };
            }
        }

        public async Task<CommandResult> ExecuteCommandAsync(string commandName, string[] args)
        {
            try
            {
                var options = CreateOptions(commandName, args);
                return await ExecuteCommandAsync(options);
            }
            catch (Exception ex)
            {
                return new CommandResult
                {
                    Status = CommandResult.StatusType.Failure,
                    Message = $"Error executing command '{commandName}': {ex.Message}"
                };
            }
        }

        public async Task<CommandResult> ExecuteCommandAsync(string[] args)
        {
            var commandName = args.Length > 0 ? args[0] : throw new ArgumentException("No command name provided in arguments.");
            return await ExecuteCommandAsync(commandName, args.Skip(1).ToArray());
        }

        public ICommandOptions CreateOptions(string commandName, string[] args)
        {
            if (_commands.TryGetValue(commandName, out var command))
            {
                var optionsType = command.CommandOptionsType;
                var optionsInstance = Activator.CreateInstance(optionsType) as ICommandOptions;
                if (optionsInstance == null)
                {
                    throw new InvalidOperationException($"Failed to create an instance of '{optionsType.Name}'.");
                }

                if (!optionsInstance.ParseArguments(args))
                {
                    throw new ArgumentException("Failed to parse command arguments.");
                }
                return optionsInstance;
            }
            else
            {
                throw new KeyNotFoundException($"Command with name '{commandName}' not found.");
            }
        }
    }
}
