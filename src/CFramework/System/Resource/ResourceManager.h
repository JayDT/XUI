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

#ifndef __CF_RESOURCE_MANAGER_H__
#define __CF_RESOURCE_MANAGER_H__

#include <System/Resource/ResourceDatabase.h>
#include <System/Uri.h>

namespace System::Resource
{
    class TC_CFRAMEWORK_API ResourceManager
    {
        typedef std::unordered_map<std::string, ResourceDatabase*> ResourceModulMap;
        typedef std::unordered_map<uint64_t, ResourceDatabase*> ResourceModulMapById;
        ResourceModulMap _resourceModules;
        ResourceModulMapById _resourceModulesById;

        ResourceObject* _findResource(System::URI const& uri, uint64_t modulID) const;

    public:

        std::queue<ResourceDatabase*> InitResourceModule;

        static ResourceManager* Instance();

        template<typename TModul>
        TModul* Register()
        {
            auto modul = new TModul();
            if (_resourceModules.find(modul->Name()) == _resourceModules.end())
            {
                _resourceModules[modul->Name()] = modul;
                _resourceModulesById[modul->GetModulId()] = modul;
            }
            else
            {
                std::abort();
            }
            return modul;
        }

        void Initialize();

        ResourceDatabase* GetResourceModul(std::string const& name) const;

        ResourceObject* FindResource(System::URI const& uri, uint64_t modulID = System::GetCurrentModule()) const
        {
            return _findResource(uri, modulID);
        }

    };
}

#endif //!__CF_RESOURCE_MANAGER_H__