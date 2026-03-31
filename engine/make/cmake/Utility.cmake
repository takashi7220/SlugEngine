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

macro(setup_options)
    # ビルド構成毎にプリプロセッサマクロとコンパイラオプションを設定
    foreach(CONFIGRATION_TYPE ${CMAKE_CONFIGURATION_TYPES})

        # 共通設定で初期化
        set(COMPILE_DEFINATIONS ${COMPILE_DEFINATIONS_ALL})
        set(COMPILE_FEATURES ${COMPILE_FEATURES_ALL})
        set(COMPILE_OPTIONS ${COMPILE_OPTIONS_ALL})
        set(LINK_PATHS ${LINK_PATHS_ALL})

        # ビルド構成に応じて追加
        list(APPEND COMPILE_DEFINATIONS ${COMPILE_DEFINATIONS_${CONFIGRATION_TYPE}})
        list(APPEND COMPILE_FEATURES ${COMPILE_FEATURES_${CONFIGRATION_TYPE}})
        list(APPEND COMPILE_OPTIONS ${COMPILE_OPTIONS_${CONFIGRATION_TYPE}})
        list(APPEND LINK_PATHS ${LINK_PATHS_${CONFIGRATION_TYPE}})

        # プリプロセッサマクロを${PROJECT_NAME}に設定
        target_compile_definitions(${PROJECT_NAME} PUBLIC $<$<CONFIG:${CONFIGRATION_TYPE}>:${COMPILE_DEFINATIONS}>)

        # コンパイルオプションを${PROJECT_NAME}に設定
        target_compile_options(${PROJECT_NAME} PUBLIC $<$<CONFIG:${CONFIGRATION_TYPE}>:${COMPILE_OPTIONS}>)

        # コンパイルFeaturesを設定
        target_compile_features(${PROJECT_NAME} PUBLIC $<$<CONFIG:${CONFIGRATION_TYPE}>:${COMPILE_FEATURES}>)
        
        # リンクするlibを設定
        target_link_libraries(${PROJECT_NAME} PUBLIC $<$<CONFIG:${CONFIGRATION_TYPE}>:${LINK_PATHS}>)
    endforeach(CONFIGRATION_TYPE)
endmacro()

macro(install_thirdparty target build_dir install_dir options)
  set(THIRDPARTY_DIR ${REPOSITORY_ROOT_PATH}/thirdparty)
  set(INSTALL_BATCH_FILE ${THIRDPARTY_DIR}/INSTALL.bat)

  execute_process(
      COMMAND ${INSTALL_BATCH_FILE} -target "${target}" -install_dir "${install_dir}" -build_dir "${build_dir}" -options "${options}"
      WORKING_DIRECTORY ${THIRDPARTY_DIR}
      RESULT_VARIABLE result
  )

  set(${target}_DEBUG_DIR ${install_dir}/Debug)
  set(${target}_RELEASE_DIR ${install_dir}/Release)
  set(${target}_INCLUDE_DIR ${${target}_RELEASE_DIR}/include)
  set(${target}_DEBUG_LIBRARY_DIR ${${target}_DEBUG_DIR}/lib)
  set(${target}_RELEASE_LIBRARY_DIR ${${target}_RELEASE_DIR}/lib)
  set(${target}_BINARY_DIR ${${target}_RELEASE_DIR}/bin)
  file(GLOB_RECURSE ${target}_DEBUG_LIBRARIES ${${target}_DEBUG_LIBRARY_DIR}/*.lib)
  file(GLOB_RECURSE ${target}_RELEASE_LIBRARIES ${${target}_RELEASE_LIBRARY_DIR}/*.lib)

endmacro()