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

#ifndef __XAMLCPP_LEVEL_H__
#define __XAMLCPP_LEVEL_H__

#include "Core/Assembler/ConstructionArgument.h"
#include "Core/Assembler/InstanceProperties.h"
#include "Core/ReflEx/MemberBase.h"
#include "Core/DeferredXamlLoader.h"
#include <standard/collection.h>

namespace XamlCPP::Core::Assembler
{
    class Level
    {
    private:
        System::Reflection::Variant m_Instance;
        Proto::XamlType *m_XamlType;
        ReflEx::MemberBase *m_Member;
        std::shared_ptr<XamlCPP::Core::DeferredXamlLoader> m_deferLoader;
        System::Collection::Generic::IContainer* m_Collection;
        std::vector<ConstructionArgument> m_CtorArguments;
        InstanceProperties m_InstanceProperties;
        bool m_IsGetObject = false;
        bool m_WasInstanceAssignedRightAfterBeingCreated = false;
        bool m_IsEmpty = false;
        bool m_IsTopLevel = false;
		bool m_markupArgument = false;

    public:
        std::shared_ptr<XamlCPP::Core::DeferredXamlLoader> const& GetDeferLoader() const
        {
            return m_deferLoader;
        }
        void SetDeferLoader(std::shared_ptr<XamlCPP::Core::DeferredXamlLoader> const& value)
        {
            m_deferLoader = value;
        }
        System::Reflection::Variant const& GetInstance() const
        {
            return m_Instance;
        }
        void SetInstance(System::Reflection::Variant const& value)
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
        ReflEx::MemberBase *GetMember() const
        {
            return m_Member;
        }
        void SetMember(ReflEx::MemberBase *value)
        {
            m_Member = value;
        }
        System::Collection::Generic::IContainer* GetCollection() const
        {
            return m_Collection;
        }
        void SetCollection(System::Collection::Generic::IContainer* value)
        {
            m_Collection = value;
        }
        bool GetIsGetObject() const
        {
            return m_IsGetObject;
        }
        void SetIsGetObject(const bool &value)
        {
            m_IsGetObject = value;
        }
        std::vector<ConstructionArgument> const& GetCtorArguments() const
        {
            return m_CtorArguments;
        }
        void SetCtorArguments(std::vector<ConstructionArgument> const& value)
        {
            m_CtorArguments = value;
        }
        bool GetWasInstanceAssignedRightAfterBeingCreated() const
        {
            return m_WasInstanceAssignedRightAfterBeingCreated;
        }
        void SetWasInstanceAssignedRightAfterBeingCreated(const bool &value)
        {
            m_WasInstanceAssignedRightAfterBeingCreated = value;
        }
        InstanceProperties *GetInstanceProperties() const
        {
            return (InstanceProperties *)&m_InstanceProperties;
        }
        bool GetIsEmpty() const
        {
            return m_IsEmpty;
        }
        void SetIsEmpty(const bool &value)
        {
            m_IsEmpty = value;
        }
        bool GetIsTopLevel() const
        {
            return m_IsTopLevel;
        }
        void SetIsTopLevel(const bool &value)
        {
            m_IsTopLevel = value;
        }
		bool GetMarkupArgument() const
		{
			return m_markupArgument;
		}
		void SetMarkupArgument(const bool &value)
		{
			m_markupArgument = value;
		}
        bool Equals(Level *other)
        {
            return GetInstance() == other->GetInstance() && GetXamlType() == other->GetXamlType() && GetMember() == other->GetMember() && GetIsGetObject() == other->GetIsGetObject() && GetCollection() == other->GetCollection()/* && GetCtorArguments() == other->GetCtorArguments()*/;
        }

        int GetHashCode()
        {
            //                unchecked
            auto hashCode = 0; // (GetInstance() != nullptr ? GetInstance()->GetHashCode() : 0);
            //hashCode = (hashCode * 397) ^ (GetXamlType() != nullptr ? GetXamlType()->GetHashCode() : 0);
            //hashCode = (hashCode * 397) ^ (GetMember() != nullptr ? GetMember()->GetHashCode() : 0);
            //hashCode = (hashCode * 397) ^ GetIsGetObject()->GetHashCode();
            //hashCode = (hashCode * 397) ^ (GetCollection() != nullptr ? GetCollection()->GetHashCode() : 0);
            //hashCode = (hashCode * 397) ^ (GetCtorArguments() != nullptr ? GetCtorArguments()->GetHashCode() : 0);
            return hashCode;
        }

        virtual std::string ToString() const
        {
            std::string str;
            if (m_Instance.GetBase())
            {
                str += "Instance: ";
                str += m_Instance.GetType()->GetFullName();
            }
            return str;
        }
    };
}


#endif	//#ifndef __XAMLCPP_LEVEL_H__
