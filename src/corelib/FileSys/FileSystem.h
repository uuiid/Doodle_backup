﻿//
// Created by teXiao on 2021/4/28.
//
#pragma once

#include <corelib/core_global.h>

namespace doodle {
class CORE_API FileSystem {
    public:
    static void localCopy(const FSys::path& sourePath, const FSys::path& targetPath, const bool backup);
};

}  // namespace doodle