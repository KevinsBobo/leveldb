// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Slice is a simple structure containing a pointer into some external
// storage and a size.  The user of a Slice must ensure that the slice
// is not used after the corresponding external storage has been
// deallocated.
//
// Slice是一个简单的结构，它包含了一个指向外部存储的指针和一个大小值。
// Slice的使用者必须确保Slice指向的外部存储没有被销毁。
//
// Multiple threads can invoke const methods on a Slice without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Slice must use
// external synchronization.
//
// 多线程可以在Slice上调用const方法而不依赖其他的外部同步，但如果一个线程
// 调用的是non-const方法，那么其他线程访问这个Slice时就必须使用外部同步。

#ifndef STORAGE_LEVELDB_INCLUDE_SLICE_H_
#define STORAGE_LEVELDB_INCLUDE_SLICE_H_

#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <string>

namespace leveldb {

class Slice {
 public:
  // 针对不同类型参数的构造函数
  // Create an empty slice.
  Slice() : data_(""), size_(0) { }

  // Create a slice that refers to d[0,n-1].
  Slice(const char* d, size_t n) : data_(d), size_(n) { }

  // Create a slice that refers to the contents of "s"
  Slice(const std::string& s) : data_(s.data()), size_(s.size()) { }

  // Create a slice that refers to s[0,strlen(s)-1]
  Slice(const char* s) : data_(s), size_(strlen(s)) { }

  // Return a pointer to the beginning of the referenced data
  // 返回一个指向引用的数据开头的指针
  const char* data() const { return data_; }

  // Return the length (in bytes) of the referenced data
  // 返回引用的数据的长度
  size_t size() const { return size_; }

  // Return true iff the length of the referenced data is zero
  // 如果数据的长度为0则返回ture
  bool empty() const { return size_ == 0; }

  // Return the ith byte in the referenced data.
  // 重载[]操作符，返回引用数据中第n个下标指向的数据
  // REQUIRES: n < size()
  // 警告：n必须小于size()的返回值
  char operator[](size_t n) const {
    assert(n < size());
    return data_[n];
  }

  // Change this slice to refer to an empty array
  // 将这个Slice指向一个内容为空的数组
  void clear() { data_ = ""; size_ = 0; }

  // Drop the first "n" bytes from this slice.
  // 丢弃data中的前n个数据内容
  void remove_prefix(size_t n) {
    assert(n <= size());
    data_ += n;
    size_ -= n;
  }

  // Return a string that contains the copy of the referenced data.
  // 将data中的内容作为string类型返回
  std::string ToString() const { return std::string(data_, size_); }

  // Three-way comparison.  Returns value:
  // 比较函数，有三种返回方式代表不同的结果
  //   <  0 iff "*this" <  "b",
  //   == 0 iff "*this" == "b",
  //   >  0 iff "*this" >  "b"
  int compare(const Slice& b) const;

  // Return true iff "x" is a prefix of "*this"
  // x的内容是当前对象的内容的前缀时返回ture
  bool starts_with(const Slice& x) const {
    return ((size_ >= x.size_) &&
            (memcmp(data_, x.data_, x.size_) == 0));
  }

 private:
  const char* data_;
  size_t size_;

  // Intentionally copyable
};

// 重载==和!=运算符
inline bool operator==(const Slice& x, const Slice& y) {
  return ((x.size() == y.size()) &&
          (memcmp(x.data(), y.data(), x.size()) == 0));
}

inline bool operator!=(const Slice& x, const Slice& y) {
  return !(x == y);
}

inline int Slice::compare(const Slice& b) const {
  const size_t min_len = (size_ < b.size_) ? size_ : b.size_;
  int r = memcmp(data_, b.data_, min_len);
  if (r == 0) {
    if (size_ < b.size_) r = -1;
    else if (size_ > b.size_) r = +1;
  }
  return r;
}

}  // namespace leveldb


#endif  // STORAGE_LEVELDB_INCLUDE_SLICE_H_
