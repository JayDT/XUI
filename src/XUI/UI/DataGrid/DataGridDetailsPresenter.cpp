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

#include "DataGridDetailsPresenter.h"
#include "DataGrid/DataGridControl.h"
#include "DataGrid/DataGridRow.h"
#include "Core/GlobalInitializerScheduler.h"

STATIC_CLASS_INITIALIZER(DataGridDetailsPresenter, XUI::UI::DataGridDetailsPresenter::StaticClassInitializer)

using namespace XUI;
using namespace XUI::UI;

void XUI::UI::DataGridDetailsPresenter::StaticClassInitializer()
{
}

XUI::UI::DataGridDetailsPresenter::DataGridDetailsPresenter()
{
}

XUI::UI::DataGridDetailsPresenter::~DataGridDetailsPresenter()
{
}

DataGrid * XUI::UI::DataGridDetailsPresenter::DataGridOwner() const
{
    return nullptr;
}

DataGridRow * XUI::UI::DataGridDetailsPresenter::RowOwner() const
{
    return nullptr;
}

void XUI::UI::DataGridDetailsPresenter::NotifyPropertyChanged(Core::Dependency::DependencyObject * d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    if (e.Property == DataGrid::RowDetailsTemplateProperty.get() /*|| e.Property == DataGridRow::DetailsTemplateProperty.get()*/ || e.Property == ContentTemplateProperty.get())
    {
        CoerceValue(ContentTemplateProperty.get());
    }
    //else if (e.Property == DataGrid::RowDetailsTemplateSelectorProperty.get() || e.Property == DataGridRow::DetailsTemplateSelectorProperty.get() || e.Property == ContentTemplateSelectorProperty.get())
    //{
    //    CoerceValue(ContentTemplateSelectorProperty.get());
    //}
}


/// <summary>
///     Update all properties that get a value from the DataGrid
/// </summary>
/// <remarks>
///     See comment on DataGridRow.OnDataGridChanged
/// </remarks>

void XUI::UI::DataGridDetailsPresenter::SyncProperties()
{
    DataGridRow* owner = RowOwner();
    Content = owner != nullptr ? owner->Item : nullptr;
    CoerceValue(ContentTemplateProperty.get());
    //CoerceValue(ContentTemplateSelectorProperty.get());
}

void XUI::UI::DataGridDetailsPresenter::OnVisualParentChanged(Controls::Visual* oldParent, Controls::Visual* newParent)
{
    //base.OnVisualParentChanged(oldParent);
    //
    //// DataGridRow.DetailsPresenter is used by automation peers
    //// Give the Row a pointer to the RowHeader so that it can propagate down change notifications
    //DataGridRow owner = DataGridRowOwner;
    //if (owner != null)
    //{
    //    owner.DetailsPresenter = this;
    //
    //    // At the time that a Row is prepared we can't Sync because the DetailsPresenter isn't created yet.
    //    // Doing it here ensures that the DetailsPresenter is in the visual tree.
    //    SyncProperties();
    //}
}

/// <summary>
///     Helper method to determine if the given object
///     is in the visual sub tree of the Details Presenter.
/// </summary>

bool XUI::UI::DataGridDetailsPresenter::IsInVisualSubTree(DependencyObject * visual)
{
    //while (visual != null)
    //{
    //    if (visual == this)
    //    {
    //        return true;
    //    }
    //    visual = VisualTreeHelper.GetParent(visual);
    //}
    return false;
}

void XUI::UI::DataGridDetailsPresenter::OnAnyMouseLeftButtonDownThunk(void * sender, Interfaces::PointerPressedEventArgs & e)
{
    //((DataGridDetailsPresenter)sender).OnAnyMouseLeftButtonDown(e);
}

void XUI::UI::DataGridDetailsPresenter::OnAnyMouseLeftButtonDown(Interfaces::PointerPressedEventArgs & e)
{
    //// Ignore actions if the button down arises from a different visual tree
    //if (!IsInVisualSubTree(e.OriginalSource as DependencyObject))
    //{
    //    return;
    //}
    //
    //DataGridRow rowOwner = DataGridRowOwner;
    //DataGrid dataGridOwner = rowOwner != null ? rowOwner.DataGridOwner : null;
    //if ((dataGridOwner != null) && (rowOwner != null))
    //{
    //    // HandleSelectionForRowHeaderAndDetailsInput below sets the CurrentCell
    //    // of datagrid to the cell with displayindex 0 in the row.
    //    // This implicitly queues a request to MakeVisible command
    //    // of ScrollViewer. The command handler calls MakeVisible method of 
    //    // VirtualizingStackPanel (of rows presenter) which works only
    //    // when the visual's parent layout is clean. DataGridCellsPanel layout is
    //    // not clean as per MakeVisible of VSP becuase we distribute the layout of cells for the
    //    // sake of row headers and hence it fails. VSP.MakeVisible method requeues a request to
    //    // ScrollViewer.MakeVisible command hence resulting into an infinite loop.
    //    // The workaround is to bring the concerned cell into the view by calling
    //    // ScrollIntoView so that by the time MakeVisible handler of ScrollViewer is 
    //    // executed the cell is already visible and the handler succeeds.
    //    if (dataGridOwner.CurrentCell.Item != rowOwner.Item)
    //    {
    //        dataGridOwner.ScrollIntoView(rowOwner.Item, dataGridOwner.ColumnFromDisplayIndex(0));
    //    }
    //
    //    dataGridOwner.HandleSelectionForRowHeaderAndDetailsInput(rowOwner, /* startDragging = */ Mouse.Captured == null);
    //}
}

