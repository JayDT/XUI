/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
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

#ifndef __XAMLCPP_SETTINGS_H__
#define __XAMLCPP_SETTINGS_H__

#include <string>
#include "Runtime/ReflectionModule.h"
#include "XamlCPP/Interfaces/IInstanceLifeCycleListener.h"
#include "XamlCPP/Interfaces/IResourceMap.h"

namespace XamlCPP::Core::Assembler
{
    class Settings
    {
    private:
        System::Reflection::Object *m_RootInstance = nullptr;
        std::shared_ptr<Interfaces::IInstanceLifeCycleListener> m_InstanceLifeCycleListener = nullptr;
        std::map<std::string, System::Reflection::Variant> m_ParsingContext;
        const System::Reflection::meta::ReflectionModule * m_defaultAssembly = nullptr;

    public:

        Settings()
            : m_defaultAssembly(System::Reflection::meta::ReflectionModulManager::Instance().GetCurrentAssembly())
        {
        }

        Settings(const System::Reflection::meta::ReflectionModule * _defaultAssembly)
            : m_defaultAssembly(_defaultAssembly)
        {
        }

        System::Reflection::Object *getRootInstance() const
        {
            return m_RootInstance;
        }
        void setRootInstance(System::Reflection::Object *value)
        {
            m_RootInstance = value;
        }
        std::shared_ptr<Interfaces::IInstanceLifeCycleListener> const& getInstanceLifeCycleListener() const
        {
            return m_InstanceLifeCycleListener;
        }
        void setInstanceLifeCycleListener(std::shared_ptr<Interfaces::IInstanceLifeCycleListener> const& value)
        {
            m_InstanceLifeCycleListener = value;
        }
        std::map<std::string, System::Reflection::Variant> const& getParsingContext() const
        {
            return m_ParsingContext;
        }
        void setParsingContext(std::map<std::string, System::Reflection::Variant> const& value)
        {
            m_ParsingContext = value;
        }
        const System::Reflection::meta::ReflectionModule * GetDefaultAssembly() const { return m_defaultAssembly; }

        bool IsTemplateLoader = false;
    };
}


#endif	//#ifndef __XAMLCPP_SETTINGS_H__
