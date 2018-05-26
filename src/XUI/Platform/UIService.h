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

#ifndef __XUI_UIService_H__
#define __XUI_UIService_H__

#include "standard/Platform/Define.h"
#include "standard/component_service.h"
#include "standard/BasicPrimities.h"
#include "Reflection/Runtime/Variant.h"

namespace XUI::UI::Controls
{
    class InputElement;
    class Visual;
}

namespace XUI::Platform
{
    class TC_XUI_API UIService : public System::ServiceContainer
    {
    public:
        static UIService* Instance();
    };
}

#endif //!__XUI_UIService_H__