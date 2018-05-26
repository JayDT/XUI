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

#ifndef __XUI_ITEMSPRESENTERBASE_H__
#define __XUI_ITEMSPRESENTERBASE_H__

#include "UI/Presenters/ContentPresenter.h"
#include "Core/Media/MediaTypes.h"
#include "XamlCPP/Interfaces/ITemplate.h"
#include "UI/Generators/IItemContainerGenerator.h"

#include <standard/collection.h>

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Panel.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class Panel;

    namespace Controls
    {
        class Control;
        class Visual;
    }
}

namespace XUI::UI::DataTemplates
{
    class ControlTemplate;
    class DataTemplate;
}

namespace XUI::UI::Presenters
{
    /// <summary>
    /// Describes the item virtualization method to use for a list.
    /// </summary>
    enum class ItemVirtualizationMode : uint8
    {
        /// <summary>
        /// Do not virtualize items.
        /// </summary>
        None,

        /// <summary>
        /// Virtualize items without smooth scrolling.
        /// </summary>
        Simple,
    };

    /// <summary>
    /// Describes how focus should be moved by directional or tab keys.
    /// </summary>
    enum class NavigationDirection : uint8
    {
        None,

        /// <summary>
        /// Move the focus to the next control in the tab order.
        /// </summary>
        Next,

        /// <summary>
        /// Move the focus to the previous control in the tab order.
        /// </summary>
        Previous,

        /// <summary>
        /// Move the focus to the first control in the tab order.
        /// </summary>
        First,

        /// <summary>
        /// Move the focus to the last control in the tab order.
        /// </summary>
        Last,

        /// <summary>
        /// Move the focus to the left.
        /// </summary>
        Left,

        /// <summary>
        /// Move the focus to the right.
        /// </summary>
        Right,

        /// <summary>
        /// Move the focus up.
        /// </summary>
        Up,

        /// <summary>
        /// Move the focus down.
        /// </summary>
        Down,

        /// <summary>
        /// Move the focus up a page.
        /// </summary>
        PageUp,

        /// <summary>
        /// Move the focus down a page.
        /// </summary>
        PageDown,
    };

    /// <summary>
    /// Base class for controls that present items inside an <see cref="ItemsControl"/>.
    /// </summary>
    class ItemsPresenterBase : public Controls::Control
    {
    public:
        /// <summary>
        /// Defines the <see cref="Items"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ItemsProperty;

        /// <summary>
        /// Defines the <see cref="ItemsPanel"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ItemsPanelProperty;

        /// <summary>
        /// Defines the <see cref="ItemTemplate"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ItemTemplateProperty;

        /// <summary>
        /// Defines the <see cref="MemberSelector"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty MemberSelectorProperty;

    private:
        System::Collection::Generic::IContainer *_items = nullptr;
        bool _createdPanel = false;
        std::shared_ptr<UI::Panel> m_Panel;
        mutable std::shared_ptr<Generators::IItemContainerGenerator> _generator;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="ItemsPresenter"/> class.
        /// </summary>
        ItemsPresenterBase();
        virtual ~ItemsPresenterBase()
        {
        }

        static void StaticClassInitializer();

        /// <summary>
        /// Gets or sets the items to be displayed.
        /// </summary>
        PROPERTY_(System::Collection::Generic::IContainer*, Items);
        System::Collection::Generic::IContainer *GetItems() const;
        void SetItems(System::Collection::Generic::IContainer *value);

        /// <summary>
        /// Gets the item container generator.
        /// </summary>
        PROPERTY_(std::shared_ptr<Generators::IItemContainerGenerator>, ItemContainerGenerator);
        std::shared_ptr<Generators::IItemContainerGenerator> const& GetItemContainerGenerator() const;
        void SetItemContainerGenerator(std::shared_ptr<Generators::IItemContainerGenerator> const& value);

        /// <summary>
        /// Gets or sets a template which creates the <see cref="Panel"/> used to display the items.
        /// </summary>
        PROPERTY_(std::shared_ptr<XamlCPP::Interfaces::ITemplate>, ItemsPanel);
        std::shared_ptr<XamlCPP::Interfaces::ITemplate> GetItemsPanel() const;
        void SetItemsPanel(std::shared_ptr<XamlCPP::Interfaces::ITemplate> const& value);

        /// <summary>
        /// Gets or sets the data template used to display the items in the control.
        /// </summary>
        PROPERTY_(std::shared_ptr<DataTemplates::DataTemplate>, ItemTemplate);
        std::shared_ptr<DataTemplates::DataTemplate> GetItemTemplate() const;
        void SetItemTemplate(std::shared_ptr<DataTemplates::DataTemplate> const& value);

        /// <summary>
        /// Selects a member from <see cref="Items"/> to use as the list item.
        /// </summary>
        PROPERTY_(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)>, MemberSelector);
        System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> GetMemberSelector() const;
        void SetMemberSelector(System::Delegate<System::Reflection::Variant(System::Reflection::Variant const&)> const& value);

        /// <summary>
        /// Gets the panel used to display the items.
        /// </summary>
        PROPERTY_(std::shared_ptr<UI::Panel>, Panel);
        std::shared_ptr<UI::Panel> GetPanel() const;
        void SetPanel(std::shared_ptr<UI::Panel> const& value);

        /// <inheritdoc/>
        void ApplyTemplate() final override;

        /// <inheritdoc/>
        virtual void ScrollIntoView(System::Reflection::Variant const& item);

        /// <summary>
        /// Creates the <see cref="ItemContainerGenerator"/> for the control.
        /// </summary>
        /// <returns>
        /// An <see cref="IItemContainerGenerator"/> or null.
        /// </returns>
    protected:
        Meta(Disable) virtual std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() const;

        /// <inheritdoc/>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        /// <inheritdoc/>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

        /// <summary>
        /// Called when the <see cref="Panel"/> is created.
        /// </summary>
        /// <param name="panel">The panel.</param>
        virtual void PanelCreated(UI::Panel* panel);

        /// <summary>
        /// Called when the items for the presenter change, either because <see cref="Items"/>
        /// has been set, the items collection has been modified, or the panel has been created.
        /// </summary>
        /// <param name="e">A description of the change.</param>
        virtual void ItemsChanged(System::Collection::NotifyCollectionChangedEventArgs& e) = 0;

        /// <inheritdoc/>
        void OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e) override;

        /// <inheritdoc/>
        void OnDetachedFromLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e) override;

    private:
        /// <summary>
        /// Creates the <see cref="Panel"/> when <see cref="ApplyTemplate"/> is called for the first
        /// time.
        /// </summary>
        void CreatePanel();

        /// <summary>
        /// Called when the <see cref="Items"/> collection changes.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The event args.</param>
        void ItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e);

        void TemplatedParentChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);
    };
}


#endif	//#ifndef __XUI_ITEMSPRESENTERBASE_H__
