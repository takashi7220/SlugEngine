using ClangSharp;
using ClangSharp.Interop;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using static System.Runtime.CompilerServices.RuntimeHelpers;
using SbReflectionLib.Reflections;

namespace SbClangLib
{
    public class ReflectionGenerator
    {
        /// <summary>
        /// インクルード対象のフォルダパス
        /// </summary>
        public IEnumerable<string> IncludeFolders { get; set; } = new List<string>();

        /// <summary>
        /// 解析対象のファイルパス
        /// </summary>
        public IEnumerable<string> AnalyzeFiles { get; set; } = new List<string>();

        /// <summary>
        /// CPPのバージョン
        /// </summary>
        public string CppVersion { get; set; }

        /// <summary>
        /// リフレクション情報を生成するためのビルダー
        /// </summary>
        public ReflectionBuilder ReflectionBuilder { get; set; } = new ReflectionBuilder();

        /// <summary>
        /// コンストラクタ
        /// </summary>
        /// <param name="param"></param>
        public ReflectionGenerator(IEnumerable<string> analyzeFiles, IEnumerable<string> includeFolders, string cppVersion) 
        {
            IncludeFolders = includeFolders;
            AnalyzeFiles = analyzeFiles;
            CppVersion = cppVersion;
        }

        /// <summary>
        /// スキーマを生成
        /// </summary>
        /// <returns></returns>
        public unsafe ReflectionData Generate(Action<int, int, string>? callback)
        {
            var clangIndex = CXIndex.Create();
            var args = GetCommandArgs();

            for (var i = 0; i < AnalyzeFiles.Count(); i++)
            {
                var analyzeFile = AnalyzeFiles.ElementAt(i);
                var errorCode = CXTranslationUnit.TryParse(
                    clangIndex,
                    analyzeFile,
                    args,
                    Array.Empty<CXUnsavedFile>(),
                    CXTranslationUnit_Flags.CXTranslationUnit_None,
                    out var translationUnit
                    );

                if (callback != null)
                {
                    callback(i, AnalyzeFiles.Count(), analyzeFile);
                }

                if (errorCode != CXErrorCode.CXError_Success) 
                {
                    throw new Exception($"CXTranslationUnit.TryParse failed, errorCode = {errorCode.ToString()}");
                }

                translationUnit.Cursor.VisitChildren(this.VisitChildrenCb, new CXClientData());
                clang.disposeTranslationUnit(translationUnit);
            }
            clang.disposeIndex(clangIndex);

            return ReflectionBuilder.GetData();
        }

        /// <summary>
        /// clangに渡すためのコマンドライン引数を取得
        /// </summary>
        /// <returns></returns>
        private unsafe string[] GetCommandArgs()
        {
            var commandLineArgs = new List<string>();
            commandLineArgs.Add("-x");
            commandLineArgs.Add("c++");
            commandLineArgs.Add(CppVersion);

            foreach (var includeFolder in IncludeFolders)
            {
                var tmp = includeFolder.Replace("\\", "/");
                commandLineArgs.Add($"-I{tmp}");
            }

            return commandLineArgs.ToArray();
        }

        /// <summary>
        /// 解析を行うためのVisitor関数
        /// </summary>
        /// <param name="cursor"></param>
        /// <param name="parent"></param>
        /// <param name="client_data"></param>
        /// <returns></returns>
        private unsafe CXChildVisitResult VisitChildrenCb(CXCursor cursor, CXCursor parent, void* client_data)
        {
            cursor.Location.GetFileLocation(out var cxVisitFilePath, out var line, out var column, out var offset);

            // 解析対象の種類によって分岐
            if (cursor.Kind == CXCursorKind.CXCursor_ClassDecl
                || cursor.Kind == CXCursorKind.CXCursor_StructDecl
                || cursor.Kind == CXCursorKind.CXCursor_UnionDecl
                || cursor.Kind == CXCursorKind.CXCursor_ClassTemplatePartialSpecialization)
            {
                AnalyzeClass(cursor);
            }
            else if (cursor.Kind == CXCursorKind.CXCursor_Namespace)
            {
                AnalyzeNamespace(cursor);
            }
            else if (cursor.Kind == CXCursorKind.CXCursor_FieldDecl)
            {
                AnalyzeField(cursor);
            }
            else if (cursor.Kind == CXCursorKind.CXCursor_TypeAliasDecl)
            {
                AnalyzeAlias(cursor);
            }
            else if (cursor.Kind == CXCursorKind.CXCursor_CXXBaseSpecifier)
            {
                AnalyzeInherit(cursor);
            }
            else if (cursor.Kind == CXCursorKind.CXCursor_EnumDecl) 
            {
                AnalyzeEnum(cursor);
            }
            else if (cursor.Kind == CXCursorKind.CXCursor_EnumConstantDecl)
            {
                AnalyzeEnumValue(cursor);
            }
            return CXChildVisitResult.CXChildVisit_Continue;
        }

