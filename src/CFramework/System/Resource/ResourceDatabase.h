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

#ifndef __CF_RESOURCE_DATABASE_H__
#define __CF_RESOURCE_DATABASE_H__

#include "System/Resource/ResourceObject.h"
#include "standard/misc.h"

namespace System::Resource
{
    class TC_CFRAMEWORK_API ResourceDatabase
    {
        typedef std::unordered_map<std::string, ResourceObject*> ResourceMap;

        uint64_t _modulId;
        std::string _modulName;
        ResourceMap _resourceMap;

    public:

        ResourceDatabase(const char* modulName, uint64_t modulId = System::GetCurrentModule());
        virtual ~ResourceDatabase() {}

        uint64_t GetModulId() const { return _modulId; }
        const char* Name() const { return _modulName.c_str(); }

        ResourceObject* CreateResourceObject(std::string const& name);

        ResourceObject* GetResource(std::string const& name) const;

        virtual void CreateResourceDescriptor() = 0;
        virtual void Initialize() = 0;
    };
}

#endif //!__CF_RESOURCE_MANAGER_H__