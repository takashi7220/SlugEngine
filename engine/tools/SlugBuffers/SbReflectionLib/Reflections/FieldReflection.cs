using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace SbReflectionLib.Reflections
{
    /// <summary>
    /// フィールドのリフレクション情報
    /// </summary>
    public class FieldReflection
    {
        /// <summary>
        /// タイプ名
        /// </summary>
        public string TypeName { get; set; } = string.Empty;

        /// <summary>
        /// メンバー名
        /// </summary>
        public string MemberName { get; set; } = string.Empty;



        public Dictionary<string, string> Descriptors { get; set; } = new Dictionary<string, string>();

        public FieldReflection Clone()
        {
            var clone = new FieldReflection();
            foreach (var property in clone.GetType().GetProperties())
            {
                var value = property.GetValue(this);
                property.SetValue(clone, value);
            }
            return clone;
        }

        public void SetupFieldComment(string comment)
        {
            var comments = comment.Split("@");
            var type = typeof(FieldReflection);
            foreach (var str in comments)
            {
                var parts = str.Split('(');
                if (parts.Count() == 2)
                {
                    string key = parts[0];
                    string value = parts[1].Replace(")", "");
                    if (!Descriptors.ContainsKey(key)) 
                    {
                        Descriptors.Add(key, value);
                    }
                }
            }
        }

        public bool CheckIgnore() 
        {
            string key = "Ignore";
            if (Descriptors.ContainsKey(key) && !string.IsNullOrEmpty(Descriptors[key]))
            {
                return true;
            }
            return false;
        }
    }
}
