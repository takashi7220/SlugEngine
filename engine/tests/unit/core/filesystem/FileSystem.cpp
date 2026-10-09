#include <gtest/gtest.h>
#include "core/filesystem/FileSystem.hpp"
#include "core/io/Path.hpp"
#include "core/container/String.hpp"
#include "core/container/Vector.hpp"

#include <algorithm>
#include <filesystem>

using namespace slug::core;

namespace
{

// テストごとに固有の作業ディレクトリを用意し、終了時に片付ける。
class FileSystemFixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        const ::testing::TestInfo* info = ::testing::UnitTest::GetInstance()->current_test_info();

        std::filesystem::path root = std::filesystem::temp_directory_path();
        root /= "slug_core_unit";
        root /= info->name();

        std::error_code ec;
        std::filesystem::remove_all(root, ec);
        std::filesystem::create_directories(root, ec);

        m_root = String(root.string().c_str());
    }

    void TearDown() override
    {
        std::error_code ec;
        std::filesystem::remove_all(std::filesystem::path(m_root.c_str()), ec);
    }

    String PathIn(const char* relative) const
    {
        return m_root + "/" + relative;
    }

    String m_root;
};

bool Contains(const TVector<String>& files, const String& fragment)
{
    return std::any_of(files.begin(), files.end(), [&fragment](const String& path)
    {
        return path.find(fragment) != String::npos;
    });
}

} // namespace

//------------------------------------------------------------------------------
// パス文字列の分解
//------------------------------------------------------------------------------
TEST(FileSystemPathTest, GetExtensionReturnsDotPrefixedExtension)
{
    EXPECT_EQ(FileSystem::GetExtension("dir/file.txt"), ".txt");
    EXPECT_EQ(FileSystem::GetExtension("file.tar.gz"), ".gz");
    EXPECT_EQ(FileSystem::GetExtension("dir/noextension"), "");
    EXPECT_EQ(FileSystem::GetExtension(""), "");
}

TEST(FileSystemPathTest, GetFilenameWithAndWithoutExtension)
{
    EXPECT_EQ(FileSystem::GetFilename("dir/sub/file.txt"), "file.txt");
    EXPECT_EQ(FileSystem::GetFilename("dir/sub/file.txt", false), "file");
    EXPECT_EQ(FileSystem::GetFilename("file.txt"), "file.txt");
    EXPECT_EQ(FileSystem::GetFilename("file.txt", false), "file");
}

TEST(FileSystemPathTest, GetDirectoryReturnsParentPath)
{
    EXPECT_EQ(FileSystem::GetDirectory("dir/sub/file.txt"), "dir/sub");
    EXPECT_EQ(FileSystem::GetDirectory("file.txt"), "");
}

//------------------------------------------------------------------------------
// TryGetLines（改行コードの扱い）
//------------------------------------------------------------------------------
TEST(FileSystemLinesTest, SplitsOnLineFeed)
{
    TVector<String> lines;
    ASSERT_TRUE(FileSystem::TryGetLines(String("a\nb\nc"), lines));

    ASSERT_EQ(lines.size(), 3u);
    EXPECT_EQ(lines[0], "a");
    EXPECT_EQ(lines[1], "b");
    EXPECT_EQ(lines[2], "c");
}

TEST(FileSystemLinesTest, SplitsOnCarriageReturnLineFeed)
{
    TVector<String> lines;
    ASSERT_TRUE(FileSystem::TryGetLines(String("a\r\nb\r\nc"), lines));

    ASSERT_EQ(lines.size(), 3u);
    EXPECT_EQ(lines[0], "a");
    EXPECT_EQ(lines[1], "b");
    EXPECT_EQ(lines[2], "c");
}

TEST(FileSystemLinesTest, SplitsOnLoneCarriageReturn)
{
    TVector<String> lines;
    ASSERT_TRUE(FileSystem::TryGetLines(String("a\rb"), lines));

    ASSERT_EQ(lines.size(), 2u);
    EXPECT_EQ(lines[0], "a");
    EXPECT_EQ(lines[1], "b");
}

TEST(FileSystemLinesTest, TrailingNewlineDoesNotProduceEmptyLine)
{
    TVector<String> lines;
    ASSERT_TRUE(FileSystem::TryGetLines(String("a\nb\n"), lines));

    ASSERT_EQ(lines.size(), 2u);
    EXPECT_EQ(lines[1], "b");
}

TEST(FileSystemLinesTest, EmptyLinesInMiddleArePreserved)
{
    TVector<String> lines;
    ASSERT_TRUE(FileSystem::TryGetLines(String("a\n\nb"), lines));

    ASSERT_EQ(lines.size(), 3u);
    EXPECT_EQ(lines[0], "a");
    EXPECT_EQ(lines[1], "");
    EXPECT_EQ(lines[2], "b");
}

