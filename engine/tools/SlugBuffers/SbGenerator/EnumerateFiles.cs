using System;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;

namespace SchemaGenerator
{
    public class EnumerateFiles
    {
        static public IEnumerable<string> EnumerateTargetFiles(IEnumerable<string> targetFolders, IEnumerable<string> extensions)
        {
            var retFiles = new List<string>();
            foreach (var targetFolder in targetFolders)
            {
                if (!Directory.Exists(targetFolder))
                {
                    continue;
                }

                var files = Directory.GetFiles(targetFolder, "*.*", SearchOption.AllDirectories);
                foreach (var file in files)
                {
                    string folderPath = Path.GetDirectoryName(file) ?? string.Empty;
                    string extension = Path.GetExtension(file).ToLower();
                    if (extensions.Contains(extension))
                    {
                        retFiles.Add(file);
                    }
                }
            }
            return retFiles;
        }

        static public IEnumerable<string> EnumerateAnalyzeFiles(IEnumerable<string> targetFiles) 
        {
            var tmpFiles = new List<string>();
            foreach (var targetFile in targetFiles) 
            {
                foreach (var line in File.ReadLines(targetFile)) 
                {
                    if (line.Contains("SLUG_REFLECTION(true)")) 
                    {
                        tmpFiles.Add(targetFile);
                        break;
                    }
                }
            }

            var retFiles = new List<string>();
            foreach (var file in tmpFiles) 
            {
                var fileName = Path.GetFileName(file) ?? string.Empty;
                var folderName = Path.GetDirectoryName(file) ?? string.Empty;
                if (!string.IsNullOrEmpty(folderName))
                {
                    var tmpDir = GetTempDir();
                    folderName = folderName.Split("include")[1];
                    folderName = tmpDir + folderName;
                }
                else 
                {
                    folderName = GetTempDir();
                }

                if (!Directory.Exists(folderName)) 
                {
                    Directory.CreateDirectory(folderName);
                }

                var writeFilePath = Path.Combine(folderName, fileName);
                using (var reader = new StreamReader(file))
                using (var writer = new StreamWriter(writeFilePath))
                {
                    string? line;
                    while ((line = reader.ReadLine()) != null)
                    {
                        if (line.Contains("SLUG_REFLECTION(true)")) 
                        {
                            var tmp = line.Replace("SLUG_REFLECTION(true)", "/// @SLUG_GENERATE_REFLECTION");
                            writer.WriteLine(tmp);
                        }
                        else if (line.Contains("SLUG_PROPERTY(...)"))
                        {
                            writer.WriteLine(line);
                        }
                        else if (line.Contains("SLUG_PROPERTY"))
                        {
                            writer.WriteLine(ConvertComment(line));
                        }
                        else 
                        {
                            writer.WriteLine(line);
                        }
                    }
                }
                retFiles.Add(writeFilePath);
            }

            return retFiles;
        }

        static private string ConvertComment(string line) 
        {
            if (line == null)
            {
                return line ?? string.Empty;
            }

            var m = Regex.Match(line, @"\bSLUG_PROPERTY\s*\((.*)\)");
            if (!m.Success)
            {
                return line;
            }

            string inner = m.Groups[1].Value;

            var items = ParseItems(inner);

            var sb = new StringBuilder("///");

            foreach (var (key, value) in items)
            {
                sb.Append(" @")
                  .Append(key)
                  .Append("(")
                  .Append(value)
                  .Append(")");
            }

            return sb.ToString();
        }

        private static List<(string key, string value)> ParseItems(string text)
        {
            var result = new List<(string, string)>();

            foreach (var part in SplitTopLevel(text))
            {
                string s = part.Trim();
                if (s.Length == 0) continue;

                int eq = IndexOfTopLevelEquals(s);

                if (eq < 0)
                {
                    // 値なし → true
                    result.Add((s, "true"));
                    continue;
                }

                string key = s.Substring(0, eq).Trim();
                string value = NormalizeValue(s.Substring(eq + 1));

                // C/C++ 識別子（数値必須ではない）
                if (!Regex.IsMatch(key, @"^[A-Za-z_][A-Za-z0-9_]*$"))
                    continue;

                result.Add((key, value));
            }

            return result;
        }

        private static IEnumerable<string> SplitTopLevel(string s)
        {
            var sb = new StringBuilder();
            int paren = 0, angle = 0;
            bool inDq = false, inSq = false;

            for (int i = 0; i < s.Length; i++)
            {
                char c = s[i];

                if (!inSq && c == '"' && (i == 0 || s[i - 1] != '\\')) inDq = !inDq;
                else if (!inDq && c == '\'' && (i == 0 || s[i - 1] != '\\')) inSq = !inSq;

                if (!inDq && !inSq)
                {
                    if (c == '(') paren++;
                    else if (c == ')') paren--;
                    else if (c == '<') angle++;
                    else if (c == '>') angle--;

                    if (c == ',' && paren == 0 && angle == 0)
                    {
                        yield return sb.ToString();
                        sb.Clear();
                        continue;
                    }
                }

                sb.Append(c);
            }

            if (sb.Length > 0)
                yield return sb.ToString();
        }

        private static int IndexOfTopLevelEquals(string s)
        {
            int paren = 0, angle = 0;
            bool inDq = false, inSq = false;

            for (int i = 0; i < s.Length; i++)
            {
                char c = s[i];

                if (!inSq && c == '"' && (i == 0 || s[i - 1] != '\\')) inDq = !inDq;
                else if (!inDq && c == '\'' && (i == 0 || s[i - 1] != '\\')) inSq = !inSq;

                if (!inDq && !inSq)
                {
                    if (c == '(') paren++;
                    else if (c == ')') paren--;
                    else if (c == '<') angle++;
                    else if (c == '>') angle--;

                    if (c == '=' && paren == 0 && angle == 0)
                        return i;
                }
            }

            return -1;
        }

        private static string NormalizeValue(string v)
        {
            string val = v.Trim();

            // クォート除去
            if ((val.StartsWith("\"") && val.EndsWith("\"")) ||
                (val.StartsWith("'") && val.EndsWith("'")))
            {
                val = val.Substring(1, val.Length - 2);
            }

            // <...> or (...)
            if ((val.StartsWith("<") && val.EndsWith(">")) ||
                (val.StartsWith("(") && val.EndsWith(")")))
            {
                val = val.Substring(1, val.Length - 2).Trim();
            }

            return val;
        }

        static public string GetTempDir() 
        {
            var tmpPath = Path.GetTempPath();
            return Path.Combine(tmpPath, "slug_reflection");
        }

        static public IEnumerable<string> SplitString(string str, string split)
        {
            return str.Split(split).Where(x => !string.IsNullOrEmpty(x)).ToList();
        }
    }
}
