
#ifndef MPACK_CONFIG_H
#define MPACK_CONFIG_H 1

// This is the configuration for the MPack test harness.

#if defined(DEBUG) || defined(_DEBUG)
#define MPACK_DEBUG 1
#endif

// Most options such as featureset and platform configuration
// are specified by the SCons buildsystem. For other platforms,
// we define the usual configuration here.
#ifndef MPACK_SCONS
    #define MPACK_READER 1
    #define MPACK_WRITER 1
    #define MPACK_EXPECT 1
    #define MPACK_NODE 1

    #define MPACK_STDLIB 1
    #define MPACK_STDIO 1
    #define MPACK_SETJMP 1
    #define MPACK_MALLOC test_malloc
    #define MPACK_FREE test_free
#endif

// Tracking matches the default config, except the test suite
// also supports MPACK_NO_TRACKING to disable it.
#if defined(MPACK_MALLOC) && !defined(MPACK_NO_TRACKING)
    #if MPACK_DEBUG && MPACK_READER
        #define MPACK_READ_TRACKING 1
    #endif
    #if MPACK_WRITER
        #define MPACK_WRITE_TRACKING 1
    #endif
#endif

#ifdef MPACK_MALLOC
#include "test-malloc.h"
#endif

// we use small buffer sizes to test flushing, growing, and malloc failures
#define MPACK_STACK_SIZE 7
#define MPACK_BUFFER_SIZE 7
#define MPACK_NODE_PAGE_SIZE 7

#ifdef MPACK_MALLOC
#define MPACK_NODE_INITIAL_DEPTH 3
#else
#define MPACK_NODE_MAX_DEPTH_WITHOUT_MALLOC 32
#endif

// don't include debug print functions in code coverage
#define MPACK_NO_PRINT 1

#endif
