// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
// 中文注释：KevinsBobo http://kevins.pro
//
// Endian-neutral encoding:
// * Fixed-length numbers are encoded with least-significant byte first
// * 固定长度的数字按照小端序的方式排序
// * In addition we support variable length "varint" encoding
// * 支持"varint"压缩编码
// * Strings are encoded prefixed by their length in varint format
// * 在varint编码的字符串中以编码长度作为字符串前缀

#ifndef STORAGE_LEVELDB_UTIL_CODING_H_
#define STORAGE_LEVELDB_UTIL_CODING_H_

#include <stdint.h>
#include <string.h>
#include <string>
#include "leveldb/slice.h"
#include "port/port.h"

namespace leveldb {

// Standard Put... routines append to a string
// 存放函数，将处理后的编码添加到字符串中
extern void PutFixed32(std::string* dst, uint32_t value);
extern void PutFixed64(std::string* dst, uint64_t value);
extern void PutVarint32(std::string* dst, uint32_t value);
extern void PutVarint64(std::string* dst, uint64_t value);
extern void PutLengthPrefixedSlice(std::string* dst, const Slice& value);

// Standard Get... routines parse a value from the beginning of a Slice
// 获取函数，从一个Slice对象中解析数据存放在value中
// and advance the slice past the parsed value.
// 并将Sliced对象指针移动到已解析值的后面
extern bool GetVarint32(Slice* input, uint32_t* value);
extern bool GetVarint64(Slice* input, uint64_t* value);
extern bool GetLengthPrefixedSlice(Slice* input, Slice* result);

// Pointer-based variants of GetVarint...  These either store a value
// GetVarint函数基于指针的变体。他们都是用来存储一个值
// in *v and return a pointer just past the parsed value, or return
// 在指针v中存储一个解析过的数据并返回指针，如果返回NULL说明发生错误
// NULL on error.  These routines only look at bytes in the range
// 这些函数只处理范围内的数据，范围是指针p到limit - 1；
// [p..limit-1]
extern const char* GetVarint32Ptr(const char* p,const char* limit, uint32_t* v);
extern const char* GetVarint64Ptr(const char* p,const char* limit, uint64_t* v);

// Returns the length of the varint32 or varint64 encoding of "v"
// 返回"v"的varint32或varint64编码长度
extern int VarintLength(uint64_t v);

// Lower-level versions of Put... that write directly into a character buffer
// Put函数的底层版本。直接把处理后的编码写进字符数组
// REQUIRES: dst has enough space for the value being written
// 要求：字符数组要有足够的空间
extern void EncodeFixed32(char* dst, uint32_t value);
extern void EncodeFixed64(char* dst, uint64_t value);

// Lower-level versions of Put... that write directly into a character buffer
// Put函数的底层版本。直接把处理后的编码写进字符数组
// and return a pointer just past the last byte written.
// 并且返回字符数组中写入最后一个字符的字节后的指针
// REQUIRES: dst has enough space for the value being written
// 要求：字符数组要有足够的空间
extern char* EncodeVarint32(char* dst, uint32_t value);
extern char* EncodeVarint64(char* dst, uint64_t value);

// Lower-level versions of Get... that read directly from a character buffer
// Get函数的底层版本。从字符数组中读取并解析编码
// without any bounds checking.
// 没有边界检查

inline uint32_t DecodeFixed32(const char* ptr) {
  if (port::kLittleEndian) {
    // Load the raw bytes
    uint32_t result;
    memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
    return result;
  } else {
    return ((static_cast<uint32_t>(static_cast<unsigned char>(ptr[0])))
        | (static_cast<uint32_t>(static_cast<unsigned char>(ptr[1])) << 8)
        | (static_cast<uint32_t>(static_cast<unsigned char>(ptr[2])) << 16)
        | (static_cast<uint32_t>(static_cast<unsigned char>(ptr[3])) << 24));
  }
}

inline uint64_t DecodeFixed64(const char* ptr) {
  if (port::kLittleEndian) {
    // Load the raw bytes
    uint64_t result;
    memcpy(&result, ptr, sizeof(result));  // gcc optimizes this to a plain load
    return result;
  } else {
    uint64_t lo = DecodeFixed32(ptr);
    uint64_t hi = DecodeFixed32(ptr + 4);
    return (hi << 32) | lo;
  }
}

// Internal routine for use by fallback path of GetVarint32Ptr
// GetVarint32Ptr函数内部使用的解析编码的完整功能版本
extern const char* GetVarint32PtrFallback(const char* p,
                                          const char* limit,
                                          uint32_t* value);
inline const char* GetVarint32Ptr(const char* p,
                                  const char* limit,
                                  uint32_t* value) {
  if (p < limit) {
    uint32_t result = *(reinterpret_cast<const unsigned char*>(p));
    if ((result & 128) == 0) {
      *value = result;
      return p + 1;
    }
  }
  return GetVarint32PtrFallback(p, limit, value);
}

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_UTIL_CODING_H_
