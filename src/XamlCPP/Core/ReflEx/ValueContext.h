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

#ifndef __XAMLCPP_VALUECONTEXT_H__
#define __XAMLCPP_VALUECONTEXT_H__

#include "Interfaces/ITypeRepository.h"
#include "Interfaces/IValueContext.h"

#include <standard/collection.h>
#include <string>
#include <vector>
#include <memory>

namespace XamlCPP::Core
{
    namespace ReflEx
    {
        class MemberBase;
        class MutableMember;
        class NamespaceDeclaration;
    }
}

namespace XamlCPP::Core::ReflEx
{
    class ValueContext : public Interfaces::IValueContext
    {
    private:
        std::shared_ptr<Interfaces::ITypeRepository> const typeRepository;
        std::shared_ptr<Interfaces::ITopDownValueContext> const topDownValueContext;
        System::Collection::Map<std::string, System::Reflection::Variant> parsingDictionary;

    public:
        virtual ~ValueContext()
        {
        }

        ValueContext(std::shared_ptr<Interfaces::ITypeRepository> typeRepository, std::shared_ptr<Interfaces::ITopDownValueContext>  topDownValueContext)
            : typeRepository(typeRepository)
            , topDownValueContext(topDownValueContext)
        {
        }

        Interfaces::ITypeRepository *getTypeRepository() const override
        {
            return typeRepository.get();
        }

        Interfaces::ITopDownValueContext *getTopDownValueContext() const override
        {
            return topDownValueContext.get();
        }

        System::Collection::Map<std::string, System::Reflection::Variant> const& getParsingDictionary() const override
        {
            return parsingDictionary;
        }
    };
}


#endif	//#ifndef __XAMLCPP_VALUECONTEXT_H__
