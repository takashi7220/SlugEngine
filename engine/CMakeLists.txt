# cmakeのバージョンを設定する
cmake_minimum_required(VERSION 3.15.0)

# パスの設定
set(REPOSITORY_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(THIRDPARTY_PATH ${REPOSITORY_ROOT_PATH}/thirdparty)

# SLUG_BUFFERSへのパスを設定
set(SLUG_BUFFUERS_PATH ${REPOSITORY_ROOT_PATH}/schema/flatbuffers)
set(SLUG_BUFFUERS_SCHEMA_PATH ${SLUG_BUFFUERS_PATH}/schema)
set(SLUG_BUFFUERS_EXE_PATH ${SLUG_BUFFUERS_PATH}/bin/SbGenerator.exe)

# ルートとなるプロジェクト名を設定
set(SOLUTION_NAME SlugEngine)

# プロジェクトをCMakeツリーに登録
project(${SOLUTION_NAME})

# 構成を設定
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# workspaceディレクトリのパス
set(WORKSPACE_DIR_PATH ${REPOSITORY_ROOT_PATH}/_workspace)

# デフォルトの出力ディレクトリ
set(DEFAULT_OUTPUT_DIR_PATH ${WORKSPACE_DIR_PATH}/bin)

# テストアプリケーションの出力ディレクトリ
set(TEST_APP_OUTPUT_DIR_PATH ${WORKSPACE_DIR_PATH}/tests)

# options
option(SLUG_USE_GOOGLETEST "use thirdparty/googletest" ON)
option(SLUG_USE_GOOGLEBENCHMARK "use thirdparty/googlebenchmark" ON)
option(SLUG_USE_MIMALLOC "use thirdparty/mimalloc" ON)
option(SLUG_USE_CEREAL "use thirdparty/cereal" ON)
option(SLUG_USE_FLATBUFFERS "use thirdparty/flatbuffers" ON)
option(SLUG_USE_FLATC "use thirdparty/flatbuffers use flatc" OFF)
option(SLUG_USE_LLVM "use thirdparty/llvm/llvm" OFF)
option(SLUG_USE_FRAME_GRAPH "use thirdparty/FrameGraph" ON)
option(SLUG_USE_REFLECT_CPP "use thirdparty/reflect-cpp" OFF)
option(SLUG_USE_RTTR "use thirdparty/rttr" OFF)
option(SLUG_USE_CORE "use libs/core" ON)
option(SLUG_USE_GRAPHICS "use libs/graphics" ON)
option(SLUG_USE_ENGINE "use libs/engine" ON)
option(SLUG_BUILD_TEST "build test" ON)

# プリプロセッサマクロを設定
set(COMPILE_DEFINATIONS_ALL PLATFORM_WINDOWS SLUG_USE_STL SLUG_USE_SIMD SLUG_OVERRIDE_NEW_DELTE CONFIGURATION="$(Configuration)")
set(COMPILE_DEFINATIONS_Debug DEBUG_MODE _DEBUG)
set(COMPILE_DEFINATIONS_Release RELEASE_MODE NDEBUG)

# コンパイルオプションを設定
set(COMPILE_OPTIONS_ALL /fp:fast /Ot /Ob1 /W4 /WX /external:W0 /experimental:external /external:anglebrackets /wd4100 /wd4201 /wd26812 /Zc:__cplusplus)
set(COMPILE_OPTIONS_Debug /MDd /Oy- /Zi /Od)
set(COMPILE_OPTIONS_Release /MD /Oy /O2)

# コンパイルFeaturesを設定(cppのバージョンを20に指定)
set(CMAKE_CXX_STANDARD 20)
set(COMPILE_FEATURES_ALL cxx_std_20)
set(COMPILE_FEATURES_Debug )
set(COMPILE_FEATURES_Release )

# リンクフラグを設定
set(EXE_LINKER_FLAGS_ALL "/SUBSYSTEM:WINDOWS /MACHINE:X64")
set(EXE_LINKER_FLAGS_Debug "/debug /INCREMENTAL")
set(EXE_LINKER_FLAGS_Release "/INCREMENTAL:NO")

# Utility.cmakeモジュールをインクルード
include(${REPOSITORY_ROOT_PATH}/make/cmake/Utility.cmake)
include(${REPOSITORY_ROOT_PATH}/make/cmake/Thirdparty.cmake)

setup_thirdparty_predefine()

# ソースコードのフィルタリングを有効化
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# appsをsubdirectory追加
add_subdirectory(thirdparty)
add_subdirectory(libs)
add_subdirectory(tests)
