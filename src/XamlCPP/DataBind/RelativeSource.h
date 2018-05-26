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

#ifndef __XUI_RELATIVESOURCE_H__
#define __XUI_RELATIVESOURCE_H__

#include <System/System.h>
#include "XamlCPP/Interfaces/IBinding.h"

namespace XamlCPP::Markup
{
	struct TC_XAMLCPP_API RelativeSource : System::Reflection::Object
	{
		virtual ~RelativeSource() {}

		RelativeSourceMode _mode = RelativeSourceMode::None;
		const System::Reflection::Type* _ancestorType = &System::Reflection::Type::Invalid();
		int _ancestorLevel = -1;

		PROPERTY_(RelativeSourceMode, Mode);
		RelativeSourceMode GetMode() const
        {
            return _mode;
        }
		void SetMode(RelativeSourceMode const& value)
        {
            _mode = value;
        }

		PROPERTY_(const System::Reflection::Type*, AncestorType);
		const System::Reflection::Type* GetAncestorType() const
        {
            return _ancestorType;
        }
		void SetAncestorType(const System::Reflection::Type* value)
        {
            _ancestorType = value;
        }

		PROPERTY_(int, AncestorLevel);
		int GetAncestorLevel() const
        {
            return _ancestorLevel;
        }
		void SetAncestorLevel(int value)
        {
            _ancestorLevel = value;
        }
	};
}


#endif	//#ifndef __XUI_RELATIVESOURCE_H__
