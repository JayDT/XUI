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

#ifndef __XAMLCPP_XML_IDEFERRED_LOADER_H__
#define __XAMLCPP_XML_IDEFERRED_LOADER_H__

#include "Reflection/Runtime/CXXReflection.h"

namespace XamlCPP::Core
{
    class Instruction;

    namespace Dependency
    {
        struct IDependencyObject;
    }
};

namespace XamlCPP::Interfaces
{
    struct IInstanceLifeCycleListener;
    struct IRuntimeTypeSource;
    struct IResourceNode;

    struct IDeferredLoader : System::Reflection::Object
    {
        virtual Interfaces::IResourceNode* GetResourceNode() const = 0;
        virtual System::Reflection::RefObject Load(XamlCPP::Interfaces::IInstanceLifeCycleListener* lifecycle, const Interfaces::IRuntimeTypeSource * typeSource = nullptr, Core::Dependency::IDependencyObject* resourceOwner = nullptr) = 0;
    };

    typedef std::map<System::Reflection::meta::Field const*, Interfaces::IDeferredLoader*> DeferredLoaderMapping;
}

#endif // !__XAMLCPP_XML_PARSER_H__