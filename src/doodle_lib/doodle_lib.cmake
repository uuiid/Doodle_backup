set(
    DOODLELIB_HEADER
    core/ContainerDevice.h
    core/core_set.h
    core/core_sql.h
    core/doodle_app_base.h
    core/doodle_lib.h
    core/filesystem_extend.h
    core/observable_container.h
    core/open_file_dialog.h
    core/program_options.h
    core/static_value.h
    core/tools_setting.h
    core/tree_container.h
    core/ue4_setting.h
    core/util.h
    exception/exception.h
    file_warp/image_sequence.h
    file_warp/maya_file.h
    file_warp/opencv_read_player.h
    file_warp/ue4_project.h
    file_warp/video_sequence.h
    gui/base_windwos.h
    gui/main_windwos.h
    gui/setting_windows.h
    gui/widget_register.h
    gui/factory/attribute_factory_interface.h
    gui/action/command.h
    gui/action/command_down_file.h
    gui/action/command_files.h
    gui/action/command_meta.h
    gui/action/command_tool.h
    gui/action/command_ue4.h
    gui/action/command_video.h
    gui/widgets/assets_file_widgets.h
    gui/widgets/assets_widget.h
    gui/widgets/edit_widgets.h
    gui/widgets/long_time_tasks_widget.h
    gui/widgets/opencv_player_widget.h
    gui/widgets/project_widget.h
    gui/widgets/time_widget.h
    gui/widgets/tool_box_widget.h
    lib_warp/WinReg.hpp
    lib_warp/boost_locale_warp.h
    lib_warp/boost_serialization_warp.h
    lib_warp/boost_uuid_warp.h
    lib_warp/cache.hpp
    lib_warp/cache_policy.hpp
    lib_warp/cmrcWarp.h
    lib_warp/entt_warp.h
    lib_warp/fifo_cache_policy.hpp
    lib_warp/imgui_warp.h
    lib_warp/json_warp.h
    lib_warp/lfu_cache_policy.hpp
    lib_warp/lru_cache_policy.hpp
    lib_warp/protobuf_warp.h
    lib_warp/protobuf_warp_cpp.h
    lib_warp/sqlppWarp.h
    lib_warp/std_warp.h
    logger/LoggerTemplate.h
    logger/logger.h
    metadata/assets.h
    metadata/assets_file.h
    metadata/assets_path.h
    metadata/comment.h
    metadata/episodes.h
    metadata/leaf_meta.h
    metadata/metadata.h
    metadata/metadata_cpp.h
    metadata/metadata_factory.h
    metadata/metadata_state.h
    metadata/project.h
    metadata/season.h
    metadata/shot.h
    metadata/time_point_wrap.h
    metadata/tree_adapter.h
    metadata/user.h
    pin_yin/convert.h
    rpc/rpc_file_system_client.h
    rpc/rpc_file_system_server.h
    rpc/rpc_metadaata_server.h
    rpc/rpc_metadata_client.h
    rpc/rpc_server_handle.h
    rpc/rpc_trans_path.h
    screenshot_widght/screenshot_action.h
    screenshot_widght/screenshot_widght.h
    thread_pool/long_term.h
    thread_pool/thread_pool.h
    toolkit/toolkit.h
    server/doodle_server.h
    exe_warp/maya_exe.h
    exe_warp/ue4_exe.h
    doodle_app.h
    doodle_lib_all.h
    doodle_lib_fwd.h
    doodle_lib_pch.h
    doodle_macro.h)
set(
    DOODLELIB_SOURCE
    core/core_set.cpp
    core/core_sql.cpp
    core/doodle_app_base.cpp
    core/doodle_lib.cpp
    core/filesystem_extend.cpp
    core/open_file_dialog.cpp
    core/program_options.cpp
    core/static_value.cpp
    core/tree_container.cpp
    core/ue4_setting.cpp
    core/util.cpp
    file_warp/image_sequence.cpp
    file_warp/maya_file.cpp
    file_warp/opencv_read_player.cpp
    file_warp/ue4_project.cpp
    file_warp/video_sequence.cpp
    gui/base_windwos.cpp
    gui/main_windwos.cpp
    gui/setting_windows.cpp
    gui/widget_register.cpp
    gui/factory/attribute_factory_interface.cpp
    gui/action/command.cpp
    gui/action/command_down_file.cpp
    gui/action/command_files.cpp
    gui/action/command_meta.cpp
    gui/action/command_tool.cpp
    gui/action/command_ue4.cpp
    gui/action/command_video.cpp
    gui/widgets/assets_file_widgets.cpp
    gui/widgets/assets_widget.cpp
    gui/widgets/edit_widgets.cpp
    gui/widgets/long_time_tasks_widget.cpp
    gui/widgets/opencv_player_widget.cpp
    gui/widgets/project_widget.cpp
    gui/widgets/time_widget.cpp
    gui/widgets/tool_box_widget.cpp
    lib_warp/boost_serialization_warp.cpp
    lib_warp/imgui_warp.cpp
    logger/logger.cpp
    metadata/assets.cpp
    metadata/assets_file.cpp
    metadata/assets_path.cpp
    metadata/comment.cpp
    metadata/episodes.cpp
    metadata/leaf_meta.cpp
    metadata/metadata.cpp
    metadata/metadata_factory.cpp
    metadata/project.cpp
    metadata/season.cpp
    metadata/shot.cpp
    metadata/time_point_wrap.cpp
    metadata/user.cpp
    pin_yin/convert.cpp
    rpc/rpc_file_system_client.cpp
    rpc/rpc_file_system_server.cpp
    rpc/rpc_metadaata_server.cpp
    rpc/rpc_metadata_client.cpp
    rpc/rpc_server_handle.cpp
    rpc/rpc_trans_path.cpp
    screenshot_widght/screenshot_action.cpp
    screenshot_widght/screenshot_widght.cpp
    thread_pool/long_term.cpp
    toolkit/toolkit.cpp
    server/doodle_server.cpp
    exe_warp/maya_exe.cpp
    exe_warp/ue4_exe.cpp
    doodle_app.cpp
    doodle_lib_all.cpp)
