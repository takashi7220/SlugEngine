#include <gtest/gtest.h>
#include "core/command/CommandService.hpp"

using namespace slug::core;

// -------------------------------------------------------------------
// テスト用オプション・コマンド定義
// -------------------------------------------------------------------

struct TestCommandAOptions : TCommandOptionsBase<TestCommandAOptions>
{
    slug::core::String inputFile {};
    slug::core::String outputDir {};
    bool triangulate { false };
    float scale { 0.0f };
    int32_t maxVertices { 0 };
    slug::core::TVector<slug::core::String> includeDirs {};

    TestCommandAOptions()
    {
        Initialize();
    }

    void RegisterDescriptorAll() override
    {
        SLUG_COMMAND_OPTIONS_REGISTER(TestCommandAOptions, slug::core::String, inputFile, "input_file", 'i', true, true, false, "input file path")
        SLUG_COMMAND_OPTIONS_REGISTER(TestCommandAOptions, slug::core::String, outputDir, "output_dir", 'o', false, true, false, "Output directory path")
        SLUG_COMMAND_OPTIONS_REGISTER(TestCommandAOptions, bool, triangulate, "triangulate", 't', false, false, false, "Triangulate the mesh")
        SLUG_COMMAND_OPTIONS_REGISTER(TestCommandAOptions, float, scale, "scale", 's', false, true, false, "Scale factor for the mesh")
        SLUG_COMMAND_OPTIONS_REGISTER(TestCommandAOptions, int32_t, maxVertices, "max_vertices", 'm', false, true, false, "Maximum number of vertices")
        SLUG_COMMAND_OPTIONS_REGISTER(TestCommandAOptions, slug::core::TVector<slug::core::String>, includeDirs, "include_dirs", 'I', false, true, true, "Include directories for additional resources")
    }
};

// input_file（必須）を持つコマンド。SLUG_AUTO_REGISTER_COMMAND で自動登録。
class TestCommandA : public CommandBase<TestCommandAOptions>
{
public:
    bool Execute(ServiceProvider& serviceProvider, const TestCommandAOptions& options) override
    {
        return true;
    }

    void OnError(const CommandParser::ParseResult& result) override
    {
    }
};
SLUG_AUTO_REGISTER_COMMAND(TestCommandA)

struct TestCommandBOptions : TCommandOptionsBase<TestCommandBOptions>
{
    bool verbose { false };

    TestCommandBOptions()
    {
        Initialize();
    }

    void RegisterDescriptorAll() override
    {
        SLUG_COMMAND_OPTIONS_REGISTER(TestCommandBOptions, bool, verbose, "verbose", 'v', false, false, false, "Verbose output")
    }
};

// 必須オプションなし。SLUG_AUTO_REGISTER_COMMAND で自動登録。
class TestCommandB : public CommandBase<TestCommandBOptions>
{
public:
    bool Execute(ServiceProvider& serviceProvider, const TestCommandBOptions& options) override
    {
        return true;
    }

    void OnError(const CommandParser::ParseResult& result) override
    {
    }
};
SLUG_AUTO_REGISTER_COMMAND(TestCommandB)

// 手動登録テスト用。自動登録しない。
class TestCommandC : public CommandBase<TestCommandBOptions>
{
public:
    bool Execute(ServiceProvider& serviceProvider, const TestCommandBOptions& options) override
    {
        return true;
    }

    void OnError(const CommandParser::ParseResult& result) override
    {
    }
};

// -------------------------------------------------------------------
// ヘルパー
// -------------------------------------------------------------------

namespace
{

TVector<String> MakeArgs(std::initializer_list<const char*> list)
{
    TVector<String> args;
    for (const char* s : list)
        args.push_back(String(s));
    return args;
}

TReferencePtr<CommandService> MakeCommandService()
{
    ServiceProvider provider;
    provider.AddSingleton<CommandService>();
    return provider.GetService<CommandService>();
}

}

// -------------------------------------------------------------------
// 自動登録
// -------------------------------------------------------------------

TEST(CommandService, AutoRegister_CommandExists)
{
    auto service = MakeCommandService();

    EXPECT_NE(service->GetCommand(SLUG_NAMEOF(TestCommandA)), nullptr);
    EXPECT_NE(service->GetCommand(SLUG_NAMEOF(TestCommandB)), nullptr);
}

TEST(CommandService, AutoRegister_UnknownCommand_ReturnsNull)
{
    auto service = MakeCommandService();

    EXPECT_EQ(service->GetCommand("unknown_command"), nullptr);
}

// -------------------------------------------------------------------
// 手動登録
// -------------------------------------------------------------------

TEST(CommandService, ManualRegister_CommandExists)
{
    auto service = MakeCommandService();
    service->RegisterCommand("TestCommandC", MakeReference<TestCommandC>());

    EXPECT_NE(service->GetCommand("TestCommandC"), nullptr);
}

TEST(CommandService, ManualRegister_Overwrite_UpdatesCommand)
{
    auto service = MakeCommandService();
    TReferencePtr<ICommand> cmdFirst  = MakeReference<TestCommandC>();
    TReferencePtr<ICommand> cmdSecond = MakeReference<TestCommandC>();
    service->RegisterCommand("TestCommandC", cmdFirst);
    service->RegisterCommand("TestCommandC", cmdSecond);

    EXPECT_EQ(service->GetCommand("TestCommandC"), cmdSecond);
}

