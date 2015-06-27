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

#import "mpack-objc.h"


// Toll-free bridging doesn't currently work on GNUstep
#ifndef GNUSTEP_BASE_VERSION
#define MPACK_OBJC_TOLL_FREE_BRIDGE 1
#endif

#if MPACK_OBJC_TOLL_FREE_BRIDGE
#import <CoreFoundation/CoreFoundation.h>
#endif


// We support both ARC and manual refcounting
#if __has_feature(objc_arc)
#define MPACK_OBJC_RELEASE(x)     ((void)(x))
#define MPACK_OBJC_AUTORELEASE(x) (x)
#else
#define MPACK_OBJC_RELEASE(x)     ((void)[(x) release])
#define MPACK_OBJC_AUTORELEASE(x) ([(x) autorelease])
#endif


#define MPACK_OBJC_SMALL_COMPOUND 16


#if MPACK_READER

@implementation NSData (MPackDecoding)

- (id)decodeMPack {
    return [self decodeMPackWithError:NULL];
}

- (id)decodeMPackWithError:(mpack_error_t*)error {
    if (error)
        *error = mpack_error_bug;

    mpack_reader_t reader;
    mpack_reader_init_data(&reader, (const char*)[self bytes], (size_t)[self length]);

    id obj = mpack_read_objc(&reader);

    mpack_error_t ret = mpack_reader_destroy(&reader);
    if (error)
        *error = ret;
    if (ret != mpack_ok)
        obj = nil;

    return obj;
}

@end

static NSString* mpack_read_objc_str(mpack_reader_t* reader, mpack_tag_t tag) {
    NSString* str;

    // small string
    if (mpack_should_read_bytes_inplace(reader, tag.v.l)) {
        const char* bytes = mpack_read_bytes_inplace(reader, tag.v.l);
        if (mpack_reader_error(reader) != mpack_ok)
            return nil;
        str = MPACK_OBJC_AUTORELEASE([[NSString alloc] initWithBytes:(const void*)bytes length:tag.v.l encoding:NSUTF8StringEncoding]);

    // big string
    } else {
        NSMutableData* mdata = [NSMutableData dataWithLength:tag.v.l];
        void* bytes = [mdata mutableBytes];
        mpack_read_bytes(reader, (char*)bytes, tag.v.l);
        if (mpack_reader_error(reader) != mpack_ok)
            return nil;
        str = MPACK_OBJC_AUTORELEASE([[NSString alloc] initWithBytes:bytes length:tag.v.l encoding:NSUTF8StringEncoding]);
    }

    mpack_done_bin(reader);
    return str;
}

static NSData* mpack_read_objc_bin(mpack_reader_t* reader, mpack_tag_t tag) {
    NSData* data;

    // small data
    if (mpack_should_read_bytes_inplace(reader, tag.v.l)) {
        const char* bytes = mpack_read_bytes_inplace(reader, tag.v.l);
        if (mpack_reader_error(reader) != mpack_ok)
            return nil;
        data = [NSData dataWithBytes:(const void*)bytes length:tag.v.l];

    // big data
    } else {
        NSMutableData* mdata = [NSMutableData dataWithLength:tag.v.l];
        mpack_read_bytes(reader, (char*)[mdata mutableBytes], tag.v.l);
        if (mpack_reader_error(reader) != mpack_ok)
            return nil;
        data = mdata;
    }

    mpack_done_bin(reader);
    return data;
}

static id mpack_read_objc_ext(mpack_reader_t* reader, mpack_tag_t tag) {

    // We don't currently support ext types. If there's a need for this,
    // we could implement some system where classes can be registered as
    // ext types, and we initialize them with the reader and tag.

    MPACK_UNUSED(tag);
    mpack_break("data contains an ext element of exttype %i length %i", (int)tag.exttype, (int)tag.v.l);
    mpack_reader_flag_error(reader, mpack_error_type);
    return nil;
}

static NSArray* mpack_read_objc_array(mpack_reader_t* reader, mpack_tag_t tag) {
    NSArray* array;

    // small array
    if (tag.v.n <= MPACK_OBJC_SMALL_COMPOUND) {
        id elements[MPACK_OBJC_SMALL_COMPOUND];
        for (size_t i = 0; i < tag.v.n; ++i) {
            elements[i] = mpack_read_objc(reader);
            if (mpack_reader_error(reader) != mpack_ok)
                return nil;
            mpack_assert(elements[i], "element %i is nil but reader did not flag an error!", (int)i);
        }
        array = [NSArray arrayWithObjects:elements count:tag.v.n];

    // big array
    } else {
        NSMutableArray* marray = [[NSMutableArray alloc] initWithCapacity:tag.v.n];
        for (size_t i = 0; i < tag.v.n; ++i) {
            id element = mpack_read_objc(reader);
            if (mpack_reader_error(reader) != mpack_ok) {
                MPACK_OBJC_RELEASE(marray);
                return nil;
            }
            mpack_assert(element, "element %i is nil but reader did not flag an error!", (int)i);
            [marray addObject:element];
        }
        array = [NSArray arrayWithArray:marray];
        MPACK_OBJC_RELEASE(marray);
    }

    mpack_done_array(reader);
    return array;
}

