#pragma once

#include "core/container/Vector.hpp"
#include "core/container/String.hpp"

namespace slug::core
{

class CommandParser
{
public:
    enum class OptionValueType
    {
        String,
        Integer,
        Float,
        Boolean,
    };

    struct OptionDefinition
    {
        OptionValueType valueType = OptionValueType::String;
        core::FixedString<32> longName;
        char shortName;
        bool required = false;
        bool takeValue = false;
        bool allowMultiple = false;
        core::FixedString<128> helpText;
    };

    struct OptionValue
    {
        OptionDefinition definition;
        bool enable = false;
        core::TVector<core::String> values;
    };

    enum class ResultType : int8_t
    {
        Success,
        MissingRequiredOption,
        InvalidOption,
        InvalidValue,
    };

    struct ParseResult
    {
        ResultType resultType = ResultType::Success;
        core::TVector<core::String> errors;
    };

public:
    virtual ParseResult Parse(int argc, char** argv);
    virtual ParseResult Parse(const core::TVector<core::String>& inArgs);
    virtual void Reset();
    void AddOption(const OptionDefinition& option);
    const OptionValue* GetOptionValue(core::StringView arg) const;

protected:
    OptionValue* FindOptionValue(core::StringView arg);
    const OptionValue* FindOptionValue(core::StringView arg) const;
    bool HasOptionValue(core::StringView arg) const;
    void CheckCommandValue(ParseResult& result) const;

protected:
    core::TVector<OptionValue> m_optionValues;
};

template<typename T>
struct CommandLineValueTypeTraits;

template<> struct CommandLineValueTypeTraits<core::String>
{
    static constexpr CommandParser::OptionValueType value = CommandParser::OptionValueType::String;
};
template<> struct CommandLineValueTypeTraits<bool>
{
    static constexpr CommandParser::OptionValueType value = CommandParser::OptionValueType::Boolean;
};
template<> struct CommandLineValueTypeTraits<float>
{
    static constexpr CommandParser::OptionValueType value = CommandParser::OptionValueType::Float;
};
template<> struct CommandLineValueTypeTraits<int32_t>
{
    static constexpr CommandParser::OptionValueType value = CommandParser::OptionValueType::Integer;
};
template<> struct CommandLineValueTypeTraits<core::TVector<core::String>>
{
    static constexpr CommandParser::OptionValueType value = CommandParser::OptionValueType::String;
};

class CommandOptionsUtility
{
public:
    static void Populate(const CommandParser& cmd, core::StringView longName, core::String& field);
    static void Populate(const CommandParser& cmd, core::StringView longName, bool& field);
    static void Populate(const CommandParser& cmd, core::StringView longName, float& field);
    static void Populate(const CommandParser& cmd, core::StringView longName, int32_t& field);
    static void Populate(const CommandParser& cmd, core::StringView longName, core::TVector<core::String>& field);
    static void BuildHelpText(const CommandParser::OptionDefinition& definition, core::String& helpText);
};

template<typename TOptions>
class TCommandOptionsBase
{
public:
    struct Descriptor
    {
        CommandParser::OptionDefinition definition;
        void (*populate)(const CommandParser&, TOptions&);
    };

public:
    virtual void RegisterDescriptorAll() = 0;

    void Initialize()
    {
        m_descroptors.clear();
        RegisterDescriptorAll();
        m_initialized = true;
    }

    bool RegisterDescriptor(const Descriptor& desc)
    {
        m_descroptors.push_back(desc);
        return true;
    }

    void RegisterOptions(CommandParser& cmdLine)
    {
        for (const auto& d : m_descroptors)
        {
            cmdLine.AddOption(d.definition);
        }
    }

    void PopulateOptions(const CommandParser& cmdLine, TOptions& options)
    {
        for (const auto& d : m_descroptors)
        {
            d.populate(cmdLine, options);
        }
    }

    void Reset()
    {
        TOptions* options = reinterpret_cast<TOptions*>(this);
        *options = TOptions();
        if (!m_initialized)
        {
            Initialize();
        }
    }

    void BuildHelpText(core::String& helpText)
    {
        for (const auto& d : m_descroptors)
        {
            CommandOptionsUtility::BuildHelpText(d.definition, helpText);
        }
    }

    bool IsInitialized() const
    {
        return m_initialized;
    }

private:
    core::TVector<Descriptor> m_descroptors;
    bool m_initialized = false;
};

template<typename TOptions>
class TCommandParser : public CommandParser
{
public:
    TCommandParser(TOptions& commandOptions)
        : m_commandOptions(commandOptions)
    {
        if (!m_commandOptions.IsInitialized())
        {
            m_commandOptions.Initialize();
        }
         m_commandOptions.RegisterOptions(*this);
    }

    ParseResult Parse(int argc, char** argv) override
    {
        ParseResult result = CommandParser::Parse(argc, argv);
        if (result.resultType == ResultType::Success)
        {
            m_commandOptions.PopulateOptions(*this, m_commandOptions);
        }
        return result;
    }

    ParseResult Parse(const core::TVector<core::String>& inArgs) override
    {
        ParseResult result = CommandParser::Parse(inArgs);
        if (result.resultType == ResultType::Success)
        {
            m_commandOptions.PopulateOptions(*this, m_commandOptions);
        }
        return result;
    }

    void Reset() override
    {
        CommandParser::Reset();
        m_commandOptions.Reset();
    }

private:
    TOptions& m_commandOptions;
};
}

// コマンドオプションの登録用マクロ
//
// 使い方:
//   struct MyOptions : CommandOptionsBase<MyOptions>
//   {
//      core::String inputFile = {};
//      virtual void RegisterDescriptorAll() override
//      {
//          SLUG_COMMAND_OPTIONS_REGISTER(MyOptions, core::String, inputFile, "input", 'i', true, true, false, "Input file path");
//          ...
//      }
//   };

#define SLUG_COMMAND_OPTIONS_REGISTER(classType, cppType, fieldName, longName, shortCh, required, takeValue, allowMultiple, helpText)   \
    RegisterDescriptor({                                                                                                                \
        { slug::core::CommandLineValueTypeTraits<cppType>::value, longName, shortCh, required, takeValue, allowMultiple, helpText},     \
        [](const slug::core::CommandParser& cmd, classType& opts) {                                                                   \
            slug::core::CommandOptionsUtility::Populate(cmd, longName, opts.fieldName);                                                 \
        }                                                                                                                               \
    });       
