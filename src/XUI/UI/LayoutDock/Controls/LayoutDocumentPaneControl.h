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

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_PANE_CONTROL_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_PANE_CONTROL_H__

#include "UI/TabControl.h"
#include "UI/LayoutDock/Controls/DocumentPaneTabPanel.h"
#include "UI/LayoutDock/Layouts/LayoutDocumentPane.h"
#include "UI/LayoutDock/Controls/ILayoutControl.h"

namespace XUI::UI
{
    class LayoutDocumentPaneControl : public UI::TabControl, public ILayoutControl
    {
    private:
        std::shared_ptr<DocumentPaneTabPanel> _tabItemHost;
        std::shared_ptr<LayoutDocumentPane> _model;

    public:

        static void StaticClassInitializer();

        LayoutDocumentPaneControl();
        LayoutDocumentPaneControl(std::shared_ptr<LayoutDocumentPane> const&);
        virtual ~LayoutDocumentPaneControl();

        void Initialize();

        PROPERTY_(std::shared_ptr<UI::LayoutDocumentPane>, Model);
        std::shared_ptr<UI::LayoutDocumentPane> const& GetModel() const { return _model; }
        void SetModel(std::shared_ptr<UI::LayoutDocumentPane> const& value)
        {
            _model = value;
        }

        LayoutElement* GetLayoutModel() override { return _model.get(); }

        void OnLayoutUpdated(void* sender, System::Events::EventArg& e);

        void OnModelItemsChanged(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e);

    protected:

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        void OnGotFocus(Interfaces::GotFocusEventArgs& e) override;
        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_DOCUMENT_PANE_CONTROL_H__