TEST(FileSystemLinesTest, EmptyTextProducesNoLines)
{
    TVector<String> lines;
    lines.push_back("stale");

    ASSERT_TRUE(FileSystem::TryGetLines(String(""), lines));
    EXPECT_TRUE(lines.empty());
}

//------------------------------------------------------------------------------
// 実ファイル入出力
//------------------------------------------------------------------------------
TEST_F(FileSystemFixture, WriteTextAndReadTextRoundTrip)
{
    const String path = PathIn("text/sample.txt");
    const String content = "line1\nline2\n";

    ASSERT_TRUE(FileSystem::WriteText(path.c_str(), content));

    String readBack;
    ASSERT_TRUE(FileSystem::ReadText(path.c_str(), readBack));
    EXPECT_EQ(readBack, content);

    EXPECT_EQ(FileSystem::ReadText(path.c_str()), content);
}

TEST_F(FileSystemFixture, WriteCreatesMissingDirectories)
{
    const String path = PathIn("a/b/c/deep.txt");

    ASSERT_TRUE(FileSystem::WriteText(path.c_str(), "ok"));

    EXPECT_TRUE(FileSystem::CheckDirectoryExists(PathIn("a/b/c").c_str()));
    EXPECT_TRUE(FileSystem::CheckFileExists(path.c_str()));
}

TEST_F(FileSystemFixture, WriteBinaryAndReadBlobRoundTrip)
{
    const String path = PathIn("bin/data.bin");

    TVector<uint8_t> payload;
    for (int i = 0; i < 256; ++i)
    {
        payload.push_back(static_cast<uint8_t>(i));
    }

    ASSERT_TRUE(FileSystem::Write(path.c_str(), payload.size(), payload.data()));

    FileBlobPtr blob = FileSystem::Read(path.c_str());
    ASSERT_NE(blob.get(), nullptr);

    const TVector<uint8_t>& data = blob->GetData();
    ASSERT_EQ(data.size(), payload.size());
    EXPECT_EQ(memcmp(data.data(), payload.data(), payload.size()), 0);
}

TEST_F(FileSystemFixture, WriteBlobRoundTrip)
{
    const String path = PathIn("bin/blob.bin");

    TVector<uint8_t> payload { 1, 2, 3, 4, 5 };
    FileBlobPtr source = MakeReference<FileBlob>(payload);
    ASSERT_NE(source.get(), nullptr);

    ASSERT_TRUE(FileSystem::Write(path.c_str(), source));

    FileBlobPtr blob = FileSystem::Read(path.c_str());
    ASSERT_NE(blob.get(), nullptr);
    ASSERT_EQ(blob->GetData().size(), payload.size());
    EXPECT_EQ(blob->GetData()[4], 5);
}

TEST_F(FileSystemFixture, ReadMissingFileFails)
{
    const String path = PathIn("does_not_exist.txt");

    String text;
    EXPECT_FALSE(FileSystem::ReadText(path.c_str(), text));
    EXPECT_FALSE(FileSystem::CheckFileExists(path.c_str()));
}

TEST_F(FileSystemFixture, TryGetLinesReadsFromFile)
{
    const String path = PathIn("lines.txt");
    ASSERT_TRUE(FileSystem::WriteText(path.c_str(), "alpha\nbeta\ngamma"));

    TVector<String> lines;
    ASSERT_TRUE(FileSystem::TryGetLines(StringView(path.c_str()), lines));

    ASSERT_EQ(lines.size(), 3u);
    EXPECT_EQ(lines[0], "alpha");
    EXPECT_EQ(lines[2], "gamma");
}

TEST_F(FileSystemFixture, TryGetLinesOnMissingFileFails)
{
    TVector<String> lines;
    const String missing = PathIn("missing.txt");
        EXPECT_FALSE(FileSystem::TryGetLines(StringView(missing.c_str()), lines));
}

//------------------------------------------------------------------------------
// ディレクトリ・存在確認
//------------------------------------------------------------------------------
TEST_F(FileSystemFixture, MakeDirectoryCreatesNestedDirectories)
{
    const String dir = PathIn("x/y/z");

    ASSERT_TRUE(FileSystem::MakeDirectory(dir.c_str()));

    EXPECT_TRUE(FileSystem::CheckDirectoryExists(dir.c_str()));
    // CheckFileExists は「パスが存在するか」を見るため、ディレクトリでも true になる。
    EXPECT_TRUE(FileSystem::CheckFileExists(dir.c_str()));
    EXPECT_FALSE(FileSystem::CheckDirectoryExists(PathIn("x/y/missing").c_str()));
}

