#ifndef EOLIAN_MONO_FUNCTION_BLACKLIST_HH
#define EOLIAN_MONO_FUNCTION_BLACKLIST_HH

namespace eolian_mono {

inline bool is_function_blacklisted(std::string const& c_name)
{
  return
    c_name == "efl_event_callback_array_priority_add"
    || c_name == "efl_event_callback_call"
    || c_name == "efl_event_callback_legacy_call"
    || c_name == "efl_canvas_group_change"
    || c_name == "efl_event_callback_priority_add"
    || c_name == "evas_obj_table_children_get"
    || c_name == "efl_loop_app_efl_version_get"
    || c_name == "efl_loop_efl_version_get"
    || c_name == "elm_interface_atspi_accessible_event_handler_add"
    || c_name == "edje_obj_part_text_anchor_geometry_get"
    || c_name == "efl_canvas_object_scale_get"
    || c_name == "efl_canvas_object_scale_set"
    ;
}

}

#endif
