#pragma once

#include <OpenDriveMap.h>

#include "./gdx.h"
#include "./utils.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/typed_dictionary.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

#include <optional>
#include <string>
#include <map>
#include <vector>


namespace odr_gd {


class OpenDriveMap : public Resource {
  GDCLASS(OpenDriveMap, Resource)

public:
  bool is_valid() const { return _native.has_value(); }

  void load_from_file(String const& path) {
    _native.emplace(path.utf8().ptr(), true);
  }


  double get_sampling_interval() const { return _sampling_interval; }

  void set_sampling_interval(double value) { _sampling_interval = value; }


  double get_mark_offset() const { return _mark_offset; }

  void set_mark_offset(double value) { _mark_offset = value; }


  TypedDictionary<String, Material> get_mark_materials() const { return _mark_materials; }

  void set_mark_materials(TypedDictionary<String, Material> const& value) { _mark_materials = value; }


  void add_lanes_to_scene(Node* parent, TypedDictionary<String, Material> const& default_lane_type_materials) const {
    for (auto const& [road_id, road] : _native->id_to_road)
    {
      auto const get_surface_arrays = [](auto& map, typename std::decay_t<decltype(map)>::key_type key) -> Array& {
        auto const iter = map.find(key);
        if (iter != map.end())
          return iter->second;

        Array& arrays = map.insert({key, Array{}}).first->second;
        arrays.resize(Mesh::ARRAY_MAX);
        return arrays;
      };

      std::map<std::pair<int, std::string>, Array> lane_and_type_to_surface_arrays{};
      std::map<std::pair<std::string, std::string>, Array> mark_surface_arrays{};
      for (auto const& [_, section] : road.s_to_lanesection)
      {
        for (auto const& [id, lane] : section.id_to_lane)
        {
          odr::Mesh3D const lane_mesh = road.get_lane_mesh(lane, _sampling_interval);
          Array& surface_arrays = get_surface_arrays(lane_and_type_to_surface_arrays, {id, lane.type});
          _add_odr_mesh_to_surface_arrays(lane_mesh, surface_arrays);

          std::vector<odr::RoadMark> const marks = lane.get_roadmarks(section.s0, road.get_lanesection_end(section));
          for (odr::RoadMark const& mark : marks)
          {
            odr::Mesh3D mark_mesh = road.get_roadmark_mesh(lane, mark, _sampling_interval);
            if (_mark_offset != 0.0) {
              for (odr::Vec3D& v : mark_mesh.vertices) {
                v[2] += _mark_offset;
              }
            }
            Array& surface_arrays = get_surface_arrays(mark_surface_arrays, {mark.type, mark.color});
            _add_odr_mesh_to_surface_arrays(mark_mesh, surface_arrays);
          }
        }
      }

      Ref<ArrayMesh> mesh{};
      mesh.instantiate();

      int32_t surface_index = 0;
      for (auto const& [key, arrays] : lane_and_type_to_surface_arrays) {
        auto const& [id, type] = key;
        mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
        String const type_string = type.c_str();
        String const name = "lane_" + String::num_int64(id) + "_" + type_string;
        mesh->surface_set_name(surface_index, name);

        Ref<Material> material = default_lane_type_materials.get(type_string, Variant{});
        if (material.is_valid()) {
          mesh->surface_set_material(surface_index, material);
        }

        ++surface_index;
      }

      for (auto const& [key, arrays] : mark_surface_arrays) {
        auto const& [mark_type, mark_color] = key;
        mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
        String const type_string = mark_type.c_str();
        String const color_string = mark_color.c_str();
        String const name = "lane_marks_" + type_string + "_" + color_string;
        mesh->surface_set_name(surface_index, name);

        Ref<Material> material = _mark_materials.get(color_string, Variant{});
        if (material.is_valid()) {
          mesh->surface_set_material(surface_index, material);
        }

        ++surface_index;
      }

      MeshInstance3D* mesh_instance = memnew(MeshInstance3D);
      mesh_instance->set_name("road_" + String{road_id.c_str()});
      mesh_instance->set_mesh(mesh);

      parent->add_child(mesh_instance);
      mesh_instance->set_owner(parent->get_owner());
    }
  }


protected:
  static void _bind_methods() {
    GDX_ADD_METHOD(is_valid);

    GDX_ADD_METHOD(load_from_file, "path");

    GDX_ADD_GET_SET_PROPERTY(sampling_interval, Variant::FLOAT);
    GDX_ADD_GET_SET_PROPERTY(mark_offset, Variant::FLOAT);
    GDX_ADD_GET_SET_PROPERTY(mark_materials, Variant::DICTIONARY);

    GDX_ADD_METHOD(add_lanes_to_scene, "parent", "default_lane_type_materials");
  }


private:
  void _add_odr_mesh_to_surface_arrays(odr::Mesh3D const& odr_mesh, Array& arrays) const {
    if (odr_mesh.vertices.empty()) {
      return;
    }

    if (!arrays[Mesh::ARRAY_VERTEX]) {
      arrays[Mesh::ARRAY_VERTEX] = PackedVector3Array{};
    }
    PackedVector3Array positions = arrays[Mesh::ARRAY_VERTEX];
    int64_t positions_start_index = positions.size();

    positions.resize(positions_start_index + odr_mesh.vertices.size());
    for (std::size_t i = 0; i < odr_mesh.vertices.size(); ++i) {
      positions[positions_start_index + i] = to_godot(odr_mesh.vertices[i]); // - center_point;
    }
    arrays[ArrayMesh::ARRAY_VERTEX] = positions;

    if (!arrays[Mesh::ARRAY_INDEX]) {
      arrays[Mesh::ARRAY_INDEX] = PackedInt32Array{};
    }
    PackedInt32Array indices = arrays[Mesh::ARRAY_INDEX];
    int64_t indices_start_index = indices.size();
    indices.resize(indices_start_index + odr_mesh.indices.size());
    int32_t index_offset = positions_start_index;
    for (std::size_t i = 0; i < odr_mesh.indices.size(); i += 3) {
      indices[indices_start_index + i] = odr_mesh.indices[i + 2] + index_offset;
      indices[indices_start_index + i + 1] = odr_mesh.indices[i + 1] + index_offset;
      indices[indices_start_index + i + 2] = odr_mesh.indices[i] + index_offset;
    }
    arrays[ArrayMesh::ARRAY_INDEX] = indices;

    if (odr_mesh.normals.size() == odr_mesh.vertices.size()) {
      if (!arrays[Mesh::ARRAY_NORMAL]) {
        arrays[Mesh::ARRAY_NORMAL] = PackedVector3Array{};
      }
      PackedVector3Array normals = arrays[Mesh::ARRAY_NORMAL];
      int64_t normals_start_index = normals.size();
      normals.resize(normals_start_index + odr_mesh.normals.size());
      for (std::size_t i = 0; i < odr_mesh.normals.size(); ++i) {
        normals[normals_start_index + i] = to_godot(odr_mesh.normals[i]);
      }
      arrays[ArrayMesh::ARRAY_NORMAL] = normals;
    }

    if (odr_mesh.st_coordinates.size() == odr_mesh.vertices.size()) {
      if (!arrays[Mesh::ARRAY_TEX_UV]) {
        arrays[Mesh::ARRAY_TEX_UV] = PackedVector2Array{};
      }
      PackedVector2Array uvs = arrays[Mesh::ARRAY_TEX_UV];
      int64_t uvs_start_index = uvs.size();
      uvs.resize(uvs_start_index + odr_mesh.st_coordinates.size());
      for (std::size_t i = 0; i < odr_mesh.st_coordinates.size(); ++i) {
        uvs[uvs_start_index + i] = to_godot(odr_mesh.st_coordinates[i]);
      }
      arrays[ArrayMesh::ARRAY_TEX_UV] = uvs;
    }
  }


  std::optional<odr::OpenDriveMap> _native{};

  double _sampling_interval = 0.5;

  double _mark_offset = 0.01;

  TypedDictionary<String, Material> _mark_materials{};
};


}
