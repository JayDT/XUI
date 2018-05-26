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

#ifndef __XAMLCPP_TYPEOPERATIONS_H__
#define __XAMLCPP_TYPEOPERATIONS_H__

#include <standard/collection.h>

namespace XamlCPP::Core
{
    namespace Proto
    {
        class XamlType;
    }
}

namespace XamlCPP::Core::Assembler
{
    class TypeOperations
    {
    private:
        std::shared_ptr<ITypeFactory> typeFactory;

    public:
        virtual ~TypeOperations()
        {
        }

        TypeOperations(ITypeFactory *typeFactory) : typeFactory(typeFactory)
        {
        }

        static void AddToCollection(System::Collection::Generic::IContainer *collection, System::Reflection::Variant const& instance)
        {
            collection->Add(instance);
        }

        static void AddToDictionary(System::Collection::Generic::IMapContainer *collection, System::Reflection::Variant& key, System::Reflection::Variant const& value)
        {
            if (!key.GetBase())
            {
                const XamlCPP::MapKeyPropertyAttribute* keyName = value.GetType()->GetMeta().GetCustomAttribute<XamlCPP::MapKeyPropertyAttribute>();
                if (keyName)
                {
                    const System::Reflection::Field& keyProp = value.GetType()->GetRuntimeProperty(keyName->GetName());
                    if (keyProp.IsValid())
                    {
                        key = keyProp.GetValue(value.ToObject());
                    }
                }

                System::ThrowIfFailed<XamlCPP::ParseException>(key.GetBase(), "Not found key");
            }

            collection->Add(key, value);
        }
    };
}

#endif	//#ifndef __XAMLCPP_TYPEOPERATIONS_H__
