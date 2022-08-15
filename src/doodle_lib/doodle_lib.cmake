set(
        DOODLELIB_HEADER
        app/app.h
        core/ContainerDevice.h
        core/filesystem_extend.h
        core/observable_container.h
        core/program_options.h
        core/util.h
        core/image_loader.h
        core/authorization.h


        file_warp/opencv_read_player.h


        lib_warp/boost_locale_warp.h
        lib_warp/imgui_warp.h


        toolkit/toolkit.h
        exe_warp/maya_exe.h
        exe_warp/ue4_exe.h
        long_task/image_to_move.h
        long_task/join_move.h
        long_task/short_cut.h
        long_task/image_load_task.h


        core/app_command_base.h
        doodle_lib_all.h
        doodle_lib_fwd.h
        doodle_lib_pch.h
)
set(
        DOODLELIB_SOURCE
        app/app.cpp


        core/filesystem_extend.cpp
        core/program_options.cpp
        core/util.cpp
        core/image_loader.cpp
        core/authorization.cpp


        file_warp/opencv_read_player.cpp


        lib_warp/imgui_warp.cpp


        toolkit/toolkit.cpp
        exe_warp/maya_exe.cpp
        exe_warp/ue4_exe.cpp
        long_task/image_to_move.cpp
        long_task/join_move.cpp
        long_task/short_cut.cpp
        long_task/image_load_task.cpp


        core/app_command_base.cpp
        doodle_lib_all.cpp
)
