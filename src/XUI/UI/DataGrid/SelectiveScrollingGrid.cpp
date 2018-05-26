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

#include "SelectiveScrollingGrid.h"
#include "UI/ScrollViewer.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

XUI::Core::Dependency::RefDependencyProperty SelectiveScrollingGrid::SelectiveScrollingOrientationProperty;

STATIC_CLASS_INITIALIZER(SelectiveScrollingGrid, XUI::UI::SelectiveScrollingGrid::StaticClassInitializer)

/// <summary>
/// Initializes static members of the <see cref="Button"/> class.
/// </summary>
void XUI::UI::SelectiveScrollingGrid::StaticClassInitializer()
{
    SelectiveScrollingOrientationProperty = (Core::Dependency::BindProperty<UI::SelectiveScrollingOrientation>::RegisterAttached<SelectiveScrollingGrid, XUI::UI::Controls::Control>(nameof(SelectiveScrollingOrientation), UI::SelectiveScrollingOrientation::Both));
}

UI::SelectiveScrollingOrientation SelectiveScrollingGrid::GetSelectiveScrollingOrientation(Core::Dependency::DependencyObject *obj)
{
    return obj->GetValue<SelectiveScrollingOrientation>(SelectiveScrollingOrientationProperty);
}

void SelectiveScrollingGrid::SetSelectiveScrollingOrientation(DependencyObject *obj, UI::SelectiveScrollingOrientation value)
{
    obj->SetValue(SelectiveScrollingOrientationProperty, value);
}

void SelectiveScrollingGrid::OnSelectiveScrollingOrientationChanged(void *d, Core::Dependency::DependencyPropertyChangedEventArgs const& e)
{
    //UIElement *element = d;
    //SelectiveScrollingOrientation orientation = e.NewValue.GetValue<SelectiveScrollingOrientation>();
    //ScrollViewer *scrollViewer = DataGridHelper::FindVisualParent<ScrollViewer*>(element);
    //if (scrollViewer != nullptr && element != nullptr)
    //{
    //    Transform *transform = element->RenderTransform;
    //
    //    if (transform != nullptr)
    //    {
    //        BindingOperations::ClearBinding(transform, TranslateTransform::XProperty);
    //        BindingOperations::ClearBinding(transform, TranslateTransform::YProperty);
    //    }
    //
    //    if (orientation == SelectiveScrollingOrientation::Both)
    //    {
    //        element->RenderTransform = nullptr;
    //    }
    //    else
    //    {
    //        TranslateTransform *translateTransform = new TranslateTransform();
    //
    //        // Add binding to XProperty of transform if orientation is not horizontal
    //        if (orientation != SelectiveScrollingOrientation::Horizontal)
    //        {
    //            Binding *horizontalBinding = new Binding("ContentHorizontalOffset");
    //            horizontalBinding->Source = scrollViewer;
    //            BindingOperations::SetBinding(translateTransform, TranslateTransform::XProperty, horizontalBinding);
    //        }
    //
    //        // Add binding to YProperty of transfrom if orientation is not vertical
    //        if (orientation != SelectiveScrollingOrientation::Vertical)
    //        {
    //            Binding *verticalBinding = new Binding("ContentVerticalOffset");
    //            verticalBinding->Source = scrollViewer;
    //            BindingOperations::SetBinding(translateTransform, TranslateTransform::YProperty, verticalBinding);
    //        }
    //
    //        element->RenderTransform = translateTransform;
    //    }
    //}
}
