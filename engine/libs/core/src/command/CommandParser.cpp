#include "core/command/CommandParser.hpp"

namespace slug::core
{
void CommandParser::AddOption(const OptionDefinition& option)
{
    if (!HasOptionValue(option.longName.c_str()))
    {
        OptionValue& value = m_optionValues.emplace_back();
        value.definition = option;
    }
}

CommandParser::ParseResult CommandParser::Parse(int argc, char** argv)
{
    core::TVector<core::String> args;
    for (int i = 0; i < argc; ++i)
    {
        args.push_back(core::String(argv[i]));
    }
    return CommandParser::Parse(args);
}

CommandParser::ParseResult CommandParser::Parse(const core::TVector<core::String>& inArgs)
{
    ParseResult result = {};
    core::TVector<core::String> args = {};
    for (const auto& arg : inArgs)
    {
        core::StringUtility::Split(arg, "=", args);
    }

    OptionValue* currentValue = nullptr;
    for (const auto& arg : args)
    {
        if (arg[0] == '-' || arg[0] == '/')
        {
            if (arg[1] == '-')
            {
                // long name
                currentValue = FindOptionValue(arg.substr(2));
            } else
            {
                // short name
                currentValue = FindOptionValue(arg.substr(1, 1));
            }

            if (currentValue)
            {
                currentValue->enable = true;
            }
        }
        else if (currentValue)
        {
            currentValue->values.push_back(arg);
        }

        if(!currentValue)
        {
            result.resultType = ResultType::InvalidOption;
            result.errors.push_back(core::StringUtility::Snprintf("Invalid option: %s", arg.c_str()));
        }
    }

    CheckCommandValue(result);
    return result;
}

const CommandParser::OptionValue* CommandParser::GetOptionValue(core::StringView arg) const
{
    return FindOptionValue(arg);
}

void CommandParser::Reset()
{
    for (auto& value : m_optionValues)
    {
        value.enable = false;
        value.values.clear();
    }
}

CommandParser::OptionValue* CommandParser::FindOptionValue(core::StringView arg)
{
    for (auto& value : m_optionValues)
    {
        if (core::StringUtility::IsEqual(arg.data(), value.definition.longName.c_str()) || (arg[0] == value.definition.shortName))
        {
            return &value;
        }
    }
    return nullptr;
}

const CommandParser::OptionValue* CommandParser::FindOptionValue(core::StringView arg) const
{
    for (const auto& value : m_optionValues)
    {
        if (core::StringUtility::IsEqual(arg.data(), value.definition.longName.c_str()))
        {
            return &value;
        }
        else if (arg.size() == 1 && arg[0] == value.definition.shortName)
        {
            return &value;
        }
    }
    return nullptr;
}

bool CommandParser::HasOptionValue(core::StringView arg) const
{
    return FindOptionValue(arg) != nullptr;
}

void CommandParser::CheckCommandValue(ParseResult& result) const
{
    for (const auto& value : m_optionValues)
    {
        if (value.definition.required && !value.enable)
        {
            result.resultType = ResultType::MissingRequiredOption;
            result.errors.push_back(core::StringUtility::Snprintf("Missing required option: %s", value.definition.longName.c_str()));
        }

        if (value.enable && value.definition.takeValue && value.values.empty())
        {
            result.resultType = ResultType::InvalidValue;
            result.errors.push_back(core::StringUtility::Snprintf("Missing value for option: %s", value.definition.longName.c_str()));
        }

        if (value.enable && !value.definition.takeValue && value.values.size() > 0)
        {
            result.resultType = ResultType::InvalidValue;
            result.errors.push_back(core::StringUtility::Snprintf("Option does not take a value: %s", value.definition.longName.c_str()));
        }

        if (value.enable && !value.definition.allowMultiple && value.values.size() > 1)
        {
            result.resultType = ResultType::InvalidValue;
            result.errors.push_back(core::StringUtility::Snprintf("Multiple values not allowed for option: %s", value.definition.longName.c_str()));
        }
    }
}

void CommandOptionsUtility::Populate(const CommandParser& cmd, core::StringView longName, core::String& field)
{
    if (const auto* v = cmd.GetOptionValue(longName); v && !v->values.empty())
    {
        field = v->values[0];
    }
}

void CommandOptionsUtility::Populate(const CommandParser& cmd, core::StringView longName, bool& field)
{
    if (const auto* v = cmd.GetOptionValue(longName))
    {
        field = v->enable;
    }
}

void CommandOptionsUtility::Populate(const CommandParser& cmd, core::StringView longName, float& field)
{
    if (const auto* v = cmd.GetOptionValue(longName); v && !v->values.empty())
    {
        field = core::StringUtility::Stof(v->values[0].c_str());
    }
}

void CommandOptionsUtility::Populate(const CommandParser& cmd, core::StringView longName, int32_t& field)
{
    if (const auto* v = cmd.GetOptionValue(longName); v && !v->values.empty())
    {
        field = core::StringUtility::Stoi(v->values[0].c_str());
    }
}

void CommandOptionsUtility::Populate(const CommandParser& cmd, core::StringView longName, core::TVector<core::String>& field)
{
    if (const auto* v = cmd.GetOptionValue(longName))
    {
        for (const auto& val : v->values)
        {
            field.push_back(val);
        }
    }
}

void CommandOptionsUtility::BuildHelpText(const CommandParser::OptionDefinition& definition, core::String& helpText)
{
    helpText.append("  ");
    if (definition.shortName != '\0')
    {
        helpText.append("-");
        helpText.append(1, definition.shortName);
        helpText.append(", ");
    }
    helpText.append("--");
    helpText.append(definition.longName.c_str());
    if (definition.takeValue)
    {
        if (definition.allowMultiple)
        {
            helpText.append(" <value1> <value2> ...");
        }
        else
        {
            helpText.append(" <value>");
        }
    }
    if (definition.required)
    {
        helpText.append(" (required)");
    }
    if (definition.helpText.size() > 0)
    {
        helpText.append(" [ ");
        helpText.append(definition.helpText.c_str());
        helpText.append(" ]");
    }
    helpText.append("\n");

}
}
