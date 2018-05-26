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

#include "AdornerLayer.h"
#include "AdornerDecorator.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

Core::Dependency::RefDependencyProperty UI::AdornerLayer::AdornedElementsProperty;

STATIC_CLASS_INITIALIZER(AdornerLayer, XUI::UI::AdornerLayer::StaticConstructor)

void XUI::UI::AdornerLayer::StaticConstructor()
{
    AdornedElementsProperty = (Core::Dependency::BindProperty<std::shared_ptr<System::Collection::List<std::shared_ptr<XUI::UI::Adorner>>>>::RegisterAttached<XUI::UI::AdornerLayer, XUI::UI::Controls::Visual>(nameof(AdornedElements)));

    IsHitTestVisibleProperty->OverrideDefaultValue<AdornerLayer>(false);
}

UI::AdornerLayer::AdornerLayer()
{
    m_visualChildrens.CollectionChanged += System::make_eventhandler(&AdornerLayer::ChildrenCollectionChanged, this);
}

System::Collection::List<std::shared_ptr<XUI::UI::Adorner>> const& UI::AdornerLayer::GetAdornedElements(std::shared_ptr<Visual> const& adorner)
{
    return *adorner->GetValue<std::shared_ptr<System::Collection::List<std::shared_ptr<XUI::UI::Adorner>>>>(AdornedElementsProperty).get();
}

void UI::AdornerLayer::Add(std::shared_ptr<XUI::UI::Controls::Visual> const& adorner, std::shared_ptr<XUI::UI::Adorner> const& adorned)
{
    adorner->GetValue<std::shared_ptr<System::Collection::List<std::shared_ptr<XUI::UI::Adorner>>>>(AdornedElementsProperty)->Add(adorned);
}

UI::AdornerLayer* UI::AdornerLayer::GetAdornerLayer(std::shared_ptr<Controls::Visual> const& visual)
{
    std::list<Visual*> visuals;
    visual->GetVisualAncestors(visuals, [](Controls::Visual* ancestor) -> bool
    {
        if (ancestor->Is<UI::AdornerLayer>())
            return true;
        return false;
    });

    if (visuals.empty())
        return nullptr;
    return (*visuals.begin())->Dynamic_As<UI::AdornerLayer>();
}

Core::Media::Size UI::AdornerLayer::ArrangeOverride(Core::Media::Size const& arrangeSize)
{
    auto parent = Parent;

    //for (auto child : GetChildren())
    //{
    //    auto info = static_cast<Adorner>(child->GetValue(s_adornedElementInfoProperty));
    //
    //    if (info != nullptr && info->Bounds)
    //    {
    //        child->RenderTransform = new MatrixTransform(info->Bounds.value().Transform);
    //        Point tempVar(0, 0);
    //        child->RenderTransformOrigin = new RelativePoint(&tempVar, RelativeUnit::Absolute);
    //        child->Arrange(info->Bounds.value().Bounds);
    //    }
    //    else
    //    {
    //        Rect tempVar2(child->DesiredSize);
    //        child->Arrange(&tempVar2);
    //    }
    //}

    return arrangeSize;
}

void UI::AdornerLayer::AdornedElementChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
{
    auto adorner = e.Sender->Dynamic_As<Controls::Visual>();
    auto adorned = e.NewValue.GetValue<std::shared_ptr<Controls::Visual>>();
    auto layer = adorner->GetVisualParent<AdornerLayer>();
    layer == nullptr ? nullptr : layer->UpdateAdornedElement(adorner, adorned.get());
}

void UI::AdornerLayer::ChildrenCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs const& e)
{
    //switch (e.Action)
    //{
    //    case System::Collection::NotifyCollectionChangedAction::Add:
    //    {
    //        for (auto i : e->NewItems)
    //        {
    //            UpdateAdornedElement(i, i->GetValue(AdornedElementProperty));
    //        }
    //
    //        break;
    //    }
    //}

    InvalidateMeasure();
}

void UI::AdornerLayer::UpdateAdornedElement(Visual *adorner, Visual *adorned)
{
    //auto info = adorner->GetValue(s_adornedElementInfoProperty);
    //
    //if (info != nullptr)
    //{
    //    delete info->Subscription;
    //
    //    if (adorned == nullptr)
    //    {
    //        adorner->ClearValue(s_adornedElementInfoProperty);
    //    }
    //}
    //
    //if (adorned != nullptr)
    //{
    //    if (info == nullptr)
    //    {
    //        info = new AdornedElementInfo();
    //        adorner->SetValue(s_adornedElementInfoProperty, info);
    //    }
    //
    //    info->Subscription = _tracker->Track(adorned).Subscribe([&](void *x)
    //    {
    //        info->Bounds = x;
    //        InvalidateArrange();
    //    });
    //}
}

