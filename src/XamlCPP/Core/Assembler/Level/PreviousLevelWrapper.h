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

#ifndef __XAMLCPP_PREVIOUSLEVELWRAPPER_H__
#define __XAMLCPP_PREVIOUSLEVELWRAPPER_H__

#include "Core/Assembler/Level/Level.h"
#include "Core/ReflEx/MemberBase.h"

namespace XamlCPP::Core::Assembler
{
    class PreviousLevelWrapper
    {
    private:
        Level * level = nullptr;

    public:
        virtual ~PreviousLevelWrapper()
        {
        }

        PreviousLevelWrapper()
        {

        }

        PreviousLevelWrapper(Level* level) : level(level)
        {
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

        System::Reflection::Variant const& getInstance() const
        {
            return level->GetInstance();
        }

        void setInstance(System::Reflection::Variant const& value)
        {
            level->SetInstance(value);
        }

        System::Collection::Generic::IContainer* getCollection() const
        {
            return level->GetCollection();
        }

        void setCollection(System::Collection::Generic::IContainer* value)
        {
            level->SetCollection(value);
        }

        bool getXamlMemberIsOneToMany() const
        {
            return getMember()->XamlType->IsCollection();
        }

        bool getIsMapContainer() const
        {
            return getCollection()->Is<System::Collection::Generic::IMapContainer>();
        }

        bool getCanHostChildren() const
        {
            return getXamlMemberIsOneToMany() && getCollection();
        }
        bool getIsEmpty() const
        {
            return level->GetIsEmpty();
        }
		bool getIsMarkupExtension() const
		{
			return getInstance().IsObject() ? getInstance().ToObject()->Is<Interfaces::IMarkupExtension>() : false;
		}
    };
}


#endif	//#ifndef __XAMLCPP_PREVIOUSLEVELWRAPPER_H__
