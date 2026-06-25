using System;
using System.Collections.Generic;
using System.Diagnostics.Tracing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SbGeneratorLib.Cpp
{
    public enum MemberAccessLevel
    {
        Public,
        Private,
        Protected
    }

    public class MacroDefinition
    {
        public string Code { get; set; } = string.Empty;

        public string GenerateCode()
        {
            return Code;
        }
    }

    public class VariantDefinition
    {
        public string Name { get; set; } = string.Empty;

        public string ValueType { get; set; } = string.Empty;

        public string Value { get; set; } = string.Empty;

        public string GenerateCode()
        {
            if (!string.IsNullOrEmpty(Value))
            {
                return $"{ValueType} {Name} = {Value};";
            }
            return $"{ValueType} {Name};";
        }
    }

    public class FunctionDefinition
    {
        public string Name { get; set; } = string.Empty;

        public string ReturnType { get; set; } = string.Empty;

        public List<VariantDefinition> Arguments { get; set; } = new List<VariantDefinition>();

        public string Code { get; set; } = string.Empty;

        public bool IsStatic { get; set; } = false;

        public string GenerateCode(string headOption = "", string backOption = "")
        {
            var sb = new StringBuilder();
            if (IsStatic)
            {
                sb.Append($"static {ReturnType} {Name}(");
            }
            else if (!string.IsNullOrEmpty(headOption))
            {
                sb.Append($"{headOption} {ReturnType} {Name}(");
            }
            else
            {
                sb.Append($"{ReturnType} {Name}(");
            }

            foreach (var argument in Arguments)
            {
                sb.Append($"{argument.GenerateCode()},");
            }

            sb.Remove(sb.Length - 1, sb.Length);
            if (!string.IsNullOrEmpty(backOption))
            {
                sb.AppendLine($") {backOption}" + "{");
            }
            else
            {
                sb.AppendLine(") {");
            }

            sb.AppendLine($"{Code}");
            sb.AppendLine("}");
            return sb.ToString();
        }
    }

    public class FieldVariantDefinition
    {
        public VariantDefinition Variant { get; set; } = new VariantDefinition();

        public string GenerateCode()
        {
            return Variant.GenerateCode();
        }
    }

    public class FieldFunctionDefinition
    {
        public FunctionDefinition Function { get; set; } = new FunctionDefinition();

        public bool isVirtual { get; set; } = false;

        public bool isOverride { get; set; } = false;

        public string GenerateCode()
        {
            return Function.GenerateCode(isVirtual ? "virtual" : "", isOverride ? "override" : "");
        }
    }

    public class ClassElementDefinition
    {
        public List<MacroDefinition> Macros { get; set; } = new List<MacroDefinition>();

        public List<ClassDefinition> Classes { get; set; } = new List<ClassDefinition>();

        public List<EnumDefinition> Enums { get; set; } = new List<EnumDefinition>();

        public List<FieldVariantDefinition> Variants { get; set; } = new List<FieldVariantDefinition>();

        public List<FieldFunctionDefinition> Functions { get; set; } = new List<FieldFunctionDefinition>();

        public string GenerateCode()
        {
            var sb = new StringBuilder();
            foreach (var macro in Macros)
            {
                sb.AppendLine(macro.Code);
            }

            foreach (var Class in Classes)
            {
                sb.AppendLine(Class.GenerateCode());
            }

            foreach (var Enum in Enums)
            {
                sb.AppendLine(Enum.GenerateCode());
            }

            foreach (var Function in Functions)
            {
                sb.AppendLine(Function.GenerateCode());
            }

            foreach (var Variant in Variants)
            {
                sb.AppendLine(Variant.GenerateCode());
            }

            return sb.ToString();
        }
    }

    public class ClassDefinition
    {
        public string Name { get; set; } = string.Empty;

        public string InheritName { get; set; } = string.Empty;

        public Dictionary<MemberAccessLevel, ClassElementDefinition> Members { get; set; } = new Dictionary<MemberAccessLevel, ClassElementDefinition>();

        public string GenerateCode()
        {
            var sb = new StringBuilder();
            if (!string.IsNullOrEmpty(InheritName))
            {
                sb.AppendLine($"class {Name} : public {InheritName}" + "{");
            }
            else
            {
                sb.AppendLine($"class {Name}" + "{");
            }

            var publicMember = Members[MemberAccessLevel.Public];
            sb.AppendLine("public:");
            sb.AppendLine(publicMember.GenerateCode());

            var protectedMember = Members[MemberAccessLevel.Protected];
            sb.AppendLine("protected:");
            sb.AppendLine(protectedMember.GenerateCode());

            var privateMember = Members[MemberAccessLevel.Private];
            sb.AppendLine("private:");
            sb.AppendLine(privateMember.GenerateCode());

            sb.AppendLine("};");
            return sb.ToString();
        }
    }

    public class EnumItemDefition
    {
        public string Name { get; set; } = string.Empty;
        public string Value { get; set; } = string.Empty;
    }

    public class EnumDefinition
    {
        public string Name { get; set; } = string.Empty;

        public string ValueType { get; set; } = string.Empty;

        public List<EnumItemDefition> Items { get; set; } = new List<EnumItemDefition>();
        public string GenerateCode()
        {
            var sb = new StringBuilder();
            sb.AppendLine($"enum class {Name} : {ValueType} " + "{");
            foreach (var item in Items)
            {
                sb.AppendLine($"{item.Name} = {item.Value},");
            }
            sb.AppendLine("};");
            return sb.ToString();
        }
    }

    public class ElementDefinition
    {
        public List<MacroDefinition> Macros { get; set; } = new List<MacroDefinition>();

        public List<EnumDefinition> Enums { get; set; } = new List<EnumDefinition>();

        public List<ClassDefinition> Classes { get; set; } = new List<ClassDefinition>();

        public List<FunctionDefinition> Functions { get; set; } = new List<FunctionDefinition>();

        public List<VariantDefinition> Variants { get; set; } = new List<VariantDefinition>();

        public string GenerateCode()
        {
            var sb = new StringBuilder();
            foreach (MacroDefinition macro in Macros)
            {
                sb.AppendLine(macro.Code);
            }

            foreach (EnumDefinition Enum in Enums)
            {
                sb.AppendLine(Enum.GenerateCode());
            }

            foreach (ClassDefinition Class in Classes)
            {
                sb.AppendLine(Class.GenerateCode());
            }

            foreach (FunctionDefinition function in Functions)
            {
                sb.AppendLine(function.GenerateCode());
            }

            foreach (VariantDefinition variant in Variants)
            {
                sb.AppendLine(variant.GenerateCode());
            }
            return sb.ToString();
        }
    }

    public class NamespaceDefinition
    {
        public string Name { get; set; } = string.Empty;

        public List<NamespaceDefinition> Namespaces { get; set; } = new List<NamespaceDefinition>();

        public ElementDefinition Element { get; set; } = new ElementDefinition();

        public string GenerateCode()
        {
            var sb = new StringBuilder();
            sb.AppendLine($"namespace {Name}" + "{");
            foreach (var Namespace in Namespaces)
            {
                sb.AppendLine(Namespace.GenerateCode());
            }

            sb.AppendLine(Element.GenerateCode());
            sb.AppendLine("}");
            return sb.ToString();
        }
    }

    public class IncludeDefinition
    {
        public bool External { get; set; } = false;

        public string IncludeFileName { get; set; } = string.Empty;

        public string GenerateCode()
        {
            var ret = string.Empty;
            if (External)
            {
                ret = $"#include<{IncludeFileName}>";
            }
            else
            {
                ret = $"#include\"{IncludeFileName}\"";
            }
            return ret;
        }
    }

    public class FileDefinition
    {
        public string Name { get; set; } = string.Empty;

        public string Extension { get; set; } = string.Empty;

        public List<IncludeDefinition> Includes { get; set; } = new List<IncludeDefinition>();

        public List<NamespaceDefinition> Namespaces { get; set; } = new List<NamespaceDefinition>();

        public ElementDefinition Element { get; set; } = new ElementDefinition();

        public string GenerateCode()
        {
            var sb = new StringBuilder();
            foreach (var include in Includes)
            {
                sb.AppendLine(include.GenerateCode());
            }

            return sb.ToString();
        }
    }
}
