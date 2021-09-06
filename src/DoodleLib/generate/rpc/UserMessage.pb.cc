// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: UserMessage.proto

#include "UserMessage.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
extern PROTOBUF_INTERNAL_EXPORT_google_2fprotobuf_2fany_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_Any_google_2fprotobuf_2fany_2eproto;
namespace doodle {
class user_databaseDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<user_database> _instance;
} _user_database_default_instance_;
class user_database_filterDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<user_database_filter> _instance;
} _user_database_filter_default_instance_;
}  // namespace doodle
static void InitDefaultsscc_info_user_database_UserMessage_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::doodle::_user_database_default_instance_;
    new (ptr) ::doodle::user_database();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_user_database_UserMessage_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_user_database_UserMessage_2eproto}, {
      &scc_info_Any_google_2fprotobuf_2fany_2eproto.base,}};

static void InitDefaultsscc_info_user_database_filter_UserMessage_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::doodle::_user_database_filter_default_instance_;
    new (ptr) ::doodle::user_database_filter();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_user_database_filter_UserMessage_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_user_database_filter_UserMessage_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_UserMessage_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_UserMessage_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_UserMessage_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_UserMessage_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::doodle::user_database, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::doodle::user_database, id_),
  PROTOBUF_FIELD_OFFSET(::doodle::user_database, uuidpath_),
  PROTOBUF_FIELD_OFFSET(::doodle::user_database, user_name_),
  PROTOBUF_FIELD_OFFSET(::doodle::user_database, userdata_cereal_),
  PROTOBUF_FIELD_OFFSET(::doodle::user_database, permission_group_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::doodle::user_database_filter, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::doodle::user_database_filter, id_),
  PROTOBUF_FIELD_OFFSET(::doodle::user_database_filter, permission_group_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::doodle::user_database)},
  { 10, -1, sizeof(::doodle::user_database_filter)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::doodle::_user_database_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::doodle::_user_database_filter_default_instance_),
};

const char descriptor_table_protodef_UserMessage_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\021UserMessage.proto\022\006doodle\032\031google/prot"
  "obuf/any.proto\"\211\001\n\ruser_database\022\n\n\002id\030\001"
  " \001(\004\022\020\n\010uuidPath\030\002 \001(\t\022\021\n\tuser_name\030\003 \001("
  "\t\022-\n\017userdata_cereal\030\004 \001(\0132\024.google.prot"
  "obuf.Any\022\030\n\020permission_group\030\005 \001(\004\"<\n\024us"
  "er_database_filter\022\n\n\002id\030\001 \001(\004\022\030\n\020permis"
  "sion_group\030\005 \001(\004b\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_UserMessage_2eproto_deps[1] = {
  &::descriptor_table_google_2fprotobuf_2fany_2eproto,
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_UserMessage_2eproto_sccs[2] = {
  &scc_info_user_database_UserMessage_2eproto.base,
  &scc_info_user_database_filter_UserMessage_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_UserMessage_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_UserMessage_2eproto = {
  false, false, descriptor_table_protodef_UserMessage_2eproto, "UserMessage.proto", 264,
  &descriptor_table_UserMessage_2eproto_once, descriptor_table_UserMessage_2eproto_sccs, descriptor_table_UserMessage_2eproto_deps, 2, 1,
  schemas, file_default_instances, TableStruct_UserMessage_2eproto::offsets,
  file_level_metadata_UserMessage_2eproto, 2, file_level_enum_descriptors_UserMessage_2eproto, file_level_service_descriptors_UserMessage_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_UserMessage_2eproto = (static_cast<void>(::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_UserMessage_2eproto)), true);
namespace doodle {

// ===================================================================

class user_database::_Internal {
 public:
  static const PROTOBUF_NAMESPACE_ID::Any& userdata_cereal(const user_database* msg);
};

const PROTOBUF_NAMESPACE_ID::Any&
user_database::_Internal::userdata_cereal(const user_database* msg) {
  return *msg->userdata_cereal_;
}
void user_database::clear_userdata_cereal() {
  if (GetArena() == nullptr && userdata_cereal_ != nullptr) {
    delete userdata_cereal_;
  }
  userdata_cereal_ = nullptr;
}
user_database::user_database(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:doodle.user_database)
}
user_database::user_database(const user_database& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  uuidpath_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_uuidpath().empty()) {
    uuidpath_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_uuidpath(), 
      GetArena());
  }
  user_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (!from._internal_user_name().empty()) {
    user_name_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_user_name(), 
      GetArena());
  }
  if (from._internal_has_userdata_cereal()) {
    userdata_cereal_ = new PROTOBUF_NAMESPACE_ID::Any(*from.userdata_cereal_);
  } else {
    userdata_cereal_ = nullptr;
  }
  ::memcpy(&id_, &from.id_,
    static_cast<size_t>(reinterpret_cast<char*>(&permission_group_) -
    reinterpret_cast<char*>(&id_)) + sizeof(permission_group_));
  // @@protoc_insertion_point(copy_constructor:doodle.user_database)
}

