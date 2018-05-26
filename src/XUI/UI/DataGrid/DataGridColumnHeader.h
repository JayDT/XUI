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

#ifndef __XUI_UI_DATACOLUMN_HEADER_H__
#define __XUI_UI_DATACOLUMN_HEADER_H__

#include "UI/Button.h"
#include "UI/Thumb.h"
#include "UI/Detail/GridLength.h"
#include "UI/DataGrid/DataGridDefines.h"
#include "UI/DataGrid/DataGridExtensions.h"

namespace XUI::UI
{
    class Panel;
    class DataGrid;
    class DataGridRow;
    class DataGridColumn;
    class DataGridColumnHeadersPresenter;

    class DataGridColumnHeader : public Button
    {
        friend class DataGridColumnHeadersPresenter;

    private:

#pragma region Data

        std::weak_ptr<DataGridColumn> _column;
        DataGridColumnHeaderReference _columnHeaderReference;
        DataGridColumnHeadersPresenter* _parentPresenter = nullptr;

        std::shared_ptr<Thumb> _leftGripper, _rightGripper;
        bool _suppressClickEvent = false;
        static constexpr const char* LeftHeaderGripperTemplateName = "PART_LeftHeaderGripper";
        static constexpr const char* RightHeaderGripperTemplateName = "PART_RightHeaderGripper";

#pragma endregion

    public:
        static Core::Dependency::RefDependencyProperty SeparatorBrushProperty;
        static Core::Dependency::RefDependencyProperty SeparatorVisibilityProperty;
        static Core::Dependency::RefDependencyProperty DisplayIndexProperty;
        static Core::Dependency::RefDependencyProperty CanUserSortProperty;
        static Core::Dependency::RefDependencyProperty SortDirectionProperty;
        static Core::Dependency::RefDependencyProperty IsFrozenProperty;

        static void StaticClassInitializer();

        DataGridColumnHeader();
        virtual ~DataGridColumnHeader();

        DataGridColumn* Column() const;
        Panel* ParentPanel() const;
        double ColumnActualWidth() const;
        GridLength ColumnWidth() const;
        DataGridColumnHeadersPresenter* ParentPresenter() const;

		PROPERTY_(int, DisplayIndex);
		int GetDisplayIndex() const { return GetValue<int>(DisplayIndexProperty); }
		void SetDisplayIndex(int const& value) { SetValue(DisplayIndexProperty, value); }

        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::IBrush>, SeparatorBrush)
        DEPENDENCY_PROPERTY_(UI::Visibility, SeparatorVisibility)
        DEPENDENCY_PROPERTY_(bool, CanUserSort)
        DEPENDENCY_PROPERTY_(UI::ListSortDirection, SortDirection)
        DEPENDENCY_PROPERTY_(bool, IsFrozen)

        /// <summary>
        /// Prepares a column header to be used.  Sets up the association between the column header and its column.
        /// </summary>
        void PrepareColumnHeader(System::Reflection::Variant const& item, DataGridColumn* column);

        void ClearHeader();

        /// <summary>
        ///     Notification for column header-related DependencyProperty changes from the grid or from columns.
        /// </summary>
        void NotifyPropertyChanged(Core::Dependency::DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

    protected:

        /// <summary>
        /// The override of ButtonBase.OnClick.
        /// Informs the owning datagrid to sort itself after the execution of usual button stuff
        /// </summary>
        void OnClick(Core::Observer::RoutedEventArgs& e) override;

        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void OnPointerMoved(Interfaces::PointerEventArgs& e) override;

        void OnPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        //void OnLostMouseCapture(Interfaces::PointerEventArgs& e) override
        //{
        //    base.OnLostMouseCapture(e);
        //    ParentPresenter.OnHeaderLostMouseCapture(e);
        //    e.Handled = true;
        //}

    private:
        /// <summary>
        /// Find grippers and register drag events
        ///
        /// The default style for DataGridHeader is
        /// +-------------------------------+
        /// +---------+           +---------+
        /// + Gripper +  Header   + Gripper +
        /// +         +           +         +
        /// +---------+           +---------+
        /// +-------------------------------+
        /// 
        /// The reason we have two grippers is we can't extend the right gripper to straddle the line between two 
        /// headers; the header to the right would render on top of it.
        /// We resize a column by grabbing the gripper to the right; the leftmost gripper thus adjusts the width of
        /// the column to its left.
        /// </summary>
        void HookupGripperEvents();

        /// <summary>
        /// Clear gripper event
        /// </summary>
        void UnhookGripperEvents();

        /// <summary>
        /// Returns either this header or the one before it depending on which Gripper fired the event.
        /// </summary>
        /// <param name="sender"></param>
        DataGridColumnHeader* HeaderToResize(System::Reflection::Object* gripper);

        // Save the original widths before header resize
        void OnColumnHeaderGripperDragStarted(void* sender, Interfaces::VectorEventArgs& e);

        void OnColumnHeaderResize(void* sender, Interfaces::VectorEventArgs& e);

        /// <summary>
        /// Method which recomputes the widths of columns on resize of a header
        /// </summary>
        static void RecomputeColumnWidthsOnColumnResize(DataGridColumnHeader* header, double horizontalChange);

        void OnColumnHeaderGripperDragCompleted(void* sender, Interfaces::VectorEventArgs& e);

        void OnGripperDoubleClicked(void* sender, Interfaces::PointerPressedEventArgs& e);

        /// <summary>
        ///     Notifies the Header of a property change.
        /// </summary>
        static void OnNotifyPropertyChanged(DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void OnCanUserResizeColumnsChanged();

        void OnCanUserResizeChanged();

        void SetLeftGripperVisibility();

        void SetLeftGripperVisibility(bool canPreviousColumnResize);

        void SetRightGripperVisibility();

        void SetNextHeaderLeftGripperVisibility(bool canUserResize);

        void OnColumnVisibilityChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

		/// <summary>
		///     Used by the resize code -- this is the header that the left gripper should be resizing.
		/// </summary>
		DataGridColumnHeader* PreviousVisibleHeader();

    };
}

#endif //! __XUI_UI_DATACOLUMN_HEADER_H__