static NSDictionary* mpack_read_objc_map(mpack_reader_t* reader, mpack_tag_t tag) {
    NSDictionary* dict;

    // small map
    if (tag.v.n <= MPACK_OBJC_SMALL_COMPOUND) {
        id keys[MPACK_OBJC_SMALL_COMPOUND];
        id values[MPACK_OBJC_SMALL_COMPOUND];
        for (size_t i = 0; i < tag.v.n; ++i) {
            keys[i] = mpack_read_objc(reader);
            values[i] = mpack_read_objc(reader);
            if (mpack_reader_error(reader) != mpack_ok)
                return nil;
            mpack_assert(keys[i], "key %i is nil but reader did not flag an error!", (int)i);
            mpack_assert(values[i], "value %i is nil but reader did not flag an error!", (int)i);
        }
        dict = [NSDictionary dictionaryWithObjects:values forKeys:keys count:tag.v.n];

    // big map
    } else {
        NSMutableDictionary* mdict = [[NSMutableDictionary alloc] initWithCapacity:tag.v.n];
        for (size_t i = 0; i < tag.v.n; ++i) {
            id key = mpack_read_objc(reader);
            id value = mpack_read_objc(reader);
            if (mpack_reader_error(reader) != mpack_ok) {
                MPACK_OBJC_RELEASE(mdict);
                return nil;
            }
            mpack_assert(key, "key %i is nil but reader did not flag an error!", (int)i);
            mpack_assert(value, "value %i is nil but reader did not flag an error!", (int)i);
            [mdict setObject:value forKey:key];
        }
        dict = [NSDictionary dictionaryWithDictionary:mdict];
        MPACK_OBJC_RELEASE(mdict);
    }

    mpack_done_map(reader);
    return dict;
}

id mpack_read_objc(mpack_reader_t* reader) {
    mpack_tag_t tag = mpack_read_tag(reader);
    if (mpack_reader_error(reader) != mpack_ok)
        return nil;

    switch (tag.type) {
        case mpack_type_nil:    return [NSNull null];
        case mpack_type_bool:   return tag.v.b ? @(YES) : @(NO);
        case mpack_type_int:    return @(tag.v.i);
        case mpack_type_uint:   return @(tag.v.u);
        case mpack_type_float:  return @(tag.v.f);
        case mpack_type_double: return @(tag.v.d);
        case mpack_type_str:    return mpack_read_objc_str(reader, tag);
        case mpack_type_bin:    return mpack_read_objc_bin(reader, tag);
        case mpack_type_ext:    return mpack_read_objc_ext(reader, tag);
        case mpack_type_array:  return mpack_read_objc_array(reader, tag);
        case mpack_type_map:    return mpack_read_objc_map(reader, tag);
        default:
            break;
    }

    mpack_assert(0, "unrecognized tag type %i", (int)tag.type);
    mpack_reader_flag_error(reader, mpack_error_bug);
    return nil;
}

#endif


#if MPACK_WRITER

@implementation NSObject (MPackEncoding)

- (NSData*)encodeMPack {
    return [self encodeMPackWithError:NULL];
}

- (NSData*)encodeMPackWithError:(mpack_error_t*)error {
    if (error)
        *error = mpack_error_bug;

    // initialize a growable writer
    mpack_writer_t writer;
    char* data;
    size_t size;
    mpack_writer_init_growable(&writer, &data, &size);

    // write data
    [self writeMPack:&writer];

    // clean up
    mpack_error_t ret = mpack_writer_destroy(&writer);
    if (error)
        *error = ret;
    if (ret != mpack_ok)
        return nil;

    // convert to NSData
    // TODO: We don't know that MPACK_MALLOC is actually malloc, so for now we just
    // copy it. This should probably be implemented with [NSMutableData -appendBytes:].
    NSData* nsdata = [NSData dataWithBytes:data length:size];
    MPACK_FREE(data);
    if (!nsdata && error)
        *error = mpack_error_memory;
    return nsdata;
}

