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

#ifndef __XUI_UI_DATACOLUMN_DETAILS_PRESENTER_H__
#define __XUI_UI_DATACOLUMN_DETAILS_PRESENTER_H__

#include "UI/Presenters/ItemsPresenter.h"
#include "UI/DataGrid/DataGridDefines.h"
#include "UI/Panel.h"

namespace XUI::UI
{
    class DataGrid;
    class DataGridRow;

    class DataGridDetailsPresenter : public Presenters::ContentPresenter
    {
    public:

        static void StaticClassInitializer();

        DataGridDetailsPresenter();
        virtual ~DataGridDetailsPresenter();

        DataGrid* DataGridOwner() const;
        DataGridRow* RowOwner() const;

		Controls::Control* DetailsElement()
		{
			if (GetVisualChildren().empty())
				return nullptr;
			return GetVisualChildren()[0]->Static_As<Controls::Control>();
		}

        void NotifyPropertyChanged(Core::Dependency::DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        ///     Update all properties that get a value from the DataGrid
        /// </summary>
        /// <remarks>
        ///     See comment on DataGridRow.OnDataGridChanged
        /// </remarks>
        void SyncProperties();

    protected:
        void OnVisualParentChanged(Controls::Visual* oldParent, Controls::Visual* newParent) override;

    private:
        /// <summary>
        ///     Helper method to determine if the given object
        ///     is in the visual sub tree of the Details Presenter.
        /// </summary>
        bool IsInVisualSubTree(DependencyObject* visual);

        static void OnAnyMouseLeftButtonDownThunk(void* sender, Interfaces::PointerPressedEventArgs& e);

        void OnAnyMouseLeftButtonDown(Interfaces::PointerPressedEventArgs& e);

        // Different parts of the DataGrid draw different pieces of the GridLines.
        // Rows draw a single horizontal line on the bottom.  The DataGridDetailsPresenter is the element that handles it.

    protected:
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
    };
}

#endif //! __XUI_UI_DATACOLUMN_DETAILS_PRESENTER_H__