using CommandLine;

namespace SchemaGenerator
{
    class CommandLineOptions
    {
        [Option("target_files", Required = false, HelpText = "Path to the file to be analyzed. ex.(-target_files project/source/test.hpp,xxx,xxx,...)")]
        public string TargetFiles { get; set; } = string.Empty;

        [Option("target_folders", Required = false, HelpText = "Specifies the folder to search for target files. ex.(-target_folders project/source/,xxx,xxx,...)")]
        public string TargetFolders { get; set; } = string.Empty;

        [Option("include_folders", Required = false, HelpText = "Folder to be added to the include path. ex.(-include_folders project/source/,xxx,xxx,...)")]
        public string IncludeFolders { get; set; } = string.Empty;

        [Option("extensions", Required = false, HelpText = "Specifies the file extension, Default Value is [.hpp,.h]. ex.(-extensions .hpp,.h,xxx,...)")]
        public string Extensions { get; set; } = "";

        [Option("output", Required = false, HelpText = "Output file destination.")]
        public string OutputFolder { get; set; } = "";

        [Option("cpp_version", Required = false, HelpText = "Specifies the cpp version.Default Value is [std=c++17]  ex.(-cpp_version std=c++17)")]
        public string CppVersion { get; set; } = "";

        [Option("log", Required = false, HelpText = "Output Log")]
        public bool Debug { get; set; } = false;

    }
}
