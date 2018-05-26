/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
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

#ifndef __RS_RESOURCE_BUILDER_H__
#define __RS_RESOURCE_BUILDER_H__

#include "ResourceCompiler.h"
#include "ResourceOptions.h"
#include <standard/concurrentQueue.h>

namespace boost::program_options
{
    class variables_map;
}

namespace System::Resource
{
    enum class ResourceTypes : uint8
    {
        RAW,
        XAML,
        FONT,
        IMAGE,
        SHADER
    };

    class ResourceBuilder
    {
        static System::Concurrent::Queue<ResourceBuilder*> _activeTasks;
        static System::Concurrent::Queue<ResourceBuilder*> _resultTasks;

        ResourceOptions _options;
        ResourceCompiler _compiler;
        ResourceTypes _resourceType = ResourceTypes::RAW;

        std::string _name;

    public:
        ResourceBuilder(ResourceOptions options, std::string const& sourcefile);

        static void Parse(const boost::program_options::variables_map &cmdLine);

        void Compile();
        void GenerateFile();
        static void Generate();
    };
}

#endif // !__RS_RESOURCE_BUILDER_H__
