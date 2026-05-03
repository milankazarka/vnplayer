cmake_minimum_required(VERSION 3.15)
project(vnplayer2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================
# Source files
# =========================
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

add_executable(vnplayer2 ${SOURCE_FILES})

# =========================
# Include
# =========================
target_include_directories(vnplayer2 PRIVATE
    ${CMAKE_SOURCE_DIR}
)

# =========================
# SDL2 via vcpkg (SPRÁVNE NÁZVY)
# =========================
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

# =========================
# Link
# =========================
target_link_libraries(vnplayer2 PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
)

# =========================
# Windows executable fix
# =========================
if(WIN32)
    set_property(TARGET vnplayer2 PROPERTY WIN32_EXECUTABLE TRUE)
endif()

# =========================
# Copy resources
# =========================
add_custom_command(TARGET vnplayer2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources
    $<TARGET_FILE_DIR:vnplayer2>/resources
)