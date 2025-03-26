#include "./open_drive_map.h"

#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>


using namespace ::godot;


namespace odr_gd {


void initialize_extension(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }

  ClassDB::register_class<OpenDriveMap>();
}


void deinitialize_extension(ModuleInitializationLevel p_level) {
  if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
    return;
  }
}


extern "C" GDE_EXPORT auto opendrive_gd_init(
  GDExtensionInterfaceGetProcAddress p_get_proc_address,
  GDExtensionClassLibraryPtr p_library,
  GDExtensionInitialization* r_initialization
) -> GDExtensionBool {
  GDExtensionBinding::InitObject init_obj{p_get_proc_address, p_library, r_initialization};
  init_obj.register_initializer(initialize_extension);
  init_obj.register_terminator(deinitialize_extension);
  init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
  return init_obj.init();
}


}