void user_database::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_user_database_UserMessage_2eproto.base);
  uuidpath_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  user_name_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  ::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
      reinterpret_cast<char*>(&userdata_cereal_) - reinterpret_cast<char*>(this)),
      0, static_cast<size_t>(reinterpret_cast<char*>(&permission_group_) -
      reinterpret_cast<char*>(&userdata_cereal_)) + sizeof(permission_group_));
}

user_database::~user_database() {
  // @@protoc_insertion_point(destructor:doodle.user_database)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void user_database::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  uuidpath_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  user_name_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete userdata_cereal_;
}

void user_database::ArenaDtor(void* object) {
  user_database* _this = reinterpret_cast< user_database* >(object);
  (void)_this;
}
void user_database::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void user_database::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const user_database& user_database::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_user_database_UserMessage_2eproto.base);
  return *internal_default_instance();
}


void user_database::Clear() {
// @@protoc_insertion_point(message_clear_start:doodle.user_database)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  uuidpath_.ClearToEmpty();
  user_name_.ClearToEmpty();
  if (GetArena() == nullptr && userdata_cereal_ != nullptr) {
    delete userdata_cereal_;
  }
  userdata_cereal_ = nullptr;
  ::memset(&id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&permission_group_) -
      reinterpret_cast<char*>(&id_)) + sizeof(permission_group_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* user_database::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // uint64 id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string uuidPath = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 18)) {
          auto str = _internal_mutable_uuidpath();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "doodle.user_database.uuidPath"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // string user_name = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 26)) {
          auto str = _internal_mutable_user_name();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "doodle.user_database.user_name"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // .google.protobuf.Any userdata_cereal = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 34)) {
          ptr = ctx->ParseMessage(_internal_mutable_userdata_cereal(), ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint64 permission_group = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          permission_group_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* user_database::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:doodle.user_database)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // uint64 id = 1;
  if (this->id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(1, this->_internal_id(), target);
  }

  // string uuidPath = 2;
  if (this->uuidpath().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_uuidpath().data(), static_cast<int>(this->_internal_uuidpath().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "doodle.user_database.uuidPath");
    target = stream->WriteStringMaybeAliased(
        2, this->_internal_uuidpath(), target);
  }

  // string user_name = 3;
  if (this->user_name().size() > 0) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_user_name().data(), static_cast<int>(this->_internal_user_name().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "doodle.user_database.user_name");
    target = stream->WriteStringMaybeAliased(
        3, this->_internal_user_name(), target);
  }

  // .google.protobuf.Any userdata_cereal = 4;
  if (this->has_userdata_cereal()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(
        4, _Internal::userdata_cereal(this), target, stream);
  }

  // uint64 permission_group = 5;
  if (this->permission_group() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(5, this->_internal_permission_group(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:doodle.user_database)
  return target;
}

size_t user_database::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:doodle.user_database)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string uuidPath = 2;
  if (this->uuidpath().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_uuidpath());
  }

  // string user_name = 3;
  if (this->user_name().size() > 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_user_name());
  }

  // .google.protobuf.Any userdata_cereal = 4;
  if (this->has_userdata_cereal()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *userdata_cereal_);
  }

  // uint64 id = 1;
  if (this->id() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
        this->_internal_id());
  }

  // uint64 permission_group = 5;
  if (this->permission_group() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
        this->_internal_permission_group());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void user_database::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:doodle.user_database)
  GOOGLE_DCHECK_NE(&from, this);
  const user_database* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<user_database>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:doodle.user_database)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:doodle.user_database)
    MergeFrom(*source);
  }
}

void user_database::MergeFrom(const user_database& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:doodle.user_database)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.uuidpath().size() > 0) {
    _internal_set_uuidpath(from._internal_uuidpath());
  }
  if (from.user_name().size() > 0) {
    _internal_set_user_name(from._internal_user_name());
  }
  if (from.has_userdata_cereal()) {
    _internal_mutable_userdata_cereal()->PROTOBUF_NAMESPACE_ID::Any::MergeFrom(from._internal_userdata_cereal());
  }
  if (from.id() != 0) {
    _internal_set_id(from._internal_id());
  }
  if (from.permission_group() != 0) {
    _internal_set_permission_group(from._internal_permission_group());
  }
}

