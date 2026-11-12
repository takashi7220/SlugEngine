using System;
using System.Reflection;

using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.DependencyInjection.Extensions;

namespace Snail.Core.Service
{
    public interface IService
    {
    }

    [AttributeUsage(AttributeTargets.Class, Inherited = false)]
    public sealed class ServiceAttribute : Attribute
    {
        public ServiceAttribute(Type? serviceType = null, bool autoRegister = true)
        {
            ServiceType = serviceType;
            AutoRegister = autoRegister;
        }

        public Type? ServiceType { get; }

        public bool AutoRegister { get; set; } = true;
    }

    public sealed class ServiceRegistry
    {
        public ServiceRegistry AddServicesFromAssembly(System.Reflection.Assembly assembly)
        {
            ArgumentNullException.ThrowIfNull(assembly);

            foreach (Type implementationType in assembly.GetTypes())
            {
                if (!implementationType.IsClass || implementationType.IsAbstract ||
                    implementationType.ContainsGenericParameters ||
                    !typeof(IService).IsAssignableFrom(implementationType))
                {
                    continue;
                }

                ServiceAttribute? attribute = implementationType.GetCustomAttribute<ServiceAttribute>(inherit: false);
                if (attribute == null || !attribute.AutoRegister)
                {
                    continue;
                }

                Type serviceType = attribute.ServiceType ?? implementationType;
                if (!typeof(IService).IsAssignableFrom(serviceType) || serviceType.ContainsGenericParameters ||
                    !serviceType.IsAssignableFrom(implementationType))
                {
                    throw new ArgumentException(
                        $"Service type '{serviceType}' must implement IService and be assignable from " +
                        $"'{implementationType}'.",
                        nameof(assembly));
                }

                _services.TryAdd(ServiceDescriptor.Singleton(serviceType, implementationType));
            }

            return this;
        }

        public ServiceRegistry AddSingleton<TService>(params object[] arguments)
            where TService : class, IService
        {
            return AddSingleton<TService, TService>(arguments);
        }

        public ServiceRegistry AddSingleton<TService, TImplementation>(params object[] arguments)
            where TService : class, IService
            where TImplementation : class, TService
        {
            ArgumentNullException.ThrowIfNull(arguments);

            if (arguments.Length == 0)
            {
                _services.AddSingleton<TService, TImplementation>();
            }
            else
            {
                var constructorArguments = (object[])arguments.Clone();
                _services.AddSingleton<TService>(provider =>
                    ActivatorUtilities.CreateInstance<TImplementation>(provider, constructorArguments));
            }

            return this;
        }

        public ServiceRegistry AddSingleton<TService>(Func<IServiceProvider, TService> factory)
            where TService : class, IService
        {
            ArgumentNullException.ThrowIfNull(factory);
            _services.AddSingleton(factory);
            return this;
        }

        public ServiceProvider BuildServiceProvider()
        {
            return _services.BuildServiceProvider(new ServiceProviderOptions
            {
                ValidateOnBuild = true,
                ValidateScopes = true
            });
        }

        private readonly IServiceCollection _services = new ServiceCollection();
    }
}
