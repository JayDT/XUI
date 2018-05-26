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

#include "CarouselPresenter.h"
#include "UI/Panel.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;
using namespace XUI::UI::Presenters;

STATIC_CLASS_INITIALIZER(CarouselPresenter, XUI::UI::Presenters::CarouselPresenter::StaticClassInitializer)

Core::Dependency::RefDependencyProperty Presenters::CarouselPresenter::IsVirtualizedProperty;
Core::Dependency::RefDependencyProperty Presenters::CarouselPresenter::SelectedIndexProperty;
Core::Dependency::RefDependencyProperty Presenters::CarouselPresenter::TransitionProperty;

void XUI::UI::Presenters::CarouselPresenter::StaticClassInitializer()
{
    IsVirtualizedProperty = (Core::Dependency::BindProperty<bool>::Register<CarouselPresenter>(nameof(IsVirtualized)));
    //TransitionProperty = (Core::Dependency::BindProperty<IPageTransition>::Register<CarouselPresenter>(nameof(Transition)));
    SelectedIndexProperty = (Core::Dependency::DirectProperty<CarouselPresenter, int>::RegisterDirect(nameof(SelectedIndex),
        [](CarouselPresenter* x) { return x->SelectedIndex; },
        [](CarouselPresenter* x, int v) { x->SelectedIndex = v; })
        );

    SelectedIndexProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        CarouselPresenter* control = e.Sender->Dynamic_As<CarouselPresenter>();
        if (control)
            control->SelectedIndexChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });
}

bool CarouselPresenter::GetIsVirtualized() const
{
    return GetValue<bool>(IsVirtualizedProperty);
}

void CarouselPresenter::SetIsVirtualized(const bool &value)
{
    SetValue(IsVirtualizedProperty, value);
}

int CarouselPresenter::GetSelectedIndex() const
{
    return _selectedIndex;
}

void CarouselPresenter::SetSelectedIndex(const int &value)
{
    auto old = SelectedIndex;
    auto effective = (value >= 0 && value < (Items ? Items->size() : 0)) ? value : -1;

    if (old != effective)
    {
        _selectedIndex = effective;
        RaisePropertyChanged(SelectedIndexProperty.get(), old, effective, Core::Dependency::BindingLevel::LocalValue);
    }
}

//IPageTransition *CarouselPresenter::GetTransition() const
//{
//    return GetValue(TransitionProperty);
//}
//
//void CarouselPresenter::SetTransition(IPageTransition *value)
//{
//    SetValue(TransitionProperty, value);
//}

void CarouselPresenter::PanelCreated(UI::Panel *panel)
{
    MoveToPage(-1, SelectedIndex);
}

void CarouselPresenter::ItemsChanged(System::Collection::NotifyCollectionChangedEventArgs& e)
{
    // TODO: Handle items changing.           
    //switch (e.Action)
    //{
    //    case System::Collection::NotifyCollectionChangedAction::Remove:
    //        if (!IsVirtualized)
    //        {
    //            auto generator = ItemContainerGenerator;
    //            auto containers = generator->RemoveRange(e->OldStartingIndex, e->OldItems->Count);
    //            Panel->Children->RemoveAll(containers->Select([&](void *x)
    //            {
    //                return x->ContainerControl;
    //            }));
    //        }
    //        break;
    //
    //}
}

std::function<void()> CarouselPresenter::MoveToPage(int fromIndex, int toIndex)
{
    //if (fromIndex != toIndex)
    //{
    //    auto generator = ItemContainerGenerator;
    //    Controls::Control *from = nullptr;
    //    Controls::Control *to = nullptr;
    //
    //    if (fromIndex != -1)
    //    {
    //        from = ItemContainerGenerator->ContainerFromIndex(fromIndex);
    //    }
    //
    //    if (toIndex != -1)
    //    {
    //        to = GetOrCreateContainer(toIndex);
    //    }
    //
    //    if (Transition != nullptr && (from != nullptr || to != nullptr))
    //    {
    //        await Transition->Start(static_cast<Visual*>(from), static_cast<Visual*>(to), fromIndex < toIndex);
    //    }
    //    else if (to != nullptr)
    //    {
    //        to->IsVisible = true;
    //    }
    //
    //    if (from != nullptr)
    //    {
    //        if (IsVirtualized)
    //        {
    //            Panel->Children->Remove(from);
    //            generator->Dematerialize(fromIndex, 1);
    //        }
    //        else
    //        {
    //            from->IsVisible = false;
    //        }
    //    }
    //}
    return {};
}

std::shared_ptr<Controls::Control> CarouselPresenter::GetOrCreateContainer(int index)
{
    auto container = nullptr; // ItemContainerGenerator->ContainerFromIndex(index);

    //if (container == nullptr)
    //{
    //    auto item = Items->Cast<void*>().ElementAt(index);
    //    auto materialized = ItemContainerGenerator->Materialize(index, item, MemberSelector);
    //    Panel->Children->Add(materialized->ContainerControl);
    //    container = materialized->ContainerControl;
    //}

    return container;
}

void CarouselPresenter::SelectedIndexChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    //if (Panel != nullptr)
    //{
    //    if (_currentTransition == nullptr)
    //    {
    //        int fromIndex = static_cast<int>(e->OldValue);
    //        int toIndex = static_cast<int>(e->NewValue);
    //
    //        for (;;)
    //        {
    //            _currentTransition = MoveToPage(fromIndex, toIndex);
    //            await *_currentTransition;
    //
    //            if (_queuedTransitionIndex != -1)
    //            {
    //                fromIndex = toIndex;
    //                toIndex = _queuedTransitionIndex;
    //                _queuedTransitionIndex = -1;
    //            }
    //            else
    //            {
    //                _currentTransition = nullptr;
    //                break;
    //            }
    //        }
    //    }
    //    else
    //    {
    //        _queuedTransitionIndex = static_cast<int>(e->NewValue);
    //    }
    //}
}

