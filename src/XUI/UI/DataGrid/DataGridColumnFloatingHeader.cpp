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

#include "Core/GlobalInitializerScheduler.h"
#include "DataGridColumnFloatingHeader.h"
#include "DataGrid/DataGridColumnHeader.h"
#include "Core/Media/MediaTypes.h"
#include "UI/Shapes/Rectangle.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(DataGridColumnFloatingHeader, XUI::UI::DataGridColumnFloatingHeader::StaticClassInitializer)

void XUI::UI::DataGridColumnFloatingHeader::OnWidthChanged(XamlCPP::Core::Dependency::IDependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    DataGridColumnFloatingHeader* header = static_cast<DataGridColumnFloatingHeader*>(d);
    double width = e.NewValue.GetValue<double>();
    if (header->_visualBrushCanvas && (!std::isnan(width) || width != -1.0) && header->_visualBrushCanvas->Background != nullptr)
    {
        Core::Media::VisualBrush* brush = header->_visualBrushCanvas->Background->Static_As<Core::Media::VisualBrush>();
        if (brush)
        {
            Core::Media::Rect viewBox = brush->Viewbox;
            brush->Viewbox = Core::Media::Rect::xywh(viewBox.X, viewBox.Y, width - header->GetVisualCanvasMarginX(), viewBox.Height);
        }
    }
}

System::Reflection::Variant XUI::UI::DataGridColumnFloatingHeader::OnCoerceWidth(Core::Dependency::DependencyObject * d, System::Reflection::Variant const & baseValue)
{
    DataGridColumnFloatingHeader* header = static_cast<DataGridColumnFloatingHeader*>(d);
    double width = baseValue.GetValue<double>();
    if (header->_referenceHeader && (!std::isnan(width) || width != -1.0))
    {
        return header->_referenceHeader->ActualWidth() + header->GetVisualCanvasMarginX();
    }

    return baseValue;
}

void XUI::UI::DataGridColumnFloatingHeader::OnHeightChanged(XamlCPP::Core::Dependency::IDependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    DataGridColumnFloatingHeader* header = static_cast<DataGridColumnFloatingHeader*>(d);
    double height = e.NewValue.GetValue<double>();
    if (header->_visualBrushCanvas && (!std::isnan(height) || height != -1.0) && header->_visualBrushCanvas->Background != nullptr)
    {
        Core::Media::VisualBrush* brush = header->_visualBrushCanvas->Background->Static_As<Core::Media::VisualBrush>();
        if (brush)
        {
            Core::Media::Rect viewBox = brush->Viewbox;
            brush->Viewbox = Core::Media::Rect::xywh(viewBox.X, viewBox.Y, viewBox.Width, height - header->GetVisualCanvasMarginX());
        }
    }
}

System::Reflection::Variant XUI::UI::DataGridColumnFloatingHeader::OnCoerceHeight(Core::Dependency::DependencyObject * d, System::Reflection::Variant const & baseValue)
{
    DataGridColumnFloatingHeader* header = static_cast<DataGridColumnFloatingHeader*>(d);
    double height = baseValue.GetValue<double>();
    if (header->_referenceHeader && (!std::isnan(height) || height != -1.0))
    {
        return header->_referenceHeader->ActualHeight() + header->GetVisualCanvasMarginY();
    }

    return baseValue;
}

void XUI::UI::DataGridColumnFloatingHeader::StaticClassInitializer()
{
    WidthProperty->OverrideMetadata<DataGridColumnFloatingHeader>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        double(0.0),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        DataGridColumnFloatingHeader::OnCoerceWidth));

    WidthProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<DataGridColumnFloatingHeader>();
        if (control)
            control->OnWidthChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

    HeightProperty->OverrideMetadata<DataGridColumnFloatingHeader>(std::make_shared<Core::Dependency::BindPropertyMetadata>(
        double(0.0),
        nullptr,
        Core::Dependency::BindingMode::OneWay,
        DataGridColumnFloatingHeader::OnCoerceHeight));

    HeightProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        auto control = e.Sender->Static_As<DataGridColumnFloatingHeader>();
        if (control)
            control->OnHeightChanged(control, (XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });

}

XUI::UI::DataGridColumnFloatingHeader::DataGridColumnFloatingHeader()
{
}

XUI::UI::DataGridColumnFloatingHeader::~DataGridColumnFloatingHeader()
{
}

void XUI::UI::DataGridColumnFloatingHeader::OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e)
{
    UI::Controls::Control::OnTemplateApplied(e);
    _visualBrushCanvas = Template->FindNameByManagedType<UI::Canvas>(VisualBrushCanvasTemplateName, this);
    UpdateVisualBrush();
}

void XUI::UI::DataGridColumnFloatingHeader::UpdateVisualBrush()
{
    if (_referenceHeader && _visualBrushCanvas)
    {
        //auto rect = std::make_shared<UI::Shapes::Rectangle>();

        auto visualBrush = std::make_shared<Core::Media::VisualBrush>();
        visualBrush->Visual = _referenceHeader;
        //visualBrush->Stretch = Core::Media::Stretch::None;
        //visualBrush->AlignmentX = Core::Media::AlignmentX::Left;
        //visualBrush->ViewboxUnits = BrushMappingMode::Absolute;

        double width = Width;
        if (std::isnan(width) || width == -1.0)
        {
            width = _referenceHeader->ActualWidth();
        }
        else
        {
            width = width - GetVisualCanvasMarginX();
        }

        double height = Height;
        if (std::isnan(height) || height == -1.0)
        {
            height = _referenceHeader->ActualHeight();
        }
        else
        {
            height = height - GetVisualCanvasMarginY();
        }

        Core::Media::Vector offset; // = _referenceHeader->Bounds.GetPosition();
        visualBrush->Viewbox = Core::Media::Rect::xywh(offset.x, offset.y, width, height);

        //rect->Width = width;
        //rect->Height = Height;
        //rect->Fill = visualBrush;
        //rect->GID = 1111;

        Width = width;
        Height = height;

        //_visualBrushCanvas->Children.Add(rect);
        _visualBrushCanvas->Background = visualBrush;
    }
}

double XUI::UI::DataGridColumnFloatingHeader::GetVisualCanvasMarginX()
{
    double delta = 0;
    if (_visualBrushCanvas)
    {
        auto margin = _visualBrushCanvas->Margin;
        delta += margin.Left;
        delta += margin.Right;
    }

    return delta;
}

double XUI::UI::DataGridColumnFloatingHeader::GetVisualCanvasMarginY()
{
    double delta = 0;
    if (_visualBrushCanvas)
    {
        auto margin = _visualBrushCanvas->Margin;
        delta += margin.Top;
        delta += margin.Bottom;
    }

    return delta;
}
