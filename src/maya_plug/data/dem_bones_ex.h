//
// Created by TD on 2022/7/1.
//

#pragma once
#include <main/maya_plug_fwd.h>

#include <DemBones/DemBonesExt.h>
namespace doodle::maya_plug {
class dem_bones_ex : public ::Dem::DemBonesExt<std::double_t, std::double_t> {
 public:
  dem_bones_ex()  = default;
  ~dem_bones_ex() = default;
  // 初始化中每次分割骨簇之前调用回调函数。
  void cbInitSplitBegin() override;
  // 初始化中每次对骨簇进行分割后都会调用回调函数
  void cbInitSplitEnd() override;
  // 在每次全局迭代更新之前调用回调函数
  void cbIterBegin() override;
  // 在每次全局迭代更新后调用回调函数，如果返回true，则停止迭代。
  bool cbIterEnd() override;
  // 在每个外观权重更新之前调用的回调函数
  void cbWeightsBegin() override;
  // 每次蒙皮权重更新后调用的回调函数
  void cbWeightsEnd() override;
  // 在每次骨骼转换更新之前调用的回调函数
  void cbTranformationsBegin() override;
  // 每次骨骼转换更新后调用的回调函数
  void cbTransformationsEnd() override;

  // 每个局部骨骼转换更新迭代后调用的回调函数，如果返回true，则停止迭代
  void cbWeightsIterBegin() override;
  // 在每个局部权重更新迭代后调用的回调函数
  bool cbWeightsIterEnd() override;

  std::vector<MObject> joins{};
};
}  // namespace doodle::maya_plug
