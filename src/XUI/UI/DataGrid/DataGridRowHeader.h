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

#ifndef __XUI_UI_DATACOLUMN_ROW_HEADER_H__
#define __XUI_UI_DATACOLUMN_ROW_HEADER_H__

#include "UI/Button.h"
#include "UI/Thumb.h"
#include "UI/DataGrid/DataGridDefines.h"

namespace XUI::UI
{
    class DataGrid;
    class DataGridRow;

    class DataGridRowHeader : public Button
    {
    private:

        std::weak_ptr<DataGridRow> _owner;
        std::shared_ptr<UI::Thumb> _topGripper, _bottomGripper;
        static constexpr const char* TopHeaderGripperTemplateName = "PART_TopHeaderGripper";
        static constexpr const char* BottomHeaderGripperTemplateName = "PART_BottomHeaderGripper";

    public:

        static Core::Dependency::RefDependencyProperty SeparatorBrushProperty;
        static Core::Dependency::RefDependencyProperty SeparatorVisibilityProperty;
        static Core::Dependency::RefDependencyProperty IsRowSelectedProperty;

        static void StaticClassInitializer();

        DataGridRowHeader();
        virtual ~DataGridRowHeader();

        DataGrid* DataGridOwner() const;
        DataGridRow* RowOwner() const;

        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::IBrush>, SeparatorBrush)
        DEPENDENCY_PROPERTY_(UI::Visibility, SeparatorVisibility)
        DEPENDENCY_PROPERTY_(bool, IsRowSelected);

        /// <summary>
        ///     Notification for column header-related DependencyProperty changes from the grid or from columns.
        /// </summary>
        void NotifyPropertyChanged(Core::Dependency::DependencyObject* d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void SyncProperties();


        /// <summary>
        /// Returns either this header or the one before it depending on which Gripper fired the event.
        /// </summary>
        /// <param name="sender"></param>
        DataGridRow* RowToResize(System::Reflection::Object* gripper)
        {
            return (gripper == _bottomGripper.get()) ? RowOwner() : PreviousRow();
        }

        // Save the original height before resize
        void OnRowHeaderGripperDragStarted(void* sender, Interfaces::VectorEventArgs& e);

        void OnRowHeaderResize(void* sender, Interfaces::VectorEventArgs& e);

        // Restores original height if canceled.
        void OnRowHeaderGripperDragCompleted(void* sender, Interfaces::VectorEventArgs& e);

        void OnGripperDoubleClicked(void* sender, Interfaces::PointerPressedEventArgs & e);

    protected:

        void SetTopGripperVisibility();

        void SetBottomGripperVisibility();

        void OnCanUserResizeRowsChanged();

        /// <summary>
        /// Measure this element and it's child elements.
        /// </summary>
        /// <remarks>
        /// DataGridRowHeader needs to update the DataGrid's RowHeaderActualWidth & use this as it's width so that they all end up the
        /// same size.
        /// </remarks>
        /// <param name="availableSize"></param>
        /// <returns></returns>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        /// <summary>
        ///     Called when the header is clicked.
        /// </summary>
        void OnClick(Core::Observer::RoutedEventArgs& e) override;

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e);

        /// <summary>
        /// Find grippers and register drag events
        ///
        /// The default style for DataGridRowHeader is
        /// +-------------------------------+
        /// +-------------------------------+ 
        /// +           Gripper             + 
        /// +-------------------------------+
        /// +            Header             +
        /// +-------------------------------+ 
        /// +           Gripper             + 
        /// +-------------------------------+
        /// +-------------------------------+
        /// 
        /// The reason we have two grippers is we can't extend the bottom gripper to straddle the line between two 
        /// headers; the header below would render on top of it.
        /// We resize a Row by grabbing the gripper to the bottom; the top gripper thus adjusts the height of
        /// the row above it.
        /// </summary>
        void HookupGripperEvents();

        /// <summary>
        /// Clear gripper event
        /// </summary>
        void UnhookGripperEvents();

        DataGridRow * PreviousRow();
    };
}

#endif //! __XUI_UI_DATACOLUMN_ROW_HEADER_H__