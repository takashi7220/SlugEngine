# フォルダ内のファイルをソースグループに整理する関数
function(create_source_group source_dir)
  # 指定されたディレクトリ以下のファイルを再帰的に取得
  file(GLOB_RECURSE sources RELATIVE "${source_dir}" "${source_dir}/*")

  foreach(source IN LISTS sources)
    get_filename_component(source_path "${source}" PATH)
    # ソースファイルのパスをフォルダ構造としてソースグループに設定
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source_dir}/${source}")
  endforeach()
endfunction()

function(setup_options project_name)
    # ビルド構成毎にプリプロセッサマクロとコンパイラオプションを設定
    foreach(CONFIGRATION_TYPE ${CMAKE_CONFIGURATION_TYPES})

        # 共通設定で初期化
        set(COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS_ALL})
        set(COMPILE_FEATURES ${COMPILE_FEATURES_ALL})
        set(COMPILE_OPTIONS ${COMPILE_OPTIONS_ALL})

        # ビルド構成に応じて追加
        list(APPEND COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS_${CONFIGRATION_TYPE}})
        list(APPEND COMPILE_FEATURES ${COMPILE_FEATURES_${CONFIGRATION_TYPE}})
        list(APPEND COMPILE_OPTIONS ${COMPILE_OPTIONS_${CONFIGRATION_TYPE}})

        # プリプロセッサマクロを${project_name}に設定
        target_compile_definitions(${project_name} PUBLIC $<$<CONFIG:${CONFIGRATION_TYPE}>:${COMPILE_DEFINITIONS}>)

        # コンパイルオプションを${project_name}に設定
        target_compile_options(${project_name} PUBLIC $<$<CONFIG:${CONFIGRATION_TYPE}>:${COMPILE_OPTIONS}>)

        # コンパイルFeaturesを設定
        target_compile_features(${project_name} PUBLIC $<$<CONFIG:${CONFIGRATION_TYPE}>:${COMPILE_FEATURES}>)
        
    endforeach(CONFIGRATION_TYPE)
    # set_target_properties(${project_name} PROPERTIES UNITY_BUILD ON)
endfunction()