// -------------------------------------------------------------------
// 登録解除
// -------------------------------------------------------------------

TEST(CommandService, UnregisterCommand_CommandNotFound)
{
    auto service = MakeCommandService();
    service->UnregisterCommand(SLUG_NAMEOF(TestCommandA));

    EXPECT_EQ(service->GetCommand(SLUG_NAMEOF(TestCommandA)), nullptr);
}

TEST(CommandService, UnregisterCommand_OtherCommandsUnaffected)
{
    auto service = MakeCommandService();
    service->UnregisterCommand(SLUG_NAMEOF(TestCommandA));

    EXPECT_NE(service->GetCommand(SLUG_NAMEOF(TestCommandB)), nullptr);
}

// -------------------------------------------------------------------
// 全消去
// -------------------------------------------------------------------

TEST(CommandService, ClearCommands_AllCommandsRemoved)
{
    auto service = MakeCommandService();
    service->ClearCommands();

    EXPECT_EQ(service->GetCommand(SLUG_NAMEOF(TestCommandA)), nullptr);
    EXPECT_EQ(service->GetCommand(SLUG_NAMEOF(TestCommandB)), nullptr);
}

// -------------------------------------------------------------------
// コマンド名一覧
// -------------------------------------------------------------------

TEST(CommandService, GetRegisteredCommandNames_ContainsAutoRegistered)
{
    auto service = MakeCommandService();
    TVector<String> names;
    service->GetRegisteredCommandNames(names);

    auto contains = [&](const char* name)
    {
        for (const auto& n : names)
            if (n == name) return true;
        return false;
    };

    EXPECT_TRUE(contains(SLUG_NAMEOF(TestCommandA)));
    EXPECT_TRUE(contains(SLUG_NAMEOF(TestCommandB)));
}

TEST(CommandService, GetRegisteredCommandNames_AfterManualRegister)
{
    auto service = MakeCommandService();
    service->RegisterCommand("TestCommandC", MakeReference<TestCommandC>());

    TVector<String> names;
    service->GetRegisteredCommandNames(names);

    bool found = false;
    for (const auto& n : names)
        if (n == "TestCommandC") { found = true; break; }

    EXPECT_TRUE(found);
}

// -------------------------------------------------------------------
// ExecuteCommand（Vector<String> オーバーロード）
// -------------------------------------------------------------------

TEST(CommandService, ExecuteCommand_ValidArgs_ReturnsTrue)
{
    ServiceProvider provider;
    provider.AddSingleton<CommandService>();
    auto service = provider.GetService<CommandService>();

    bool result = service->ExecuteCommand(provider, SLUG_NAMEOF(TestCommandA), MakeArgs({ "--input_file", "foo.txt" }));

    EXPECT_TRUE(result);
}

TEST(CommandService, ExecuteCommand_NoRequiredOption_ReturnsFalse)
{
    ServiceProvider provider;
    provider.AddSingleton<CommandService>();
    auto service = provider.GetService<CommandService>();

    // input_file（必須）を渡さない
    bool result = service->ExecuteCommand(provider, SLUG_NAMEOF(TestCommandA), MakeArgs({ "--triangulate" }));

    EXPECT_FALSE(result);
}

TEST(CommandService, ExecuteCommand_UnknownCommand_ReturnsFalse)
{
    ServiceProvider provider;
    provider.AddSingleton<CommandService>();
    auto service = provider.GetService<CommandService>();

    bool result = service->ExecuteCommand(provider, "unknown_command", MakeArgs({ "--input_file", "foo.txt" }));

    EXPECT_FALSE(result);
}

TEST(CommandService, ExecuteCommand_EmptyArgs_ReturnsFalse)
{
    ServiceProvider provider;
    provider.AddSingleton<CommandService>();
    auto service = provider.GetService<CommandService>();

    bool result = service->ExecuteCommand(provider, SLUG_NAMEOF(TestCommandB), MakeArgs({}));

    EXPECT_FALSE(result);
}

TEST(CommandService, ExecuteCommand_NoRequiredOption_SucceedsForOptionalOnlyCommand)
{
    ServiceProvider provider;
    provider.AddSingleton<CommandService>();
    auto service = provider.GetService<CommandService>();

    // TestCommandB は必須オプションなし
    bool result = service->ExecuteCommand(provider, SLUG_NAMEOF(TestCommandB), MakeArgs({ "--verbose" }));

    EXPECT_TRUE(result);
}

// -------------------------------------------------------------------
// ExecuteCommand（argc / argv オーバーロード）
// -------------------------------------------------------------------

TEST(CommandService, ExecuteCommand_ArgcArgv_ValidArgs_ReturnsTrue)
{
    ServiceProvider provider;
    provider.AddSingleton<CommandService>();
    auto service = provider.GetService<CommandService>();

    const char* argv[] = { "--input_file", "foo.txt" };
    bool result = service->ExecuteCommand(provider, SLUG_NAMEOF(TestCommandA), 2, const_cast<char**>(argv));

    EXPECT_TRUE(result);
}

TEST(CommandService, ExecuteCommand_ArgcArgv_MissingRequired_ReturnsFalse)
{
    ServiceProvider provider;
    provider.AddSingleton<CommandService>();
    auto service = provider.GetService<CommandService>();

    const char* argv[] = { "--triangulate" };
    bool result = service->ExecuteCommand(provider, SLUG_NAMEOF(TestCommandA), 1, const_cast<char**>(argv));

    EXPECT_FALSE(result);
}
