using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SbReflectionLib.Reflections
{
    /// <summary>
    /// クラスの種類
    /// </summary>
    public enum ClassType
    {
        Struct,
        Class,
        Union,
        Template
    }

    /// <summary>
    /// クラスのリフレクション処理
    /// </summary>
    public class ClassReflection
    {
        /// <summary>
        /// 名前空間
        /// </summary>
        public string Namespace { get; set; } = string.Empty;

        /// <summary>
        /// クラス名
        /// </summary>
        public string ClassName { get; set; } = string.Empty;

        /// <summary>
        /// クラスの種類
        /// </summary>
        public ClassType ClassType { get; set; } = ClassType.Class;

        /// <summary>
        /// 継承元のクラス名
        /// </summary>
        public string InheritClassName { get; set; } = string.Empty;

        /// <summary>
        /// 正規のタイプ名
        /// </summary>
        public string CanonicalTypeName { get; set; } = string.Empty;

        /// <summary>
        /// ゲームオブジェクトコンポーネントか
        /// </summary>
        public string IsGameObjectComponent { get; set; } = string.Empty;

        /// <summary>
        /// ゲームオブジェクトか
        /// </summary>
        public string IsGameObject { get; set; } = string.Empty;

        /// <summary>
        /// 内部クラスのリフレクション情報
        /// </summary>
        public List<ClassReflection> NestedClasses { get; set; } = new List<ClassReflection>();

        /// <summary>
        /// 内部enumのリフレクション情報
        /// </summary>
        public List<EnumReflection> NestedEnums { get; set; } = new List<EnumReflection>();

        /// <summary>
        /// フィールド情報
        /// </summary>
        public List<FieldReflection> Fields { get; set; } = new List<FieldReflection>();

        /// <summary>
        /// クローン
        /// </summary>
        /// <returns></returns>
        public ClassReflection Clone()
        {
            var clone = new ClassReflection();
            clone.ClassName = ClassName;
            clone.Namespace = Namespace;
            foreach (var member in Fields)
            {
                clone.Fields.Add(member.Clone());
            }
            return clone;
        }

        /// <summary>
        /// 等価判定
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public bool CustomEquals(object? obj)
        {
            var v1 = obj as ClassReflection;
            if (v1 == null) 
            {
                return false;
            }

            return Namespace.Equals(v1.Namespace)
                && ClassName.Equals(v1.ClassName)
                && CanonicalTypeName.Equals(v1.CanonicalTypeName)
                && ClassType.Equals(v1.ClassType);
        }

        /// <summary>
        /// 継承元を保持しているか
        /// </summary>
        /// <returns></returns>
        public bool HasInheritClass() 
        {
            return !string.IsNullOrEmpty(InheritClassName);
        }
    }
}