- (void)writeMPack:(mpack_writer_t*)writer {
    mpack_break("-writeMPack is not implemented on %s", [NSStringFromClass([self class]) UTF8String]);
    mpack_writer_flag_error(writer, mpack_error_bug);
}

@end

@interface NSDictionary (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer;
@end

@implementation NSDictionary (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer {
    mpack_start_map(writer, (uint32_t)[self count]);
    for (id key in self) {
        if (mpack_writer_error(writer) != mpack_ok) return;
        [key writeMPack:writer];
        [[self objectForKey:key] writeMPack:writer];
    }
    mpack_finish_map(writer);
}
@end

@interface NSArray (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer;
@end

@implementation NSArray (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer {
    mpack_start_array(writer, (uint32_t)[self count]);
    for (id element in self) {
        if (mpack_writer_error(writer) != mpack_ok) return;
        [element writeMPack:writer];
    }
    mpack_finish_array(writer);
}
@end

@interface NSString (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer;
@end

@implementation NSString (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer {
    // TODO length check uint32
    mpack_write_str(writer, [self UTF8String], (uint32_t)[self length]);
}
@end

@interface NSData (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer;
@end

@implementation NSData (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer {
    // TODO length check uint32
    mpack_write_bin(writer, (const char*)[self bytes], (uint32_t)[self length]);
}
@end

@interface NSNull (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer;
@end

@implementation NSNull (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer {
    mpack_write_nil(writer);
}
@end

@interface NSNumber (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer;
@end

@implementation NSNumber (MPackEncoding)
- (void)writeMPack:(mpack_writer_t*)writer {

    // Boolean values are implemented as singletons in Objective-C.
    if ((id)self == (id)@(YES)) {
        mpack_write_true(writer);
        return;
    }
    if ((id)self == (id)@(NO)) {
        mpack_write_false(writer);
        return;
    }

    #if MPACK_OBJC_TOLL_FREE_BRIDGE
    CFNumberRef ref = (CFNumberRef)self;

    // Try to recognize the type exactly
    switch (CFNumberGetType(ref)) {
        case kCFNumberSInt8Type:
        case kCFNumberSInt16Type:
        case kCFNumberSInt32Type:
        case kCFNumberCharType:
        case kCFNumberShortType:
        case kCFNumberIntType:
            mpack_write_i32(writer, [self intValue]);
            return;

        case kCFNumberSInt64Type:
        case kCFNumberLongLongType:
            mpack_write_i64(writer, [self longLongValue]);
            return;

        case kCFNumberLongType:
            if (sizeof(long) == sizeof(int32_t)) {
                mpack_write_i32(writer, [self intValue]);
            } else {
                mpack_write_i64(writer, [self longLongValue]);
            }
            return;

        case kCFNumberNSIntegerType:
            if (sizeof(NSInteger) == sizeof(int32_t)) {
                mpack_write_i32(writer, [self intValue]);
            } else {
                mpack_write_i64(writer, [self longLongValue]);
            }
            return;

        case kCFNumberCFIndexType:
            if (sizeof(CFIndex) == sizeof(int32_t)) {
                mpack_write_i32(writer, [self intValue]);
            } else {
                mpack_write_i64(writer, [self longLongValue]);
            }
            return;

        case kCFNumberFloat32Type:
        case kCFNumberFloatType:
            mpack_write_float(writer, [self floatValue]);
            return;

        case kCFNumberFloat64Type:
        case kCFNumberDoubleType:
            mpack_write_double(writer, [self doubleValue]);
            return;

        case kCFNumberCGFloatType:
            if (sizeof(CGFloat) == sizeof(float)) {
                mpack_write_float(writer, [self floatValue]);
            } else {
                mpack_write_double(writer, [self doubleValue]);
            }
            return;

        default:
            break;
    }

    // Check if we're some special float type
    if (CFNumberIsFloatType(ref)) {
        mpack_write_double(writer, [self doubleValue]);
        return;
    }

    #else

    // Handle floats
    const char* type = [self objCType];
    if (strcmp(type, @encode(float)) == 0) {
        mpack_write_float(writer, [self floatValue]);
        return;
    }
    if (strcmp(type, @encode(double)) == 0) {
        mpack_write_double(writer, [self doubleValue]);
        return;
    }

    #endif

    // Fall back to a safe int writer. This allows us to handle numbers
    // in the range (INT64_MAX,UINT64_MAX] (since they are internally
    // stored as a 128-bit signed number). We just do an extra sign
    // check to make sure it's safe.
    if ([self compare:@(0)] == NSOrderedAscending) {
        mpack_write_i64(writer, [self longLongValue]);
        return;
    }
    mpack_write_u64(writer, [self unsignedLongLongValue]);
}
@end

#endif
