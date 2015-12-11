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
 * Defines C++ helpers
 */

#ifndef MPACK_CXX_HELPERS_H
#define MPACK_CXX_HELPERS_H 1

#include "mpack-cxx-platform.h"

#if defined(__cplusplus)



/*
 * Exception wrappers
 */

#if MPACK_CXX_EXCEPTIONS

#if MPACK_READER
/**
 * Enables throwing C++ exceptions of type mpack_error_t when an
 * exception occurs.
 *
 * @warning If the reader is already in an error state, this will
 * throw immediately! You should use this inside a try block with
 * a corresponding catch block of mpack_error_t.
 */
void mpack_reader_enable_throw(mpack_reader_t* reader);
#endif

#if MPACK_WRITER
/**
 * Enables throwing C++ exceptions of type mpack_error_t when an
 * exception occurs.
 *
 * @warning If the writer is already in an error state, this will
 * throw immediately! You should use this inside a try block with
 * a corresponding catch block of mpack_error_t.
 */
void mpack_writer_enable_throw(mpack_writer_t* writer);
#endif

#if MPACK_NODE
/**
 * Enables throwing C++ exceptions of type mpack_error_t when an
 * exception occurs.
 *
 * @warning If the tree is already in an error state, this will
 * throw immediately! You should use this inside a try block with
 * a corresponding catch block of mpack_error_t.
 */
void mpack_tree_enable_throw(mpack_tree_t* tree);
#endif

#endif



/*
 * STL helper functions
 */

#if MPACK_CXX_STL
#if MPACK_READER
/**
 * Reads count bytes from a string, binary blob or extension object, appending
 * them to the given vector.
 *
 * One of these types must have been opened for reading via mpack_read_tag()
 * or an expect function.
 *
 * If a vector allocation fails, std::bad_alloc is thrown (or the program will
 * crash if exceptions are disabled, since std::vector cannot handle allocation
 * failures without exceptions.) Make sure you limit count separately from
 * this call to avoid too large an allocation.
 *
 * If any MPack error occurs, the vector may have invalid data appended to it.
 *
 * @see mpack_read_bytes()
 * @see mpack_expect_str()
 * @see mpack_expect_bin()
 * @see mpack_expect_ext()
 */
void mpack_read_into_vector(mpack_reader_t* reader, size_t count, std::vector<char>& out);

/**
 * Reads count bytes from a string, binary blob or extension object, returning
 * a vector containing the bytes.
 *
 * One of these types must have been opened for reading via mpack_read_tag()
 * or an expect function.
 *
 * If a vector allocation fails, std::bad_alloc is thrown (or the program will
 * crash if exceptions are disabled, since std::vector cannot handle allocation
 * failures without exceptions.) Make sure you limit count separately from
 * this call to avoid too large an allocation.
 *
 * If any MPack error occurs, the returned vector will be empty.
 *
 * @see mpack_read_bytes()
 * @see mpack_expect_str()
 * @see mpack_expect_bin()
 * @see mpack_expect_ext()
 */
std::vector<char> mpack_read_vector(mpack_reader_t* reader, size_t count);

void mpack_read_into_ivector(mpack_reader_t* reader, size_t count, std::vector<int8_t>&  out); /**< @copydoc mpack_read_into_vector */
void mpack_read_into_uvector(mpack_reader_t* reader, size_t count, std::vector<uint8_t>& out); /**< @copydoc mpack_read_into_vector */
std::vector<int8_t>  mpack_read_ivector(mpack_reader_t* reader, size_t count); /**< @copydoc mpack_read_vector */
std::vector<uint8_t> mpack_read_uvector(mpack_reader_t* reader, size_t count); /**< @copydoc mpack_read_vector */

/**
 * Reads count bytes from a string, appending them to the given string.
 *
 * The string must have been opened for reading via mpack_read_tag() or
 * an expect function.
 *
 * If a string allocation fails, std::bad_alloc is thrown (or the program will
 * crash if exceptions are disabled, since std::string cannot handle allocation
 * failures without exceptions.) Make sure you limit count separately from
 * this call to avoid too large an allocation.
 *
 * If any MPack error occurs, the string may have invalid data appended to it.
 *
 * @see mpack_read_bytes()
 * @see mpack_expect_str()
 * @see mpack_expect_str_max()
 * @see mpack_expect_str_length()
 */
void mpack_read_into_string(mpack_reader_t* reader, size_t count, std::string& out);

/**
 * Reads count bytes from a string, returning a string containing the bytes.
 *
 * The string must have been opened for reading via mpack_read_tag() or
 * an expect function.
 *
 * If a string allocation fails, std::bad_alloc is thrown (or the program will
 * crash if exceptions are disabled, since std::string cannot handle allocation
 * failures without exceptions.) Make sure you limit count separately from
 * this call to avoid too large an allocation.
 *
 * If any MPack error occurs, the returned string will be empty.
 *
 * @see mpack_read_bytes()
 * @see mpack_expect_str()
 * @see mpack_expect_str_max()
 * @see mpack_expect_str_length()
 */
std::string mpack_read_string(mpack_reader_t* reader, size_t count);
#endif


#if MPACK_EXPECT
// TODO require max for these since they allocate
std::string mpack_expect_string(mpack_reader_t* reader);
std::vector<char>    mpack_expect_bin_vector(mpack_reader_t* reader);
std::vector<int8_t>  mpack_expect_bin_ivector(mpack_reader_t* reader);
std::vector<uint8_t> mpack_expect_bin_uvector(mpack_reader_t* reader);
#endif


#if MPACK_NODE
// TODO don't require max since these are already in memory
std::string mpack_node_string(mpack_node_t* node);
std::vector<char> mpack_node_bin_vector(mpack_node_t* node);
#endif


#if MPACK_WRITER
void mpack_write_str(mpack_writer_t* writer, const std::string& str);
void mpack_write_bin(mpack_writer_t* writer, const std::vector<char>& data);
void mpack_write_bin(mpack_writer_t* writer, const std::vector<int8_t>& data);
void mpack_write_bin(mpack_writer_t* writer, const std::vector<uint8_t>& data);
#endif
#endif



}

#endif

#endif



