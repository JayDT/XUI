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

#ifndef __XUI_UI_DATAGRID_ROW_H__
#define __XUI_UI_DATAGRID_ROW_H__

#include "UI/Controls/Control.h"
#include "UI/DataGrid/DataGridDefines.h"
#include "UI/DataGrid/DataGridExtensions.h"

namespace XUI::UI
{
    enum class Visibility : byte;
    class DataGrid;
    class DataGridCell;
    class DataGridCellsPresenter;
    class DataGridDetailsPresenter;
    class DataGridRowHeader;

    class DataGridRow : public Controls::Control
    {
        friend class DataGrid;
    private:

        static constexpr const char* PART_CELLPRESENTER = "PART_CellPresenter";
        static constexpr const char* PART_DETAILPRESENTER = "PART_DetailsPresenter";
        static constexpr const char* PART_HEADER = "PART_Header";

#pragma region Data

        // Tracks whether row details have been displayed.
        //      true - row details template has been loaded and has rendered at least once
        //      false - row details template has either is unset, or has never been asked to render
        bool _detailsLoaded = false;
		bool _isEditing = false;
		bool _isSelected = false;

        std::weak_ptr<DataGrid> _owner;
        std::shared_ptr<DataGridCellsPresenter> _cellsPresenter;
        std::shared_ptr<DataGridDetailsPresenter> _detailsPresenter;
        std::shared_ptr<DataGridRowHeader> _rowHeader;
        System::Collection::RefManager<DataGridCell, DataGridRow> _cellTrackingRoot;
        DataGridRowReference _rowReference;
        double _cellsPresenterResizeHeight = 0.0;

#pragma endregion
    public:

        static Core::Dependency::RefDependencyProperty ItemProperty;
        static Core::Dependency::RefDependencyProperty HeaderProperty;
        static Core::Dependency::RefDependencyProperty HeaderStyleProperty;
        static Core::Dependency::RefDependencyProperty HeaderTemplateProperty;
        static Core::Dependency::RefDependencyProperty DetailTemplateProperty;
        static Core::Dependency::RefDependencyProperty DetailsVisibilityProperty;
        static Core::Dependency::RefDependencyProperty AlternationIndexProperty;
        static Core::Dependency::RefDependencyProperty IsSelectedProperty;
        static Core::Dependency::RefDependencyProperty IsEditingProperty;

        static Core::Observer::RefRoutedEvent SelectedEvent;
        static Core::Observer::RefRoutedEvent UnselectedEvent;

        /// <summary>
        /// Defines the <see cref="ItemsPanel"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ItemsPanelProperty;

        static void StaticClassInitializer();

        DataGridRow();
        virtual ~DataGridRow();

        System::Collection::RefManager<DataGridCell, DataGridRow>& GetCells() { return _cellTrackingRoot; }

        DataGridCell* GetCell(int columnIndex);

        PROPERTY_(DataGridCellsPresenter*, CellsPresenter);
        DataGridCellsPresenter* GetCellsPresenter() const { return _cellsPresenter.get(); }
        void SetCellsPresenter(DataGridCellsPresenter* value) { /*_cellsPresenter = value;*/ }

        PROPERTY_(DataGridDetailsPresenter*, DetailsPresenter);
        DataGridDetailsPresenter* GetDetailsPresenter() const { return _detailsPresenter.get(); }
        void SetDetailsPresenter(DataGridDetailsPresenter* value) { /*_detailsPresenter = value;*/ }

        PROPERTY_(DataGridRowHeader*, RowHeader);
        DataGridRowHeader* GetRowHeader() const { return _rowHeader.get(); }
        void SetRowHeader(DataGridRowHeader* value);

        PROPERTY_(System::Reflection::RefObject, Header);
        System::Reflection::RefObject GetHeader() const { return GetValue<System::Reflection::RefObject>(HeaderProperty.get()); }
        void SetHeader(System::Reflection::RefObject const& value) { SetValue(HeaderProperty, value); }

        PROPERTY_(std::shared_ptr<XUI::Styling::Style>, HeaderStyle);
        std::shared_ptr<XUI::Styling::Style> GetHeaderStyle() const { return GetValue<std::shared_ptr<XUI::Styling::Style>>(HeaderStyleProperty); }
        void SetHeaderStyle(std::shared_ptr<XUI::Styling::Style> const& value) { SetValue(HeaderStyleProperty, value); }

        PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate>, HeaderTemplate);
        std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate> GetHeaderTemplate() const { return GetValue<std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate>>(HeaderTemplateProperty); }
        void SetHeaderTemplate(std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate> const& value) { SetValue(HeaderTemplateProperty, value); }

        PROPERTY_(std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate>, DetailTemplate);
        std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate> GetDetailTemplate() const { return GetValue<std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate>>(DetailTemplateProperty); }
        void SetDetailTemplate(std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate> const& value) { SetValue(DetailTemplateProperty, value); }

        PROPERTY_(std::shared_ptr<XamlCPP::Interfaces::ITemplate>, ItemsPanel);
        std::shared_ptr<XamlCPP::Interfaces::ITemplate> GetItemsPanel() const { return GetValue<std::shared_ptr<XamlCPP::Interfaces::ITemplate>>(ItemsPanelProperty); }
        void SetItemsPanel(std::shared_ptr<XamlCPP::Interfaces::ITemplate> const& value) { SetValue(ItemsPanelProperty, value); }

        PROPERTY_(System::Reflection::Variant, Item);
        System::Reflection::Variant GetItem() const { return GetValue(ItemProperty.get()); }
        void SetItem(System::Reflection::Variant const& value) { SetValue(ItemProperty, value); }

        PROPERTY_(UI::Visibility, DetailsVisibility);
        UI::Visibility GetDetailsVisibility() const { return GetValue<UI::Visibility>(DetailsVisibilityProperty); }
        void SetDetailsVisibility(UI::Visibility const& value) { SetValue(DetailsVisibilityProperty, value); }

		PROPERTY_(bool, IsSelected);
		bool GetIsSelected() const { return _isSelected; }
		void SetIsSelected(bool value) { SetAndRaise(IsSelectedProperty, _isSelected, value); }

		PROPERTY_(bool, IsEditing);
		bool GetIsEditing() const { return _isEditing; }
		void SetIsEditing(bool value) { SetAndRaise(IsEditingProperty, _isEditing, value); }

        DEPENDENCY_PROPERTY_(int, AlternationIndex)

        DataGrid* DataGridOwner() const { return _owner.lock().get(); }

        /// <summary>
        /// Caled when the <see cref="Item"/> property changes.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void ItemChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);
        virtual void ItemsPanelChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        ///     Prepares a row container for active use.
        /// </summary>
        /// <remarks>
        ///     Instantiates or updates a MultipleCopiesCollection ItemsSource in
        ///     order that cells be generated.
        /// </remarks>
        /// <param name="item">The data item that the row represents.</param>
        /// <param name="owningDataGrid">The DataGrid owner.</param>
        void PrepareRow(System::Reflection::Variant const& item, DataGrid* owningDataGrid);

        /// <summary>
        ///     Clears the row of references.
        /// </summary>
        void ClearRow(DataGrid* owningDataGrid);

        void OnRowResizeStarted();

        void OnRowResize(double changeAmount);

        void OnRowResizeCompleted(bool canceled);

        void OnRowResizeReset();

