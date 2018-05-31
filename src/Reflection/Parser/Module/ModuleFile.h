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

class Class;
class Global;
class Function;
class Enum;

struct ModuleFile
{
    std::string name;

    std::list<std::shared_ptr<Class>> classes;
    std::list<std::shared_ptr<Global>> globals;
    std::list<std::shared_ptr<Function>> globalFunctions;
    std::list<std::shared_ptr<Enum>> enums;
    std::set<std::string> includes;
};