using System;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Runtime.Loader;

using ReflectionAssembly = System.Reflection.Assembly;

using Snail.Core.Service;

namespace Snail.Core.Assembly
{
    public struct ReflecshEventArgs
    {
        public IReadOnlySet<ReflectionAssembly> Assemblies { get; set; }

        public IReadOnlySet<string> UnresolvedAssemblies { get; set; }
    }

    [ServiceAttribute(typeof(AssemblyService), true)]
    public sealed class AssemblyService : IService
    {
        public AssemblyService()
        {
            Refresh();
        }

        public IReadOnlySet<ReflectionAssembly> Assemblies => _assemblies;

        public IReadOnlySet<string> UnresolvedAssemblies => _unresolvedAssemblies;

        /// <summary>
        /// Collects the entry assembly, loaded assemblies, and their transitive managed dependencies.
        /// Returns false if any referenced assemblies could not be loaded; available assemblies are retained.
        /// Call again to include assemblies loaded after construction.
        /// </summary>
        public bool Refresh()
        {
            _assemblies.Clear();
            _unresolvedAssemblies.Clear();

            var pending = new Stack<ReflectionAssembly>(AppDomain.CurrentDomain.GetAssemblies());
            ReflectionAssembly? entryAssembly = ReflectionAssembly.GetEntryAssembly();
            if (entryAssembly != null)
            {
                pending.Push(entryAssembly);
            }

            while (pending.TryPop(out ReflectionAssembly? assembly))
            {
                if (!_assemblies.Add(assembly) || assembly.IsDynamic)
                {
                    continue;
                }

                AssemblyLoadContext loadContext =
                    AssemblyLoadContext.GetLoadContext(assembly) ?? AssemblyLoadContext.Default;

                foreach (AssemblyName referenceName in assembly.GetReferencedAssemblies())
                {
                    try
                    {
                        pending.Push(loadContext.LoadFromAssemblyName(referenceName));
                    }
                    catch (Exception exception) when (
                        exception is FileNotFoundException or FileLoadException or BadImageFormatException)
                    {
                        _unresolvedAssemblies.Add(referenceName.FullName);
                    }
                }
            }

            RefleshEvent.Invoke(this, new ReflecshEventArgs()
            {
                Assemblies = _assemblies,
                UnresolvedAssemblies = _unresolvedAssemblies
            });
            return _unresolvedAssemblies.Count == 0;
        }

        public EventHandler<ReflecshEventArgs> RefleshEvent = delegate { };

        private readonly HashSet<ReflectionAssembly> _assemblies = new HashSet<ReflectionAssembly>();
        private readonly HashSet<string> _unresolvedAssemblies = new HashSet<string>(StringComparer.Ordinal);
    }
}
