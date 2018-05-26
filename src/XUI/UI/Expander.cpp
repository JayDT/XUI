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
#include "Expander.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

XUI::Core::Dependency::RefDependencyProperty Expander::ContentTransitionProperty;
XUI::Core::Dependency::RefDependencyProperty Expander::ExpandDirectionProperty;
XUI::Core::Dependency::RefDependencyProperty Expander::IsExpandedProperty;

STATIC_CLASS_POST_INITIALIZER(Expander, Expander::StaticClassInitializer)

void Expander::StaticClassInitializer()
{
    //ContentTransitionProperty = (Core::Dependency::DirectProperty<Expander, IPageTransition>::RegisterDirect(nameof(ContentTransition),
    //    [](Expander* x) { return  x->ContentTransition; },
    //    [](Expander* x, IPageTransition v) { x->ContentTransition = v; })
    //    );
    ExpandDirectionProperty = (Core::Dependency::DirectProperty<Expander, XUI::UI::ExpandDirection>::RegisterDirect(nameof(ExpandDirection),
        [](Expander* x) { return  x->ExpandDirection; },
        [](Expander* x, XUI::UI::ExpandDirection v) { x->ExpandDirection = v; })
        );
    IsExpandedProperty = (Core::Dependency::DirectProperty<Expander, bool>::RegisterDirect(nameof(IsExpanded),
        [](Expander* x) { return  x->IsExpanded; },
        [](Expander* x, bool v) { x->IsExpanded = v; })
        );

    //        PseudoClass(ExpandDirectionProperty, [&] (System::Reflection::Variant d)
    //        {
    //            return d == ExpandDirection::Down, ":down";
    //        });
    //        PseudoClass(ExpandDirectionProperty, [&] (System::Reflection::Variant d)
    //        {
    //            return d == ExpandDirection::Up, ":up";
    //        });
    //        PseudoClass(ExpandDirectionProperty, [&] (System::Reflection::Variant d)
    //        {
    //            return d == ExpandDirection::Left, ":left";
    //        });
    //        PseudoClass(ExpandDirectionProperty, [&] (System::Reflection::Variant d)
    //        {
    //            return d == ExpandDirection::Right, ":right";
    //        });
    //        
    //        PseudoClass(IsExpandedProperty, ":expanded");

    IsExpandedProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        static_cast<Expander*>(e.Sender)->OnIsExpandedChanged(e);
    });
}

//IPageTransition *Expander::GetContentTransition() const
//{
//    return _contentTransition;
//}
//
//void Expander::SetContentTransition(IPageTransition *value)
//{
//    SetAndRaise(ContentTransitionProperty, _contentTransition, value);
//}

XUI::UI::ExpandDirection Expander::GetExpandDirection() const
{
    return _expandDirection;
}

void Expander::SetExpandDirection(const XUI::UI::ExpandDirection &value)
{
    SetAndRaise(ExpandDirectionProperty, _expandDirection, value);
}

bool Expander::GetIsExpanded() const
{
    return _isExpanded;
}

void Expander::SetIsExpanded(const bool &value)
{
    SetAndRaise(IsExpandedProperty, _isExpanded, value);
}

void Expander::OnIsExpandedChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    auto visualContent = Presenter;

    //if (Content != nullptr && ContentTransition != nullptr && visualContent != nullptr)
    //{
    //    bool forward = ExpandDirection == ExpandDirection::Left || ExpandDirection == ExpandDirection::Up;
    //    if (IsExpanded)
    //    {
    //        ContentTransition->Start(nullptr, visualContent, forward);
    //    }
    //    else
    //    {
    //        ContentTransition->Start(visualContent, nullptr, !forward);
    //    }
    //}
}
