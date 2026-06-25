using CommandLine;
using SchemaGenerator;
using SbReflectionLib.Reflections;
using System.Text;
using SlugTools.Core;
using SchemaGeneratorLib.Xml;
using System.Xml.Linq;

class Program
{
    static bool CheckCommandOptions(ref CommandLineOptions options) 
    {
        if (string.IsNullOrEmpty(options.Extensions)) 
        {
            options.Extensions = ".hpp,h";
        }

        if (string.IsNullOrEmpty(options.OutputFolder)) 
        {
            options.OutputFolder = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "schema");
        }

        if (string.IsNullOrEmpty(options.CppVersion)) 
        {
            options.CppVersion = "-std=c++17";
        }

        return true;
    }

    static SchemaGeneratorLib.Cpp.ReflectionGenerator.SetupParameter GenerateSetupParameter(CommandLineOptions option)
    {
        Logger.WriteLine(Logger.LogType.Debug, "Start Search Analyze Files...");

        var includeFolders = new List<string>();
        var targetFiles = new List<string>();

        // Scehmaに渡すための解析対象のファイル、インクルードフォルダを列挙します。
        includeFolders.AddRange(EnumerateFiles.SplitString(option.IncludeFolders, ","));
        targetFiles.AddRange(EnumerateFiles.SplitString(option.TargetFiles, ","));

        var targetFolders = EnumerateFiles.SplitString(option.TargetFolders, ",");
        var extensions = EnumerateFiles.SplitString(option.Extensions, ",");
        targetFiles.AddRange(EnumerateFiles.EnumerateTargetFiles(targetFolders, extensions));

        var analyzeFiles = EnumerateFiles.EnumerateAnalyzeFiles(targetFiles);

        var ret = new SchemaGeneratorLib.Cpp.ReflectionGenerator.SetupParameter();
        ret.analyzeFiles = analyzeFiles;
        ret.includeFolders = includeFolders;
        ret.version = option.CppVersion;

        return ret;
    }

    static ReflectionData GenerateSchema(CommandLineOptions option)
    {
        var param = GenerateSetupParameter(option);
        param.AnalyzeCallBack = (index, totalCount, analyzeFile) =>
        {
            Logger.Write("\r");
            Logger.Write($"{index + 1} / {totalCount}");
        };

        Logger.WriteLine(Logger.LogType.Debug, "Start Analyze Files...");
        var schemaGenerator = new SchemaGeneratorLib.Cpp.ReflectionGenerator();
        var scehme = SchemaGeneratorLib.Cpp.ReflectionGenerator.Generate(param);

        return scehme;
    }

    static string GenerateSchemaXml(ReflectionData reflections)
    {
        Logger.WriteLine(Logger.LogType.Debug, "Start Generate Xml...");
        var param = new XmlGenerator.SetupParameter();
        param.ClassReflections = reflections.ClassReflections;
        param.AliasReflections = reflections.AliasReflections;
        param.EnumReflections = reflections.EnumReflections;
        var xmlGenerator = new XmlGenerator(param);
        var xml = xmlGenerator.GenerateXml();

        return xml;
    }

    static void WriteSchemaXml(string schemaXml, string path)
    {
        FileUtility.WriteFile(path, schemaXml);
        Logger.WriteLine(Logger.LogType.Debug, $"Output Schema : {path}");
    }

    static void WriteSchemaXml(CommandLineOptions options, string filename) 
    {
        var path = Path.Combine(options.OutputFolder, filename);
        var reflections = GenerateSchema(options);
        var reflectionsXml = GenerateSchemaXml(reflections);
        WriteSchemaXml(reflectionsXml, path);
    }


    static int Main(string[] args)
    {
        bool success = false;
        try
        {
            // コマンドラインオプションをパース
            var parseResult = Parser.Default.ParseArguments<CommandLineOptions>(args);
            if (parseResult.Tag == ParserResultType.NotParsed)
            {
                throw new Exception("Failed command line parse");
            }

            CommandLineOptions options = parseResult.Value;

            CheckCommandOptions(ref options);
            WriteSchemaXml(options, "slug_schema.xml");

            Logger.WriteLine(Logger.LogType.Debug, "Success", ConsoleColor.White, ConsoleColor.Green);
            Logger.ResetColor();
            success = true;
        }
        catch (Exception ex)
        {
            Logger.WriteLine(Logger.LogType.Debug, ex.Message, ConsoleColor.White, ConsoleColor.Red);
            Logger.ResetColor();
        }

        return success ? 0 : 1;

    }
}