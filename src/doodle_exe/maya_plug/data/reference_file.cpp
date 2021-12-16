//
// Created by TD on 2021/11/30.
//

#include "reference_file.h"

#include "doodle_lib/metadata/metadata.h"
#include "doodle_lib/metadata/project.h"
#include <maya/MDagPath.h>
#include <maya/MFileIO.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnReference.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItSelectionList.h>
#include <maya/MObjectArray.h>
#include <maya/MPlug.h>
#include <maya/MTime.h>
#include <maya/MUuid.h>
#include "maya_file_io.h"
#include "qcloth_shape.h"
#include "maya_plug_fwd.h"

namespace doodle::maya_plug {
reference_file::reference_file()
    : prj_ref(boost::uuids::nil_uuid()),
      path(),
      use_sim(false),
      high_speed_sim(false),
      collision_model(),
      ref_file_uuid(),
      p_m_object(){};

reference_file::reference_file(const entt::handle &in_uuid, const MObject &in_ref_node)
    : reference_file() {
  set_project(in_uuid);
  set_path(in_ref_node);
}

void reference_file::set_path(const MObject &in_ref_node) {
  MStatus k_s{};
  MFnReference k_ref{in_ref_node, &k_s};
  DOODLE_CHICK(k_s);
  path = d_str{k_ref.fileName(false, true, true, &k_s)};
  DOODLE_CHICK(k_s);
  ref_file_uuid = d_str{k_ref.uuid().asString()};
  p_m_object    = in_ref_node;
}

MSelectionList reference_file::get_collision_model() const {
  MSelectionList l_list{};
  for (const auto &str : collision_model) {
    l_list.add(str.c_str(), true);
  }
  return l_list;
}

void reference_file::chick_mobject() {
  if (p_m_object.isNull()) {
    if (!ref_file_uuid.empty()) {
      MStatus k_s;
      MFnReference k_file;
      for (MItDependencyNodes refIter(MFn::kReference); !refIter.isDone(); refIter.next()) {
        k_s = k_file.setObject(refIter.thisNode());
        DOODLE_CHICK(k_s);
        if (k_file.uuid().asString().asUTF8() == ref_file_uuid) {
          p_m_object = refIter.thisNode();
        }
      }
    }
    if (p_m_object.isNull())
      throw doodle_error{"无法找到引用"};
  }
}
void reference_file::chick_mobject() const {
  if (p_m_object.isNull()) {
    throw doodle_error{"没有初始化mobject"};
  }
}
void reference_file::set_collision_model(const MSelectionList &in_list) {
  collision_model.clear();
  collision_model_show_str.clear();
  chick_mobject();
  MStatus k_s{};
  MDagPath l_path{};
  MFnDagNode l_node{};
  for (MItSelectionList l_it{in_list, MFn::Type::kMesh, &k_s};
       !l_it.isDone(&k_s);
       l_it.next()) {
    DOODLE_CHICK(k_s);
    k_s = l_it.getDagPath(l_path);
    DOODLE_CHICK(k_s);
    auto k_obj = l_path.transform(&k_s);
    DOODLE_CHICK(k_s);
    k_s = l_node.setObject(k_obj);
    DOODLE_CHICK(k_s);
    collision_model_show_str.emplace_back(d_str{l_node.name(&k_s)});
    DOODLE_CHICK(k_s);

    collision_model.emplace_back(d_str{l_node.fullPathName(&k_s)});
    DOODLE_CHICK(k_s);
  }
}

void reference_file::init_show_name() {
  collision_model_show_str.clear();
  MStatus k_s{};
  MDagPath l_path{};
  MFnDagNode l_node{};
  for (MItSelectionList l_it{get_collision_model(), MFn::Type::kMesh, &k_s};
       !l_it.isDone(&k_s);
       l_it.next()) {
    DOODLE_CHICK(k_s);
    k_s = l_it.getDagPath(l_path);
    DOODLE_CHICK(k_s);
    auto k_obj = l_path.transform(&k_s);
    DOODLE_CHICK(k_s);
    k_s = l_node.setObject(k_obj);
    DOODLE_CHICK(k_s);
    collision_model_show_str.emplace_back(d_str{l_node.name(&k_s)});
    DOODLE_CHICK(k_s);
  }
}
string reference_file::get_namespace() const {
  chick_mobject();
  MFnReference k_ref{p_m_object};
  MStatus k_s{};
  string k_r = d_str{k_ref.associatedNamespace(false, &k_s)};
  DOODLE_CHICK(k_s);
  /// \brief 再没有名称空间时, 我们使用引用名称计算并映射到导出名称中去
  if (k_r.empty()) {
    //    k_r = d_str{k_ref.fileName(true, true, true, &k_s)};
    //    DOODLE_CHICK(k_s);
    //    throw;
    DOODLE_LOG_WARN("名称空间为空, 可能是引用时未使用时未使用");
    throw doodle_error{"名称空间为空, 可能是引用时未使用时未使用"};
  }
  return k_r;
}
string reference_file::get_namespace() {
  chick_mobject();
  return std::as_const(*this).get_namespace();
}
string reference_file::get_ref_file_name() const {
  //  chick_mobject();
  //  MFnReference k_ref{p_m_object};
  //  MStatus k_s{};
  //  string k_r = d_str{k_ref.fileName(true, true, true, &k_s)};
  //  DOODLE_CHICK(k_s);
  //  boost::replace_all(k_r, " ", "_");
  //  return k_r;
  return get_namespace();
}
bool reference_file::replace_sim_assets_file() {
  if (!use_sim) {
    DOODLE_LOG_WARN("跳过不解算的文件 {}", path);
    return false;
  }

  chick_mobject();
  MFnReference k_ref{p_m_object};
  MStatus k_s{};

  /// \brief 检查各种必须属性
  if (!k_ref.isLoaded(&k_s)) {
    DOODLE_CHICK(k_s);
    DOODLE_LOG_WARN("引用没有加载, 跳过!");
    return false;
  }
  auto k_prj = get_prj();
  if (!k_prj)
    throw doodle_error{"无法找到项目配置"};

  chick_component<project::cloth_config>(k_prj);
  auto &k_cfg = k_prj.get<project::cloth_config>();
  FSys::path k_m_str{d_str{k_ref.fileName(true, true, false, &k_s)}.str()};
  DOODLE_CHICK(k_s);
  auto k_vfx_path = k_cfg.vfx_cloth_sim_path / fmt::format("{}_cloth{}", k_m_str.stem().generic_string(), k_m_str.extension().generic_string());
  DOODLE_LOG_INFO("推测资产路径 {}", k_vfx_path);
  if (!FSys::exists(k_vfx_path))
    return false;

  /// \brief 替换引用文件
  auto k_comm = fmt::format(R"(
file -loadReference "{}" "{}";
)",
                            d_str{k_ref.name()}.str(), k_vfx_path.generic_string(), true);
  k_s         = MGlobal::executeCommand(d_str{k_comm});
  DOODLE_CHICK(k_s);
  return true;
}

