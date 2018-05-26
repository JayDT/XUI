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

#ifndef __XAMLCPP_STOREDINSTANCE_H__
#define __XAMLCPP_STOREDINSTANCE_H__

namespace XamlCPP::Core
{
    namespace Proto
    {
        class XamlType;
    }
}

namespace XamlCPP::Core::Assembler
{
    class StoredInstance
    {
    private:
        System::Reflection::Object *m_Instance;
        Proto::XamlType *m_XamlType;
        int orderID = -1;

    public:
        StoredInstance(System::Reflection::Object *instance, Proto::XamlType *xamlType, int order = -1)
            : orderID(order)
        {
            SetInstance(instance);
            SetXamlType(xamlType);
        }

        System::Reflection::Object *GetInstance() const
        {
            return m_Instance;
        }
        void SetInstance(System::Reflection::Object *value)
        {
            m_Instance = value;
        }
        Proto::XamlType *GetXamlType() const
        {
            return m_XamlType;
        }
        void SetXamlType(Proto::XamlType *value)
        {
            m_XamlType = value;
        }

        int GetOrderID() const { return orderID; }

        bool operator==(StoredInstance const& rhs) const
        {
            return m_Instance == rhs.m_Instance && m_XamlType == rhs.m_XamlType;
        }

        bool operator<(const System::Reflection::Object * rhs) const
        {
            if (m_Instance < rhs)
                return true;
            else
                return false;
        }

        bool operator<(const StoredInstance& rhs) const
        {
            if (m_Instance < rhs.m_Instance)
                return true;
            else
                return false;
        }

        bool operator==(const System::Reflection::Object * rhs)
        {
            if (m_Instance == rhs)
                return true;
            else
                return false;
        }

        bool operator==(const StoredInstance& rhs)
        {
            if (m_Instance == rhs.m_Instance)
                return true;
            else
                return false;
        }
    };
}


#endif	//#ifndef __XAMLCPP_STOREDINSTANCE_H__
