#pragma once


#include <Math.hpp>

#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>


namespace odr_gd {


inline godot::Vector2 to_godot(odr::Vec2D const& vec) {
  return godot::Vector2{(real_t)vec[0], (real_t)vec[1]};
}

inline godot::Vector3 to_godot(odr::Vec3D const& vec) {
  return godot::Vector3{(real_t)vec[0], (real_t)vec[2], -(real_t)vec[1]};
}


}
