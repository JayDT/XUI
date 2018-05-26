/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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

#ifndef __XUI_NAMESCOPE_H__
#define __XUI_NAMESCOPE_H__

#include "XamlCPP/Interfaces/INamedScope.h"
#include <standard/events.h>

namespace XUI::UI::Controls
{
    /// <summary>
    /// Implements a name scope.
    /// </summary>
    class Meta(AbstractType) TC_XUI_API NameScope : public XamlCPP::Interfaces::INameScope
    {
    public:
        typedef std::unordered_map<std::string, std::weak_ptr<System::Reflection::meta::Object>> NameScopeMap; // google::dense_hash_map
        //static const std::shared_ptr<AttachedProperty<std::shared_ptr<INameScope>>> NameScopeProperty;

    private:
        NameScopeMap _inner;

    public:

        NameScope();
        virtual ~NameScope();

        /// <summary>
        /// Raised when an element is registered with the name scope.
        /// </summary>

        /// <summary>
        /// Registers an element with the name scope.
        /// </summary>
        /// <param name="name">The element name.</param>
        /// <param name="element">The element.</param>
        virtual void Register(std::string const & name, System::Reflection::RefObject element) override;

        /// <summary>
        /// Finds a named element in the name scope.
        /// </summary>
        /// <param name="name">The name.</param>
        /// <returns>The element, or null if the name was not found.</returns>
        virtual System::Reflection::RefObject Find(std::string const & name) override;

        /// <summary>
        /// Unregisters an element with the name scope.
        /// </summary>
        /// <param name="name">The name.</param>
        virtual void Unregister(std::string const & name) override;
    };
}


#endif	//#ifndef __XUI_NAMESCOPE_H__
