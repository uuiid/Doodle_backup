//
// Created by TD on 2022/5/27.
//
#include <doodle_main_exe/limited_exe/gui/app.h>
// #include <doodle_lib/DoodleApp.h>
// #include <boost/locale.hpp>

extern "C" int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR strCmdLine, int nCmdShow) try {
  limited_app app{doodle::doodle_main_app::in_gui_arg{
      doodle::doodle_main_app::in_app_args{hInstance, strCmdLine},
      nCmdShow, nullptr}};
  try {
    return app.run();
  } catch (const std::exception& err) {
    DOODLE_LOG_WARN(boost::diagnostic_information(boost::diagnostic_information(err)));
    return 1;
  }
} catch (...) {
  return 1;
}