entt::handle reference_file::get_prj() const {
  auto k_prj_view = g_reg()->view<project>();
  for (auto k_e : k_prj_view) {
    auto k_h = make_handle(k_e);
    if (k_h.get<database>() == prj_ref) {
      return k_h;
    }
  }
  return entt::handle{};
}
void reference_file::set_project(const entt::handle &in_prj) {
  chick_component<database>(in_prj);
  prj_ref = in_prj.get<database>().uuid();
}

bool reference_file::has_ref_project() const {
  return !prj_ref.is_nil();
}

bool reference_file::rename_material() const {
  chick_mobject();
  MStatus k_s{};
  MFnReference k_ref{p_m_object, &k_s};
  MObjectArray k_list{};
  k_ref.nodes(k_list, &k_s);
  DOODLE_CHICK(k_s);
  MFnDependencyNode k_node;
  for (auto i = 0; i < k_list.length(); ++i) {
    auto k_obj = k_list[i];
    if (k_obj.hasFn(MFn::Type::kShadingEngine)) {  /// \brief 找到符合的着色集

      k_node.setObject(k_obj);
      auto k_plug = k_node.findPlug(d_str{"surfaceShader"}, true, &k_s);
      DOODLE_CHICK(k_s);
      MPlugArray l_m_plug_array{};
      auto k_source = k_plug.source(&k_s);
      DOODLE_CHICK(k_s);
      if (k_source.isNull(&k_s)) {
        continue;
      }
      DOODLE_CHICK(k_s);
      auto k_mat = k_source.node(&k_s);  /// \brief 从属性链接获得材质名称
      DOODLE_CHICK(k_s);
      MFnDependencyNode k_mat_node{};
      k_mat_node.setObject(k_mat);
      string k_mat_node_name = d_str{k_mat_node.name(&k_s)};
      DOODLE_CHICK(k_s);
      /// \brief 重命名材质名称
      k_mat_node.setName(d_str{fmt::format("{}_mat", k_mat_node_name)}, false, &k_s);
      DOODLE_CHICK(k_s);
      DOODLE_LOG_INFO("重命名材质 {} -> {}", d_str{k_node.name()}.str(), k_mat_node_name);

      k_node.setName(d_str{k_mat_node_name}, false, &k_s);
    }
  }

  return true;
}
bool reference_file::export_abc(const MTime &in_start, const MTime &in_endl) const {
  rename_material();
  MSelectionList k_select{};
  MStatus k_s{};
  auto &k_cfg = get_prj().get<project::cloth_config>();
  k_s         = k_select.add(d_str{fmt::format("{}:*{}", get_namespace(), k_cfg.export_group)}, true);
  DOODLE_CHICK(k_s);

  if (k_select.isEmpty())
    return false;

  k_s = MGlobal::setActiveSelectionList(k_select);
  DOODLE_CHICK(k_s);
  auto k_name = fmt::format("{}_export_abc", get_namespace());
  k_s         = MGlobal::executeCommand(
              d_str{fmt::format(R"(polyUnite -ch 1 -mergeUVSets 1 -centerPivot -name "{}";)",
                                k_name)},
              true);
  DOODLE_CHICK(k_s);

  k_select.clear();
  k_s = k_select.add(d_str{k_name}, true);
  DOODLE_CHICK(k_s);
  if (k_select.isEmpty())
    return false;

  MDagPath k_mesh_path{};
  k_s = k_select.getDagPath(0, k_mesh_path);
  DOODLE_CHICK(k_s);

  auto k_seance_name = maya_file_io::get_current_path().stem().generic_string();
  auto k_path        = maya_file_io::work_path(fmt::format("abc/{}", k_seance_name));

  if (!exists(k_path)) {
    create_directories(k_path);
  }
  k_path /= fmt::format("{}_{}_{}-{}.abc", k_seance_name, get_namespace(), in_start.as(MTime::uiUnit()), in_endl.as(MTime::uiUnit()));

  /// \brief 导出abc命令
  k_s = MGlobal::executeCommand(d_str{
                                    fmt::format(R"(
AbcExport -j "-frameRange {} {} -stripNamespaces -uvWrite -writeFaceSets -worldSpace -dataFormat ogawa -root {} -file {}";
)",
                                                in_start.as(MTime::uiUnit()),                 /// \brief 开始时间
                                                in_endl.as(MTime::uiUnit()),                  /// \brief 结束时间
                                                d_str{k_mesh_path.fullPathName(&k_s)}.str(),  /// \brief 导出物体的根路径
                                                k_path.generic_string())},
                                true);  /// \brief 导出文件路径，包含文件名和文件路径
  DOODLE_CHICK(k_s);
  return true;
}
string reference_file::get_unique_name() const {
  chick_mobject();
  MFnReference k_ref{p_m_object};
  MStatus k_s{};

  auto k_name = k_ref.fileName(false, false, true, &k_s);
  DOODLE_CHICK(k_s);
  return d_str{k_name};
}
bool reference_file::add_collision() const {
  if (collision_model.empty())
    return true;

  MStatus k_s{};
  auto l_item = this->get_collision_model();
  k_s         = l_item.add(d_str{fmt::format("{}:qlSolver1", get_namespace())}, true);
  DOODLE_CHICK(k_s);
  k_s = MGlobal::setActiveSelectionList(l_item);
  DOODLE_CHICK(k_s);
  k_s = MGlobal::executeCommand(d_str{"qlCreateCollider;"});
  DOODLE_CHICK(k_s);
  return true;
}
void reference_file::generate_cloth_proxy() {
  auto k_prj = get_prj();
  if (!k_prj)
    throw doodle_error{"无法找到项目配置"};

  chick_component<project::cloth_config>(k_prj);
  auto &k_cfg = k_prj.get<project::cloth_config>();
  MStatus k_s{};
  MSelectionList k_qcloth_list{};
  k_s = k_qcloth_list.add(
      d_str{fmt::format("{}:*{}", get_namespace(), k_cfg.cloth_proxy)},
      true);
  DOODLE_CHICK(k_s);
  MDagPath k_path{};
  for (MItSelectionList k_iter{k_qcloth_list}; !k_iter.isDone(); k_iter.next()) {
    auto k_h = make_handle();
    k_s      = k_iter.getDagPath(k_path);
    DOODLE_CHICK(k_s);
    auto &k_q = k_h.emplace<qcloth_shape>(make_handle(*this), k_path.node());
  }
  return;
}
void reference_file::export_fbx(const MTime &in_start, const MTime &in_end) const {
  MSelectionList k_select{};
  MStatus k_s{};
  auto &k_cfg = get_prj().get<project::cloth_config>();
  try {
    k_s = k_select.add(d_str{fmt::format("{}:*{}", get_namespace(), k_cfg.export_group)}, true);
    DOODLE_CHICK(k_s);
  } catch (const maya_error &err) {
    if (err.maya_status == MStatus::MStatusCode::kInvalidParameter) {
      DOODLE_LOG_WARN("没有物体被配置文件中的 export_group 值选中, 疑似场景文件, 或为不符合配置的文件, 不进行导出")
      return;
    } else
      throw;
  }

  if (k_select.isEmpty()) {
    DOODLE_LOG_WARN("没有选中的物体, 不进行输出")
    return;
  }

  k_s = MGlobal::setActiveSelectionList(k_select);
  DOODLE_CHICK(k_s);

  auto k_file_path = maya_file_io::work_path("fbx") / maya_file_io::get_current_path().stem();

  if (!FSys::exists(k_file_path))
    FSys::create_directories(k_file_path);

  k_file_path /= fmt::format("{}_{}_{}-{}.fbx",
                             maya_file_io::get_current_path().stem().generic_string(),
                             get_namespace(),
                             in_start.value(),
                             in_end.value());

  auto k_comm = fmt::format("FBXExportBakeComplexStart -v {};", in_start.value());
  k_s         = MGlobal::executeCommand(d_str{k_comm});
  DOODLE_CHICK(k_s);

  k_comm = fmt::format("FBXExportBakeComplexEnd -v {};", in_end.value());
  k_s    = MGlobal::executeCommand(d_str{k_comm});
  DOODLE_CHICK(k_s);

  k_comm = string{"FBXExportBakeComplexAnimation -v true;"};
  k_s    = MGlobal::executeCommand(d_str{k_comm});
  DOODLE_CHICK(k_s);

  k_comm = string{"FBXExportConstraints -v true;"};
  k_s    = MGlobal::executeCommand(d_str{k_comm});
  DOODLE_CHICK(k_s);

  k_comm = fmt::format(R"(FBXExport -f "{}" -s;)", k_file_path.generic_string());
  k_s    = MGlobal::executeCommand(d_str{k_comm});
  DOODLE_CHICK(k_s);
}

}  // namespace doodle::maya_plug
