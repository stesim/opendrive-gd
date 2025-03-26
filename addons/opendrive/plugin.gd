@tool
extends EditorPlugin


const ImportPlugin := preload("./import_plugin.gd")


var _import_plugin : ImportPlugin = null


func _enter_tree() -> void:
	_import_plugin = ImportPlugin.new()
	add_import_plugin(_import_plugin)


func _exit_tree() -> void:
	remove_import_plugin(_import_plugin)
	_import_plugin = null
