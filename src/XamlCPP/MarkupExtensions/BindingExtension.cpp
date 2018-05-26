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

#include "BindingExtension.h"
#include "DataBind/Binding.h"

using namespace XamlCPP::Markup;

BindingExtension::BindingExtension()
{
    Priority = BindingLevel::LocalValue;
}

BindingExtension::BindingExtension(const std::string &path)
{
    Priority = BindingLevel::LocalValue;
    Path = path;
}

System::Reflection::Variant BindingExtension::ProvideValue(XamlCPP::Core::MarkupExtensionContext *extensionContext)
{
    auto bind = std::make_shared<DataBind::Binding>();
    bind->Converter = Converter;
    bind->ConverterParameter = ConverterParameter;
    bind->ElementName = ElementName;
    bind->FallbackValue = FallbackValue;
    bind->Mode = Mode;
    bind->Path = Path;
    bind->BindingLevel = Priority;
	if (RelativeSource)
	{
		bind->RelativeSource->Mode = RelativeSource->Mode;
		bind->RelativeSource->AncestorType = RelativeSource->AncestorType;
		bind->RelativeSource->AncestorLevel = RelativeSource->AncestorLevel;
	}
    System::ThrowIfFailed<System::ArgumentException>(!bind->Path.empty());
    return bind;
}