        /// <summary>
        /// クラスを解析
        /// </summary>
        /// <param name="cursor"></param>
        private unsafe void AnalyzeClass(CXCursor cursor)
        {
            if (CheckSerializeTarget(cursor))
            {
                var classType = ClassType.Class;
                switch (cursor.Kind)
                {
                    case CXCursorKind.CXCursor_ClassDecl:
                        classType = ClassType.Class;
                        break;
                    case CXCursorKind.CXCursor_UnionDecl:
                        classType = ClassType.Union;
                        break;
                    case CXCursorKind.CXCursor_StructDecl:
                        classType = ClassType.Struct;
                        break;
                    case CXCursorKind.CXCursor_ClassTemplatePartialSpecialization:
                        classType = ClassType.Template;
                        break;
                }

                var cls = new ClassReflection
                {
                    ClassType = classType,
                    ClassName = cursor.Spelling.ToString(),
                    CanonicalTypeName = cursor.Type.CanonicalType.ToString(),
                    Namespace = ReflectionBuilder.CurrentNamespace
                };

                cls.IsGameObject = CheckGameObject(cursor) ? "true" : "false";
                cls.IsGameObjectComponent = CheckGameObjectComponent(cursor) ? "true" : "false";

                ReflectionBuilder.BeginClass(cls);
                cursor.VisitChildren(this.VisitChildrenCb, new CXClientData());
                ReflectionBuilder.EndClass();
            }
        }

        /// <summary>
        /// Namespaceを解析
        /// </summary>
        /// <param name="cursor"></param>
        private unsafe void AnalyzeNamespace(CXCursor cursor) 
        {
            ReflectionBuilder.BegineNamespace(cursor.Spelling.ToString());
            cursor.VisitChildren(this.VisitChildrenCb, new CXClientData());
            ReflectionBuilder.EndNamespace(cursor.Spelling.ToString());
        }

        /// <summary>
        /// Fieldを解析
        /// </summary>
        /// <param name="cursor"></param>
        private unsafe void AnalyzeField(CXCursor cursor) 
        {
            var field = new FieldReflection();
            field.MemberName = cursor.Spelling.ToString();
            field.TypeName = cursor.Type.Spelling.ToString();
            var tmp = cursor.Type.CanonicalType.Spelling.ToString();
            var rawComment = clang.Cursor_getRawCommentText(cursor).ToString();
            if (!string.IsNullOrWhiteSpace(rawComment))
            {
                field.SetupFieldComment(rawComment.Trim());
            }
            ReflectionBuilder.PushField(field);
        }

        /// <summary>
        /// エイリアスを解析
        /// </summary>
        /// <param name="cursor"></param>
        private unsafe void AnalyzeAlias(CXCursor cursor) 
        {
            var alias = new AliasReflection();
            alias.TypeName = cursor.Spelling.ToString();
            alias.CanonicalTypeName = cursor.Type.CanonicalType.ToString();
            ReflectionBuilder.PushAlias(alias);
        }

        /// <summary>
        /// 継承関係を解析
        /// </summary>
        /// <param name="cursor"></param>
        private unsafe void AnalyzeInherit(CXCursor cursor) 
        {
            ReflectionBuilder.PushInherit(cursor.Spelling.ToString());
        }

        /// <summary>
        /// Enumを解析
        /// </summary>
        /// <param name="cursor"></param>
        private unsafe void AnalyzeEnum(CXCursor cursor) 
        {
            if (CheckSerializeTarget(cursor))
            {
                var enumReflection = new EnumReflection();
                enumReflection.TypeName = cursor.Type.Spelling.ToString();
                enumReflection.Namespace = ReflectionBuilder.CurrentNamespace;
                ReflectionBuilder.BeginEnum(enumReflection);
                cursor.VisitChildren(this.VisitChildrenCb, new CXClientData());
                ReflectionBuilder.EndEnum();
            }

        }

        /// <summary>
        /// Enumの値を解析
        /// </summary>
        /// <param name="cursor"></param>
        private unsafe void AnalyzeEnumValue(CXCursor cursor)
        {
            var enumItemReflection = new EnumItemReflection();
            enumItemReflection.Name = cursor.Spelling.ToString();
            enumItemReflection.Value = clang.getEnumConstantDeclValue(cursor).ToString();
            ReflectionBuilder.PushEnumValue(enumItemReflection);
        }

        /// <summary>
        /// シリアライズ対象か確認
        /// </summary>
        /// <param name="cursor"></param>
        /// <returns></returns>
        private unsafe bool CheckSerializeTarget(CXCursor cursor) 
        {
            var rawComment = clang.Cursor_getRawCommentText(cursor).ToString();
            bool serializeTarget = false;
            if (!string.IsNullOrWhiteSpace(rawComment))
            {
                serializeTarget = rawComment.Contains("@SLUG_GENERATE_REFLECTION");
            }

            // definition じゃなければ「前方宣言 or 宣言のみ」の扱いにして除外
            if (clang.isCursorDefinition(cursor) == 0) 
            {
                serializeTarget = false;
            }
            return serializeTarget;
        }

        /// <summary>
        /// GameObjectComponentか確認
        /// </summary>
        /// <param name="cursor"></param>
        /// <returns></returns>
        private unsafe bool CheckGameObjectComponent(CXCursor cursor)
        {
            var rawComment = clang.Cursor_getRawCommentText(cursor).ToString();
            bool serializeTarget = false;
            if (!string.IsNullOrWhiteSpace(rawComment))
            {
                serializeTarget = rawComment.Contains("@SLUG_GAMEOBJECT_COMPONENT");
            }
            return serializeTarget;
        }

        /// <summary>
        /// GameObjectか確認
        /// </summary>
        /// <param name="cursor"></param>
        /// <returns></returns>
        private unsafe bool CheckGameObject(CXCursor cursor)
        {
            var rawComment = clang.Cursor_getRawCommentText(cursor).ToString();
            bool serializeTarget = false;
            if (!string.IsNullOrWhiteSpace(rawComment))
            {
                serializeTarget = rawComment.Contains("@SLUG_GAMEOBJECT");
            }
            return serializeTarget;
        }
    }
}
