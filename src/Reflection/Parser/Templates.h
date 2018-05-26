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

#pragma once

#define EXT ".mustache"

const auto kTemplateModuleHeader       = "module-header" EXT;
const auto kTemplateModuleSource       = "module-source" EXT;
const auto kTemplateModuleFileHeader   = "module-file-header" EXT;
const auto kTemplateModuleFileSource   = "module-file-source" EXT;
const auto kTemplateModuleFileDefSource= "module-file-definition-source" EXT;
        
const auto kPartialGlobalGetter        = "global-getter" EXT;
const auto kPartialGlobalSetter        = "global-setter" EXT;
                                       
const auto kPartialFieldGetter         = "field-getter" EXT;
const auto kPartialFieldSetter         = "field-setter" EXT;

const auto kPartialInstanceDefination  = "instance-definitation" EXT;
const auto kPartialArgumentDefination  = "argument-definitation" EXT;

const auto kPartialMetaInitializerList = "meta-initializer-list" EXT;

#undef EXT