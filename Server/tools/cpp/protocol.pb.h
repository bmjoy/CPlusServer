// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: protocol.proto

#ifndef PROTOBUF_protocol_2eproto__INCLUDED
#define PROTOBUF_protocol_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace protobuf_protocol_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[2];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
void InitDefaultsinner_testImpl();
void InitDefaultsinner_test();
void InitDefaultstestImpl();
void InitDefaultstest();
inline void InitDefaults() {
  InitDefaultsinner_test();
  InitDefaultstest();
}
}  // namespace protobuf_protocol_2eproto
namespace test_protobuf {
class inner_test;
class inner_testDefaultTypeInternal;
extern inner_testDefaultTypeInternal _inner_test_default_instance_;
class test;
class testDefaultTypeInternal;
extern testDefaultTypeInternal _test_default_instance_;
}  // namespace test_protobuf
namespace test_protobuf {

enum test_enTest {
  test_enTest_DEFAULT = 0,
  test_enTest_TEST = 1,
  test_enTest_test_enTest_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  test_enTest_test_enTest_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool test_enTest_IsValid(int value);
const test_enTest test_enTest_enTest_MIN = test_enTest_DEFAULT;
const test_enTest test_enTest_enTest_MAX = test_enTest_TEST;
const int test_enTest_enTest_ARRAYSIZE = test_enTest_enTest_MAX + 1;

const ::google::protobuf::EnumDescriptor* test_enTest_descriptor();
inline const ::std::string& test_enTest_Name(test_enTest value) {
  return ::google::protobuf::internal::NameOfEnum(
    test_enTest_descriptor(), value);
}
inline bool test_enTest_Parse(
    const ::std::string& name, test_enTest* value) {
  return ::google::protobuf::internal::ParseNamedEnum<test_enTest>(
    test_enTest_descriptor(), name, value);
}
// ===================================================================

class inner_test : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:test_protobuf.inner_test) */ {
 public:
  inner_test();
  virtual ~inner_test();

  inner_test(const inner_test& from);

