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

#ifndef __XUI_UI_DATACOLUMN_TEXT_H__
#define __XUI_UI_DATACOLUMN_TEXT_H__

#include "UI/DataGrid/DataGridColumn.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/DataGrid/DataGridCell.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DataGridTextColumn : public DataGridColumn
    {
    public:
        /// <summary>
        /// Defines the <see cref="FontFamily"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty FontFamilyProperty;

        /// <summary>
        /// Defines the <see cref="FontSize"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty FontSizeProperty;

        /// <summary>
        /// Defines the <see cref="FontStyle"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty FontStyleProperty;

        /// <summary>
        /// Defines the <see cref="FontWeight"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty FontWeightProperty;

        /// <summary>
        /// Defines the <see cref="Foreground"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty ForegroundProperty;

        //static Style _defaultElementStyle;
        //static Style _defaultEditingElementStyle;

        static void StaticClassInitializer();

        DataGridTextColumn();
        virtual ~DataGridTextColumn();

        /// <summary>
        /// Gets or sets the font family used to draw the control's text.
        /// </summary>
        PROPERTY_(std::string, FontFamily);
        std::string GetFontFamily() const;
        void SetFontFamily(std::string value);

        /// <summary>
        /// Gets or sets the size of the control's text in points.
        /// </summary>
        PROPERTY_(double, FontSize);
        double GetFontSize() const;
        void SetFontSize(double value);

        /// <summary>
        /// Gets or sets the font style used to draw the control's text.
        /// </summary>
        PROPERTY_(Core::Media::FontStyle, FontStyle);
        Core::Media::FontStyle GetFontStyle() const;
        void SetFontStyle(Core::Media::FontStyle value);

        /// <summary>
        /// Gets or sets the font weight used to draw the control's text.
        /// </summary>
        PROPERTY_(Core::Media::FontWeight, FontWeight);
        Core::Media::FontWeight GetFontWeight() const;
        void SetFontWeight(Core::Media::FontWeight value);

        /// <summary>
        /// Gets or sets the brush used to draw the control's text and other foreground elements.
        /// </summary>
        PROPERTY_(std::shared_ptr<XUI::Interfaces::IBrush>, Foreground);
        std::shared_ptr<XUI::Interfaces::IBrush> GetForeground() const;
        void SetForeground(std::shared_ptr<XUI::Interfaces::IBrush> value);

        // Inherited via DataGridColumn
        virtual std::shared_ptr<XUI::UI::Controls::Control> GenerateElement(DataGridCell * cell, System::Reflection::Variant const& dataItem) override;

        virtual std::shared_ptr<XUI::UI::Controls::Control> GenerateEditingElement(DataGridCell * cell, System::Reflection::Variant const& dataItem) override;

        void OnInput(Core::Observer::RoutedEventArgs const& e);

        bool CommitCellEdit(XUI::UI::Controls::Control* editingElement) override;

        void CancelCellEdit(XUI::UI::Controls::Control* editingElement, System::Reflection::Variant& uneditedValue) override;

        virtual System::Reflection::Variant PrepareCellForEdit(XUI::UI::Controls::Control* editingElement, Core::Observer::RoutedEventArgs& editingEventArgs) override;

    private:

        void SyncProperties(XUI::UI::Controls::Control* e);

    };
}

#endif //!__XUI_UI_DATACOLUMN_TEXT_H__