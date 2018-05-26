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

#include <string>

#define DEFINE_SWITCH(name, value)                   \
    const std::string kSwitch##name = value;         \
    const std::string kSwitch##name##Shorthand = ""; \

#define DEFINE_SWITCH_FULL(name, value, shorthand)              \
    const std::string kSwitch##name = value;                    \
    const std::string kSwitch##name##Shorthand = "," shorthand; \

#define SWITCH_OPTION(name)                                 \
    ((kSwitch##name) + (kSwitch##name##Shorthand)).c_str( ) \

DEFINE_SWITCH_FULL( Help,                      "help",                "h" );
DEFINE_SWITCH_FULL( TargetName,                "target-name",         "t" );
DEFINE_SWITCH_FULL( SourceRoot,                "source-root",         "r" );
DEFINE_SWITCH_FULL( InputSource,               "in-source",           "i" );
DEFINE_SWITCH_FULL( ModuleHeaderFile,          "module-header",       "m" );
DEFINE_SWITCH_FULL( OutputModuleSource,        "out-source",          "s" );
DEFINE_SWITCH_FULL( OutputModuleFileDirectory, "out-dir",             "c" );
DEFINE_SWITCH_FULL( TemplateDirectory,         "tmpl-directory",      "d" );
DEFINE_SWITCH_FULL( PrecompiledHeader,         "pch",                 "p" );
DEFINE_SWITCH_FULL( ForceRebuild,              "force-rebuild",       "e" );
DEFINE_SWITCH_FULL( DisplayDiagnostics,        "display-diagnostics", "o" );
DEFINE_SWITCH_FULL( CompilerIncludes,          "includes",            "f" );
DEFINE_SWITCH_FULL( CompilerDefines,           "defines",             "x" );
