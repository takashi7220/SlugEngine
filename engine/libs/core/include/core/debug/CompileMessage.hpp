#pragma once

#include "core/Define.hpp"

#define SLUG_STRINGIZE_DETAIL(x) #x
#define SLUG_STRINGIZE(x) SLUG_STRINGIZE_DETAIL(x)

#if SLUG_DEBUG_MODE
#define SLUG_LOCATION __FILE__ ":" SLUG_STRINGIZE(__LINE__)
#define SLUG_COMPILE_NOTE_MESSAGE(msg) _Pragma(SLUG_STRINGIZE(message("@note : " msg ": " SLUG_LOCATION)))
#define SLUG_COMPILE_TODO_MESSAGE(msg) _Pragma(SLUG_STRINGIZE(message("@todo : " msg ": " SLUG_LOCATION)))
#define SLUG_COMPILE_WARNING_MESSAGE(msg) _Pragma(SLUG_STRINGIZE(message("@warning : " msg ": " SLUG_LOCATION)))
#else
#define SLUG_LOCATION
#define SLUG_COMPILE_NOTE_MESSAGE(msg)
#define SLUG_COMPILE_TODO_MESSAGE(msg)
#define SLUG_COMPILE_WARNING_MESSAGE(msg)
#endif
