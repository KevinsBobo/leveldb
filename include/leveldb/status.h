// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// A Status encapsulates the result of an operation.  It may indicate success,
// or it may indicate an error with an associated error message.
//
// 封装了一个操作完成后返回的状态信息。它可以表示执行成功，也可以表示执行失败
// 并包含一条错误信息。
//
// Multiple threads can invoke const methods on a Status without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Status must use
// external synchronization.
//
// 多线程可以调用一个Status的const方法，而不必使用外部同步。但如果一个线程调用
// 的是non-const方法，那其他线程想要访问这个相同的Status就必须使用外部同步。

#ifndef STORAGE_LEVELDB_INCLUDE_STATUS_H_
#define STORAGE_LEVELDB_INCLUDE_STATUS_H_

#include <string>
#include "leveldb/slice.h"

namespace leveldb {

class Status {
 public:
  // Create a success status.
  // 创建一个表示操作成功的Status
  Status() : state_(NULL) { }
  ~Status() { delete[] state_; }

  // Copy the specified status.
  // 拷贝一个特定的Status
  Status(const Status& s);
  void operator=(const Status& s);

  // Return a success status.
  // 返回一个表示操作成功的Status
  static Status OK() { return Status(); }

  // Return error status of an appropriate type.
  // 返回适当的错误Status类型
  static Status NotFound(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kNotFound, msg, msg2);
  }
  static Status Corruption(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kCorruption, msg, msg2);
  }
  static Status NotSupported(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kNotSupported, msg, msg2);
  }
  static Status InvalidArgument(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kInvalidArgument, msg, msg2);
  }
  static Status IOError(const Slice& msg, const Slice& msg2 = Slice()) {
    return Status(kIOError, msg, msg2);
  }

  // Returns true iff the status indicates success.
  // 返回操作的执行结果，操作成功返回ture
  bool ok() const { return (state_ == NULL); }

  // Returns true iff the status indicates a NotFound error.
  // 如果是NotFound错误就返回ture
  bool IsNotFound() const { return code() == kNotFound; }

  // Returns true iff the status indicates a Corruption error.
  // 如果是Corruption错误就返回ture
  bool IsCorruption() const { return code() == kCorruption; }

  // Returns true iff the status indicates an IOError.
  // 如果是IOError就返回ture
  bool IsIOError() const { return code() == kIOError; }

  // Returns true iff the status indicates a NotSupportedError.
  // 如果是NotSupported错误就返回ture
  bool IsNotSupportedError() const { return code() == kNotSupported; }

  // Returns true iff the status indicates an InvalidArgument.
  // 如果是InvalidArgument就返回ture
  bool IsInvalidArgument() const { return code() == kInvalidArgument; }

  // Return a string representation of this status suitable for printing.
  // 返回一个适合打印的string类型的状态说明
  // Returns the string "OK" for success.
  // 如果返回的是"OK"说明执行成功
  std::string ToString() const;

 private:
  // OK status has a NULL state_.  Otherwise, state_ is a new[] array
  // of the following form:
  // OK状态包含一个空的state_指针。不然的话state_指针指向的是一个
  // 新的数组，该数组包含的内容如下：
  //    state_[0..3] == length of message
  //    state_[4]    == code
  //    state_[5..]  == message
  const char* state_;

  enum Code {
    kOk = 0,
    kNotFound = 1,
    kCorruption = 2,
    kNotSupported = 3,
    kInvalidArgument = 4,
    kIOError = 5
  };

  Code code() const {
    return (state_ == NULL) ? kOk : static_cast<Code>(state_[4]);
  }


  Status(Code code, const Slice& msg, const Slice& msg2);
  static const char* CopyState(const char* s);
};

inline Status::Status(const Status& s) {
  state_ = (s.state_ == NULL) ? NULL : CopyState(s.state_);
}

// 重载赋值操作符
inline void Status::operator=(const Status& s) {
  // The following condition catches both aliasing (when this == &s),
  // 下面的判断条件也捕获两个别名，即（当 this == &s），
  // and the common case where both s and *this are ok.
  // 通常用于s和*this都是ok的情况下。
  if (state_ != s.state_) {
    delete[] state_;
    state_ = (s.state_ == NULL) ? NULL : CopyState(s.state_);
  }
}

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_INCLUDE_STATUS_H_
