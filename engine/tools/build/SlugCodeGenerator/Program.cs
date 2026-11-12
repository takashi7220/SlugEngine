using SlugCodeGenerator;

internal static class Program
{
    private static void Main(string[] args)
    {
        CommandsOptions options = new CommandsOptions();
        if(options.ParseArguments(args))
        {
            var files = Directory.GetFiles(options.InputDir, "*.hpp", SearchOption.AllDirectories);
            var includeDirs = Directory.GetDirectories(options.InputDir, "public*", SearchOption.AllDirectories);


        }
    }
}
