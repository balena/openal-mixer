# Locate OpenAL
# This module defines
# OPENAL_LIBRARY
# OPENAL_FOUND, if false, do not try to link to OpenAL 
# OPENAL_INCLUDE_DIR, where to find the headers
#
# $OPENALDIR is an environment variable that would
# correspond to the ./configure --prefix=$OPENALDIR
# used in building OpenAL.
#
# Created by Eric Wing. This was influenced by the FindSDL.cmake module.

# This makes the presumption that you are include al.h like
# #include "al.h"
# and not 
# #include <AL/al.h>
# The reason for this is that the latter is not entirely portable.
# Windows/Creative Labs does not by default put their headers in AL/ and 
# OS X uses the convention <OpenAL/al.h>.
# 
# For Windows, Creative Labs seems to have added a registry key for their 
# OpenAL 1.1 installer. I have added that key to the list of search paths,
# however, the key looks like it could be a little fragile depending on 
# if they decide to change the 1.00.0000 number for bug fix releases.
# Also, they seem to have laid down groundwork for multiple library platforms
# which puts the library in an extra subdirectory. Currently there is only
# Win32 and I have hardcoded that here. This may need to be adjusted as 
# platforms are introduced.
# The OpenAL 1.0 installer doesn't seem to have a useful key I can use.
# I do not know if the Nvidia OpenAL SDK has a registry key.
# 
# For OS X, remember that OpenAL was added by Apple in 10.4 (Tiger). 
# To support the framework, I originally wrote special framework detection 
# code in this module which I have now removed with CMake's introduction
# of native support for frameworks.
# In addition, OpenAL is open source, and it is possible to compile on Panther. 
# Furthermore, due to bugs in the initial OpenAL release, and the 
# transition to OpenAL 1.1, it is common to need to override the built-in
# framework. 
# Per my request, CMake should search for frameworks first in
# the following order:
# ~/Library/Frameworks/OpenAL.framework/Headers
# /Library/Frameworks/OpenAL.framework/Headers
# /System/Library/Frameworks/OpenAL.framework/Headers
#
# On OS X, this will prefer the Framework version (if found) over others.
# People will have to manually change the cache values of 
# OPENAL_LIBRARY to override this selection or set the CMake environment
# CMAKE_INCLUDE_PATH to modify the search paths.

IF(OPENAL_INCLUDE_DIR AND OPENAL_LIBRARY)

  # in cache already
  SET(OPENAL_FOUND TRUE)

ELSE(OPENAL_INCLUDE_DIR AND OPENAL_LIBRARY)

  IF(UNIX)
    SET(OPENAL_PATH_PREFIXES
      ~/Library/Frameworks
      /Library/Frameworks
      /usr/local
      /usr
      /sw # Fink
      /opt/local # DarwinPorts
      /opt/csw # Blastwave
      /opt
    )
  ENDIF(UNIX)

  IF(WIN32)
    SET(OPENAL_PATH_PREFIXES
      $ENV{SystemDrive}/devtools/openal
      [HKEY_LOCAL_MACHINE\\SOFTWARE\\Creative\ Labs\\OpenAL\ 1.1\ Software\ Development\ Kit\\1.00.0000;InstallDir]
    )
  ENDIF(WIN32)

  FIND_PATH(OPENAL_INCLUDE_DIR al.h
    HINTS
    $ENV{OPENALDIR}
    PATH_SUFFIXES include/AL include/OpenAL include
    PATHS
    ${OPENAL_PATH_PREFIXES}
  )

  FIND_LIBRARY(OPENAL_LIBRARY 
    NAMES OpenAL al openal OpenAL32
    HINTS
    $ENV{OPENALDIR}
    PATH_SUFFIXES lib64 lib libs64 libs libs/Win32
    PATHS
    ${OPENAL_PATH_PREFIXES}
  )

  IF(NOT OPENAL_LIBRARY)
    FIND_LIBRARY(OPENAL_LIBRARY
      NAMES soft_oal
      HINTS
      $ENV{OPENALDIR}
      PATH_SUFFIXES lib64 lib libs64 libs libs/Win32
      PATHS
      ${OPENAL_PATH_PREFIXES}
    )
  ENDIF(NOT OPENAL_LIBRARY)

  FIND_FILE(OPENAL32_DLL OpenAL32.dll
    HINTS
    $ENV{OPENALDIR}
    PATH_SUFFIXES redist lib64 lib libs64 libs libs/Win32
    PATHS
    ${OPENAL_PATH_PREFIXES}
  )

  FIND_FILE(WRAP_OAL_DLL wrap_oal.dll
    HINTS
    $ENV{OPENALDIR}
    PATH_SUFFIXES redist lib64 lib libs64 libs libs/Win32
    PATHS
    ${OPENAL_PATH_PREFIXES}
  )

  FIND_FILE(SOFT_OAL_DLL soft_oal.dll
    HINTS
    $ENV{OPENALDIR}
    PATH_SUFFIXES redist lib64 lib libs64 libs libs/Win32
    PATHS
    ${OPENAL_PATH_PREFIXES}
  )

  IF(OPENAL_LIBRARY AND OPENAL_INCLUDE_DIR)
    SET(OPENAL_FOUND TRUE)
  ENDIF(OPENAL_LIBRARY AND OPENAL_INCLUDE_DIR)

  IF(OPENAL_FOUND)
     IF(NOT OpenAL_FIND_QUIETLY)
        MESSAGE(STATUS "Found OpenAL: ${OPENAL_LIBRARY}")
     ENDIF(NOT OpenAL_FIND_QUIETLY)
  ELSE(OPENAL_FOUND)
     IF(OpenAL_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could NOT find OpenAL")
     ENDIF(OpenAL_FIND_REQUIRED)
  ENDIF(OPENAL_FOUND)

  SET(OPENAL_LIBRARY ${OPENAL_LIBRARY} CACHE PATH "OpenAL library")
  SET(OPENAL32_DLL ${OPENAL32_DLL} CACHE PATH "OpenAL32.dll library")
  SET(WRAP_OAL_DLL ${WRAP_OAL_DLL} CACHE PATH "wrap_oal.dll library")
  SET(SOFT_OAL_DLL ${SOFT_OAL_DLL} CACHE PATH "soft_oal.dll library")
  MARK_AS_ADVANCED(FORCE OPENAL_INCLUDE_DIR OPENAL_LIBRARY OPENAL32_DLL WRAP_OAL_DLL SOFT_OAL_DLL)

ENDIF(OPENAL_INCLUDE_DIR AND OPENAL_LIBRARY)

