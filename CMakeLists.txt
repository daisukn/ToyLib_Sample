
cmake_minimum_required(VERSION 3.15)

# プロジェクト名
project(ToyLib_Sample LANGUAGES CXX)

# C++17を使用
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows用UTF-8オプション設定
if (MSVC)
    add_compile_options("/utf-8")
endif()

# 出力ディレクトリを構成ごとに指定（Visual Studio対応）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/Release)

# vcpkg ユーザーへのガイド（コメント）
# Note: On Windows with vcpkg, configure using:
# cmake -B build -G "Visual Studio 17 2022" -DCMAKE_TOOLCHAIN_FILE="[vcpkg_root]/scripts/buildsystems/vcpkg.cmake"

# 必要なパッケージの検索
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
find_package(ASSIMP REQUIRED)

# ToyCore ライブラリ部分
file(GLOB_RECURSE TOYCORE_SRC
    ${CMAKE_SOURCE_DIR}/ToyCore/src/*.cpp
    ${CMAKE_SOURCE_DIR}/ToyCore/src/*.h
)

include_directories(
    ${CMAKE_SOURCE_DIR}/ToyCore/include
    ${SDL2_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
)

add_library(ToyCore STATIC ${TOYCORE_SRC})

# リンクライブラリ
 target_link_libraries(ToyCore
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${ASSIMP_LIBRARIES}
)

# GameApp（サンプルアプリケーション）
file(GLOB_RECURSE GAMEAPP_SRC
    ${CMAKE_SOURCE_DIR}/GameApp/*.cpp
    ${CMAKE_SOURCE_DIR}/GameApp/*.h
)

add_executable(GameApp ${GAMEAPP_SRC})

# GameApp に ToyCore をリンク
 target_link_libraries(GameApp ToyCore)

# Windows環境でDLL自動コピー設定（SDL2, GLEW, Assimp の例）
if (WIN32)
    add_custom_command(TARGET GameApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_ROOT}/installed/x64-windows/bin/SDL2.dll"
        $<TARGET_FILE_DIR:GameApp>
    )
    add_custom_command(TARGET GameApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_ROOT}/installed/x64-windows/bin/glew32.dll"
        $<TARGET_FILE_DIR:GameApp>
    )
    add_custom_command(TARGET GameApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${VCPKG_ROOT}/installed/x64-windows/bin/assimp-vc143-mt.dll"
        $<TARGET_FILE_DIR:GameApp>
    )
endif()
