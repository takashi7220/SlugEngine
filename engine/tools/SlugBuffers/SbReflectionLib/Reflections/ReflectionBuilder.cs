using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace SbReflectionLib.Reflections
{
    public class ReflectionData 
    {
        public List<AliasReflection> AliasReflections { get; set; } = new List<AliasReflection>();

        public List<ClassReflection> ClassReflections { get; set; } = new List<ClassReflection>();

        public List<EnumReflection> EnumReflections { get; set; } = new List<EnumReflection>();
    }

    public class ReflectionBuilder
    {

        public string CurrentNamespace { get; set; } = string.Empty;

        private EnumReflection? m_CurrentEnumReflection = null;

        private readonly Stack<ClassReflection> m_ClassStack = new Stack<ClassReflection>();

        public List<AliasReflection> AliasReflections { get; } = new List<AliasReflection>();

        public List<ClassReflection> ClassReflections { get; } = new List<ClassReflection>();

        public List<EnumReflection> EnumReflections { get; set; } = new List<EnumReflection>();

        public void BegineNamespace(string ns)
        {
            CurrentNamespace += "::" + ns;
        }

        public void EndNamespace(string ns)
        {
            var str = "::" + ns;
            CurrentNamespace = CurrentNamespace.Replace(str, "");
        }

        public void BeginClass(ClassReflection cls)
        {
            if (m_ClassStack.Count > 0)
            {
                m_ClassStack.Peek().NestedClasses.Add(cls);
            }

            m_ClassStack.Push(cls);
        }

        public void EndClass()
        {
            var tmp = m_ClassStack.Pop();
            if (m_ClassStack.Count() == 0) 
            {
                if (!ClassReflections.Where(x => x.CustomEquals(tmp)).Any())
                {
                    ClassReflections.Add(tmp);
                }
            }
        }

        public void BeginEnum(EnumReflection enumReflection) 
        {
            if (m_ClassStack.Count > 0)
            {
                m_ClassStack.Peek().NestedEnums.Add(enumReflection);
            }
            else 
            {
                if (!EnumReflections.Where(x => x.CustomEquals(enumReflection)).Any()) 
                {
                    EnumReflections.Add(enumReflection);
                }
            }
            m_CurrentEnumReflection = enumReflection;
        }

        public void EndEnum()
        {
            m_CurrentEnumReflection = null;
        }

        public void PushEnumValue(EnumItemReflection enumItemReflection) 
        {
            m_CurrentEnumReflection?.EnumItemReflections.Add(enumItemReflection);
        }

        public void PushField(FieldReflection field) 
        {
            m_ClassStack.Peek().Fields.Add(field);
        }

        public void PushAlias(AliasReflection alias) 
        {
            if (!AliasReflections.Where(x => x.CustomEquals(alias)).Any())
            {
                AliasReflections.Add(alias);
            }
        }

        public void PushInherit(string inheritName) 
        {
            m_ClassStack.Peek().InheritClassName = inheritName;
        }

        public ReflectionData GetData() 
        { 
            var ret = new ReflectionData();
            ret.AliasReflections = AliasReflections;
            ret.ClassReflections = ClassReflections;
            ret.EnumReflections = EnumReflections;
            return ret;
        }
    }
}