  inline inner_test& operator=(const inner_test& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  inner_test(inner_test&& from) noexcept
    : inner_test() {
    *this = ::std::move(from);
  }

  inline inner_test& operator=(inner_test&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const inner_test& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const inner_test* internal_default_instance() {
    return reinterpret_cast<const inner_test*>(
               &_inner_test_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(inner_test* other);
  friend void swap(inner_test& a, inner_test& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline inner_test* New() const PROTOBUF_FINAL { return New(NULL); }

  inner_test* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const inner_test& from);
  void MergeFrom(const inner_test& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(inner_test* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string strInnerTest = 2;
  void clear_strinnertest();
  static const int kStrInnerTestFieldNumber = 2;
  const ::std::string& strinnertest() const;
  void set_strinnertest(const ::std::string& value);
  #if LANG_CXX11
  void set_strinnertest(::std::string&& value);
  #endif
  void set_strinnertest(const char* value);
  void set_strinnertest(const char* value, size_t size);
  ::std::string* mutable_strinnertest();
  ::std::string* release_strinnertest();
  void set_allocated_strinnertest(::std::string* strinnertest);

  // int32 nInnerTest = 1;
  void clear_ninnertest();
  static const int kNInnerTestFieldNumber = 1;
  ::google::protobuf::int32 ninnertest() const;
  void set_ninnertest(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:test_protobuf.inner_test)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr strinnertest_;
  ::google::protobuf::int32 ninnertest_;
  mutable int _cached_size_;
  friend struct ::protobuf_protocol_2eproto::TableStruct;
  friend void ::protobuf_protocol_2eproto::InitDefaultsinner_testImpl();
};
// -------------------------------------------------------------------

class test : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:test_protobuf.test) */ {
 public:
  test();
  virtual ~test();

  test(const test& from);

  inline test& operator=(const test& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  test(test&& from) noexcept
    : test() {
    *this = ::std::move(from);
  }

  inline test& operator=(test&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const test& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const test* internal_default_instance() {
    return reinterpret_cast<const test*>(
               &_test_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    1;

  void Swap(test* other);
  friend void swap(test& a, test& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline test* New() const PROTOBUF_FINAL { return New(NULL); }

  test* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const test& from);
  void MergeFrom(const test& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(test* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  typedef test_enTest enTest;
  static const enTest DEFAULT =
    test_enTest_DEFAULT;
  static const enTest TEST =
    test_enTest_TEST;
  static inline bool enTest_IsValid(int value) {
    return test_enTest_IsValid(value);
  }
  static const enTest enTest_MIN =
    test_enTest_enTest_MIN;
  static const enTest enTest_MAX =
    test_enTest_enTest_MAX;
  static const int enTest_ARRAYSIZE =
    test_enTest_enTest_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  enTest_descriptor() {
    return test_enTest_descriptor();
  }
  static inline const ::std::string& enTest_Name(enTest value) {
    return test_enTest_Name(value);
  }
  static inline bool enTest_Parse(const ::std::string& name,
      enTest* value) {
    return test_enTest_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // repeated int32 nTestArray = 5;
  int ntestarray_size() const;
  void clear_ntestarray();
  static const int kNTestArrayFieldNumber = 5;
  ::google::protobuf::int32 ntestarray(int index) const;
  void set_ntestarray(int index, ::google::protobuf::int32 value);
  void add_ntestarray(::google::protobuf::int32 value);
  const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      ntestarray() const;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_ntestarray();

  // string strTest = 3;
  void clear_strtest();
  static const int kStrTestFieldNumber = 3;
  const ::std::string& strtest() const;
  void set_strtest(const ::std::string& value);
  #if LANG_CXX11
  void set_strtest(::std::string&& value);
  #endif
  void set_strtest(const char* value);
  void set_strtest(const char* value, size_t size);
  ::std::string* mutable_strtest();
  ::std::string* release_strtest();
  void set_allocated_strtest(::std::string* strtest);

  // .test_protobuf.inner_test innerTest = 4;
  bool has_innertest() const;
  void clear_innertest();
  static const int kInnerTestFieldNumber = 4;
  const ::test_protobuf::inner_test& innertest() const;
  ::test_protobuf::inner_test* release_innertest();
  ::test_protobuf::inner_test* mutable_innertest();
  void set_allocated_innertest(::test_protobuf::inner_test* innertest);

  // int32 nTest = 1;
  void clear_ntest();
  static const int kNTestFieldNumber = 1;
  ::google::protobuf::int32 ntest() const;
  void set_ntest(::google::protobuf::int32 value);

  // sint32 snTest = 2;
  void clear_sntest();
  static const int kSnTestFieldNumber = 2;
  ::google::protobuf::int32 sntest() const;
  void set_sntest(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:test_protobuf.test)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > ntestarray_;
  mutable int _ntestarray_cached_byte_size_;
  ::google::protobuf::internal::ArenaStringPtr strtest_;
  ::test_protobuf::inner_test* innertest_;
  ::google::protobuf::int32 ntest_;
  ::google::protobuf::int32 sntest_;
  mutable int _cached_size_;
  friend struct ::protobuf_protocol_2eproto::TableStruct;
  friend void ::protobuf_protocol_2eproto::InitDefaultstestImpl();
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// inner_test

// int32 nInnerTest = 1;
inline void inner_test::clear_ninnertest() {
  ninnertest_ = 0;
}
inline ::google::protobuf::int32 inner_test::ninnertest() const {
  // @@protoc_insertion_point(field_get:test_protobuf.inner_test.nInnerTest)
  return ninnertest_;
}
inline void inner_test::set_ninnertest(::google::protobuf::int32 value) {
  
  ninnertest_ = value;
  // @@protoc_insertion_point(field_set:test_protobuf.inner_test.nInnerTest)
}

// string strInnerTest = 2;
inline void inner_test::clear_strinnertest() {
  strinnertest_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& inner_test::strinnertest() const {
  // @@protoc_insertion_point(field_get:test_protobuf.inner_test.strInnerTest)
  return strinnertest_.GetNoArena();
}
inline void inner_test::set_strinnertest(const ::std::string& value) {
  
  strinnertest_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:test_protobuf.inner_test.strInnerTest)
}
#if LANG_CXX11
inline void inner_test::set_strinnertest(::std::string&& value) {
  
  strinnertest_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:test_protobuf.inner_test.strInnerTest)
}
#endif
inline void inner_test::set_strinnertest(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  strinnertest_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:test_protobuf.inner_test.strInnerTest)
}
inline void inner_test::set_strinnertest(const char* value, size_t size) {
  
  strinnertest_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:test_protobuf.inner_test.strInnerTest)
}
inline ::std::string* inner_test::mutable_strinnertest() {
  
  // @@protoc_insertion_point(field_mutable:test_protobuf.inner_test.strInnerTest)
  return strinnertest_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* inner_test::release_strinnertest() {
  // @@protoc_insertion_point(field_release:test_protobuf.inner_test.strInnerTest)
  
  return strinnertest_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void inner_test::set_allocated_strinnertest(::std::string* strinnertest) {
  if (strinnertest != NULL) {
    
  } else {
    
  }
  strinnertest_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), strinnertest);
  // @@protoc_insertion_point(field_set_allocated:test_protobuf.inner_test.strInnerTest)
}

// -------------------------------------------------------------------

// test

// int32 nTest = 1;
inline void test::clear_ntest() {
  ntest_ = 0;
}
inline ::google::protobuf::int32 test::ntest() const {
  // @@protoc_insertion_point(field_get:test_protobuf.test.nTest)
  return ntest_;
}
inline void test::set_ntest(::google::protobuf::int32 value) {
  
  ntest_ = value;
  // @@protoc_insertion_point(field_set:test_protobuf.test.nTest)
}

// sint32 snTest = 2;
inline void test::clear_sntest() {
  sntest_ = 0;
}
inline ::google::protobuf::int32 test::sntest() const {
  // @@protoc_insertion_point(field_get:test_protobuf.test.snTest)
  return sntest_;
}
inline void test::set_sntest(::google::protobuf::int32 value) {
  
  sntest_ = value;
  // @@protoc_insertion_point(field_set:test_protobuf.test.snTest)
}

// string strTest = 3;
inline void test::clear_strtest() {
  strtest_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& test::strtest() const {
  // @@protoc_insertion_point(field_get:test_protobuf.test.strTest)
  return strtest_.GetNoArena();
}
inline void test::set_strtest(const ::std::string& value) {
  
  strtest_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:test_protobuf.test.strTest)
}
#if LANG_CXX11
inline void test::set_strtest(::std::string&& value) {
  
  strtest_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:test_protobuf.test.strTest)
}
#endif
inline void test::set_strtest(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  strtest_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:test_protobuf.test.strTest)
}
inline void test::set_strtest(const char* value, size_t size) {
  
  strtest_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:test_protobuf.test.strTest)
}
inline ::std::string* test::mutable_strtest() {
  
  // @@protoc_insertion_point(field_mutable:test_protobuf.test.strTest)
  return strtest_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* test::release_strtest() {
  // @@protoc_insertion_point(field_release:test_protobuf.test.strTest)
  
  return strtest_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void test::set_allocated_strtest(::std::string* strtest) {
  if (strtest != NULL) {
    
  } else {
    
  }
  strtest_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), strtest);
  // @@protoc_insertion_point(field_set_allocated:test_protobuf.test.strTest)
}

// .test_protobuf.inner_test innerTest = 4;
inline bool test::has_innertest() const {
  return this != internal_default_instance() && innertest_ != NULL;
}
inline void test::clear_innertest() {
  if (GetArenaNoVirtual() == NULL && innertest_ != NULL) {
    delete innertest_;
  }
  innertest_ = NULL;
}
inline const ::test_protobuf::inner_test& test::innertest() const {
  const ::test_protobuf::inner_test* p = innertest_;
  // @@protoc_insertion_point(field_get:test_protobuf.test.innerTest)
  return p != NULL ? *p : *reinterpret_cast<const ::test_protobuf::inner_test*>(
      &::test_protobuf::_inner_test_default_instance_);
}
inline ::test_protobuf::inner_test* test::release_innertest() {
  // @@protoc_insertion_point(field_release:test_protobuf.test.innerTest)
  
  ::test_protobuf::inner_test* temp = innertest_;
  innertest_ = NULL;
  return temp;
}
inline ::test_protobuf::inner_test* test::mutable_innertest() {
  
  if (innertest_ == NULL) {
    innertest_ = new ::test_protobuf::inner_test;
  }
  // @@protoc_insertion_point(field_mutable:test_protobuf.test.innerTest)
  return innertest_;
}
inline void test::set_allocated_innertest(::test_protobuf::inner_test* innertest) {
  ::google::protobuf::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == NULL) {
    delete innertest_;
  }
  if (innertest) {
    ::google::protobuf::Arena* submessage_arena = NULL;
    if (message_arena != submessage_arena) {
      innertest = ::google::protobuf::internal::GetOwnedMessage(
          message_arena, innertest, submessage_arena);
    }
    
  } else {
    
  }
  innertest_ = innertest;
  // @@protoc_insertion_point(field_set_allocated:test_protobuf.test.innerTest)
}

// repeated int32 nTestArray = 5;
inline int test::ntestarray_size() const {
  return ntestarray_.size();
}
inline void test::clear_ntestarray() {
  ntestarray_.Clear();
}
inline ::google::protobuf::int32 test::ntestarray(int index) const {
  // @@protoc_insertion_point(field_get:test_protobuf.test.nTestArray)
  return ntestarray_.Get(index);
}
inline void test::set_ntestarray(int index, ::google::protobuf::int32 value) {
  ntestarray_.Set(index, value);
  // @@protoc_insertion_point(field_set:test_protobuf.test.nTestArray)
}
inline void test::add_ntestarray(::google::protobuf::int32 value) {
  ntestarray_.Add(value);
  // @@protoc_insertion_point(field_add:test_protobuf.test.nTestArray)
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
test::ntestarray() const {
  // @@protoc_insertion_point(field_list:test_protobuf.test.nTestArray)
  return ntestarray_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
test::mutable_ntestarray() {
  // @@protoc_insertion_point(field_mutable_list:test_protobuf.test.nTestArray)
  return &ntestarray_;
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace test_protobuf

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::test_protobuf::test_enTest> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::test_protobuf::test_enTest>() {
  return ::test_protobuf::test_enTest_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_protocol_2eproto__INCLUDED