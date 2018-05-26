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

#include "ResourceDatabase.h"
#include "ResourceManager.h"
#include "ResourceObject.h"

using namespace System::Resource;

System::Resource::ResourceDatabase::ResourceDatabase(const char* modulName, uint64_t modulId)
    : _modulName(modulName)
    , _modulId(modulId)
{

}

void System::Resource::ResourceDatabase::Initialize()
{

}

ResourceObject * System::Resource::ResourceDatabase::CreateResourceObject(std::string const & name)
{
    ResourceObject * res = GetResource(name);
    if (res)
        return res;

    auto& newres = _resourceMap[name];
    newres = new ResourceObject(name);
    return newres;
}

ResourceObject * System::Resource::ResourceDatabase::GetResource(std::string const & name) const
{
    auto res = _resourceMap.find(name);
    return res != _resourceMap.end() ? res->second : nullptr;
}
