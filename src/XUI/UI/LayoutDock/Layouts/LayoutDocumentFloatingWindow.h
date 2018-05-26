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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_FLOATING_WINDOW_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_FLOATING_WINDOW_H__

#include "UI/LayoutDock/Layouts/LayoutFloatingWindow.h"
#include "UI/LayoutDock/Layouts/LayoutDocument.h"

namespace XUI::UI
{
    //[ContentProperty("RootPanel")]
    class LayoutDocumentFloatingWindow : public LayoutFloatingWindow
    {
    private:

        std::shared_ptr<LayoutDocument> _rootDocument;

    public:

        LayoutDocumentFloatingWindow();
        virtual ~LayoutDocumentFloatingWindow();

        System::Events::EventHandler<System::Events::EventArg> RootDocumentChanged;

        //Meta(XamlCPP::ContentAttribute())
        PROPERTY_(std::shared_ptr<LayoutDocument>, RootDocument);
        std::shared_ptr<LayoutDocument> const& GetRootDocument() const { return _rootDocument; }
        void SetRootDocument(std::shared_ptr<LayoutDocument> const& value);

        bool IsValid() override;

        void AddChild(LayoutElement* child) override;

        void RemoveChild(LayoutElement* child) override;

        void ReplaceChild(LayoutElement* oldElement, LayoutElement* newElement) override;

    protected:

    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_FLOATING_WINDOW_H__