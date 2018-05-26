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

#include "TemplateBindingExtension.h"
#include "Binding.h"

using namespace XamlCPP;
using namespace XamlCPP::Markup;

TemplateBindingExtension::TemplateBindingExtension()
{
    Mode = BindingMode::OneWay;
    Priority = BindingLevel::LocalValue;
}

TemplateBindingExtension::TemplateBindingExtension(const std::string &path)
{
    Mode = BindingMode::OneWay;
    Priority = BindingLevel::LocalValue;
    Path = path;
}

System::Reflection::Variant TemplateBindingExtension::ProvideValue(Core::MarkupExtensionContext *extensionContext)
{
    auto bind = std::make_shared<DataBind::Binding>();
    bind->Converter = Converter;
    bind->ElementName = ElementName;
    bind->Mode = Mode;
    bind->RelativeSource->Mode = RelativeSourceMode::TemplatedParent;
    bind->Path = Path;
    bind->BindingLevel = Priority;
    return bind;
}
