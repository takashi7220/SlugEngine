#include <gtest/gtest.h>
#include "core/command/CommandParser.hpp"

using namespace slug::core;

struct CommandOptions : TCommandOptionsBase<CommandOptions>
{
    slug::core::String inputFile {};
    slug::core::String outputDir {};
    bool triangulate { false };
    float scale { 0.0f };
    int32_t maxVertices { 0 };
    slug::core::TVector<slug::core::String> includeDirs {};

    CommandOptions()
    {
        Initialize();
    }

    void RegisterDescriptorAll() override
    {
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, slug::core::String, inputFile, "input_file", 'i', true, true, false, "input file path")
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, slug::core::String, outputDir, "output_dir", 'o', false, true, false, "Output directory path")
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, bool, triangulate, "triangulate", 't', false, false, false, "Triangulate the mesh")
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, float, scale, "scale", 's', false, true, false, "Scale factor for the mesh")
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, int32_t, maxVertices, "max_vertices", 'm', false, true, false, "Maximum number of vertices")
        SLUG_COMMAND_OPTIONS_REGISTER(CommandOptions, slug::core::TVector<slug::core::String>, includeDirs, "include_dirs", 'I', false, true, true, "Include directories for additional resources")
    }
};

namespace
{

TVector<String> MakeArgs(std::initializer_list<const char*> list)
{
    TVector<String> args;
    for (const char* s : list)
        args.push_back(String(s));
    return args;
}

// -------------------------------------------------------------------
// 正常系
// -------------------------------------------------------------------

TEST(CommandParserTest, Parse_LongName_KeyEqualsValue)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({"--input_file=foo.txt"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::Success);
    EXPECT_EQ(options.inputFile, "foo.txt");
}

TEST(CommandParserTest, Parse_LongName_SeparateValue)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({"--input_file", "foo.txt"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::Success);
    EXPECT_EQ(options.inputFile, "foo.txt");
}

TEST(CommandParserTest, Parse_ShortName_SeparateValue)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({"-i", "foo.txt"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::Success);
    EXPECT_EQ(options.inputFile, "foo.txt");
}

TEST(CommandParserTest, Parse_SlashPrefix_ShortName)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({"/i", "foo.txt"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::Success);
    EXPECT_EQ(options.inputFile, "foo.txt");
}

TEST(CommandParserTest, Parse_BoolFlag_Enabled)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({"--input_file=a.txt", "--triangulate"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::Success);
    EXPECT_TRUE(options.triangulate);
}

TEST(CommandParserTest, Parse_BoolFlag_NotSpecified_RemainsDefault)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({"--input_file=a.txt"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::Success);
    EXPECT_FALSE(options.triangulate);
}

TEST(CommandParserTest, Parse_FloatOption)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({"--input_file=a.txt", "--scale=1.5"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::Success);
    EXPECT_FLOAT_EQ(options.scale, 1.5f);
}

TEST(CommandParserTest, Parse_IntOption)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({"--input_file=a.txt", "--max_vertices=256"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::Success);
    EXPECT_EQ(options.maxVertices, 256);
}

TEST(CommandParserTest, Parse_StringVector_MultipleEntries)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({"--input_file=a.txt", "-I", "dir1", "-I", "dir2"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::Success);
    ASSERT_EQ(options.includeDirs.size(), 2u);
    EXPECT_EQ(options.includeDirs[0], "dir1");
    EXPECT_EQ(options.includeDirs[1], "dir2");
}

TEST(CommandParserTest, Parse_AllOptions)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({
        "--input_file=mesh.obj",
        "--output_dir=./out",
        "--triangulate",
        "--scale=2.0",
        "--max_vertices=1000",
        "-I", "res/textures",
        "-I", "res/shaders",
    }));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::Success);
    EXPECT_EQ(options.inputFile,    "mesh.obj");
    EXPECT_EQ(options.outputDir,    "./out");
    EXPECT_TRUE(options.triangulate);
    EXPECT_FLOAT_EQ(options.scale,  2.0f);
    EXPECT_EQ(options.maxVertices,  1000);
    ASSERT_EQ(options.includeDirs.size(), 2u);
    EXPECT_EQ(options.includeDirs[0], "res/textures");
    EXPECT_EQ(options.includeDirs[1], "res/shaders");
}

// -------------------------------------------------------------------
// Reset
// -------------------------------------------------------------------

TEST(CommandParserTest, Reset_ClearsAllValues)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);
    cmdLine.Parse(MakeArgs({"--input_file=foo.txt", "--triangulate", "--scale=3.0"}));

    cmdLine.Reset();

    EXPECT_EQ(options.inputFile, "");
    EXPECT_FALSE(options.triangulate);
    EXPECT_FLOAT_EQ(options.scale, 0.0f);
}

TEST(CommandParserTest, Reset_AllowsReparse)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);
    cmdLine.Parse(MakeArgs({"--input_file=first.txt"}));
    cmdLine.Reset();

    auto result = cmdLine.Parse(MakeArgs({"--input_file=second.txt"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::Success);
    EXPECT_EQ(options.inputFile, "second.txt");
}

// -------------------------------------------------------------------
// エラー系
// -------------------------------------------------------------------

TEST(CommandParserTest, Parse_MissingRequiredOption_Failure)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({"--triangulate"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::MissingRequiredOption);
    EXPECT_FALSE(result.errors.empty());
}

TEST(CommandParserTest, Parse_UnknownOption_InvalidOption)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    auto result = cmdLine.Parse(MakeArgs({"--input_file=second.txt", "--unknown_options" }));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::InvalidOption);
    EXPECT_FALSE(result.errors.empty());
}

TEST(CommandParserTest, Parse_TakeValueOption_MissingValue_InvalidValue)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    // --scale を指定したが値を渡さず次のオプションが続く
    auto result = cmdLine.Parse(MakeArgs({"--input_file=a.txt", "--scale", "--triangulate"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::InvalidValue);
    EXPECT_FALSE(result.errors.empty());
}

TEST(CommandParserTest, Parse_MultipleValues_NotAllowed_InvalidValue)
{
    CommandOptions options;
    TCommandParser<CommandOptions> cmdLine(options);

    // input_file は allowMultiple=false なのに値を2つ渡す
    auto result = cmdLine.Parse(MakeArgs({"--input_file", "a.txt", "b.txt"}));

    EXPECT_EQ(result.resultType, CommandParser::ResultType::InvalidValue);
    EXPECT_FALSE(result.errors.empty());
}

// -------------------------------------------------------------------
// BuildHelpText
// -------------------------------------------------------------------

TEST(CommandParserTest, BuildHelpText_ContainsAllOptionNames)
{
    CommandOptions options;
    String helpText;

    options.BuildHelpText(helpText);
    EXPECT_NE(helpText.find("input_file"),   String::npos);
    EXPECT_NE(helpText.find("output_dir"),   String::npos);
    EXPECT_NE(helpText.find("triangulate"),  String::npos);
    EXPECT_NE(helpText.find("scale"),        String::npos);
    EXPECT_NE(helpText.find("max_vertices"), String::npos);
    EXPECT_NE(helpText.find("include_dirs"), String::npos);
}

}
