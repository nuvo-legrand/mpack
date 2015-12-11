/*
 * Copyright (c) 2015 Nicholas Fraser
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/**
 * @file
 *
 * Abstracts C++ platform and feature detection
 */

#ifndef MPACK_CXX_PLATFORM_H
#define MPACK_CXX_PLATFORM_H 1

#include "mpack.h"

#if defined(__cplusplus)



// C++ platform and feature detection

#ifndef MPACK_CXX11
#if __cplusplus >= 201103L
#define MPACK_CXX11 1
#endif
#endif

#ifndef MPACK_CXX_EXCEPTIONS
#if (defined(__GNUC__) || defined(__clang__)) && defined(__EXCEPTIONS)
#define MPACK_CXX_EXCEPTIONS 1
#elif defined(MSC_VER) && defined(_CPPUNWIND)
#define MPACK_CXX_EXCEPTIONS 1
#endif



// Define undefined values to 0 to support -Wundef (as in mpack-platform.h)

#ifndef MPACK_CXX_STL
#define MPACK_CXX_STL 0
#endif
#ifndef MPACK_CXX11
#define MPACK_CXX11 0
#endif
#ifndef MPACK_CXX_EXCEPTIONS
#define MPACK_CXX_EXCEPTIONS 0
#endif



// C++ includes

#if MPACK_CXX_STL
#include <string>
#include <vector>
#endif



#endif
#endif
