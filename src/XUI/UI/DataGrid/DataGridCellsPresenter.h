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

#ifndef __XUI_UI_DATACOLUMN_CELLS_PRESENTER_H__
#define __XUI_UI_DATACOLUMN_CELLS_PRESENTER_H__

#include "UI/Controls/ItemsControl.h"
#include "UI/DataGrid/DataGridDefines.h"
#include "UI/Panel.h"

namespace XUI::UI
{
    class DataGrid;
    class DataGridColumnCollection;

    class DataGridCellsPresenter : public Controls::ItemsControl
    {
    private:

#pragma region Data

        System::Reflection::Variant _item;
        //ContainerTracking<DataGridCell> _cellTrackingRoot;    // Root of a linked list of active cell containers
        std::shared_ptr<UI::Panel> _internalItemsHost;
        std::weak_ptr<DataGridRow> _dataGridRowOwner;

#pragma endregion

    public:

        static Core::Dependency::RefDependencyProperty ItemProperty;


        static void StaticClassInitializer();

        DataGridCellsPresenter();
        virtual ~DataGridCellsPresenter();

        PROPERTY_(System::Reflection::Variant, Item);
        System::Reflection::Variant GetItem() const { return _item; }
        void SetItem(System::Reflection::Variant const& value) { SetAndRaise(ItemProperty, _item, value); }

        DataGrid* DataGridOwner() const;

        /// <summary>
        ///     The DataGridRow that owns this control.
        /// </summary>
        DataGridRow* DataGridRowOwner() const;

        DataGridColumnCollection* GetColumns();

        /// <summary>
        ///     Update all properties that get a value from the DataGrid
        /// </summary>
        /// <remarks>
        ///     See comment on DataGridRow.SyncProperties
        /// </remarks>
        void SyncProperties(bool forcePrepareCells);

        /// <summary>
        ///     Method which returns the result of IsItemItsOwnContainerOverride to be used internally
        /// </summary>
        bool IsItemItsOwnContainerInternal(System::Reflection::Variant const& item)
        {
            return IsItemItsOwnContainerOverride(item);
        }

#pragma region Notification Propagation

        /// <summary>
        /// Notification of Height & MinHeight changes.
        /// </summary>
        static void OnNotifyHeightPropertyChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        ///     General notification for DependencyProperty changes from the grid or from columns.
        /// </summary>
        void NotifyPropertyChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e, NotificationTarget target);

        /// <summary>
        ///     General notification for DependencyProperty changes from the grid or from columns.
        /// </summary>
        void NotifyPropertyChanged(DependencyObject* d, std::string const& propertyName, Core::Dependency::DependencyPropertyChangedEventArgs& e, NotificationTarget target);

#pragma endregion

        /// <summary>
        ///     Method which tries to scroll a cell for given index into the scroll view
        /// </summary>
        /// <param name="index"></param>
        void ScrollCellIntoView(int index);

        void OnMaterializedHeader(void* sender, Interfaces::ItemContainerEventArgs& e);
        void OnDematerializedHeader(void* sender, Interfaces::ItemContainerEventArgs& e);

    protected:

        /// <inheritdoc/>
        std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() override;

        /// <summary>
        ///     Called when the value of the Item property changes.
        /// </summary>
        /// <param name="oldItem">The old value of Item.</param>
        /// <param name="newItem">The new value of Item.</param>
        virtual void OnItemChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

#pragma region Cell Container Generation

        /// <summary>
        ///     Determines if an item is its own container.
        /// </summary>
        /// <param name="item">The item to test.</param>
        /// <returns>true if the item is a DataGridCell, false otherwise.</returns>
        bool IsItemItsOwnContainerOverride(System::Reflection::Variant const& item);

        /// <summary>
        ///     Instantiates an instance of a container.
        /// </summary>
        /// <returns>A new DataGridCell.</returns>
        std::shared_ptr<Core::Dependency::DependencyObject> GetContainerForItemOverride();

        /// <summary>
        ///     Prepares a new container for a given item.
        /// </summary>
        /// <param name="element">The new container.</param>
        /// <param name="item">The item that the container represents.</param>
        void PrepareContainerForItemOverride(DependencyObject* element, System::Reflection::Variant const& item);

        /// <summary>
        ///     Clears a container of references.
        /// </summary>
        /// <param name="element">The container being cleared.</param>
        /// <param name="item">The data item that the container represented.</param>
        void ClearContainerForItemOverride(DependencyObject* element, System::Reflection::Variant const& item);

	public:
        /// <summary>
        ///     Notification from the DataGrid that the columns collection has changed.
        /// </summary>
        /// <param name="columns">The columns collection.</param>
        /// <param name="e">The event arguments from the collection's change event.</param>
        virtual void OnColumnsChanged(System::Collection::Vector<std::shared_ptr<DataGridColumn>> const& columns, System::Collection::NotifyCollectionChangedEventArgs& e);

#pragma endregion

	protected:

#pragma region GridLines

        // Different parts of the DataGrid draw different pieces of the GridLines.
        // Rows draw a single horizontal line on the bottom.  The DataGridDetailsPresenter is the element that handles it.

        /// <summary>
        ///     Measure.  This is overridden so that the row can extend its size to account for a grid line on the bottom.
        /// </summary>
        /// <param name="availableSize"></param>
        /// <returns></returns>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        /// <summary>
        ///     Arrange.  This is overriden so that the row can position its content to account for a grid line on the bottom.
        /// </summary>
        /// <param name="finalSize">Arrange size</param>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

        /// <summary>
        ///     OnRender.  Overriden to draw a horizontal line underneath the content.
        /// </summary>
        /// <param name="drawingContext"></param>
        void Render(Interfaces::IDrawingContextImpl* drawingContext) override;

#pragma endregion

    private:
        bool InvalidateCellsPanelOnColumnChange();

        /// <summary>
        ///     Property changed callback for VirtualizingStackPanel.IsVirtualizing property
        /// </summary>
        static void OnIsVirtualizingPropertyChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        ///     Helper method which invalidate the underlying itemshost's measure and arrange
        /// </summary>
        void InvalidateDataGridCellsPanelMeasureAndArrange();

        /// <summary>
        ///     Helper method which invalidate the underlying itemshost's measure and arrange
        /// </summary>
        /// <param name="withColumnVirtualization">
        ///     True to invalidate only when virtualization is on.
        ///     False to invalidate only when virtualization is off.
        /// </param>
        void InvalidateDataGridCellsPanelMeasureAndArrange(bool withColumnVirtualization);
    };
}

#endif //!__XUI_UI_DATACOLUMN_CELLS_PRESENTER_H__