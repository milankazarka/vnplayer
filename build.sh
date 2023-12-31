#!/bin/bash
# pkg-config: `pkg-config --cflags --libs SDL2 SDL2_image SDL2_mixer SDL2_ttf`
# sdl-config: `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_mixer -lSDL2_ttf
#
g++ -o vnplayer2 vnplayer2.cpp \
    tinyxml.cpp tinystr.cpp tinyxmlerror.cpp tinyxmlparser.cpp \
    engine/CActivity.cpp engine/CEffect.cpp	engine/COutputLog.cpp engine/CScript.cpp \
    engine/CAttr.cpp engine/CFrame.cpp engine/COverlay.cpp engine/CStats.cpp \
    engine/CCheckpoint.cpp engine/CLayer.cpp engine/CPage.cpp engine/CTextarea.cpp \
    engine/CChoice.cpp engine/CMisc.cpp	engine/CPageAction.cpp engine/CVideo.cpp \
    engine/CCommon.cpp engine/CMusicEffect.cpp engine/CParagraph.cpp engine/CVideoEffect.cpp \
    engine/CContext.cpp	engine/CMusicInfo.cpp engine/CScene.cpp \
    engine/CCurrentContext.cpp engine/CObject.cpp engine/CScope.cpp \
    `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_mixer -lSDL2_ttf \
    -Wno-deprecated -Wno-deprecated-declarations -I./ \
    -Wno-c++11-compat-deprecated-writable-strings -Wno-format-security -Wno-c++11-extensions
