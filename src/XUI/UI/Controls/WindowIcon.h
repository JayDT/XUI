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

#ifndef __XUI_WINDOWICON_H__
#define __XUI_WINDOWICON_H__

#include "Interfaces/IBitmap.h"
#include "Interfaces/IWindowIconImp.h"
#include "Interfaces/IPlatformIconLoader.h"
#include "Platform/UIService.h"

namespace XUI::UI
{
    /// <summary>
    /// Represents an icon for a window.
    /// </summary>
    class Meta(AbstractType) WindowIcon
        : public System::Reflection::Object
    {
    private:
        std::shared_ptr<Interfaces::IWindowIconImpl> m_PlatformImpl;

    public:
        WindowIcon(std::shared_ptr<Interfaces::IBitmapImpl> bitmap)
        {
            auto platform = XUI::Platform::UIService::Instance()->Get<Interfaces::IPlatformIconLoader>();
            if (platform != nullptr)
                m_PlatformImpl = platform->LoadIcon(bitmap);
        }

        WindowIcon(const std::string &fileName)
        {
            auto platform = XUI::Platform::UIService::Instance()->Get<Interfaces::IPlatformIconLoader>();
            if (platform != nullptr)
                m_PlatformImpl = platform->LoadIcon(fileName);
        }

        WindowIcon(std::vector<byte> const& stream)
        {
            auto platform = XUI::Platform::UIService::Instance()->Get<Interfaces::IPlatformIconLoader>();
            if (platform != nullptr)
                m_PlatformImpl = platform->LoadIcon(stream);
        }

        PROPERTY_TG(std::shared_ptr<Interfaces::IWindowIconImpl>, PlatformImpl)
        PROPERTY_T_GET(PlatformImpl, )
        {
            return m_PlatformImpl;
        }

        std::vector<byte> Save()
        {
            if (m_PlatformImpl)
                return *m_PlatformImpl->Save().get();

            return {};
        }
    };
}


#endif	//#ifndef __XUI_WINDOWICON_H__
