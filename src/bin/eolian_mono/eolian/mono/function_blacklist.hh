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
    || c_name == "efl_player_position_get"
    || c_name == "efl_access_component_layer_get" // duplicated signature
    || c_name == "efl_access_component_alpha_get"
    || c_name == "efl_image_load_error_get"
    || c_name == "efl_text_font_source_get"
    || c_name == "efl_text_font_source_set"
    || c_name == "evas_canvas_pointer_inside_get"
    || c_name == "efl_canvas_object_pointer_inside_get"
    || c_name == "evas_canvas_smart_objects_calculate"
    || c_name == "efl_ui_focus_manager_focus_get"
    || c_name == "efl_ui_text_password_get"
    || c_name == "efl_ui_text_password_set"
    || c_name == "elm_obj_entry_calc_force"
    || c_name == "elm_interface_atspi_accessible_event_handler_add"
    || c_name == "elm_interface_atspi_action_name_get"
    || c_name == "elm_interface_atspi_component_layer_get" // duplicated signature
    || c_name == "elm_interface_atspi_component_size_set"
    || c_name == "elm_interface_atspi_component_size_get"
    || c_name == "elm_interface_atspi_component_alpha_get"
    || c_name == "elm_interface_atspi_component_extents_get"
    || c_name == "elm_interface_atspi_accessible_parent_get"
    || c_name == "elm_interface_atspi_accessible_name_set"
    || c_name == "elm_interface_atspi_accessible_name_get"
    || c_name == "elm_interface_atspi_accessible_root_get"
    || c_name == "elm_interface_atspi_accessible_role_get"
    || c_name == "elm_interface_atspi_accessible_type_get"
    || c_name == "elm_interface_atspi_accessible_description_set"
    || c_name == "elm_interface_atspi_accessible_description_get"
    || c_name == "elm_interface_scrollable_repeat_events_get"
    || c_name == "elm_interface_scrollable_repeat_events_set"
    || c_name == "elm_interface_fileselector_model_get"
    || c_name == "elm_interface_fileselector_model_set"
    || c_name == "elm_wdg_item_del"
    || c_name == "elm_wdg_item_focus_get"
    || c_name == "elm_wdg_item_focus_set"
    || c_name == "elm_obj_layout_signal_emit"
    || c_name == "elm_obj_layout_signal_callback_del"
    || c_name == "elm_obj_layout_signal_callback_add"
    || c_name == "elm_obj_widget_focus_get"
    || c_name == "elm_obj_widget_focus_set"
    || c_name == "elm_obj_widget_focus_highlight_style_get"
    || c_name == "elm_obj_widget_focus_highlight_style_set"
    || c_name == "elm_obj_widget_mirrored_set"
    || c_name == "elm_obj_widget_signal_emit"
    || c_name == "elm_obj_widget_signal_callback_add"
    || c_name == "elm_obj_widget_part_text_set"
    || c_name == "elm_obj_widget_part_text_get"
    || c_name == "elm_obj_widget_scale_set"
    || c_name == "elm_obj_widget_scale_get"
    || c_name == "elm_obj_widget_mirrored_get"
    || c_name == "elm_obj_colorselector_color_set"
    || c_name == "elm_obj_colorselector_color_get"
    || c_name == "elm_obj_bg_color_set"
    || c_name == "elm_obj_bg_color_get"
    || c_name == "elm_obj_calendar_weekdays_names_get"
    || c_name == "elm_interface_scrollable_mirrored_set"
    || c_name == "edje_obj_play_set"
    || c_name == "edje_obj_play_get"
    || c_name == "edje_obj_load_error_get"
    || c_name == "efl_ui_win_focus_get" // conflicts with efl_ui_focus_object_focus_get
    || c_name == "efl_ui_win_name_set"
    || c_name == "efl_ui_win_name_get"
    || c_name == "efl_ui_radio_value_set"
    || c_name == "efl_ui_radio_value_get"
    || c_name == "efl_ui_focus_user_parent_get"
    || c_name == "elm_obj_genlist_item_parent_get"
    || c_name == "elm_obj_scroller_page_scroll_limit_set"
    || c_name == "elm_obj_scroller_propagate_events_set"
    || c_name == "elm_obj_scroller_propagate_events_get"
    || c_name == "efl_gfx_stack_raise"
    || c_name == "edje_obj_part_text_anchor_geometry_get" // duplicated signature
    || c_name == "efl_canvas_object_scale_get" // duplicated signature
    || c_name == "efl_canvas_object_scale_set" // duplicated signature
    ;
}

}

#endif
