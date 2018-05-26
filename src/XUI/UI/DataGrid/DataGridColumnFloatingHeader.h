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

#ifndef __XUI_DATAGRID_FLOATING_HEADER_H__
#define __XUI_DATAGRID_FLOATING_HEADER_H__

#include "UI/Controls/Control.h"
#include "UI/Canvas.h"
#include "Core/Media/VisualBrush.h"

namespace XUI::UI
{
    class DataGridColumnHeader;

    /// <summary>
    /// Panel that lays out individual rows top to bottom.  
    /// </summary>
    class DataGridColumnFloatingHeader : public UI::Controls::Control
    {
    private:

        static constexpr char* VisualBrushCanvasTemplateName = "PART_VisualBrushCanvas";
        std::shared_ptr<Canvas> _visualBrushCanvas;

    public:
        std::shared_ptr<DataGridColumnHeader> _referenceHeader;

        static void OnWidthChanged(XamlCPP::Core::Dependency::IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);
        static System::Reflection::Variant OnCoerceWidth(Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue);
        static void OnHeightChanged(XamlCPP::Core::Dependency::IDependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);
        static System::Reflection::Variant OnCoerceHeight(Core::Dependency::DependencyObject* d, System::Reflection::Variant const& baseValue);

        static void StaticClassInitializer();

        DataGridColumnFloatingHeader();
        virtual ~DataGridColumnFloatingHeader();

        DataGridColumnHeader* ReferenceHeader() { return _referenceHeader.get(); }

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        void ClearHeader()
        {
            _referenceHeader = nullptr;
            if (_visualBrushCanvas != nullptr)
            {
                _visualBrushCanvas->Background = nullptr;
            }
        }

    private:

        void UpdateVisualBrush();

        double GetVisualCanvasMarginX();

        double GetVisualCanvasMarginY();
    };
}


#endif	//#ifndef __XUI_DATAGRID_FLOATING_HEADER_H__
