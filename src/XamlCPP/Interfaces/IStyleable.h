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

#ifndef __XUI_ISTYLEABLE_H__
#define __XUI_ISTYLEABLE_H__

#include <System/System.h>
#include <standard/collection.h>

#include <rxcpp/rx.hpp>

#undef FindResource

namespace XamlCPP::Core::Dependency
{
    class IDependencyObject;
}

namespace XamlCPP::Interfaces
{
    struct IStyle;

    /// <summary>
    /// Interface for styleable elements.
    /// </summary>
    struct Meta(Enable) IStyleable
    {
        /// <summary>
        /// Signalled when the control's style should be removed.
        /// </summary>
        virtual rxcpp::subjects::subject<IStyleable*> *GetStyleDetach() = 0;

        /// <summary>
        /// Gets the list of classes for the control.
        /// </summary>
        //virtual std::set<std::string> const& GetClasses() const = 0;

        virtual std::shared_ptr<XamlCPP::Interfaces::IStyle> const& GetStyle() const = 0;
        virtual std::shared_ptr<XamlCPP::Interfaces::IStyle> const& GetFocusVisualStyle() const = 0;

        /// <summary>
        /// Gets the template parent of this element if the control comes from a template.
        /// </summary>
        //virtual ITemplatedControl *GetTemplatedParent() = 0;

        virtual Core::Dependency::IDependencyObject* ToIDependencyObject() = 0;
    };
}


#endif	//#ifndef __XUI_ISTYLEABLE_H__
