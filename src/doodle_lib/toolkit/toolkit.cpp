#include <doodle_lib/Exception/exception.h>
#include <doodle_lib/Logger/logger.h>
#include <doodle_lib/core/core_set.h>
#include <doodle_lib/file_sys/file_system.h>
#include <doodle_lib/file_warp/image_sequence.h>
#include <doodle_lib/file_warp/maya_file.h>
#include <doodle_lib/file_warp/ue4_project.h>
#include <doodle_lib/file_warp/video_sequence.h>
#include <doodle_lib/metadata/episodes.h>
#include <doodle_lib/metadata/project.h>
#include <doodle_lib/metadata/shot.h>
#include <doodle_lib/toolkit/toolkit.h>

#if defined(_WIN32)

#include <ShlObj.h>
#endif

namespace doodle {

void toolkit::installMayaPath() {
  try {
    auto mayadoc = win::get_pwd();
    mayadoc /= "maya";
    mayadoc /= "modules";
    mayadoc /= "doodle";

    auto sourePath = core_set::program_location().parent_path();
    sourePath /= "plug/maya";

    if (!FSys::exists(mayadoc)) {
      FSys::create_directories(mayadoc);
    } else
      FSys::remove_all(mayadoc);

    file_system::local_copy(sourePath, mayadoc, false);

    static std::string k_mod{R"(+ doodle 1.1 .\doodle
MYMODULE_LOCATION:= .
PATH+:= plug-ins
PYTHONPATH+:= scripts
)"};
    {
      auto k_p = mayadoc.parent_path() / "doodle.mod";
      DOODLE_LOG_INFO("写入 {}", k_p);
      FSys::ofstream k_file{k_p};
      k_file << k_mod;
    }
  } catch (FSys::filesystem_error &err) {
    DOODLE_LOG_ERROR(err.what());
    throw;
  }
}

void toolkit::installUePath(const FSys::path &path) {
  try {
    auto &set       = core_set::getSet();

    auto sourePath  = FSys::current_path().parent_path();
    sourePath       = sourePath / "plug" / "uePlug";
    sourePath       = sourePath / set.get_ue4_setting().get_version();
    sourePath       = sourePath / "Plugins" / "Doodle";
    auto targetPath = path / "Plugins" / "Doodle";

    if (FSys::exists(targetPath)) {
      FSys::remove_all(targetPath);
    } else {
      FSys::create_directories(targetPath);
    }

    DOODLE_LOG_INFO(fmt::format("install plug : {} --> {}", sourePath, targetPath));
    file_system::local_copy(sourePath, targetPath, false);
  } catch (FSys::filesystem_error &error) {
    DOODLE_LOG_ERROR(error.what());
    throw;
  }
}

bool toolkit::update() {
  return false;
}

void toolkit::modifyUeCachePath() {
  auto ue_path     = core_set::getSet().get_ue4_setting().get_path() / "Engine/Config/BaseEngine.ini";
  //做备份
  auto backup_path = FSys::path{ue_path}.replace_extension(".ini.backup");
  FSys::copy(ue_path, FSys::add_time_stamp(backup_path), FSys::copy_options::update_existing);
  FSys::fstream file{ue_path, std::ios::in | std::ios::out | std::ios::binary};
  std::string line{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

  static std::string str{R"("%ENGINEVERSIONAGNOSTICUSERDIR%DerivedDataCache")"};
  auto it = line.find(str);
  while (it != std::string::npos) {
    line.replace(it, str.size(), R"("%GAMEDIR%DerivedDataCache")");
    it = line.find(str);
  }
  file.close();
  file.open(ue_path, std::ios::out | std::ios::trunc | std::ios::binary);
  file << line;
}

bool toolkit::deleteUeCache() {
#if defined(_WIN32)
  //这里我们手动做一些工作
  //获取环境变量
  PWSTR pManager;
  SHGetKnownFolderPath(FOLDERID_LocalAppData, NULL, nullptr, &pManager);
  chick_true<doodle_error>(pManager, DOODLE_LOC, "无法找到保存路径");

  FSys::path path{pManager};
  CoTaskMemFree(pManager);

  path /= "UnrealEngine";
  DOODLE_LOG_INFO(fmt::format("delete Folder : {}", path));
  FSys::remove_all(path);
#endif
  return true;
}

}  // namespace doodle
