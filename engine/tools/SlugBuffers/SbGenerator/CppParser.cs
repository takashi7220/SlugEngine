using SbGeneratorLib.Cpp;
using SbReflectionLib.Reflections;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml.Linq;

namespace SbGenerator
{
    public class Alias
    {
        public string Type { get; set; } = string.Empty;
        public string CanonicalType { get; set; } = string.Empty;
    }

    public class CppParser
    {
        static public void ParseCppDefinition(string xml)
        {
            var doc = XDocument.Parse(xml);
            var schema = doc.Element(SchemaGeneratorLib.Xml.XmlSchemaName.Schema);
            if (schema == null)
            {
                return;
            }

            var aliasList = ParseAlias(schema.Element(SchemaGeneratorLib.Xml.XmlSchemaName.AliasReflections));
        }

        static public FileDefinition ParseClassDefinition(XElement? classReflections) 
        {
            var fileDefinition = new FileDefinition();
            if (classReflections == null) 
            {
                return fileDefinition;
            }

            var classElements = classReflections.Descendants(SchemaGeneratorLib.Xml.XmlSchemaName.Element);
            foreach (var classElement in classElements) 
            {
                var namespaceName = classElement.Attribute(SchemaGeneratorLib.Xml.XmlSchemaName.Namespace)?.Value ?? "";
                var className = classElement.Attribute(SchemaGeneratorLib.Xml.XmlSchemaName.ClassName)?.Value ?? "";

            }
            return fileDefinition;
        }

        static public List<Alias> ParseAlias(XElement? aliasReflections)
        {
            var aliasList = new List<Alias>();
            if (aliasReflections == null)
            {
                return aliasList;
            }

            var aliasElements = aliasReflections.Descendants(SchemaGeneratorLib.Xml.XmlSchemaName.Element);
            foreach (var aliasElement in aliasElements) 
            {
                var type = aliasElement.Attribute(SchemaGeneratorLib.Xml.XmlSchemaName.Type)?.Value ?? "";
                var canonicalType = aliasElement.Attribute(SchemaGeneratorLib.Xml.XmlSchemaName.CanonicalType)?.Value ?? "";
                if (!string.IsNullOrEmpty(type) && !string.IsNullOrEmpty(canonicalType)) 
                {
                    aliasList.Add(new Alias 
                    {
                        Type = type,
                        CanonicalType = canonicalType
                    });
                }
            }
            return aliasList;
        }
    }
}