void user_database::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:doodle.user_database)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void user_database::CopyFrom(const user_database& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:doodle.user_database)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool user_database::IsInitialized() const {
  return true;
}

void user_database::InternalSwap(user_database* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  uuidpath_.Swap(&other->uuidpath_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  user_name_.Swap(&other->user_name_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(user_database, permission_group_)
      + sizeof(user_database::permission_group_)
      - PROTOBUF_FIELD_OFFSET(user_database, userdata_cereal_)>(
          reinterpret_cast<char*>(&userdata_cereal_),
          reinterpret_cast<char*>(&other->userdata_cereal_));
}

::PROTOBUF_NAMESPACE_ID::Metadata user_database::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

class user_database_filter::_Internal {
 public:
};

user_database_filter::user_database_filter(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:doodle.user_database_filter)
}
user_database_filter::user_database_filter(const user_database_filter& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&id_, &from.id_,
    static_cast<size_t>(reinterpret_cast<char*>(&permission_group_) -
    reinterpret_cast<char*>(&id_)) + sizeof(permission_group_));
  // @@protoc_insertion_point(copy_constructor:doodle.user_database_filter)
}

void user_database_filter::SharedCtor() {
  ::memset(reinterpret_cast<char*>(this) + static_cast<size_t>(
      reinterpret_cast<char*>(&id_) - reinterpret_cast<char*>(this)),
      0, static_cast<size_t>(reinterpret_cast<char*>(&permission_group_) -
      reinterpret_cast<char*>(&id_)) + sizeof(permission_group_));
}

user_database_filter::~user_database_filter() {
  // @@protoc_insertion_point(destructor:doodle.user_database_filter)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void user_database_filter::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void user_database_filter::ArenaDtor(void* object) {
  user_database_filter* _this = reinterpret_cast< user_database_filter* >(object);
  (void)_this;
}
void user_database_filter::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void user_database_filter::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const user_database_filter& user_database_filter::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_user_database_filter_UserMessage_2eproto.base);
  return *internal_default_instance();
}


void user_database_filter::Clear() {
// @@protoc_insertion_point(message_clear_start:doodle.user_database_filter)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&permission_group_) -
      reinterpret_cast<char*>(&id_)) + sizeof(permission_group_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* user_database_filter::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // uint64 id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint64 permission_group = 5;
      case 5:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 40)) {
          permission_group_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* user_database_filter::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:doodle.user_database_filter)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // uint64 id = 1;
  if (this->id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(1, this->_internal_id(), target);
  }

  // uint64 permission_group = 5;
  if (this->permission_group() != 0) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt64ToArray(5, this->_internal_permission_group(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:doodle.user_database_filter)
  return target;
}

size_t user_database_filter::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:doodle.user_database_filter)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // uint64 id = 1;
  if (this->id() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
        this->_internal_id());
  }

  // uint64 permission_group = 5;
  if (this->permission_group() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt64Size(
        this->_internal_permission_group());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void user_database_filter::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:doodle.user_database_filter)
  GOOGLE_DCHECK_NE(&from, this);
  const user_database_filter* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<user_database_filter>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:doodle.user_database_filter)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:doodle.user_database_filter)
    MergeFrom(*source);
  }
}

void user_database_filter::MergeFrom(const user_database_filter& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:doodle.user_database_filter)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.id() != 0) {
    _internal_set_id(from._internal_id());
  }
  if (from.permission_group() != 0) {
    _internal_set_permission_group(from._internal_permission_group());
  }
}

void user_database_filter::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:doodle.user_database_filter)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void user_database_filter::CopyFrom(const user_database_filter& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:doodle.user_database_filter)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool user_database_filter::IsInitialized() const {
  return true;
}

void user_database_filter::InternalSwap(user_database_filter* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(user_database_filter, permission_group_)
      + sizeof(user_database_filter::permission_group_)
      - PROTOBUF_FIELD_OFFSET(user_database_filter, id_)>(
          reinterpret_cast<char*>(&id_),
          reinterpret_cast<char*>(&other->id_));
}

::PROTOBUF_NAMESPACE_ID::Metadata user_database_filter::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace doodle
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::doodle::user_database* Arena::CreateMaybeMessage< ::doodle::user_database >(Arena* arena) {
  return Arena::CreateMessageInternal< ::doodle::user_database >(arena);
}
template<> PROTOBUF_NOINLINE ::doodle::user_database_filter* Arena::CreateMaybeMessage< ::doodle::user_database_filter >(Arena* arena) {
  return Arena::CreateMessageInternal< ::doodle::user_database_filter >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
