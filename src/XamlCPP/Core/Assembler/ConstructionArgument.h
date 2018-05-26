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

#ifndef __XAMLCPP_CONSTRUCTIONARGUMENT_H__
#define __XAMLCPP_CONSTRUCTIONARGUMENT_H__

#include <string>
#include "Interfaces/IMarkupExtension.h"

namespace XamlCPP::Core::Assembler
{
    class ConstructionArgument
    {
    private:
        std::string StringValue;
        System::Reflection::meta::Variant Value;

    public:
        std::string getStringValue() const
        {
            return StringValue;
        }

        ConstructionArgument(const std::string &stringValue)
        {
            StringValue = stringValue;
        }

        ConstructionArgument(const std::string &stringValue, const std::string &value)
        {
            StringValue = stringValue;
            setValue(value);
        }

		ConstructionArgument(const std::shared_ptr<Interfaces::IMarkupExtension> &value)
		{
			setValue(value);
		}

        System::Reflection::meta::Variant getValue() const
        {
            return Value;
        }

        void setValue(System::Reflection::meta::Variant value)
        {
            Value = value;
        }

    public:
        bool Equals(ConstructionArgument const& other)
        {
            return getStringValue() == other.getStringValue() && getValue().IsFastEqual(other.getValue());
        }

        int GetHashCode()
        {
            //                unchecked
            return 0; //((getStringValue() != L"" ? getStringValue().GetHashCode() : 0) * 397) ^ (getValue() != nullptr ? getValue()->GetHashCode() : 0);
        }
    };
}


#endif	//#ifndef __XAMLCPP_CONSTRUCTIONARGUMENT_H__
