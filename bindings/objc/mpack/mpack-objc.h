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
 * Defines the Objective-C bindings to MPack.
 */

#ifndef MPACK_OBJC_H
#define MPACK_OBJC_H 1

#include "mpack.h"

#ifdef __OBJC__
#import <Foundation/Foundation.h>

#if MPACK_READER
@interface NSData (MPackDecoding)

/**
 * Decodes MessagePack data into Objective-C Foundation types.
 *
 * If a MessagePack extension type object is encountered, it is treated as
 * an error. (There currently isn't support for registering ext types.)
 *
 * @return An NSDictionary, NSArray, NSString, NSData, NSNumber, NSNull, or nil if decoding failed.
 */
- (id)decodeMPack;

/**
 * Decodes MessagePack data into Objective-C Foundation types.
 *
 * If a MessagePack extension type object is encountered, it is treated as
 * an error. (There currently isn't support for registering ext types.)
 *
 * @param [out] error The error flag to set if an error occurred, or nil to ignore it.
 * @return An NSDictionary, NSArray, NSString, NSData, NSNumber, NSNull, or nil if decoding failed.
 */
- (id)decodeMPackWithError:(mpack_error_t*)error;
@end

/**
 * Reads an element (and its children) from an MPack reader into Objective-C Foundation types.
 *
 * If a MessagePack extension type object is encountered, it is treated as
 * an error. (There currently isn't support for registering ext types.)
 *
 * @param The reader from which to read an object
 * @return An NSDictionary, NSArray, NSString, NSData, NSNumber, NSNull, or nil if reading failed.
 */
MPACK_EXTERN_C id mpack_read_objc(mpack_reader_t* reader);
#endif

#if MPACK_WRITER
@interface NSObject (MPackEncoding)

/**
 * Encodes an Objective-C object (and its children) into an NSData.
 *
 * The object must have -writeMPack implemented. This is implemented for
 * the basic Foundation types: NSDictionary, NSArray, NSString, NSData,
 * NSNumber and NSNull. You can implement it on your own types to easily
 * convert them to MessagePack.
 *
 * You should not override this method. Override -writeMPack: instead.
 *
 * @return An NSDictionary, NSArray, NSString, NSData, NSNumber, NSNull, or nil if encoding failed.
 */
- (NSData*)encodeMPack;

/**
 * Encodes an Objective-C object (and its children) into an NSData.
 *
 * The object must have -writeMPack implemented. This is implemented for
 * the basic Foundation types: NSDictionary, NSArray, NSString, NSData,
 * NSNumber and NSNull. You can implement it on your own types to easily
 * convert them to MessagePack.
 *
 * You should not override this method. Override -writeMPack: instead.
 *
 * @param [out] error The error flag to set if an error occurred, or nil to ignore it.
 * @return An NSDictionary, NSArray, NSString, NSData, NSNumber, NSNull, or nil if encoding failed.
 */
- (NSData*)encodeMPackWithError:(mpack_error_t*)error;

/**
 * Writes an Objective-C object (and its children) to an MPack writer.
 *
 * This method is implemented for the basic Foundation types: NSDictionary,
 * NSArray, NSString, NSData, NSNumber and NSNull. You can implement this method on
 * your own types to easily convert them to MessagePack with any of the
 * other encode and write APIs.
 */
- (void)writeMPack:(mpack_writer_t*)writer;
@end

/**
 * Writes an Objective-C object (and its children.)
 *
 * The object must have -writeMPack implemented. This is implemented for
 * the basic Foundation types: NSDictionary, NSArray, NSString, NSData,
 * NSNumber and NSNull. You can implement it on your own types to easily
 * convert them to MessagePack.
 */
static inline void mpack_write_objc(mpack_writer_t* writer, id obj) {
    [(NSObject*)obj writeMPack:writer];
}
#endif

#endif
#endif
