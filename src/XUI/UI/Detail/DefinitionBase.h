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

#ifndef __XUI_DEFINITIONBASE_H__
#define __XUI_DEFINITIONBASE_H__

#include "Core/Dependencies/DependencyObject.h"
#include "Core/Dependencies/Models/DataProperty.h"

#include <string>

namespace XUI::UI
{
    /// <summary>
    /// Base class for <see cref="ColumnDefinition"/> and <see cref="RowDefinition"/>.
    /// </summary>
    struct DefinitionBase : Core::Dependency::DependencyObject
    {
        /// <summary>
        /// Defines the <see cref="SharedSizeGroup"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty SharedSizeGroupProperty;

        /// <summary>
        /// Gets or sets the name of the shared size group of the column or row.
        /// </summary>
        PROPERTY_(std::string, SharedSizeGroup);
        std::string GetSharedSizeGroup() { return GetValue<std::string>(SharedSizeGroupProperty.get()); }
        void SetSharedSizeGroup(std::string const& value) { SetValue(SharedSizeGroupProperty.get(), value); }
    };
}

#endif	//#ifndef __XUI_DEFINITIONBASE_H__
