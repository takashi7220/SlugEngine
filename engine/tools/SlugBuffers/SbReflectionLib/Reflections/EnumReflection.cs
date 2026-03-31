using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SbReflectionLib.Reflections
{
    /// <summary>
    /// Enumの要素のリフレクション
    /// </summary>
    public class EnumItemReflection 
    {
        /// <summary>
        /// 名前
        /// </summary>
        public string Name { get; set; } = string.Empty;

        /// <summary>
        /// 値
        /// </summary>
        public string Value { get; set; } = string.Empty;
    }

    /// <summary>
    /// Enumのリフレクション情報
    /// </summary>
    public class EnumReflection
    {
        /// <summary>
        /// 名前空間
        /// </summary>
        public string Namespace { get; set; } = string.Empty;

        /// <summary>
        /// タイプ名
        /// </summary>
        public string TypeName { get; set; } = string.Empty;

        /// <summary>
        /// 要素のリフレクション情報
        /// </summary>
        public List<EnumItemReflection> EnumItemReflections { get; set; } = new List<EnumItemReflection>();

        /// <summary>
        /// 等価判定
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public bool CustomEquals(object? obj) 
        {
            var er = obj as EnumReflection;
            if (er == null) 
            {
                return false;
            }
            return Namespace == er.Namespace && TypeName == er.TypeName;
        }
    }
}
