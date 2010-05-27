/*
 * ALx
 * ALx API Header File
 *
 * Copyright (c) 2002-2009
 *
 * Written by Guilherme Balena Versiani
 *
 * OpenAL mixer is intended to work side-by-side with OpenAL.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef AL_ALX_H
#define AL_ALX_H

#include <alc.h>

#if defined(__cplusplus)
extern "C" {
#endif

#if defined(_WIN32) && !defined(_XBOX)
 /* _OPENAL32LIB is deprecated */
 #if defined(ALX_STATIC_LIBRARY)
  #define ALX_API
 #else
  #if defined(ALX_BUILD_LIBRARY) || defined (_OPENAL32LIB)
   #define ALX_API __declspec(dllexport)
  #else
   #define ALX_API __declspec(dllimport)
  #endif
 #endif
#else
 #if defined(AL_BUILD_LIBRARY) && defined(HAVE_GCC_VISIBILITY)
  #define ALX_API __attribute__((visibility("default")))
 #else
  #define ALX_API extern
 #endif
#endif

#if defined(_WIN32)
 #define ALX_APIENTRY __cdecl
#else
 #define ALX_APIENTRY
#endif

#if defined(TARGET_OS_MAC) && TARGET_OS_MAC
 #pragma export on
#endif

#define ALX_VERSION_0_1         1

typedef struct ALXdevice_struct ALXdevice;


/** character */
typedef char ALXchar;

/** 8-bit boolean */
typedef char ALXboolean;

/** enumerated 32-bit value */
typedef int ALXenum;

/** signed 32-bit 2's complement integer */
typedef int ALXint;

/** 32-bit IEEE754 floating-point */
typedef float ALXfloat;


/* Enumerant values begin at column 50. No tabs. */

/* Boolean False. */
#define ALX_FALSE                                0

/* Boolean True. */
#define ALX_TRUE                                 1

/**
 * followed by <float> 0.0-1.0
 */
#define ALX_MASTER_VOLUME                        0x2002

/**
 * followed by AL_TRUE, AL_FALSE
 */
#define ALX_PCM_OUTPUT                           0x2003

/**
 * followed by <float> 0.0-1.0
 */
#define ALX_PCM_OUTPUT_VOLUME                    0x2004

/**
 * followed by <float> 0.0-1.0
 */
#define ALX_INPUT_VOLUME                         0x2005

/**
 * errors
 */

/**
 * No error
 */
#define ALX_NO_ERROR                             ALX_FALSE

/**
 * No device
 */
#define ALX_INVALID_DEVICE                       0xA001

/**
 * invalid context ID
 */
#define ALX_INVALID_CONTEXT                      0xA002

/**
 * bad enum
 */
#define ALX_INVALID_ENUM                         0xA003

/**
 * bad value
 */
#define ALX_INVALID_VALUE                        0xA004

/**
 * Out of memory.
 */
#define ALX_OUT_OF_MEMORY                        0xA005


#define ALX_EXTENSIONS                           0x2006

#define ALX_MAJOR_VERSION                        0x2000
#define ALX_MINOR_VERSION                        0x2001
#define ALX_DEVICE_SPECIFIER                     0x2007
#define ALX_CAPTURE_DEVICE_SPECIFIER             0x2008

/**
 * Multiple selectors
 */
#define ALX_INPUT_SOURCE                         0x2009
#define ALX_INPUT_SOURCE_SPECIFIER               0x200A
#define ALX_OUTPUT_VOLUME                        0x200B
#define ALX_OUTPUT_VOLUME_SPECIFIER              0x200C


/*
 * Create/Destroy Mixer
 */
ALX_API ALXdevice *     ALX_APIENTRY alxOpenDevice( const ALXchar *devicename );

ALX_API ALXdevice *     ALX_APIENTRY alxOpenCaptureDevice( const ALXchar *devicename );

ALX_API ALXdevice *     ALX_APIENTRY alxMapDevice( ALCdevice *device );

ALX_API ALXdevice *     ALX_APIENTRY alxMapCaptureDevice( ALCdevice *device );

ALX_API void            ALX_APIENTRY alxCloseDevice( ALXdevice *mixer );


/*
 * Error support.
 * Obtain the most recent Context error
 */
ALX_API ALCenum         ALX_APIENTRY alxGetError( ALXdevice *mixer );


/*
 * Query functions
 */
ALX_API ALXfloat        ALX_APIENTRY alxGetFloat( ALXdevice *mixer, ALXenum param );

