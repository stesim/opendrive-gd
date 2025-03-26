@tool
extends EditorImportPlugin


enum Preset {
	DEFAULT,
}


static var LANE_TYPES := PackedStringArray([
	"none",
	"driving",
	"stop",
	"shoulder",
	"biking",
	"sidewalk",
	"border",
	"restricted",
	"parking",
	"bidirectional",
	"median",
	"special1",
	"special2",
	"special3",
	"roadWorks",
	"tram",
	"rail",
	"entry",
	"exit",
	"offRamp",
	"onRamp",
])


static var MARK_COLORS := PackedStringArray([
	"standard",
	"blue",
	"green",
	"red",
	"white",
	"yellow",
])


func _get_importer_name() -> String:
	return "stesim.opendrive"


func _get_visible_name() -> String:
	return "Scene"


func _get_priority() -> float:
	return 2.0


func _get_import_order() -> int:
	return IMPORT_ORDER_SCENE


func _get_recognized_extensions() -> PackedStringArray:
	return ["xodr"]


func _get_save_extension() -> String:
	return "scn"


func _get_resource_type() -> String:
	return "PackedScene"


func _get_preset_count() -> int:
	return Preset.size()


func _get_preset_name(preset_index : int) -> String:
	match preset_index:
		Preset.DEFAULT: return "Default"
	return "Unknown"


func _get_import_options(_path : String, _preset_index : int) -> Array[Dictionary]:
	var options : Array[Dictionary] = [
		{
			name = "sampling_interval",
			default_value = 0.5,
			property_hint = PROPERTY_HINT_RANGE,
			hint_string = "0.001,2.0,0.001,or_greater",
		},
		{
			name = "mark_offset",
			default_value = 0.01,
			property_hint = PROPERTY_HINT_RANGE,
			hint_string = "0.0,0.1,0.001,or_greater",
		},
	]

	for lane_type in LANE_TYPES:
		options.push_back({
			name = "materials/lanes/" + lane_type,
			default_value = "",
			property_hint = PROPERTY_HINT_FILE,
			hint_string = "*.res,*.tres",
		})

	for mark_color in MARK_COLORS:
		options.push_back({
			name = "materials/marks/" + mark_color,
			default_value = "",
			property_hint = PROPERTY_HINT_FILE,
			hint_string = "*.res,*.tres",
		})

	return options


func _get_option_visibility(_path : String, _option_name : StringName, _options : Dictionary) -> bool:
	return true


func _import(source_file : String, save_path : String, options : Dictionary, _platform_variants : Array[String], _gen_files : Array[String]) -> Error:
	var default_lane_type_materials : Dictionary[String, Material] = {}
	var default_mark_materials : Dictionary[String, Material] = {}
	for option : StringName in options:
		if option.begins_with("materials/lanes/"):
			var type := option.trim_prefix("materials/lanes/")
			var material_error := _load_material(type, options[option], default_lane_type_materials)
			if material_error != OK:
				return material_error
		elif option.begins_with("materials/marks/"):
			var type := option.trim_prefix("materials/marks/")
			var material_error := _load_material(type, options[option], default_mark_materials)
			if material_error != OK:
				return material_error

	var map = OpenDriveMap.new()
	map.sampling_interval = options.sampling_interval
	map.mark_offset = options.mark_offset
	map.mark_materials = default_mark_materials
	map.load_from_file(ProjectSettings.globalize_path(source_file))
	if not map.is_valid():
		return ERR_PARSE_ERROR

	var root := Node3D.new()
	root.name = source_file.get_file().get_basename().to_snake_case()

	var roads := Node3D.new()
	roads.name = &"roads"
	root.add_child(roads)
	roads.owner = root

	map.add_lanes_to_scene(roads, default_lane_type_materials)

	var scene := PackedScene.new()
	var pack_error := scene.pack(root)
	root.free()

	if pack_error != OK:
		return pack_error

	var full_save_path := "%s.%s" % [save_path, _get_save_extension()]
	return ResourceSaver.save(scene, full_save_path, ResourceSaver.FLAG_COMPRESS)


func _load_material(type : String, path : String, materials : Dictionary[String, Material]) -> Error:
	if path.is_empty():
		return OK

	if not ResourceLoader.exists(path):
		return ERR_FILE_NOT_FOUND

	var material := ResourceLoader.load(path, "Material")
	if material == null:
		return ERR_CANT_ACQUIRE_RESOURCE

	materials[type] = material
	return OK
