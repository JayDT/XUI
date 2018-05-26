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

#ifndef __XAMLCPP_CURRENTLEVELWRAPPER_H__
#define __XAMLCPP_CURRENTLEVELWRAPPER_H__

#include "Core/Assembler/Level/Level.h"
#include "Core/ReflEx/Member.h"
#include "Core/Assembler/ConstructionArgument.h"
#include "Core/Assembler/InstanceProperties.h"
#include "Interfaces/IMarkupExtension.h"
#include <string>

namespace XamlCPP::Core::Assembler
{
    class CurrentLevelWrapper
    {
    private:
        Level * level = nullptr;
        Interfaces::IValueContext * valueContext = nullptr;

    public:
        virtual ~CurrentLevelWrapper()
        {
        }

        CurrentLevelWrapper()
        {

        }

        CurrentLevelWrapper(Level* level, Interfaces::IValueContext * valueContext) : level(level), valueContext(valueContext)
        {
        }

        std::string getInstanceName() const
        {
            auto runtimeNameMember = getXamlType()->getRuntimeNamePropertyMember();
            return runtimeNameMember == nullptr ? "" : runtimeNameMember->GetValue(getInstance().ToObject()).ToString();
        }
        void setInstanceName(const std::string &value)
        {
            auto runtimeNameMember = getXamlType()->getRuntimeNamePropertyMember();
            runtimeNameMember == nullptr ? nullptr : runtimeNameMember->SetValue(getInstance().ToObject(), value, valueContext);
        }

        Proto::XamlType *getInstanceXamlType() const
        {
            return valueContext->getTypeRepository()->GetByType(getInstance().GetType());
        }

        Level * GetLevel() const
        {
            return level;
        }

        ReflEx::MemberBase *getMember() const
        {
            return level->GetMember();
        }
        void setMember(ReflEx::MemberBase *value)
        {
            level->SetMember(value);
        }

        bool getIsGetObject() const
        {
            return level->GetIsGetObject();
        }
        void setIsGetObject(const bool &value)
        {
            level->SetIsGetObject(value);
        }

        System::Collection::Generic::IContainer* getCollection() const
        {
            return level->GetCollection();
        }
        void setCollection(System::Collection::Generic::IContainer* value)
        {
            level->SetCollection(value);
        }

        System::Reflection::Variant const& getInstance() const
        {
            return level->GetInstance();
        }
        void setInstance(System::Reflection::Variant const& value)
        {
            level->SetInstance(value);

            if (value.GetBase() && value.IsObject())
            {
                auto type = value.GetType();
                if (type)
                {
                    auto xamlType = valueContext->getTypeRepository()->GetByType(type);
                    valueContext->getTopDownValueContext()->Add(value.ToObject(), xamlType);
                }
            }
        }

        Proto::XamlType *getXamlType() const
        {
            return level->GetXamlType();
        }
        void setXamlType(Proto::XamlType *value)
        {
            level->SetXamlType(value);
        }

        std::vector<ConstructionArgument> const& getCtorArguments() const
        {
            return level->GetCtorArguments();
        }
        void setCtorArguments(std::vector<ConstructionArgument> const& value)
        {
            level->SetCtorArguments(value);
        }

        bool getHasInstance() const
        {
            return getInstance().GetBase();
        }
        bool getIsMarkupExtension() const
        {
            return getInstance().IsObject() ? getInstance().ToObject()->As<Interfaces::IMarkupExtension>() : false;
        }

        InstanceProperties *getInstanceProperties() const
        {
            return level->GetInstanceProperties();
        }

        bool getWasAssociatedRightAfterCreation() const
        {
            return level->GetWasInstanceAssignedRightAfterBeingCreated();
        }
        void setWasAssociatedRightAfterCreation(const bool &value)
        {
            level->SetWasInstanceAssignedRightAfterBeingCreated(value);
        }
    };
}


#endif	//#ifndef __XAMLCPP_CURRENTLEVELWRAPPER_H__
