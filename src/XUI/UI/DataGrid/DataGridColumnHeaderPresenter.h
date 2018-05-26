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

#ifndef __XUI_UI_DATACOLUMN_HEADER_PRESENTER_H__
#define __XUI_UI_DATACOLUMN_HEADER_PRESENTER_H__

#include "UI/Controls/ItemsControl.h"
#include "UI/Thumb.h"
#include "UI/DataGrid/DataGridDefines.h"
#include "UI/DataGrid/DataGridExtensions.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/DataGrid/DataGridColumnHeader.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DataGrid;
    class DataGridRow;
    class DataGridColumnHeader;

    class DataGridColumnHeadersPresenter : public Controls::ItemsControl
    {
    private:

        static constexpr char* ElementFillerColumnHeader = "PART_FillerColumnHeader";

        DataGrid * _dataGridOwner = nullptr;
        System::Collection::RefManager<DataGridColumnHeader, DataGridColumnHeadersPresenter> _columnHeaderTrackingRoot;         // Root of a linked list of active header containers

        bool _prepareColumnHeaderDragging = false;
        bool _isColumnHeaderDragging = false;
        std::shared_ptr<DataGridColumnHeader> _draggingSrcColumnHeader;
        Core::Media::Point _columnHeaderDragStartPosition;
        Core::Media::Point _columnHeaderDragStartRelativePosition;
        Core::Media::Point _columnHeaderDragCurrentPosition;
        std::shared_ptr<Controls::Control> _columnHeaderDropLocationIndicator;
        std::shared_ptr<Controls::Control> _columnHeaderDragIndicator;

    public:

        static void StaticClassInitializer();

        DataGridColumnHeadersPresenter();
        virtual ~DataGridColumnHeadersPresenter();

        System::Collection::RefManager<DataGridColumnHeader, DataGridColumnHeadersPresenter>& GetHeaders() { return _columnHeaderTrackingRoot; }

        /// <summary>
        ///     Notification for column header-related DependencyProperty changes from the grid or from columns.
        /// </summary>
        void NotifyPropertyChanged(DependencyObject* d, std::string const& propertyName, Core::Dependency::DependencyPropertyChangedEventArgs& e, NotificationTarget target);

        void OnDataGridColumnsChanged(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e);

        DataGrid* ParentDataGrid();

        static System::Reflection::Variant SelectHeader(System::Reflection::Variant const& o);

        void OnMaterializedHeader(void* sender, Interfaces::ItemContainerEventArgs& e);
        void OnDematerializedHeader(void* sender, Interfaces::ItemContainerEventArgs& e);

    protected:

        /// <inheritdoc/>
        std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() override;

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        /// <summary>
        ///     Measure
        /// </summary>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        /// <summary>
        ///     Arrange
        /// </summary>
        /// <param name="finalSize">Arrange size</param>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

        DataGridColumn* ColumnFromContainer(DataGridColumnHeader* container);

    public:

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

        /// <summary>
        ///     Workaround for not being able to access the panel instance of 
        ///     itemscontrol directly
        /// </summary>
        UI::Panel* GetInternalItemsHost() const;
        void SetInternalItemsHost(UI::Panel* value);

        /// <summary>
        ///     Override of GetVisualChild which accomodates the indicators as visual children
        /// </summary>
        Controls::Visual* GetVisualChild(int index);

        /// <summary>
        ///     Gets called on mouse left button down of child header, and ensures preparation for column header drag
        /// </summary>
        void OnHeaderMouseLeftButtonDown(Interfaces::PointerPressedEventArgs& e);

        /// <summary>
        ///     Gets called on mouse move of child header, and ensures column header drag
        /// </summary>
        void OnHeaderMouseMove(Interfaces::PointerEventArgs& e);

        /// <summary>
        ///     Gets called on mouse left button up of child header, and ensures reordering of columns on successful completion of drag
        /// </summary>
        void OnHeaderMouseLeftButtonUp(Interfaces::PointerReleasedEventArgs& e);

        /// <summary>
        ///     Gets called on mouse lost capture of child header and ensures that when capture gets lost
        ///     the drag ends in appropriate state. In this case it restore the drag state to
        ///     the start of the operation by finishing the drag with cancel flag
        /// </summary>
        void OnHeaderLostMouseCapture(Interfaces::PointerEventArgs& e);

        /// <summary>
        ///     Helper method which clears the header drag state
        /// </summary>
        void ClearColumnHeaderDragInfo();

        /// <summary>
        ///     Method which prepares the state for the start of column header drag
        /// </summary>
        void PrepareColumnHeaderDrag(DataGridColumnHeader* header, Core::Media::Point const& pos, Core::Media::Point const& relativePos);

        /// <summary>
        ///     Method which checks if mouse move is sufficient to start the drag
        /// </summary>
        static bool CheckStartColumnHeaderDrag(Core::Media::Point& currentPos, Core::Media::Point& originalPos);

        /// <summary>
        ///     Method which checks during and after the drag if the position is valid for the drop
        /// </summary>
        bool IsMousePositionValidForColumnDrag(double dragFactor);

        /// <summary>
        ///     Method which checks during and after the drag if the position is valid for the drop and returns the drop display index
        /// </summary>
        bool IsMousePositionValidForColumnDrag(double dragFactor, int& nearestDisplayIndex);

        /// <summary>
        ///     Method which start the column header drag. Includes raising events and creating default ghosts
        /// </summary>
        void StartColumnHeaderDrag();

        /// <summary>
        ///     Method which returns a default control for column header drag indicator
        /// </summary>
        std::shared_ptr<Controls::Control> CreateColumnHeaderDragIndicator();

        /// <summary>
        ///     Method which set the default values on drag indicator
        /// </summary>
        void SetDefaultsOnDragIndicator();

        /// <summary>
        ///     Method which returns the default control for the column header drop indicator
        /// </summary>
        std::shared_ptr<Controls::Control> CreateColumnHeaderDropIndicator();

        /// <summary>
        ///     Method which sets the default values on drop indicator
        /// </summary>
        void SetDefaultsOnDropIndicator();

        /// <summary>
        ///     Method which completes the column header drag. Includes raising of events and changing column display index if needed.
        /// </summary>
        void FinishColumnHeaderDrag(bool isCancel);

        /// <summary>
        ///     Helper method to determine the display index based on the given position
        /// </summary>
        int FindDisplayIndexByPosition(Core::Media::Point const& startPos, bool findNearestColumn);

        /// <summary>
        ///     Helper method to determine the column header based on the given position
        /// </summary>
        DataGridColumnHeader* FindColumnHeaderByPosition(Core::Media::Point const& startPos);

        /// <summary>
        ///     Helper method to determine the position of drop indicator based on the given mouse position
        /// </summary>
        Core::Media::Point FindColumnHeaderPositionByCurrentPosition(Core::Media::Point const& startPos, bool findNearestColumn);

        /// <summary>
        ///     Helper method which estimates the column width
        /// </summary>
        static double GetColumnEstimatedWidth(DataGridColumn* column, double averageColumnWidth);

        /// <summary>
        ///     Helper method to find display index, header and header start position based on given mouse position
        /// </summary>
        Meta(Disable) void FindDisplayIndexAndHeaderPosition(Core::Media::Point const& startPos, bool findNearestColumn, int& displayIndex, Core::Media::Point& headerPos, DataGridColumnHeader*& header);

    };
}

#endif //! __XUI_UI_DATACOLUMN_HEADER_PRESENTER_H__