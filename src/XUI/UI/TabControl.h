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

#ifndef __XUI_TABCONTROL_H__
#define __XUI_TABCONTROL_H__

#include "UI/Detail/SelectingItemsControl.h"

namespace XUI::UI
{
    enum class Dock : uint8;

    /// <summary>
    /// A tab control that displays a tab strip along with the content of the selected tab.
    /// </summary>
    class TC_XUI_API TabControl : public SelectingItemsControl
    {
    protected:
        std::weak_ptr<Controls::Control> m_Pages;
        std::weak_ptr<Controls::Control> m_TabStrip;
        std::weak_ptr<Controls::Control> m_SelectedContentHost;

        System::Reflection::Variant m_SelectedContent;

    public:
        /// <summary>
        /// Defines the <see cref="Transition"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty TransitionProperty;

        static XUI::Core::Dependency::RefDependencyProperty SelectedContentProperty;
        static XUI::Core::Dependency::RefDependencyProperty SelectorContentProperty;

        /// <summary>
        /// Defines an <see cref="IMemberSelector"/> that selects the content of a <see cref="TabItem"/>.
        /// </summary>
        static std::function<System::Reflection::Variant(System::Reflection::Variant const&)> ContentSelector;

        /// <summary>
        /// Defines an <see cref="IMemberSelector"/> that selects the header of a <see cref="TabItem"/>.
        /// </summary>
        static std::function<System::Reflection::Variant(System::Reflection::Variant const&)> HeaderSelector;

        /// <summary>
        /// Defines the <see cref="TabStripPlacement"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty TabStripPlacementProperty;

        /// <summary>
        /// Initializes static members of the <see cref="TabControl"/> class.
        /// </summary>
        static void StaticClassInitializer();
        TabControl();
        virtual ~TabControl();

        /// <summary>
        /// Gets the pages portion of the <see cref="TabControl"/>'s template.
        /// </summary>
        PROPERTY_(Controls::Control * , Pages);
        Controls::Control *GetPages();
        void SetPages(Controls::Control *value);

        /// <summary>
        /// Gets the tab strip portion of the <see cref="TabControl"/>'s template.
        /// </summary>
        PROPERTY_(Controls::Control * , TabStrip);
        Controls::Control *GetTabStrip();
        void SetTabStrip(Controls::Control *value);

        PROPERTY_(System::Reflection::Variant, SelectedContent);
        System::Reflection::Variant const& GetSelectedContent() { return m_SelectedContent; }
        void SetSelectedContent(System::Reflection::Variant const& value)
        {
            SetAndRaise(SelectedContentProperty, m_SelectedContent, value);
        }

        PROPERTY_(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>, SelectorContent);
        System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> GetSelectorContent();
        void SetSelectorContent(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const& value);

        //PROPERTY_GET_ONLY(std::shared_ptr<Controls::Control>, SelectedContent);
        //std::shared_ptr<Controls::Control> GetSelectedContent() { return m_SelectedContent.lock(); }

        /// <summary>
        /// Gets or sets the transition to use when switching tabs.
        /// </summary>
        //PROPERTY_(IPageTransition * , Transition);
        //IPageTransition *GetTransition();
        //void SetTransition(IPageTransition *value);

        /// <summary>
        /// Gets or sets the tabstrip placement of the tabcontrol.
        /// </summary>
        PROPERTY_(UI::Dock , TabStripPlacement);
        UI::Dock GetTabStripPlacement();
        void SetTabStripPlacement(Dock value);

        /// <inheritdoc/>
        void EndInit() override final;

        void Render(Interfaces::IDrawingContextImpl * context) override
        {
            UI::SelectingItemsControl::Render(context);
        }

    protected:

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        virtual void UpdateSelectedTab();
        virtual void LostSelection() override;

    private:
        /// <summary>
        /// Selects the content of a tab item.
        /// </summary>
        /// <param name="o">The tab item.</param>
        /// <returns>The content.</returns>
        static System::Reflection::Variant SelectContent(System::Reflection::Variant const& o);

        /// <summary>
        /// Selects the header of a tab item.
        /// </summary>
        /// <param name="o">The tab item.</param>
        /// <returns>The content.</returns>
        static System::Reflection::Variant SelectHeader(System::Reflection::Variant const& o);

    };
}


#endif	//#ifndef __XUI_TABCONTROL_H__
