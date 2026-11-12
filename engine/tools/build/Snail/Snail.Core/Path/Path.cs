using System;
using System.Collections.Generic;
using System.Text;

namespace Snail.Core.Path
{
    public static class Path
    {
        public static string Combine(params string[] paths)
        {
            return System.IO.Path.Combine(paths);
        }

        public static string GetFullPath(string path)
        {
            return System.IO.Path.GetFullPath(path);
        }

        public static string GetRelativePath(string relativeTo, string path)
        {
            return System.IO.Path.GetRelativePath(relativeTo, path);
        }

        public static string GetDirectoryName(string path)
        {
            return System.IO.Path.GetDirectoryName(path) ?? string.Empty;
        }

        public static string GetFileName(string path)
        {
            return System.IO.Path.GetFileName(path);
        }

        public static string GetFileNameWithoutExtension(string path)
        {
            return System.IO.Path.GetFileNameWithoutExtension(path);
        }

        public static string GetExtension(string path)
        {
            return System.IO.Path.GetExtension(path);
        }

        public static string ChangeExtension(string path, string? extension)
        {
            return System.IO.Path.ChangeExtension(path, extension);
        }

        public static string GetTempFileName()
        {
            return System.IO.Path.GetTempFileName();
        }

        public static string GetTempPath()
        {
            return Path.Combine(System.IO.Path.GetTempPath(), "slug");
        }

        public static string GetRandomFileName()
        {
            return System.IO.Path.GetRandomFileName();
        }

        public static string GetInvalidFileNameChars()
        {
            return new string(System.IO.Path.GetInvalidFileNameChars());
        }

        public static string GetInvalidPathChars()
        {
            return new string(System.IO.Path.GetInvalidPathChars());
        }
    }
}
