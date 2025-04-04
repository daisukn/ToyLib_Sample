cmake_minimum_required(VERSION 3.15)
project(game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
set(TOYLIB_PATH ToyLib_Sample/ToyLibCore)
set(GAME_PATH ToyLib_Sample/GameApp)

find_package(OpenGL REQUIRED)

# ソースファイル収集
file(GLOB_RECURSE SOURCES
    ${CMAKE_SOURCE_DIR}/*.cpp
    ${CMAKE_SOURCE_DIR}/${TOYLIB_PATH}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/${GAME_PATH}/*.cpp
)

# 内部ファイル除去
foreach(source ${SOURCES})
    if(${source} MATCHES ".*/CMakeFiles/.*")
        list(REMOVE_ITEM SOURCES ${source})
    endif()
endforeach()

add_executable(${PROJECT_NAME} ${SOURCES})

include_directories(
    ${CMAKE_SOURCE_DIR}/${TOYLIB_PATH}/include
    ${CMAKE_SOURCE_DIR}/${GAME_PATH}/
)

if(WIN32)
    message(STATUS "Configuring for Windows build")

    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(OpenGL REQUIRED)

    include_directories(
        ${CMAKE_SOURCE_DIR}/${TOYLIB_PATH}/include
        ${CMAKE_SOURCE_DIR}/${GAME_PATH}
        C:/dev/local/assimp/include
    )

    link_directories(
        "C:/dev/local/assimp/lib"
    )

    target_link_libraries(${PROJECT_NAME}
        OpenGL32
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
        SDL2_mixer::SDL2_mixer
        ${GLEW_LIBRARIES}
        C:/dev/local/assimp/lib/assimp-vc143-mt.lib
    )

    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    target_link_options(${PROJECT_NAME} PRIVATE /SUBSYSTEM:CONSOLE)

    set(DLL_FILES
        "C:/dev/vcpkg/installed/x64-windows/bin/SDL2.dll"
        "C:/dev/vcpkg/installed/x64-windows/bin/SDL2_image.dll"
        "C:/dev/vcpkg/installed/x64-windows/bin/jpeg62.dll"
        "C:/dev/vcpkg/installed/x64-windows/bin/libpng16.dll"
        "C:/dev/vcpkg/installed/x64-windows/bin/zlib1.dll"
        "C:/dev/vcpkg/installed/x64-windows/bin/SDL2_ttf.dll"
        "C:/dev/vcpkg/installed/x64-windows/bin/SDL2_mixer.dll"
        "C:/dev/vcpkg/installed/x64-windows/bin/glew32.dll"
        "C:/dev/local/assimp/bin/assimp-vc143-mt.dll"
    )

    foreach(dll ${DLL_FILES})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${dll}
                ${CMAKE_BINARY_DIR}
            COMMENT "Copying ${dll} to ${CMAKE_BINARY_DIR}"
        )
    endforeach()


    # Assimp DLL のコピー先はビルドディレクトリ
    set(ASSIMP_DLL_PATH "C:/dev/local/assimp/bin/assimp-vc143-mt.dll")  # バージョンに合わせて適宜変更

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ASSIMP_DLL_PATH}
            ${CMAKE_BINARY_DIR}
        COMMENT "Copying Assimp DLL to build directory"
    )


elseif(UNIX AND NOT APPLE)
    message(STATUS "Configuring for Linux build")

    find_package(OpenGL REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
    find_package(GLEW REQUIRED)
    #find_package(ASSIMP REQUIRED)

    include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        #${ASSIMP_INCLUDE_DIRS}
        /usr/local/include
    )

    target_link_libraries(${PROJECT_NAME}
        ${OPENGL_LIBRARIES}
        ${SDL2_LIBRARIES} 
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES} 
        ${GLEW_LIBRARIES}
        #${ASSIMP_LIBRARIES}
        assimp
    )

elseif(APPLE)
    message(STATUS "Configuring for macOS build")

    find_package(OpenGL REQUIRED)
    #pkg_check_modules(SDL2 REQUIRED sdl2)
    #pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    find_package(SDL2 REQUIRED HINTS /opt/homebrew/opt/sdl2)
    find_package(SDL2_image REQUIRED HINTS /opt/homebrew/opt/sdl2_image)
    find_package(GLEW REQUIRED HINTS /opt/homebrew/opt/glew)
    find_package(ASSIMP REQUIRED)

    include_directories(
        /opt/homebrew/include
        /usr/local/include
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${GLEW_INCLUDE_DIRS}
        ${ASSIMP_INCLUDE_DIRS}
    )

    target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        SDL2::SDL2
        SDL2_image::SDL2_image
        GLEW::GLEW
        ${ASSIMP_LIBRARIES}
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )

    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path/../lib;/opt/homebrew/lib;/usr/local/lib"
    )
endif()

# Shaderファイルのコピー
set(SOURCE_SHADER_DIR "${CMAKE_SOURCE_DIR}/ToyLib_Sample/ToyLibCore/Shaders")
set(DEST_SHADER_DIR "${CMAKE_BINARY_DIR}/ToyLibCore/Shaders")

add_custom_target(copy_Shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_SHADER_DIR} ${DEST_SHADER_DIR}
    COMMENT "Copying Shaders to build directory"
)

#Assetファイルのコピー
set(SOURCE_ASSET_DIR "${CMAKE_SOURCE_DIR}/ToyLib_Sample/Assets")
set(DEST_ASSET_DIR "${CMAKE_BINARY_DIR}/Assets")

add_custom_target(copy_Assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_ASSET_DIR} ${DEST_ASSET_DIR}
    COMMENT "Copying Assets to build directory"
)

# Settingsファイルのコピー
set(SOURCE_SETTINGS_DIR "${CMAKE_SOURCE_DIR}/ToyLib_Sample/Settings")
set(DEST_SETTINGS_DIR "${CMAKE_BINARY_DIR}/Settings")

add_custom_target(copy_Settings ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_SETTINGS_DIR} ${DEST_SETTINGS_DIR}
    COMMENT "Copying Settings to build directory"
)

