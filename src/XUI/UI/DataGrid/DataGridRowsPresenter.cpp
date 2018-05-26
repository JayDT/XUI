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
#include "DataGridRowsPresenter.h"
#include "Core/Media/MediaTypes.h"

using namespace XUI::UI;

void DataGridRowsPresenter::InternalBringIndexIntoView(int index)
{
    //BringIndexIntoView(index);
}

//void DataGridRowsPresenter::OnIsItemsHostChanged(bool oldIsItemsHost, bool newIsItemsHost)
//{
//    VirtualizingStackPanel::OnIsItemsHostChanged(oldIsItemsHost, newIsItemsHost);
//
//    if (newIsItemsHost)
//    {
//        DataGrid *dataGrid = Owner;
//        if (dataGrid != nullptr)
//        {
//            // ItemsHost should be the "root" element which has
//            // IsItemsHost = true on it.  In the case of grouping,
//            // IsItemsHost is true on all panels which are generating
//            // content.  Thus, we care only about the panel which
//            // is generating content for the ItemsControl.
//            IItemContainerGenerator *generator = dynamic_cast<IItemContainerGenerator*>(dataGrid->ItemContainerGenerator);
//            if (generator != nullptr && generator == generator->GetItemContainerGeneratorForPanel(this))
//            {
//                dataGrid->InternalItemsHost = this;
//            }
//        }
//    }
//    else
//    {
//        // No longer the items host, clear out the property on the DataGrid
//        if ((_owner != nullptr) && (_owner->InternalItemsHost == this))
//        {
//            _owner->InternalItemsHost = nullptr;
//        }
//
//        _owner = nullptr;
//    }
//}

//void DataGridRowsPresenter::OnViewportSizeChanged(Size *oldViewportSize, Size *newViewportSize)
//{
//    DataGrid *dataGrid = Owner;
//    if (dataGrid != nullptr)
//    {
//        ScrollContentPresenter *scrollContentPresenter = dataGrid->InternalScrollContentPresenter;
//        if (scrollContentPresenter == nullptr || scrollContentPresenter->CanContentScroll)
//        {
//            dataGrid->OnViewportSizeChanged(oldViewportSize, newViewportSize);
//        }
//    }
//}

XUI::Core::Media::Size DataGridRowsPresenter::MeasureOverride(XUI::Core::Media::Size const& constraint)
{
    _availableSize = constraint;
    return VirtualizingStackPanel::MeasureOverride(constraint);
}

XUI::Core::Media::Size DataGridRowsPresenter::GetAvailableSize() const
{
    return _availableSize;
}

//void DataGridRowsPresenter::OnCleanUpVirtualizedItem(CleanUpVirtualizedItemEventArgs *e)
//{
//    VirtualizingStackPanel::OnCleanUpVirtualizedItem(e);
//
//    if (e->UIElement != nullptr && Validation::GetHasError(e->UIElement))
//    {
//        e->Cancel = true;
//    }
//}

DataGrid *DataGridRowsPresenter::GetOwner() const
{
    //if (_owner == nullptr)
    //{
    //    _owner = dynamic_cast<DataGrid*>(ItemsControl::GetItemsOwner(this));
    //}
    //Core::Dependency::BindingLevel;
    return _owner.get();
}
