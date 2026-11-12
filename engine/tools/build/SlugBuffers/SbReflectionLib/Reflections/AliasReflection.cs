using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SbReflectionLib.Reflections
{
    /// <summary>
    /// エイリアス宣言のリフレクション情報
    /// </summary>
    public class AliasReflection
    {
        /// <summary>
        /// タイプ名
        /// </summary>
        public string TypeName { get; set; } = string.Empty;

        /// <summary>
        /// 宣言先のタイプ名
        /// </summary>
        public string CanonicalTypeName { get; set; } = string.Empty;

        /// <summary>
        /// 等価判定
        /// </summary>
        /// <param name="obj"></param>
        /// <returns></returns>
        public bool CustomEquals(object? obj) 
        {
            var v1 = obj as AliasReflection;
            if (v1 == null) 
            {
                return false;
            }
            return TypeName.Equals(v1.TypeName) && CanonicalTypeName.Equals(v1.CanonicalTypeName);
        }
    }
}
