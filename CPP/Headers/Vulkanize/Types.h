#pragma once

#if defined(__EMSCRIPTEN__)
  #define VKZ_PLATFORM_WEB 1
  #define VKZ_PLATFORM_DETERMINED 1
#endif

#if !defined(VKZ_PLATFORM_DETERMINED)
  #if defined(__APPLE__)
    #if defined(TARGET_IPHONE_SIMULATOR)
      #if TARGET_IPHONE_SIMULATOR
        #define VKZ_PLATFORM_IOS 1
      #endif
    #endif

    #if !defined(VKZ_PLATFORM_IOS)
      #if defined(TARGET_OS_IPHONE)
        #if TARGET_OS_IPHONE
          #define VKZ_PLATFORM_IOS 1
        #endif
      #endif
    #endif

    #if !defined(VKZ_PLATFORM_IOS)
      #define VKZ_PLATFORM_MACOS 1
    #endif

    #define VKZ_PLATFORM_DETERMINED 1
  #endif
#endif

#if !defined(VKZ_PLATFORM_DETERMINED)
  #if defined(_WIN32)
  #  define VKZ_PLATFORM_WINDOWS 1
  #elif defined(__ANDROID__)
  #  define VKZ_PLATFORM_ANDROID 1
  #elif defined(__linux__)
  #  define VKZ_PLATFORM_LINUX 1
  #elif defined(__CYGWIN__)
  #  define VKZ_PLATFORM_LINUX  1
  #  define VKZ_PLATFORM_CYGWIN 1
  #else
  #  define VKZ_PLATFORM_UNKNOWN 1
  #endif
#endif

#if defined(VKZ_PLATFORM_WINDOWS)
  #pragma warning(disable: 4297) /* unexpected throw warnings */
  #if !defined(UNICODE)
    #define UNICODE
  #endif
  #include <windows.h>
  #include <signal.h>
#else
  #define VKZ_PLATFORM_UNIX_COMPATIBLE 1
  #include <climits>
  #include <cstdint>
#endif

#include <cmath>
#include <cstdlib>
#include <cstring>

//------------------------------------------------------------------------------
// Logging
//------------------------------------------------------------------------------
#ifdef __ANDROID__
  #include <android/log.h>
  #define VKZ_LOG(...)       __android_log_print( ANDROID_LOG_INFO,  "Vulkanize", __VA_ARGS__ )
  #define VKZ_LOG_ERROR(...) __android_log_print( ANDROID_LOG_ERROR, "Vulkanize", __VA_ARGS__ )
#else
  #define VKZ_LOG(...)       printf( __VA_ARGS__ )
  #define VKZ_LOG_ERROR(...) fprintf( stderr, __VA_ARGS__ )
#endif

namespace VKZ
{
  //------------------------------------------------------------------------------
  // Primitive Types
  //------------------------------------------------------------------------------
#if defined(VKZ_PLATFORM_WINDOWS)
    typedef double           Real64;
    typedef float            Real32;
    typedef __int64          Int64;
    typedef __int32          Int32;
    typedef __int32          Character;
    typedef unsigned __int16 Word;
    typedef unsigned char    Byte;
    typedef int              Logical;
    typedef unsigned __int64 UInt64;
    typedef unsigned __int32 UInt32;
#else
    typedef double           Real64;
    typedef float            Real32;
    typedef int64_t          Int64;
    typedef int32_t          Int32;
    typedef int32_t          Character;
    typedef uint16_t         Word;
    typedef uint8_t          Byte;
    typedef int              Logical;
    typedef uint64_t         UInt64;
    typedef uint32_t         UInt32;
#endif
}; // namespace VKZ
