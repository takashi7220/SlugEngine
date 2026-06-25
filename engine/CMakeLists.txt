# cmakeのバージョンを設定する
cmake_minimum_required(VERSION 3.15.0)

# パスの設定
set(REPOSITORY_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(THIRDPARTY_PATH ${REPOSITORY_ROOT_PATH}/thirdparty)

# ルートとなるプロジェクト名を設定
set(SOLUTION_NAME SlugEngine)

# プロジェクトをCMakeツリーに登録
project(${SOLUTION_NAME})

# 構成を設定
set(CMAKE_CONFIGURATION_TYPES Debug Optimize Release)

# workspaceディレクトリのパス
set(WORKSPACE_DIR_PATH ${REPOSITORY_ROOT_PATH}/_workspace)

# デフォルトの出力ディレクトリ
set(DEFAULT_OUTPUT_DIR_PATH ${WORKSPACE_DIR_PATH}/bin)

# テストアプリケーションの出力ディレクトリ
set(TEST_APP_OUTPUT_DIR_PATH ${WORKSPACE_DIR_PATH}/tests)

# options
option(SLUG_USE_MIMALLOC "use mimmaloc" ON)
option(SLUG_BUILD_TEST "build test" ON)
option(SLUG_BUILD_SHADER "build shader" ON)

# プリプロセッサマクロを設定
set(COMPILE_DEFINITIONS_ALL PLATFORM_WINDOWS SLUG_USE_STL SLUG_USE_SIMD SLUG_OVERRIDE_NEW_DELETE CONFIGURATION="$(Configuration)")
set(COMPILE_DEFINITIONS_Debug DEBUG_MODE _DEBUG)
set(COMPILE_DEFINITIONS_Optimize DEBUG_MODE NDEBUG)
set(COMPILE_DEFINITIONS_Release RELEASE_MODE NDEBUG)

if(SLUG_USE_MIMALLOC)
    list(APPEND COMPILE_DEFINITIONS_ALL SLUG_USE_MIMALLOC)
endif()

# コンパイルオプションを設定
set(COMPILE_OPTIONS_ALL /fp:fast /Ot /Ob1 /W4 /WX /external:W0 /experimental:external /external:anglebrackets /wd4100 /wd4201 /wd26812 /Zc:__cplusplus)
set(COMPILE_OPTIONS_Debug /MDd /Oy- /Zi /Od)
set(COMPILE_OPTIONS_Optimize /MD /Zo /Oy- /Zi /O2)
set(COMPILE_OPTIONS_Release /MD /Oy /O2)

# コンパイルFeaturesを設定(cppのバージョンを20に指定)
set(CMAKE_CXX_STANDARD 20)
set(COMPILE_FEATURES_ALL cxx_std_20)
set(COMPILE_FEATURES_Debug )
set(COMPILE_FEATURES_Optimize )
set(COMPILE_FEATURES_Release )

# リンクフラグを設定
set(EXE_LINKER_FLAGS_ALL "/SUBSYSTEM:WINDOWS /MACHINE:X64")
set(EXE_LINKER_FLAGS_Debug "/debug /INCREMENTAL")
set(EXE_LINKER_FLAGS_Optimize "/debug /INCREMENTAL")
set(EXE_LINKER_FLAGS_Release "/INCREMENTAL:NO")

# Utility.cmakeモジュールをインクルード
include(${REPOSITORY_ROOT_PATH}/make/cmake/Utility.cmake)

# MSVCの時はutf8でコンパイル
if (MSVC)
    add_compile_options(/utf-8)
    add_compile_options(/MP)
endif()

# ソースコードのフィルタリングを有効化
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# subdirectory追加
add_subdirectory(thirdparty)
add_subdirectory(libs)

if(SLUG_BUILD_TEST)
    add_subdirectory(tests)
endif()
