AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = vnplayer2

vnplayer2_SOURCES = \
    vnplayer2.cpp \
    tinyxml.cpp \
    tinystr.cpp \
    tinyxmlerror.cpp \
    tinyxmlparser.cpp \
    engine/CActivity.cpp \
    engine/CEffect.cpp \
    engine/COutputLog.cpp \
    engine/CScript.cpp \
    engine/CAttr.cpp \
    engine/CFrame.cpp \
    engine/COverlay.cpp \
    engine/CStats.cpp \
    engine/CCheckpoint.cpp \
    engine/CLayer.cpp \
    engine/CPage.cpp \
    engine/CTextarea.cpp \
    engine/CChoice.cpp \
    engine/CMisc.cpp \
    engine/CPageAction.cpp \
    engine/CVideo.cpp \
    engine/CCommon.cpp \
    engine/CMusicEffect.cpp \
    engine/CParagraph.cpp \
    engine/CVideoEffect.cpp \
    engine/CContext.cpp \
    engine/CMusicInfo.cpp \
    engine/CScene.cpp \
    engine/CCurrentContext.cpp \
    engine/CObject.cpp \
    engine/CScope.cpp

vnplayer2_CXXFLAGS = \
    -I./ \
    $(SDL2_CFLAGS) \
    $(SDL2_IMAGE_CFLAGS) \
    $(SDL2_MIXER_CFLAGS) \
    $(SDL2_TTF_CFLAGS) \
    -Wno-deprecated \
    -Wno-deprecated-declarations \
    -Wno-writable-strings \
    -Wno-unused-command-line-argument \
    -Wno-c++11-compat-deprecated-writable-strings \
    -Wno-format-security \
    -Wno-c++11-extensions

vnplayer2_LDADD = $(SDL2_LIBS) -lSDL2_image -lSDL2_mixer -lSDL2_ttf
