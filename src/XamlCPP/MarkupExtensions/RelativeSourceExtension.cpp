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

#include "RelativeSourceExtension.h"
#include "RelativeSource.h"

using namespace XamlCPP;
using namespace XamlCPP::Markup;

RelativeSourceExtension::RelativeSourceExtension()
{
	m_Mode = RelativeSourceMode::None;
	m_AncestorType = &System::Reflection::Type::Invalid();
	m_AncestorLevel = -1;
}

RelativeSourceExtension::RelativeSourceExtension(RelativeSourceMode mode)
{
    Mode = mode;
	m_AncestorType = &System::Reflection::Type::Invalid();
	m_AncestorLevel = -1;
}

System::Reflection::Variant RelativeSourceExtension::ProvideValue(Core::MarkupExtensionContext *extensionContext)
{
    auto source = std::make_shared<RelativeSource>();
    source->Mode = Mode;
    source->AncestorType = AncestorType;
    source->AncestorLevel = AncestorLevel;
    return source;
}
