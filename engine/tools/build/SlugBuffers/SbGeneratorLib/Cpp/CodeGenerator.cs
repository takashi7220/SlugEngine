using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SbGeneratorLib.Cpp
{
    public class CodeGenerator
    {
        public class SetupParameter 
        {
            public List<FileDefinition> fileDefinitions = new List<FileDefinition>(); 
        }


        public class Result 
        {
            public class Item
            {
                public string fileName { get; set; } = string.Empty;

                public string data { get; set; } = string.Empty;
            }

            public List<Item> items { get; set; } = new List<Item>();
        }

        public Result Generate(SetupParameter param) 
        {
            var ret = new Result();
            foreach (var fileDefinition in param.fileDefinitions) 
            {
                var fileName = $"{fileDefinition.Name}{fileDefinition.Extension}";
                var data = fileDefinition.GenerateCode();
                ret.items.Add(new Result.Item
                {
                    fileName = fileName,
                    data = data
                });
            }
            return ret;
        }

        public void ParseIncludeDefinitions() 
        {
        
        }
    }
}