        /// <summary>
        ///     General notification for DependencyProperty changes from the grid or from columns.
        /// </summary>
        void NotifyPropertyChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e, NotificationTarget target);

        /// <summary>
        ///     General notification for DependencyProperty changes from the grid or from columns.
        /// </summary>
        void NotifyPropertyChanged(DependencyObject* d, std::string const& propertyName, Core::Dependency::DependencyPropertyChangedEventArgs& e, NotificationTarget target);

        System::Reflection::Variant DelayedValidateWithoutUpdate(System::Reflection::Variant const& arg);

        /// <summary>
        ///     Fired when the Row is attached to the DataGrid.  The scenario here is if the user is scrolling and
        ///     the Row is a recycled container that was just added back to the visual tree.  Properties that rely on a value from
        ///     the Grid should be reevaluated because they may be stale.  
        /// </summary>
        /// <remarks>
        ///     Properties can obviously be stale if the DataGrid's value changes while the row is disconnected.  They can also
        ///     be stale for unobvious reasons.
        /// 
        ///     For example, the Style property is invalidated when we detect a new Visual parent.  This happens for 
        ///     elements in the row (such as the RowHeader) before Prepare is called on the Row.  The coercion callback
        ///     will thus be unable to find the DataGrid and will return the wrong value.  
        /// 
        ///     There is a potential for perf work here.  If we know a DP isn't invalidated when the visual tree is reconnected
        ///     and we know that the Grid hasn't modified that property then its value is likely fine.  We could also cache whether
        ///     or not the Grid's property is the one that's winning.  If not, no need to redo the coercion.  This notification 
        ///     is pretty fast already and thus not worth the work for now.
        /// </remarks>
        void SyncProperties(bool forcePrepareCells);

        static void OnIsSelectedChanged(System::Reflection::Object* sender, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        ///     Method which tries to scroll a cell for given index into the scroll view
        /// </summary>
        /// <param name="index"></param>
        void ScrollCellIntoView(int index);

        Core::Media::Size ArrangeOverride(Core::Media::Size const& availableSize) override;

        /// <summary>
        ///     Returns the index of this row within the DataGrid's list of item containers.
        /// </summary>
        /// <remarks>
        ///     This method performs a linear search.
        /// </remarks>
        /// <returns>The index, if found, -1 otherwise.</returns>
        int GetIndex();

        /// <summary>
        ///     Searchs up the visual parent chain from the given element until
        ///     a DataGridRow element is found.
        /// </summary>
        /// <param name="element">The descendent of a DataGridRow.</param>
        /// <returns>
        ///     The first ancestor DataGridRow of the element parameter.
        ///     Returns null of none is found.
        /// </returns>
        static DataGridRow* GetRowContainingElement(UI::Controls::Visual* element);

        /// <summary>
        /// Returns true if the DetailsPresenter is supposed to draw gridlines for the row.  Only true
        /// if the DetailsPresenter hooked itself up properly to the Row.
        /// </summary>
        bool DetailsPresenterDrawsGridLines() const;

        /// <summary>
        ///     Acceses the CellsPresenter and attempts to get the cell at the given index.
        ///     This is not necessarily the display order.
        /// </summary>
        DataGridCell* TryGetCell(int index);

		/// <summary>
		///     Determines if a row can be selected, based on the DataGrid's SelectionUnit property.
		/// </summary>
		bool IsSelectable() const;

    protected:
        /// <summary>
        ///     Notification from the DataGrid that the columns collection has changed.
        /// </summary>
        /// <param name="columns">The columns collection.</param>
        /// <param name="e">The event arguments from the collection's change event.</param>
        virtual void OnColumnsChanged(System::Collection::Vector<std::shared_ptr<DataGridColumn>> const& columns, System::Collection::NotifyCollectionChangedEventArgs& e);

        virtual void OnItemChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        ///     Called when the value of the Header property changes.
        /// </summary>
        /// <param name="oldHeader">The old value of Header</param>
        /// <param name="newHeader">The new value of Header</param>
        virtual void OnHeaderChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
        {
        }

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

    private:
        void PersistAttachedItemValue(DependencyObject* objectWithProperty, Core::Dependency::DependencyProperty* property);

        void RestoreAttachedItemValue(DependencyObject* objectWithProperty, Core::Dependency::DependencyProperty* property);

        static void OnNotifyRowPropertyChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        static void OnNotifyRowAndRowHeaderPropertyChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        static void OnNotifyDetailsTemplatePropertyChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        static void OnNotifyDetailsVisibilityChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        ///     Notifies the DataGrid that the visibility is changed.  This is intended to be Invoked at lower than Layout priority to give the template time to expand.
        /// </summary>
        static System::Reflection::Object* DelayedRowDetailsVisibilityChanged(System::Reflection::Object* arg);

        void RaiseSelectionChangedEvent(bool isSelected)
        {
            Core::Observer::RoutedEventArgs e;
            e.Sender = this;
            e.Source = this;

            if (isSelected)
                e.RoutedEvent = SelectedEvent.get();
            else
                e.RoutedEvent = UnselectedEvent.get();

            RaiseEvent(e);
        }

    };
}

#endif //!__XUI_UI_DATAGRID_ROW_H__