#pragma once

#include <cassert>

void PrintAssertMessage(const char* func, const char* file, int line, const char* format, ...);

#ifdef _DEBUG

#define ASSERT(condition) \
    do { \
        if (! (condition)) { \
			PrintAssertMessage( __FUNCTION__, __FILE__, __LINE__, #condition); \
            assert(condition); \
        } \
    } while (false)

#define ASSERT_VERBOSE(condition, fmt, ...) \
    do { \
        if (! (condition)) { \
			PrintAssertMessage( __FUNCTION__, __FILE__, __LINE__, fmt, __VA_ARGS__); \
            assert(condition); \
        } \
    } while (false)

#else
# define ASSERT(condition) do { } while (false)
# define ASSERT_VERBOSE(condition, fmt, ...) do { } while (false)
#endif