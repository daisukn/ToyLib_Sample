cmake_minimum_required(VERSION 3.15)
project(game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

find_package(OpenGL REQUIRED)

# ソースファイル収集
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/*.cpp
    ${CMAKE_SOURCE_DIR}/ToyLib_Sample/ToyCore/src/*.cpp
    ${CMAKE_SOURCE_DIR}/ToyLib_Sample/GameApp/*.cpp
)

# 内部ファイル除去
foreach(source ${SOURCES})
    if(${source} MATCHES ".*/CMakeFiles/.*")
        list(REMOVE_ITEM SOURCES ${source})
    endif()
endforeach()

add_executable(${PROJECT_NAME} ${SOURCES})


message(STATUS "Configuring for Windows build")

include_directories(
    ${CMAKE_SOURCE_DIR}/ToyLib_Sample/ToyCore/include
    ${CMAKE_SOURCE_DIR}/ToyLib_Sample/GameApp
    C:/dev/local/assimp/include
)

link_directories(
    "C:/dev/vcpkg/installed/x64-windows/lib"
    "C:/dev/local/assimp/lib"
)

target_link_libraries(${PROJECT_NAME}
    OpenGL32
    SDL2
    SDL2main
    SDL2_image
    SDL2_ttf
    SDL2_mixer
    glew32
    C:/dev/local/assimp/lib/assimp-vc143-mt.lib
)

# UTF-8 文字コードオプション追加（MSVC専用）
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()

# 実行時に main() を使うためのサブシステム指定
target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:CONSOLE)

# DLL 自動コピー
set(DLL_FILES
    "C:/dev/vcpkg/installed/x64-windows/bin/SDL2.dll"
    "C:/dev/vcpkg/installed/x64-windows/bin/SDL2_image.dll"
    "C:/dev/vcpkg/installed/x64-windows/bin/jpeg62.dll"
    "C:/dev/vcpkg/installed/x64-windows/bin/libpng16.dll"
    "C:/dev/vcpkg/installed/x64-windows/bin/zlib1.dll"
    "C:/dev/vcpkg/installed/x64-windows/bin/SDL2_ttf.dll"
    "C:/dev/vcpkg/installed/x64-windows/bin/SDL2_mixer.dll"
    "C:/dev/vcpkg/installed/x64-windows/bin/glew32.dll"
    "C:/dev/local/assimp/bin/assimp-vc143-mt.dll"   # バージョン名はビルド結果に応じて確認
)

foreach(dll ${DLL_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${dll}
            ${CMAKE_BINARY_DIR}
        COMMENT "Copying ${dll} to ${CMAKE_BINARY_DIR}"
    )
endforeach()

    
# Shaderスクリプトをコピー
set(SOURCE_SHADER_DIR "${CMAKE_SOURCE_DIR}/ToyLib_Sample/ToyCore/Shaders")
set(DEST_SHADER_DIR "${CMAKE_BINARY_DIR}/ToyCore/Shaders")

add_custom_command(
    OUTPUT ${DEST_SHADER_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_SHADER_DIR} ${DEST_SHADER_DIR}
    COMMENT "Copying Shaders to build directory"
)

add_custom_target(copy_Shaders ALL DEPENDS ${DEST_SHADER_DIR})