/// <summary>
///     Measure.  This is overridden so that the row can extend its size to account for a grid line on the bottom.
/// </summary>
/// <param name="availableSize"></param>
/// <returns></returns>

Core::Media::Size XUI::UI::DataGridDetailsPresenter::MeasureOverride(Core::Media::Size const & availableSize)
{
    //// Make space for the GridLine on the bottom.
    //// Remove space from the constraint (since it implicitly includes the GridLine's thickness), 
    //// call the base implementation, and add the thickness back for the returned size.
    //var row = DataGridRowOwner;
    //Debug.Assert(row != null, "DetailsPresenter should have a DataGridRowOwner when measure is called.");
    //
    //var dataGrid = row.DataGridOwner;
    //Debug.Assert(dataGrid != null, "DetailsPresenter should have a DataGridOwner when measure is called.");
    //
    //if (row.DetailsPresenterDrawsGridLines &&
    //    DataGridHelper.IsGridLineVisible(dataGrid, /*isHorizontal = */ true))
    //{
    //    double thickness = dataGrid.HorizontalGridLineThickness;
    //    Size desiredSize = base.MeasureOverride(DataGridHelper.SubtractFromSize(availableSize, thickness, /*height = */ true));
    //    desiredSize.Height += thickness;
    //    return desiredSize;
    //}
    //else
    //{
    //    return base.MeasureOverride(availableSize);
    //}
    return {};
}

/// <summary>
///     Arrange.  This is overriden so that the row can position its content to account for a grid line on the bottom.
/// </summary>
/// <param name="finalSize">Arrange size</param>

Core::Media::Size XUI::UI::DataGridDetailsPresenter::ArrangeOverride(Core::Media::Size const & finalSize)
{
    //// We don't need to adjust the Arrange position of the content.  By default it is arranged at 0,0 and we're
    //// adding a line to the bottom.  All we have to do is compress and extend the size, just like Measure.
    //var row = DataGridRowOwner;
    //Debug.Assert(row != null, "DetailsPresenter should have a DataGridRowOwner when arrange is called.");
    //
    //var dataGrid = row.DataGridOwner;
    //Debug.Assert(dataGrid != null, "DetailsPresenter should have a DataGridOwner when arrange is called.");
    //
    //if (row.DetailsPresenterDrawsGridLines &&
    //    DataGridHelper.IsGridLineVisible(dataGrid, /*isHorizontal = */ true))
    //{
    //    double thickness = dataGrid.HorizontalGridLineThickness;
    //    Size returnSize = base.ArrangeOverride(DataGridHelper.SubtractFromSize(finalSize, thickness, /*height = */ true));
    //    returnSize.Height += thickness;
    //    return returnSize;
    //}
    //else
    //{
    //    return base.ArrangeOverride(finalSize);
    //}
    return {};
}

/// <summary>
///     OnRender.  Overriden to draw a horizontal line underneath the content.
/// </summary>
/// <param name="drawingContext"></param>

void XUI::UI::DataGridDetailsPresenter::Render(Interfaces::IDrawingContextImpl * drawingContext)
{
    //base.OnRender(drawingContext);
    //
    //var row = DataGridRowOwner;
    //Debug.Assert(row != null, "DetailsPresenter should have a DataGridRowOwner when OnRender is called.");
    //
    //var dataGrid = row.DataGridOwner;
    //Debug.Assert(dataGrid != null, "DetailsPresenter should have a DataGridOwner when OnRender is called.");
    //
    //if (row.DetailsPresenterDrawsGridLines &&
    //    DataGridHelper.IsGridLineVisible(dataGrid, /*isHorizontal = */ true))
    //{
    //    double thickness = dataGrid.HorizontalGridLineThickness;
    //    Rect rect = new Rect(new Size(RenderSize.Width, thickness));
    //    rect.Y = RenderSize.Height - thickness;
    //
    //    drawingContext.DrawRectangle(dataGrid.HorizontalGridLinesBrush, null, rect);
    //}
}
