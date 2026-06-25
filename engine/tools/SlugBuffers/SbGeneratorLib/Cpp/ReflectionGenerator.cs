using SbReflectionLib.Reflections;
using SbClangLib;

namespace SchemaGeneratorLib.Cpp
{
    public class ReflectionGenerator
    {

        /// <summary>
        /// セットアップ用のパラメータ
        /// </summary>
        public class SetupParameter
        {
            /// <summary>
            /// 解析対象のファイルパス
            /// </summary>
            public IEnumerable<string> analyzeFiles { get; set; } = new List<string>();

            /// <summary>
            /// インクルード対象のフォルダ
            /// </summary>
            public IEnumerable<string> includeFolders { get; set; } = new List<string>();

            /// <summary>
            /// CPPのversion
            /// </summary>
            public string version { get; set; } = "";

            /// <summary>
            /// 解析時のコールバック
            /// </summary>
            public Action<int, int, string>? AnalyzeCallBack = null;
        }

        /// <summary>
        /// Schemaを生成
        /// </summary>
        /// <param name="param"></param>
        /// <returns></returns>
        public static ReflectionData Generate(SetupParameter param)
        {
            var internalGenerator = new SbClangLib.ReflectionGenerator(param.analyzeFiles, param.includeFolders, param.version);
            return internalGenerator.Generate(param.AnalyzeCallBack);
        }
    }
}
