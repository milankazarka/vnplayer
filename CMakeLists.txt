cmake_minimum_required(VERSION 3.0)
project(vnplayer2)

# Set C++ compiler
set(CMAKE_CXX_STANDARD 11)

# Add compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-deprecated-declarations -Wno-writable-strings -Wno-unused-command-line-argument -Wno-c++11-compat-deprecated-writable-strings -Wno-format-security -Wno-c++11-extensions")

# Add your source files
set(SOURCE_FILES
    vnplayer2.cpp
    tinyxml.cpp
    tinystr.cpp
    tinyxmlerror.cpp
    tinyxmlparser.cpp
    engine/CActivity.cpp
    engine/CEffect.cpp
    engine/COutputLog.cpp
    engine/CScript.cpp
    engine/CAttr.cpp
    engine/CFrame.cpp
    engine/COverlay.cpp
    engine/CStats.cpp
    engine/CCheckpoint.cpp
    engine/CLayer.cpp
    engine/CPage.cpp
    engine/CTextarea.cpp
    engine/CChoice.cpp
    engine/CMisc.cpp
    engine/CPageAction.cpp
    engine/CVideo.cpp
    engine/CCommon.cpp
    engine/CMusicEffect.cpp
    engine/CParagraph.cpp
    engine/CVideoEffect.cpp
    engine/CContext.cpp
    engine/CMusicInfo.cpp
    engine/CScene.cpp
    engine/CCurrentContext.cpp
    engine/CObject.cpp
    engine/CScope.cpp
)

# Find SDL2, SDL2_image, SDL2_mixer, and SDL2_ttf
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

# Add executable target
add_executable(vnplayer2 ${SOURCE_FILES})

# Execute sdl2-config and capture its output
execute_process(
    COMMAND pkg-config --cflags --libs SDL2 SDL2_image SDL2_mixer SDL2_ttf
    OUTPUT_VARIABLE SDL2_CONFIG_OUTPUT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Append compiler and linker flags from sdl2-config to target properties
string(REPLACE " " ";" SDL2_CONFIG_FLAGS ${SDL2_CONFIG_OUTPUT})
target_compile_options(vnplayer2 PRIVATE ${SDL2_CONFIG_FLAGS})

# Link libraries
target_link_libraries(vnplayer2
    ${SDL2_CONFIG_OUTPUT}
)

# Copy the binary to the destination folder after build
add_custom_command(
    TARGET vnplayer2
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vnplayer2> "../"
)
