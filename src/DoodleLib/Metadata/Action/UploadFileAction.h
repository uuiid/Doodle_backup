﻿//
// Created by TD on 2021/6/17.
//

#pragma once

#include <DoodleLib/DoodleLib_fwd.h>
#include <DoodleLib/Metadata/Action/Action.h>
namespace doodle {
class UploadFileAction : public Action {
 public:
  UploadFileAction();
  explicit UploadFileAction(std::any&& in_any);

  virtual std::string class_name() override;
  virtual void run(const MetadataPtr& in_data) override;
  virtual void operator()(const MetadataPtr& in_data) override;
};

}  // namespace doodle
