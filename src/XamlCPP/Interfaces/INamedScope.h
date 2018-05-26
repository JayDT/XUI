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

#ifndef __XAMLCPP_INAMESCOPE_H__
#define __XAMLCPP_INAMESCOPE_H__

#include "Reflection/Runtime/Object.h"
#include "standard/events.h"

#include <boost/uuid/uuid.hpp>

namespace XamlCPP::DataBind
{
    class InstancedBinding;
}

namespace XamlCPP::Interfaces
{
    struct NameScopeEventArgs : System::Events::EventArg
    {
        NameScopeEventArgs(std::string const& name, System::Reflection::RefObject element)
        {
            Name = name;
            Element = element;
        }

        std::string Name;
        System::Reflection::RefObject Element;
    };

    /// <summary>
    /// Defines a name scope.
    /// </summary>
    struct Meta(Enable) INameScope
    {
        //// {1D058562-95AB-4C06-BD39-369AB73331AD}
        static constexpr boost::uuids::uuid __uuid = { 0x1d, 0x058, 0x56, 0x2, 0x95, 0xab, 0x4c, 0x06, 0xbd, 0x39, 0x36, 0x9a, 0xb7, 0x33, 0x31, 0xad };

        /// <summary>
        /// Raised when an element is registered with the name scope.
        /// </summary>
        System::Events::EventHandler<NameScopeEventArgs> Registered;

        /// <summary>
        /// Raised when an element is unregistered with the name scope.
        /// </summary>
        System::Events::EventHandler<NameScopeEventArgs> Unregistered;

        /// <summary>
        /// Registers an element eith the name scope.
        /// </summary>
        /// <param name="name">The element name.</param>
        /// <param name="element">The element.</param>
        virtual void Register(std::string const& name, System::Reflection::RefObject element) = 0;

        /// <summary>
        /// Finds a named element in the name scope.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <returns>The element, or null if the name was not found.</returns>
        virtual System::Reflection::RefObject Find(std::string const& name) = 0;

        /// <summary>
        /// Unregisters an element with the name scope.
        /// </summary>
        /// <param name="name">The name.</param>
        virtual void Unregister(std::string const& name) = 0;

        template<typename TType>
        TType Get(std::string const & name)
        {
            auto namedObj = Find(name);

            if (namedObj)
                return namedObj->Dynamic_As<std::remove_pointer<TType>::type>();

            return TType();
        }
    };

    /// <summary>
    /// Defines a name scope.
    /// </summary>
    struct Meta(Enable) IInstancedBindingManager
    {
        //// {056490FE-FC01-4D37-9900-BEE00AB55DC4}
        static constexpr boost::uuids::uuid __uuid = { 0x56, 0x49, 0x0f, 0xe, 0xfc, 0x01, 0x4d, 0x37, 0x99, 0x0, 0xbe, 0xe0, 0xa, 0xb5, 0x5d, 0xc4 };

        virtual void AddInstancedBinding(XamlCPP::DataBind::InstancedBinding*) = 0;
        virtual void RemoveInstancedBinding(XamlCPP::DataBind::InstancedBinding*) = 0;
    };
}

#endif // !__XAMLCPP_INAMESCOPE_H__
