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

#ifndef __XUI_CAROUSELPRESENTER_H__
#define __XUI_CAROUSELPRESENTER_H__

#include "UI/Presenters/ItemsPresenterBase.h"
#include <functional>

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Panel.h"))
MetaAssemblyEnd

namespace XUI::UI::Presenters
{
    /// <summary>
    /// Displays pages inside an <see cref="ItemsControl"/>.
    /// </summary>
    class CarouselPresenter : public ItemsPresenterBase
    {
    public:
        /// <summary>
        /// Defines the <see cref="IsVirtualized"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty IsVirtualizedProperty;

        /// <summary>
        /// Defines the <see cref="SelectedIndex"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty SelectedIndexProperty;

        /// <summary>
        /// Defines the <see cref="Transition"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty TransitionProperty;

    private:
        int _selectedIndex = -1;
        std::function<void()> _currentTransition;
        int _queuedTransitionIndex = -1;

        /// <summary>
        /// Initializes static members of the <see cref="CarouselPresenter"/> class.
        /// </summary>
    public:
        virtual ~CarouselPresenter()
        {
        }

        static void StaticClassInitializer();

    public:
        /// <summary>
        /// Gets or sets a value indicating whether the items in the carousel are virtualized.
        /// </summary>
        /// <remarks>
        /// When the carousel is virtualized, only the active page is held in memory.
        /// </remarks>
        PROPERTY_(bool, IsVirtualized);
        bool GetIsVirtualized() const;
        void SetIsVirtualized(const bool &value);

        /// <summary>
        /// Gets or sets the index of the selected page.
        /// </summary>
        PROPERTY_(int, SelectedIndex);
        int GetSelectedIndex() const;
        void SetSelectedIndex(const int &value);

        /// <summary>
        /// Gets or sets a transition to use when switching pages.
        /// </summary>
        //IPageTransition * __declspec(property(get = GetTransition, put = SetTransition)) Transition;
        //IPageTransition *GetTransition() const;
        //void SetTransition(IPageTransition *value);

    protected:
        /// <inheritdoc/>
        void PanelCreated(UI::Panel *panel) override;

        /// <inheritdoc/>
        void ItemsChanged(System::Collection::NotifyCollectionChangedEventArgs& e) override;

        /// <summary>
        /// Moves to the selected page, animating if a <see cref="Transition"/> is set.
        /// </summary>
        /// <param name="fromIndex">The index of the old page.</param>
        /// <param name="toIndex">The index of the new page.</param>
        /// <returns>A task tracking the animation.</returns>
    private:
        std::function<void()> MoveToPage(int fromIndex, int toIndex);

        std::shared_ptr<Controls::Control> GetOrCreateContainer(int index);

        /// <summary>
        /// Called when the <see cref="SelectedIndex"/> property changes.
        /// </summary>
        /// <param name="e">The event args.</param>
        void SelectedIndexChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);
    };
}


#endif	//#ifndef __XUI_CAROUSELPRESENTER_H__
