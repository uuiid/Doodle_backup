﻿#pragma once

#include <corelib/core_global.h>
#include <boost/filesystem.hpp>
#include <corelib/libWarp/BoostUuidWarp.h>
#include <corelib/core/Ue4Setting.h>
DOODLE_NAMESPACE_S

enum class Department {
  None_,
  Executive,
  Light,
  VFX,
  modle,
  rig,
  Anm,
  direct,
  paint
};

struct synPath_struct {
  fileSys::path local;
  fileSys::path server;
};

/*
 *全局静态设置类
 */

class CORE_API coreSet {
 public:
  static coreSet &getSet();

  DOODLE_DISABLE_COPY(coreSet)
  //初始化函数
  void init();
  void reInit();

  void appendEnvironment() const;

  //获得运行程序目录
  static fileSys::path program_location();
  static fileSys::path program_location(const fileSys::path &path);

  // user设置
  [[nodiscard]] dstring getUser() const;
  [[nodiscard]] dstring getUser_en() const;
  void setUser(const dstring &value);

  //部门设置
  [[nodiscard]] dstring getDepartment() const;
  void setDepartment(const dstring &value);

  //缓存路径
  [[nodiscard]] fileSys::path getCacheRoot() const;

  // doc路径
  [[nodiscard]] fileSys::path getDoc() const;

  Ue4Setting &gettUe4Setting() const { return ue4_setting; };

  void writeDoodleLocalSet();

  static dstring toIpPath(const dstring &path);

  boost::uuids::uuid getUUID();

 private:
  //私有化构造函数
  coreSet();
  //获得缓存磁盘路径
  void getCacheDiskPath();
  //获得服务器上的统一设置
  void getServerSetting();
  //获得本地的有限设置
  void getSetting();

 private:
  const static dstring settingFileName;
  boost::uuids::random_generator p_uuid_gen;

  //用户名称
  dstring user;
  //部门
  Department department;

  fileSys::path synPath;

  fileSys::path cacheRoot;
  fileSys::path doc;

  Ue4Setting &ue4_setting;
};

DOODLE_NAMESPACE_E
