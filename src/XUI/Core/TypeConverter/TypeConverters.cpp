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

#include "StdAfx.h"
#include "TypeConverters.h"
#include "XamlCPP/Core/ReflEx/TypeConverterRegistration.h"
#include "XamlCPP/XamlCPP.h"
#include "XamlCPP/Core/Assembler/Settings.h"
#include "XamlCPP/Core/ReflEx/TypeConverterRegistration.h"
#include "XamlCPP/Interfaces/ITypeConverter.h"

#include "Core/Media/Thickness_t.h"
#include "Core/Media/Size_t.h"
#include "Core/Media/RelativePoint_t.h"
#include "Core/Media/Brush.h"
#include "Core/Media/Color_t.h"
#include "Core/Media/Geometry.h"
#include "Core/Media/Brush.h"
#include "Core/Media/MediaTypes.h"
#include "Core/Input/Cursors.h"
#include "UI/Detail/GridLength.h"
#include "UI/Detail/ColumnDefinition.h"
#include "UI/Detail/RowDefinition.h"
#include "UI/Detail/ToggleButton.h"
#include "Core/TypeConverter/TypeConverterImpl.h"

#include "Interfaces/IBrush.h"

#include <System/BaseTypes.h>

// __VA_ARGS__
#define RegisterStringCompatibleConverter(...) \
    { \
        XamlCPP::Core::TypeConverterRegistration converter(&typeof(__VA_ARGS__), std::make_shared<XamlCPP::Interfaces::StringCompatibleTypeConverter<__VA_ARGS__>>()); \
        XamlCPP::Xaml::GetRuntimeTypeSource()->AddTypeConverter(converter); \
    }

template<typename TEnum>
struct XUIEnumConverter
{
    static TEnum Parse(std::string const& s)
    {
        bool bvalue;
        long long llvalue;
        if (System::Bool::TryParse(s, bvalue))
            return TEnum((int)bvalue);
        if (System::Int64::TryParse(s, llvalue))
            return TEnum(llvalue);
        return TEnum();
    }

    static std::string ToString(TEnum _enum)
    {
        static const System::Reflection::Type& enumType = typeof(TEnum);
        if (enumType.IsValid())
            return enumType.GetEnum()->GetKey((unsigned long long)_enum);
        return "";
    }
};

void XUI::Core::TypeConverter::Initialize()
{
    {
        XamlCPP::Core::TypeConverterRegistration converter(&typeof(XUI::Interfaces::IBitmapImpl), std::make_shared<XUI::Core::TypeConversion::BitMapConverter>());
        XamlCPP::Xaml::GetRuntimeTypeSource()->AddTypeConverter(converter);
    }
    {
        XamlCPP::Core::TypeConverterRegistration converter(&typeof(XUI::Interfaces::IBrush), std::make_shared<XUI::Core::TypeConversion::BrushValueConverter>());
        XamlCPP::Xaml::GetRuntimeTypeSource()->AddTypeConverter(converter);
    }
    {
        XamlCPP::Core::TypeConverterRegistration converter(&typeof(XUI::Core::Media::SolidColorBrush), std::make_shared<XamlCPP::Interfaces::StringCompatiblePointerTypeConverter<Interfaces::IBrush, XUI::Core::Media::Brush>>());
        XamlCPP::Xaml::GetRuntimeTypeSource()->AddTypeConverter(converter);
    }
    {
        XamlCPP::Core::TypeConverterRegistration converter(&typeof(XUI::Interfaces::ICursor), std::make_shared<XamlCPP::Interfaces::StringCompatiblePointerTypeConverter<XUI::Interfaces::ICursor, XUI::Core::Input::Cursor>>());
        XamlCPP::Xaml::GetRuntimeTypeSource()->AddTypeConverter(converter);
    }

    RegisterStringCompatibleConverter(XUI::Core::Media::Color)
    RegisterStringCompatibleConverter(XUI::Core::Media::Thickness)
    RegisterStringCompatibleConverter(XUI::Core::Media::Size)
    RegisterStringCompatibleConverter(XUI::Core::Media::RelativePoint)
    RegisterStringCompatibleConverter(XUI::Core::Media::Rect)
    RegisterStringCompatibleConverter(XUI::Core::Media::Point)
    RegisterStringCompatibleConverter(XUI::Core::Media::Vector)

    // Grid
    RegisterStringCompatibleConverter(XUI::UI::GridLength)

    {
        XamlCPP::Core::TypeConverterRegistration converter(&typeof(XUI::UI::ColumnDefinition), std::make_shared<XamlCPP::Interfaces::StringCompatiblePointerTypeConverter<XUI::UI::ColumnDefinition>>());
        XamlCPP::Xaml::GetRuntimeTypeSource()->AddTypeConverter(converter);
    }
    {
        XamlCPP::Core::TypeConverterRegistration converter(&typeof(XUI::UI::RowDefinition), std::make_shared<XamlCPP::Interfaces::StringCompatiblePointerTypeConverter<XUI::UI::RowDefinition>>());
        XamlCPP::Xaml::GetRuntimeTypeSource()->AddTypeConverter(converter);
    }

    {
        XamlCPP::Core::TypeConverterRegistration converter(&typeof(XUI::Core::Media::Geometry), std::make_shared<XamlCPP::Interfaces::StringCompatiblePointerTypeConverter<XUI::Core::Media::Geometry>>());
        XamlCPP::Xaml::GetRuntimeTypeSource()->AddTypeConverter(converter);
    }

    // Special
    {
        XamlCPP::Core::TypeConverterRegistration converter(&typeof(XUI::UI::ToggleButtonState), std::make_shared<XamlCPP::Interfaces::EnumTypeConverter<XUI::UI::ToggleButtonState, XUIEnumConverter<XUI::UI::ToggleButtonState>>>());
        XamlCPP::Xaml::GetRuntimeTypeSource()->AddTypeConverter(converter);
    }

    //XamlCPP::Core::TypeConverterRegistration converter(&typeof(Core::Observer::PropertyRoutedEventHandler), std::make_shared<>());
    //XamlCPP::Xaml::GetRuntimeTypeSource()->AddTypeConverter(converter);
}
