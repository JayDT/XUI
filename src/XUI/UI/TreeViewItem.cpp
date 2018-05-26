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

#include "TreeViewItem.h"
#include "TreeView.h"
#include "UI/StackPanel.h"
#include "UI/Detail/HeaderedContentControl.h"
#include "UI/Generators/TreeContainerIndex.h"
#include "UI/Generators/TreeItemContainerGenerator.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(TreeViewItem, TreeViewItem::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty TreeViewItem::IsExpandedProperty;
XUI::Core::Dependency::RefDependencyProperty TreeViewItem::IsSelectedProperty;

std::shared_ptr<XamlCPP::Interfaces::ITemplate> TreeViewItem::DefaultPanel = std::make_shared<DataTemplates::FuncTemplate<std::shared_ptr<XUI::UI::Panel>>>([]()
{
    auto panel = std::make_shared<UI::StackPanel>();
    //            tempVar.[KeyboardNavigation::DirectionalNavigationProperty] = KeyboardNavigationMode::Continue;
    return panel;
});

void TreeViewItem::StaticClassInitializer()
{
    IsExpandedProperty = (Core::Dependency::DirectProperty<TreeViewItem, bool>::RegisterDirect(nameof(IsExpanded),
        [](TreeViewItem* x) { return x->IsExpanded; },
        [](TreeViewItem* x, bool v) { x->IsExpanded = v; })
        );
    IsSelectedProperty = (Core::Dependency::BindProperty<bool>::Register<TreeViewItem>(nameof(IsSelected)));

    FocusableProperty->OverrideDefaultValue<TreeViewItem>(true);
    ItemsPanelProperty->OverrideDefaultValue<TreeViewItem>(DefaultPanel);
}

bool TreeViewItem::GetIsExpanded() const
{
    return _isExpanded;
}

void TreeViewItem::SetIsExpanded(const bool &value)
{
    SetAndRaise(IsExpandedProperty, _isExpanded, value);
}

bool TreeViewItem::GetIsSelected() const
{
    return GetValue<bool>(IsSelectedProperty);
}

void TreeViewItem::SetIsSelected(const bool &value)
{
    SetValue(IsSelectedProperty, value);
}

std::shared_ptr<Generators::IItemContainerGenerator> XUI::UI::TreeViewItem::CreateItemContainerGenerator()
{
    auto result = std::make_shared<Generators::TreeItemContainerGenerator<TreeViewItem>>(
        this,
        TreeViewItem::HeaderProperty.get(),
        TreeViewItem::ItemTemplateProperty.get(),
        TreeViewItem::ItemsProperty.get(),
        TreeViewItem::IsExpandedProperty.get(),
        !_treeView.expired() ? static_cast<Generators::TreeItemContainerGenerator<TreeViewItem>*>(_treeView.lock()->GetItemContainerGenerator().get())->Index : std::make_shared<Generators::TreeContainerIndex>());
    return result;
}

void TreeViewItem::OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e)
{
    HeaderedItemsControl::OnAttachedToLogicalTree(e);

    std::list<Controls::Control*> logicals;
    GetLogicalAncestors(logicals, [](Controls::Control* x)
    {
        return x && x->Is<TreeView>();
    });

    _treeView = !logicals.empty() ? (*logicals.begin())->weak_from_base_static<TreeView>() : std::shared_ptr<TreeView>(nullptr);

    if (ItemTemplate == nullptr && !_treeView.expired() && _treeView.lock()->ItemTemplate != nullptr)
    {
        ItemTemplate = _treeView.lock()->ItemTemplate;
    }
}

void TreeViewItem::OnDetachedFromLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e)
{
    HeaderedItemsControl::OnDetachedFromLogicalTree(e);
    GetItemContainerGenerator()->Clear();
}

void TreeViewItem::OnKeyDown(KeyEventArgs& e)
{
    if (!e.Handled)
    {
        switch (e.Key)
        {
            case Key::Right:
                if (Items && Items->size())
                {
                    IsExpanded = true;
                }

                e.Handled = true;
                break;

            case Key::Left:
                IsExpanded = false;
                e.Handled = true;
                break;
        }
    }

    HeaderedItemsControl::OnKeyDown(e);
}
