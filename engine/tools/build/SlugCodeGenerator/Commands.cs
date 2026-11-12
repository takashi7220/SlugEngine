using System;
using System.Collections.Generic;
using System.Text;

using CommandLine;

namespace SlugCodeGenerator
{
    internal class CommandsOptions : Snail.Core.Commands.ICommandOptions
    {
        [Option(shortName: 'i', longName: "input-dir", Required = true, HelpText = "")]
        public string InputDir { get; set; } = string.Empty;

        [Option(shortName: 'o', longName: "output-dir", Required = true, HelpText = "")]
        public string OutputDir { get; set; } = string.Empty;

        public string CommandName => "slug-code-generator";

        public bool ParseArguments(string[] args)
        {
            var parseResult = Parser.Default.ParseArguments<CommandsOptions>(args);
            if (parseResult.Tag == ParserResultType.NotParsed)
            {
                return false;
            }

            InputDir = parseResult.Value.InputDir;
            OutputDir = parseResult.Value.OutputDir;

            return true;
        }
    }
}
