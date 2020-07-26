#include "CompiletimeDefines.h"

/* definition to expand macro then apply to pragma message */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

/* Some example here */
#pragma message(VAR_NAME_VALUE(SET_BUILD_BINARY_FILENAME))
#pragma message(VAR_NAME_VALUE(SET_BUILD_PLATFORM))
#pragma message(VAR_NAME_VALUE(SET_BUILD_GIT_HEAD))


#ifndef SET_BUILD_BINARY_FILENAME
  #define SET_BUILD_BINARY_FILENAME "firmware"
#endif // ifndef SET_BUILD_BINARY_FILENAME

#ifndef SET_BUILD_PLATFORM
# define SET_BUILD_PLATFORM "unknown"
#endif // ifndef SET_BUILD_PLATFORM

#ifndef SET_BUILD_GIT_HEAD
# define SET_BUILD_GIT_HEAD ""
#endif // ifndef SET_BUILD_GIT_HEAD

String get_binary_filename() {
 #if !defined(CORE_POST_2_5_0) && !defined(ESP32)
    return F("firmware.bin");
  #else
    return F(SET_BUILD_BINARY_FILENAME ".bin");
  #endif
}

String get_build_time() {
  return F(__TIME__);
}

String get_build_date() {
  return F(__DATE__);
}

String get_build_origin() {
  #if defined(CONTINUOUS_INTEGRATION)
  return F("Travis");
  #elif defined(VAGRANT_BUILD)
  return F("Vagrant");
  #else 
  return F("Self built");
  #endif
}

String get_build_platform() {
 #if !defined(CORE_POST_2_5_0) && !defined(ESP32)
    return "";
  #else
  return F(SET_BUILD_PLATFORM);
  #endif
}

String get_git_head() {
 #if !defined(CORE_POST_2_5_0) && !defined(ESP32)
    return "";
  #else
  return F(SET_BUILD_GIT_HEAD);
  #endif
}
