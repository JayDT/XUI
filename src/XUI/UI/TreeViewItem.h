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

#ifndef __XUI_TREEVIEWITEM_H__
#define __XUI_TREEVIEWITEM_H__

#include "UI/Detail/HeaderedItemsControl.h"

namespace XUI::UI
{
    class TreeView;

    /// <summary>
    /// An item in a <see cref="TreeView"/>.
    /// </summary>
    class TC_XUI_API TreeViewItem : public HeaderedItemsControl //, public ISelectable
    {
    public:
        /// <summary>
        /// Defines the <see cref="IsExpanded"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsExpandedProperty;

        /// <summary>
        /// Defines the <see cref="IsSelected"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsSelectedProperty;

    private:
        static std::shared_ptr<XamlCPP::Interfaces::ITemplate> DefaultPanel;

        std::weak_ptr<TreeView> _treeView;
        bool _isExpanded = false;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="TreeViewItem"/> class.
        /// </summary>
        static void StaticClassInitializer();

        /// <summary>
        /// Gets or sets a value indicating whether the item is expanded to show its children.
        /// </summary>
    public:
        PROPERTY_(bool , IsExpanded);
        bool GetIsExpanded() const;
        void SetIsExpanded(const bool &value);

        /// <summary>
        /// Gets or sets the selection state of the item.
        /// </summary>
        PROPERTY_(bool , IsSelected);
        bool GetIsSelected() const;
        void SetIsSelected(const bool &value);

    protected:
        /// <inheritdoc/>
        std::shared_ptr<Generators::IItemContainerGenerator> XUI::UI::TreeViewItem::CreateItemContainerGenerator() override;

        /// <inheritdoc/>
        void OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e) override;

        void OnDetachedFromLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e) override;

        /// <inheritdoc/>
        void OnKeyDown(Interfaces::KeyEventArgs& e) override;
    };
}


#endif	//#ifndef __XUI_TREEVIEWITEM_H__
