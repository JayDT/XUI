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

#include "ResourceManager.h"

using namespace System::Resource;

System::Resource::ResourceManager* System::Resource::ResourceManager::Instance()
{
    static System::Resource::ResourceManager _instance;
    return &_instance;
}

void System::Resource::ResourceManager::Initialize()
{
    std::list<ResourceDatabase*> moduls;
    while (!InitResourceModule.empty())
    {
        moduls.push_back(InitResourceModule.front());
        InitResourceModule.pop();
    }

    for (auto const& modul : moduls)
    {
        modul->CreateResourceDescriptor();
    }

    for (auto const& modul : moduls)
    {
        modul->Initialize();
    }
}

ResourceDatabase * System::Resource::ResourceManager::GetResourceModul(std::string const& name) const
{
    auto modul = _resourceModules.find(name);
    return modul != _resourceModules.end() ? modul->second : nullptr;
}

ResourceObject * System::Resource::ResourceManager::_findResource(System::URI const& uri, uint64_t modulID) const
{
    if (uri.GetDescriptor().m_scheme != "pack")
        return nullptr;

    const char* resourceRef = uri.GetDescriptor().m_path.LocalPath[0] == '/' ? &uri.GetDescriptor().m_path.LocalPath[1] : uri.GetDescriptor().m_path.LocalPath.c_str();

    ResourceDatabase * modul = nullptr;
    if (!uri.GetDescriptor().m_path.Assembly.empty())
    {
        const char* modulRef = uri.GetDescriptor().m_path.Assembly[0] == '/' ? &uri.GetDescriptor().m_path.Assembly[1] : uri.GetDescriptor().m_path.Assembly.c_str();

        modul = GetResourceModul(modulRef);
        if (!modul)
            return nullptr;

        auto resource = modul->GetResource(resourceRef);
        if (!resource)
            return nullptr;
        return resource;
    }

    auto iModul = _resourceModulesById.find(modulID);

    if (iModul != _resourceModulesById.end())
    {
        ResourceObject* resource = iModul->second->GetResource(resourceRef);
        if (resource)
            return resource;
    }

    for (auto db : _resourceModules)
    {
        ResourceObject* resource = db.second->GetResource(resourceRef);
        if (resource)
            return resource;
    }

    return nullptr;
}
