using SbReflectionLib.Reflections;
using System.Xml;

namespace SchemaGeneratorLib.Xml
{

    /// <summary>
    /// XMLのScehmaで使用する名前
    /// </summary>
    public class XmlSchemaName 
    {
        public static string Schema = "Schema";
        public static string AliasReflections = "AliasReflections";
        public static string EnumReflections = "EnumReflections";
        public static string EnumItemReflections = "EnumItemReflections";
        public static string ClassReflections = "ClassReflections";
        public static string FieldReflections = "FieldReflections";
        public static string Element = "Element";
        public static string Name = "Name";
        public static string Value = "Value";
        public static string Type = "Type";
        public static string Namespace = "Namespace";
        public static string ClassName = "ClassName";
        public static string ClassType = "ClassType";
        public static string InheritClassName = "InheritClassName";
        public static string ParentClass = "ParentClass";
        public static string CanonicalType = "CanonicalType";
        public static string GameObjectComponent = "GameObjectComponent";
        public static string GameObject = "GameObject";
        public static string MemberName = "MemberName";
        public static string DisplayName = "DisplayName";
        public static string Description = "Description";
        public static string Browsable = "Browsable";
        public static string EditCondition = "EditCondition";
        public static string InlineEditToggle = "InlineEditToggle";
        public static string Max = "Max";
        public static string Min = "Min";
        public static string Step = "Step";
        public static string Default = "Default";
    }

    /// <summary>
    /// ScehmaのXMLを生成するクラス
    /// </summary>
    public class XmlGenerator
    {
        /// <summary>
        /// セットアップパラメータ
        /// </summary>
        public class SetupParameter 
        {
            /// <summary>
            /// クラスのリフレクション情報
            /// </summary>
            public IEnumerable<ClassReflection> ClassReflections { get; set; } = new List<ClassReflection>();

            /// <summary>
            /// エイリアスのリフレクション情報
            /// </summary>
            public IEnumerable<AliasReflection> AliasReflections { get; set; } = new List<AliasReflection>();

            /// <summary>
            /// enumのリフレクション情報
            /// </summary>
            public IEnumerable<EnumReflection> EnumReflections { get; set; } = new List<EnumReflection>();

        }

        public SetupParameter Param = new SetupParameter();

        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="param"></param>
        public XmlGenerator(SetupParameter param) 
        {
            Param = param;
        }

        /// <summary>
        /// XMLを生成
        /// </summary>
        /// <returns></returns>
        public string GenerateXml() 
        {
            var doc = new XmlDocument();
            XmlElement root = doc.CreateElement(XmlSchemaName.Schema);
            doc.AppendChild(root);

            {
                XmlElement aliasXml = doc.CreateElement(XmlSchemaName.AliasReflections);
                ParseAliasReflections(doc, aliasXml, Param.AliasReflections);
                root.AppendChild(aliasXml);
            }

            {
                XmlElement enumRootXml = doc.CreateElement(XmlSchemaName.EnumReflections);
                ParseEnumReflections(doc, enumRootXml, Param.EnumReflections);
                root.AppendChild(enumRootXml);
            }

            {
                XmlElement classRootXml = doc.CreateElement(XmlSchemaName.ClassReflections);
                ParseClassReflections(doc, classRootXml, Param.ClassReflections);
                root.AppendChild(classRootXml);
            }

            return ConvertString(doc);
        }

        /// <summary>
        /// XMLDocumentからstringに変換
        /// </summary>
        /// <param name="doc"></param>
        /// <returns></returns>
        public string ConvertString(XmlDocument doc) 
        {
            string xmlString;
            using (var stringWriter = new StringWriter())
            {
                using (var xmlTextWriter = new XmlTextWriter(stringWriter))
                {
                    xmlTextWriter.Formatting = Formatting.Indented;
                    doc.WriteTo(xmlTextWriter);
                    xmlTextWriter.Flush();
                    xmlString = stringWriter.GetStringBuilder().ToString();
                }
            }
            return xmlString;
        }

        /// <summary>
        /// Enumのリフレクションをパース
        /// </summary>
        /// <param name="doc"></param>
        /// <param name="root"></param>
        /// <param name="enumReflections"></param>
        /// <param name="parentClassName"></param>
        public void ParseEnumReflections(XmlDocument doc, XmlElement root, IEnumerable<EnumReflection> enumReflections, string parentClassName = "")
        {
            bool hasParentClass = !string.IsNullOrEmpty(parentClassName);
            foreach (var enumReflection in enumReflections)
            {
                XmlElement enumXml = doc.CreateElement(XmlSchemaName.Element);
                enumXml.SetAttribute(XmlSchemaName.Type, enumReflection.TypeName);
                enumXml.SetAttribute(XmlSchemaName.Namespace, enumReflection.Namespace);
                if (hasParentClass) 
                {
                    enumXml.SetAttribute(XmlSchemaName.ParentClass, parentClassName);
                }
                
                XmlElement enumItemRootXml = doc.CreateElement(XmlSchemaName.EnumItemReflections);
                foreach (var enumItemReflections in enumReflection.EnumItemReflections) 
                {
                    XmlElement enumItemXml = doc.CreateElement(XmlSchemaName.Element);
                    enumItemXml.SetAttribute(XmlSchemaName.Name, enumItemReflections.Name);
                    enumItemXml.SetAttribute(XmlSchemaName.Value, enumItemReflections.Value);
                    enumItemRootXml.AppendChild(enumItemXml);
                }
                enumXml.AppendChild(enumItemRootXml);
                root.AppendChild(enumXml);
            }
        }