ALX_API void            ALX_APIENTRY alxSetFloat( ALXdevice *mixer, ALXenum param, ALXfloat value );

ALX_API ALXboolean      ALX_APIENTRY alxGetBoolean( ALXdevice *mixer, ALXenum param );

ALX_API void            ALX_APIENTRY alxSetBoolean( ALXdevice *mixer, ALXenum param, ALXboolean value );

ALX_API const ALXchar * ALX_APIENTRY alxGetString( ALXdevice *mixer, ALXenum param );

ALX_API ALXint          ALX_APIENTRY alxGetInteger( ALXdevice *mixer, ALXenum param );

ALX_API void            ALX_APIENTRY alxSetInteger( ALXdevice *mixer, ALXenum param, ALXint value );

ALX_API const ALXchar * ALX_APIENTRY alxGetIndexedString( ALXdevice *mixer, ALXenum param, ALXint index );

ALX_API ALXfloat        ALX_APIENTRY alxGetIndexedFloat( ALXdevice *mixer, ALXenum param, ALXint index );

ALX_API ALXboolean      ALX_APIENTRY alxGetIndexedBoolean( ALXdevice *mixer, ALXenum param, ALXint index );

ALX_API void            ALX_APIENTRY alxSetIndexedFloat( ALXdevice *mixer, ALXenum param, ALXint index, ALXfloat value );

ALX_API void            ALX_APIENTRY alxSetIndexedBoolean( ALXdevice *mixer, ALXenum param, ALXint index, ALXboolean value );

ALX_API void *          ALX_APIENTRY alxGetProcAddress( ALXdevice *device, const ALXchar *funcName );

/*
 * Pointer-to-function types, useful for dynamically getting ALX entry points.
 */
typedef ALXdevice *     (ALX_APIENTRY *LPALXOPENDEVICE)( ALCdevice *device );
typedef ALXdevice *     (ALX_APIENTRY *LPALXOPENCAPTUREDEVICE)( ALCdevice *device );
typedef ALXdevice *     (ALX_APIENTRY *LPALXMAPDEVICE)( ALCdevice *device );
typedef ALXdevice *     (ALX_APIENTRY *LPALXMAPCAPTUREDEVICE)( ALCdevice *device );
typedef void            (ALX_APIENTRY *LPALXCLOSEDEVICE)( ALXdevice *mixer );
typedef ALCenum         (ALX_APIENTRY *LPALXGETERROR)( ALXdevice *mixer );
typedef ALXfloat        (ALX_APIENTRY *LPALXGETFLOAT)( ALXdevice *mixer, ALXenum param );
typedef void            (ALX_APIENTRY *LPALXSETFLOAT)( ALXdevice *mixer, ALXenum param, ALXfloat value );
typedef ALXboolean      (ALX_APIENTRY *LPALXGETBOOLEAN)( ALXdevice *mixer, ALXenum param );
typedef void            (ALX_APIENTRY *LPALXSETBOOLEAN)( ALXdevice *mixer, ALXenum param, ALXboolean value );
typedef const ALXchar * (ALX_APIENTRY *LPALXGETSTRING)( ALXdevice *mixer, ALXenum param );
typedef ALXint          (ALX_APIENTRY *LPALXGETINTEGER)( ALXdevice *mixer, ALXenum param );
typedef void            (ALX_APIENTRY *LPALXSETINTEGER)( ALXdevice *mixer, ALXenum param, ALXint value );
typedef void            (ALX_APIENTRY *LPALXGETINDEXEDSTRING)( ALXdevice *mixer, ALXenum param, ALXint index );
typedef ALXfloat        (ALX_APIENTRY *LPALXGETINDEXEDFLOAT)( ALXdevice *mixer, ALXenum param, ALXint index );
typedef ALXboolean      (ALX_APIENTRY *LPALXGETINDEXEDBOOLEAN)( ALXdevice *mixer, ALXenum param, ALXint index );
typedef void            (ALX_APIENTRY *LPALXSETINDEXEDFLOAT)( ALXdevice *mixer, ALXenum param, ALXint index, ALXfloat value );
typedef void            (ALX_APIENTRY *LPALXSETINDEXEDBOOLEAN)( ALXdevice *mixer, ALXenum param, ALXint index, ALXboolean value );
typedef void *          (ALX_APIENTRY *LPALXGETPROCADDRESS)( ALXdevice *device, const ALXchar *funcName );


#if defined(TARGET_OS_MAC) && TARGET_OS_MAC
 #pragma export off
#endif

#if defined(__cplusplus)
}
#endif

#endif /* AL_ALX_H */
