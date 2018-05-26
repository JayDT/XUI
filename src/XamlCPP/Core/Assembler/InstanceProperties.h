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

#ifndef __XAMLCPP_INSTANCEPROPERTIES_H__
#define __XAMLCPP_INSTANCEPROPERTIES_H__

#include <string>

namespace XamlCPP::Core::Assembler
{
    class InstanceProperties
    {
    private:
        std::string m_Name;
        System::Reflection::meta::Variant m_Key;
        bool m_HadPreviousName = false;

    public:
        std::string getName() const
        {
            return m_Name;
        }
        void setName(const std::string &value)
        {
            m_Name = value;
        }

        System::Reflection::meta::Variant const& getKey() const
        {
            return m_Key;
        }
        void setKey(System::Reflection::meta::Variant value)
        {
            m_Key = value;
        }

        bool getHadPreviousName() const
        {
            return m_HadPreviousName;
        }
        void setHadPreviousName(const bool &value)
        {
            m_HadPreviousName = value;
        }
    };
}


#endif	//#ifndef __XAMLCPP_INSTANCEPROPERTIES_H__
