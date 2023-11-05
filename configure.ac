# Initialize Autoconf
AC_INIT([vnplayer2], [1.0], [author@example.com])

# Enable foreign Automake mode (no GNU-style files)
AM_INIT_AUTOMAKE([foreign])

# Check for required tools
AC_PROG_CXX
AC_PROG_CC

# Check for optional packages (in this case, SDL2, SDL2_image, SDL2_mixer, SDL2_ttf)
PKG_CHECK_MODULES([SDL2], [sdl2],, [AC_MSG_ERROR([SDL2 is required])])
PKG_CHECK_MODULES([SDL2_IMAGE], [sdl2_image],, [AC_MSG_ERROR([SDL2_image is required])])
PKG_CHECK_MODULES([SDL2_MIXER], [sdl2_mixer],, [AC_MSG_ERROR([SDL2_mixer is required])])
PKG_CHECK_MODULES([SDL2_TTF], [sdl2_ttf],, [AC_MSG_ERROR([SDL2_ttf is required])])

# Include directory for project source files
AC_CONFIG_HEADERS([config.h])

# Output Makefile
AC_OUTPUT(Makefile)
