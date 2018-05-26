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

#ifndef __XUI_UI_DATACOLUMN_COMBOBOX_H__
#define __XUI_UI_DATACOLUMN_COMBOBOX_H__

#include "UI/DataGrid/DataGridColumn.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/DataGrid/DataGridCell.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DataGridComboBoxColumn : public DataGridColumn
    {
    public:

        static  Core::Dependency::RefDependencyProperty ItemsSourceProperty;

        static void StaticClassInitializer();

        DataGridComboBoxColumn();
        virtual ~DataGridComboBoxColumn();

        void OnInput(Core::Observer::RoutedEventArgs const& e);

        bool CommitCellEdit(XUI::UI::Controls::Control* editingElement) override;

        void CancelCellEdit(XUI::UI::Controls::Control* editingElement, System::Reflection::Variant& uneditedValue) override;

        virtual System::Reflection::Variant PrepareCellForEdit(XUI::UI::Controls::Control* editingElement, Core::Observer::RoutedEventArgs& editingEventArgs) override;

        DEPENDENCY_PROPERTY_(std::shared_ptr<System::Collection::Generic::IContainer>, ItemsSource)

    private:

        void SyncProperties(XUI::UI::Controls::Control* e);

	protected:
        // Inherited via DataGridColumn
        virtual std::shared_ptr<XUI::UI::Controls::Control> GenerateElement(DataGridCell * cell, System::Reflection::Variant const& dataItem) override;
        virtual std::shared_ptr<XUI::UI::Controls::Control> GenerateEditingElement(DataGridCell * cell, System::Reflection::Variant const& dataItem) override;
    };
}

#endif //!__XUI_UI_DATACOLUMN_COMBOBOX_H__