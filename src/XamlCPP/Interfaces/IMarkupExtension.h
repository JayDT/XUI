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

#pragma once

#include <System/System.h>
#include "XamlCPP/Interfaces/IValueContext.h"

namespace XamlCPP::Core
{
    class MarkupExtensionContext
    {
    private:
        System::Reflection::Variant m_TargetObject;
        const System::Reflection::meta::Field *m_TargetProperty = nullptr;
        Interfaces::IValueContext *m_ValueContext = nullptr;

    public:
        System::Reflection::Variant const& getTargetObject() const
        {
            return m_TargetObject;
        }
        void setTargetObject(System::Reflection::Variant const& value)
        {
            m_TargetObject = value;
        }
        const System::Reflection::meta::Field *getTargetProperty() const
        {
            return m_TargetProperty;
        }
        void setTargetProperty(const System::Reflection::meta::Field *value)
        {
            m_TargetProperty = value;
        }
        Interfaces::IValueContext *getValueContext() const
        {
            return m_ValueContext;
        }
        void setValueContext(Interfaces::IValueContext *value)
        {
            m_ValueContext = value;
        }
    };
}

namespace XamlCPP
{
    enum class BindingErrorType : uint8
    {
        None,
        Error,
        DataValidationError,
    };

    enum class BindingMode : uint8
    {
        TwoWay = 0,
        OneWay = 1,
        OneTime = 2,
        OneWayToSource = 3,
        Default = 4
    };

    enum class BindingLevel : int8
    {
        Animation = 0,
        StyleTrigger = 1,
        LocalValue = 2,
        TemplatedParent,
        Style,
        Max,
        Unset = Max,
    };

    enum class RelativeSourceMode : uint8
    {
        None,
        Self,
        DataContext,
        TemplatedParent,
		FindAncestor,
    };

    namespace Interfaces
    {
        struct IMarkupExtension : System::Reflection::Object, System::EnableSharedFromThisEx<IMarkupExtension>
        {
            virtual System::Reflection::Variant ProvideValue(Core::MarkupExtensionContext* markupExtensionContext) = 0;
        };
    }
}


