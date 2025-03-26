#pragma once

#include <godot_cpp/core/class_db.hpp>


using namespace godot;


#define GDX_ADD_SIGNAL(name, ...) ::godot::ClassDB::add_signal(get_class_static(), ::godot::MethodInfo{name __VA_OPT__(,) __VA_ARGS__})
#define GDX_ADD_GROUP(m_name, m_prefix) ::godot::ClassDB::add_property_group(get_class_static(), m_name, m_prefix)
#define GDX_ADD_SUBGROUP(m_name, m_prefix) ::godot::ClassDB::add_property_subgroup(get_class_static(), m_name, m_prefix)
#define GDX_ADD_PROPERTY(m_property, m_setter, m_getter) ::godot::ClassDB::add_property(get_class_static(), m_property, m_setter, m_getter)

#define GDX_ADD_METHOD(name, ...) ::godot::ClassDB::bind_method(D_METHOD(#name __VA_OPT__(,) __VA_ARGS__), &self_type::name);
#define GDX_ADD_PRIVATE_METHOD(name, ...) ::godot::ClassDB::bind_method(D_METHOD("_"#name __VA_OPT__(,) __VA_ARGS__), &self_type::name);
#define GDX_ADD_ENUM_CONSTANT(constant) ::godot::ClassDB::bind_integer_constant(get_class_static(), ::godot::_gde_constant_get_enum_name(constant, #constant), #constant, constant)

#define GDX_ADD_GET_SET_PROPERTY(name, type) \
  GDX_ADD_PRIVATE_METHOD(get_##name); \
  GDX_ADD_PRIVATE_METHOD(set_##name, "value"); \
  GDX_ADD_PROPERTY(::godot::PropertyInfo(type, #name), "_set_"#name, "_get_"#name)

#define GDX_ADD_GET_SET_OBJECT_PROPERTY(name, type) \
  GDX_ADD_PRIVATE_METHOD(get_##name); \
  GDX_ADD_PRIVATE_METHOD(set_##name, "value"); \
  GDX_ADD_PROPERTY(::godot::PropertyInfo(::godot::Variant::OBJECT, #name, ::godot::PROPERTY_HINT_RESOURCE_TYPE, #type), "_set_"#name, "_get_"#name)

#define GDX_ADD_GET_SET_NODE_PROPERTY(name, type) \
  GDX_ADD_PRIVATE_METHOD(get_##name); \
  GDX_ADD_PRIVATE_METHOD(set_##name, "value"); \
  GDX_ADD_PROPERTY(::godot::PropertyInfo(::godot::Variant::OBJECT, #name, ::godot::PROPERTY_HINT_NODE_TYPE, #type), "_set_"#name, "_get_"#name)

#define GDX_ADD_GET_SET_NODE_PATH_PROPERTY(name, type) \
  GDX_ADD_PRIVATE_METHOD(get_##name); \
  GDX_ADD_PRIVATE_METHOD(set_##name, "value"); \
  GDX_ADD_PROPERTY(::godot::PropertyInfo(::godot::Variant::NODE_PATH, #name, ::godot::PROPERTY_HINT_NODE_PATH_VALID_TYPES, #type), "_set_"#name, "_get_"#name)

#define GDX_ADD_GET_SET_ENUM_PROPERTY(name, enum_string) \
  GDX_ADD_PRIVATE_METHOD(get_##name); \
  GDX_ADD_PRIVATE_METHOD(set_##name, "value"); \
  GDX_ADD_PROPERTY(::godot::PropertyInfo(::godot::Variant::INT, #name, ::godot::PROPERTY_HINT_ENUM, enum_string), "_set_"#name, "_get_"#name)

#define GDX_ADD_IS_SET_PROPERTY(name) \
  GDX_ADD_PRIVATE_METHOD(is_##name); \
  GDX_ADD_PRIVATE_METHOD(set_##name, "value"); \
  GDX_ADD_PROPERTY(::godot::PropertyInfo(::godot::Variant::BOOL, #name), "_set_"#name, "_is_"#name)
