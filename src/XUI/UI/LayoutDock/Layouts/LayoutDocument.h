/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_H__

#include "UI/LayoutDock/Layouts/LayoutContent.h"

namespace XUI::UI
{
    class LayoutDocument : public LayoutContent
    {
    private:

        std::string _description;

    public:

        LayoutDocument();
        virtual ~LayoutDocument();

        PROPERTY_(std::string, Description);
        std::string const& GetDescription() const { return _description; }
        void SetDescription(std::string const& value);

        void Close() override;

        bool CloseDocument();

    protected:

        void InternalDock() override;
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_H__