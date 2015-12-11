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

#include "mpack-cxx-helpers.h"



/*
 * Exceptions
 */

#if MPACK_CXX_EXCEPTIONS

#if MPACK_READER || MPACK_WRITER || MPACK_NODE
template <class T>
static void mpack_cxx_throw_error(T* object, mpack_error_t error) {
    throw error;
}
#endif

#if MPACK_READER
void mpack_reader_enable_throw(mpack_reader_t* reader) {
    if (reader->error != mpack_ok)
        throw reader->error;
    mpack_reader_set_error_handler(reader, &mpack_cxx_throw_error<mpack_reader_t>);
}
#endif

#if MPACK_WRITER
void mpack_writer_enable_throw(mpack_writer_t* writer) {
    if (writer->error != mpack_ok)
        throw writer->error;
    mpack_writer_set_error_handler(writer, &mpack_cxx_throw_error<mpack_writer_t>);
}
#endif

#if MPACK_NODE
void mpack_tree_enable_throw(mpack_tree_t* tree) {
    if (tree->error != mpack_ok)
        throw tree->error;
    mpack_tree_set_error_handler(tree, &mpack_cxx_throw_error<mpack_tree_t>);
}
#endif

#endif



/*
 * STL
 */

#if MPACK_CXX_STL

#if MPACK_READER

template <class B>
void mpack_read_into_tvector(mpack_reader_t* reader, size_t count, std::vector<B>& out) {
    if (mpack_reader_error(reader) != mpack_ok)
        return;

    size_t start = buffer.size();
    buffer.resize(start + count);
    mpack_read_bytes(reader, &buffer[start], count);

    if (mpack_reader_error(reader) != mpack_ok)
        out.resize(start);
}

template <class T>
std::vector<B> mpack_read_tvector(mpack_reader_t* reader, size_t count) {
    std::vector<B> data;
    mpack_read_into_vector(reader, count, data);

    // mpack_read_into_vector() will truncate the vector if an
    // error occurs without exceptions, so we can just return it.
    return data;
}

void mpack_read_into_vector (mpack_reader_t* reader, size_t count, std::vector<char>&    out) {return mpack_read_into_tvector(reader, count, out);}
void mpack_read_into_uvector(mpack_reader_t* reader, size_t count, std::vector<uint8_t>& out) {return mpack_read_into_tvector(reader, count, out);}
void mpack_read_into_ivector(mpack_reader_t* reader, size_t count, std::vector<int8_t>&  out) {return mpack_read_into_tvector(reader, count, out);}

std::vector<char>    mpack_read_vector (mpack_reader_t* reader, size_t count) {return mpack_read_tvector(reader, count);}
std::vector<uint8_t> mpack_read_uvector(mpack_reader_t* reader, size_t count) {return mpack_read_tvector(reader, count);}
std::vector<int8_t>  mpack_read_ivector(mpack_reader_t* reader, size_t count) {return mpack_read_tvector(reader, count);}

void mpack_read_into_string(mpack_reader_t* reader, size_t count, std::string& out) {
    size_t original_length = out.length();

    char buf[128];
    while (count > 0) {
        size_t step = (count < sizeof(buf)) ? count : sizeof(buf);
        mpack_read_bytes(reader, buf, sizeof(buf));

        if (mpack_reader_error(reader) != mpack_ok) {
            out.resize(original_length);
            return;
        }

        out.append(buf, step);
    }
}

std::string mpack_read_string(mpack_reader_t* reader, size_t count) {
    std::string str;
    mpack_read_into_string(reader, count, str);

    // mpack_read_into_string() will truncate the string if an
    // error occurs without exceptions, so we can just return it.
    return data;
}

#endif

#if MPACK_EXPECT
std::string mpack_expect_string(mpack_reader_t* reader) {
    uint32_t count = mpack_expect_str(reader);
    std::string str = mpack_read_str_string(reader);
    mpack_done_str(reader);
    return str;
}

std::vector<char> mpack_expect_bin_vector(mpack_reader_t* reader) {
    uint32_t count = mpack_expect_bin(reader);
    std::vector<char> data(count);
    // TODO no, read in place
    mpack_read_bytes(reader, &buffer[start], count);
    mpack_done_bin(reader);
    return data;
}
#endif

#if MPACK_NODE
std::string mpack_node_string(mpack_node_t* node) {
    size_t size = mpack_node_strlen(node);
    const char* data = mpack_node_data(node);
    if (!data)
        return std::string();
    return std::string(data, size);
}

std::vector<char> mpack_node_bin_vector(mpack_node_t* node) {
    uint32_t count = mpack_node_bin(node);
    std::vector<char> data(count);
    // TODO no
    mpack_read_bytes(node, &buffer[start], count);
    mpack_done_bin(node);
    return data;
}
#endif

#if MPACK_WRITER
void mpack_write_str(mpack_writer_t* writer, const std::string& str) {
    mpack_write_str(writer, str.data(), str.size());
}

void mpack_write_bin(mpack_writer_t* writer, const std::vector<char>& data) {
    mpack_write_bin(writer, data.data(), data.size());
}

void mpack_write_bin(mpack_writer_t* writer, const std::vector<int8_t>& data) {
    mpack_write_bin(writer, static_cast<const char*>(data.data()), data.size());
}

void mpack_write_bin(mpack_writer_t* writer, const std::vector<uint8_t>& data) {
    mpack_write_bin(writer, static_cast<const char*>(data.data()), data.size());
}
#endif

#endif

#endif
