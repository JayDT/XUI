/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

//#pragma once
#ifndef __META_PCH_DEF_H__
#define __META_PCH_DEF_H__

//namespace std
//{
//    template<typename Ty>
//    struct auto_ptr : public std::unique_ptr<Ty>
//    {};
//}
#define _HAS_AUTO_PTR_ETC 1

#include <memory>

#include <standard/Platform/Common.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <functional>

#include <clang-c/Index.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "MetaUtils.h"
#include "MetaDataConfig.h"

#include <Wrappers/Mustache.h>

#include "Logger.h"

#endif // __META_PCH_DEF_H__