// CheckFileExists がディレクトリ判定に委譲していると、通常ファイルで false になる。
TEST_F(FileSystemFixture, CheckFileExistsReportsRegularFile)
{
    const String dir = PathIn("entries");
    const String file = PathIn("entries/file.txt");

    ASSERT_TRUE(FileSystem::MakeDirectory(dir.c_str()));
    ASSERT_TRUE(FileSystem::WriteText(file.c_str(), "content"));

    EXPECT_TRUE(FileSystem::CheckFileExists(file.c_str()));
    EXPECT_FALSE(FileSystem::CheckDirectoryExists(file.c_str()));

    EXPECT_TRUE(FileSystem::CheckDirectoryExists(dir.c_str()));
    EXPECT_FALSE(FileSystem::CheckFileExists(PathIn("entries/missing.txt").c_str()));
}

//------------------------------------------------------------------------------
// ファイル列挙
//------------------------------------------------------------------------------
TEST_F(FileSystemFixture, EnumrateFilesFindsAllFiles)
{
    ASSERT_TRUE(FileSystem::WriteText(PathIn("one.txt").c_str(), "1"));
    ASSERT_TRUE(FileSystem::WriteText(PathIn("two.log").c_str(), "2"));
    ASSERT_TRUE(FileSystem::WriteText(PathIn("nested/three.txt").c_str(), "3"));

    TVector<String> files;
    const uint32_t count = FileSystem::EnumrateFiles(m_root.c_str(), files);

    EXPECT_EQ(count, files.size());
    EXPECT_GE(files.size(), 3u);
    EXPECT_TRUE(Contains(files, "one.txt"));
    EXPECT_TRUE(Contains(files, "two.log"));
    EXPECT_TRUE(Contains(files, "three.txt"));
}

TEST_F(FileSystemFixture, EnumrateFilesByNameFiltersResults)
{
    ASSERT_TRUE(FileSystem::WriteText(PathIn("target.txt").c_str(), "1"));
    ASSERT_TRUE(FileSystem::WriteText(PathIn("other.txt").c_str(), "2"));

    TVector<String> files;
    FileSystem::EnumrateFiles(m_root.c_str(), "target.txt", files);

    EXPECT_TRUE(Contains(files, "target.txt"));
    EXPECT_FALSE(Contains(files, "other.txt"));
}

TEST_F(FileSystemFixture, EnumrateFilesExtensionFiltersResults)
{
    ASSERT_TRUE(FileSystem::WriteText(PathIn("a.txt").c_str(), "1"));
    ASSERT_TRUE(FileSystem::WriteText(PathIn("b.log").c_str(), "2"));
    ASSERT_TRUE(FileSystem::WriteText(PathIn("nested/c.txt").c_str(), "3"));

    TVector<String> files;
    FileSystem::EnumrateFilesExtension(m_root.c_str(), ".txt", files);

    EXPECT_TRUE(Contains(files, "a.txt"));
    EXPECT_TRUE(Contains(files, "c.txt"));
    EXPECT_FALSE(Contains(files, "b.log"));
}

TEST_F(FileSystemFixture, EnumrateFilesOnEmptyDirectoryReturnsZero)
{
    const String empty = PathIn("empty");
    ASSERT_TRUE(FileSystem::MakeDirectory(empty.c_str()));

    TVector<String> files;
    const uint32_t count = FileSystem::EnumrateFiles(empty.c_str(), files);

    EXPECT_EQ(count, 0u);
    EXPECT_TRUE(files.empty());
}

//------------------------------------------------------------------------------
// PathUtility
//------------------------------------------------------------------------------
TEST_F(FileSystemFixture, PathUtilityCreateAndCheck)
{
    const Path dir = Path(m_root.c_str()) / "pathutil" / "nested";

    EXPECT_FALSE(PathUtility::CheckFileExists(dir));
    EXPECT_TRUE(PathUtility::CreateDirectories(dir));
    EXPECT_TRUE(PathUtility::CheckFileExists(dir));
    EXPECT_TRUE(PathUtility::CheckDirectory(dir));

    const Path file = dir / "file.txt";
    ASSERT_TRUE(FileSystem::WriteText(String(file.string().c_str()).c_str(), "x"));
    EXPECT_TRUE(PathUtility::CheckFileExists(file));
    EXPECT_FALSE(PathUtility::CheckDirectory(file));
}

TEST_F(FileSystemFixture, PathUtilityCreateDirectoryReturnsFalseWhenAlreadyExists)
{
    const Path dir = Path(m_root.c_str()) / "once";

    EXPECT_TRUE(PathUtility::CreateDirectory(dir));
    // 既存ディレクトリに対しては新規作成が行われないため false。
    EXPECT_FALSE(PathUtility::CreateDirectory(dir));
}