        /// <summary>
        /// Enumのリフレクションをパース(クラス内に定義しているとき)
        /// </summary>
        /// <param name="doc"></param>
        /// <param name="root"></param>
        /// <param name="classReflections"></param>
        /// <param name="parentClassName"></param>
        public void ParseEnumReflections(XmlDocument doc, XmlElement root, IEnumerable<ClassReflection> classReflections, string parentClassName = "")
        {
            foreach (var classReflection in classReflections)
            {
                var name = GetClassName(classReflection.ClassName, parentClassName);
                ParseEnumReflections(doc, root, classReflection.NestedEnums, name);
            }

            foreach (var classReflection in classReflections) 
            {
                var name = GetClassName(classReflection.ClassName, parentClassName);
                ParseEnumReflections(doc, root, classReflection.NestedEnums, name);
            }
        }

        /// <summary>
        /// エイリアスをパース
        /// </summary>
        /// <param name="doc"></param>
        /// <param name="root"></param>
        /// <param name="aliasReflections"></param>
        public void ParseAliasReflections(XmlDocument doc, XmlElement root, IEnumerable<AliasReflection> aliasReflections) 
        {
            foreach (var aliasReflection in aliasReflections) 
            {
                XmlElement aliasXml = doc.CreateElement(XmlSchemaName.Element);
                aliasXml.SetAttribute(XmlSchemaName.Type, aliasReflection.TypeName);
                aliasXml.SetAttribute(XmlSchemaName.CanonicalType, aliasReflection.CanonicalTypeName);
                root.AppendChild(aliasXml);
            }            
        }

        /// <summary>
        /// クラスをパース
        /// </summary>
        /// <param name="doc"></param>
        /// <param name="root"></param>
        /// <param name="classReflections"></param>
        /// <param name="parentClassName"></param>
        public void ParseClassReflections(XmlDocument doc, XmlElement root, IEnumerable<ClassReflection> classReflections, string parentClassName = "")
        {
            bool hasParentClass = !string.IsNullOrEmpty(parentClassName);
            foreach (var classReflection in classReflections) 
            {
                XmlElement classXml = doc.CreateElement(XmlSchemaName.Element);
                classXml.SetAttribute(XmlSchemaName.Namespace, classReflection.Namespace);
                if (hasParentClass) 
                {
                    classXml.SetAttribute(XmlSchemaName.ParentClass, parentClassName);
                }
                classXml.SetAttribute(XmlSchemaName.ClassName, classReflection.ClassName);
                classXml.SetAttribute(XmlSchemaName.ClassType, classReflection.ClassType.ToString());
                classXml.SetAttribute(XmlSchemaName.GameObject, classReflection.IsGameObject);
                classXml.SetAttribute(XmlSchemaName.GameObjectComponent, classReflection.IsGameObjectComponent);
                if (classReflection.HasInheritClass())
                {
                    classXml.SetAttribute(XmlSchemaName.InheritClassName, classReflection.InheritClassName);
                }
                classXml.SetAttribute(XmlSchemaName.CanonicalType, classReflection.CanonicalTypeName);
                ParseFieldReflections(doc, classXml, classReflection.Fields);
                root.AppendChild(classXml);
            }

            foreach (var classReflection in classReflections) 
            {
                var name = GetClassName(classReflection.ClassName, parentClassName);
                ParseClassReflections(doc, root, classReflection.NestedClasses, name);
            }
        }

        /// <summary>
        /// フィールドをパース
        /// </summary>
        /// <param name="doc"></param>
        /// <param name="root"></param>
        /// <param name="fieldReflections"></param>
        public void ParseFieldReflections(XmlDocument doc, XmlElement root, IEnumerable<FieldReflection> fieldReflections) 
        {
            var fieldRootXml = doc.CreateElement(XmlSchemaName.FieldReflections);
            foreach (var fieldReflection in fieldReflections)
            {
                if (fieldReflection.CheckIgnore())
                {
                    continue;
                }

                var fieldXml = doc.CreateElement(XmlSchemaName.Element);
                fieldXml.SetAttribute(XmlSchemaName.MemberName, fieldReflection.MemberName);
                fieldXml.SetAttribute(XmlSchemaName.Type, fieldReflection.TypeName);
                foreach (var descriptor in fieldReflection.Descriptors) 
                {
                    fieldRootXml.SetAttribute(descriptor.Key, descriptor.Value);
                }

                fieldRootXml.AppendChild(fieldXml);
            }
            root.AppendChild(fieldRootXml);
        }

        /// <summary>
        /// クラス名を親クラスも考慮して取得
        /// </summary>
        /// <param name="className"></param>
        /// <param name="parentClassName"></param>
        /// <returns></returns>
        public string GetClassName(string className, string parentClassName)
        {
            var name = $"{className}";
            if (!string.IsNullOrEmpty(parentClassName))
            {
                name = $"{parentClassName}::{className}";
            }
            return name;
        }

    }
}
