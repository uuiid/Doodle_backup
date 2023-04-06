//
// Created by TD on 2022/9/29.
//

#pragma once

#include <doodle_core/doodle_core_fwd.h>

#include <doodle_app/configure/doodle_app_export.h>

namespace doodle {

namespace details {
class program_options;
}  // namespace details

namespace gui {

namespace details {
class main_proc_handle;

}
using main_proc_handle = gui::details::main_proc_handle;
}  // namespace gui

using program_options = details::program_options;

}  // namespace doodle
