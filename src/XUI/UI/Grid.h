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

#ifndef __XUI_GRID_H__
#define __XUI_GRID_H__

#include "UI/Panel.h"
#include "UI/Detail/GridLength.h"
#include "UI/Detail/ColumnDefinition.h"
#include "UI/Detail/RowDefinition.h"

namespace XUI::UI
{
    /// <summary>
    /// Lays out child controls according to a grid.
    /// </summary>
    class TC_XUI_API Grid : public Panel
    {
    private:

        struct Segment
        {
            double OriginalSize = 0;
            double Max = 0;
            double Min = 0;
            double DesiredSize = 0;
            double OfferedSize = 0;
            double Stars = 0;
            GridUnitType Type = GridUnitType::Auto;

            Segment() {}
            Segment(double offeredSize, double min, double max, GridUnitType type);
        };

    public:
        typedef std::vector<std::vector<Segment>> Segments;

    private:
        struct GridNode
        {
            int Row = 0;
            int Column = 0;
            double Size = 0.0;
            Segments* Matrix = nullptr;

            GridNode() {}
            GridNode(Segments* matrix, int row, int col, double size);

            bool operator==(GridNode const& rhs) const
            {
                return Row == rhs.Row && Column == rhs.Column && Size == rhs.Size && Matrix == rhs.Matrix;
            }
        };

        class GridWalker
        {
            bool m_HasAutoAuto = false;
            bool m_HasStarAuto = false;
            bool m_HasAutoStar = false;

        public:
            GridWalker() {}
            GridWalker(Grid *grid, Segments* rowMatrix, Segments* colMatrix);

            PROPERTY_GET_ONLY(bool, HasAutoAuto);
            bool GetHasAutoAuto() const { return m_HasAutoAuto; }

            PROPERTY_GET_ONLY(bool, HasStarAuto);
            bool GetHasStarAuto() const { return m_HasStarAuto; }

            PROPERTY_GET_ONLY(bool, HasAutoStar);
            bool GetHasAutoStar() const { return m_HasAutoStar; }
        };

    public:
        typedef System::Collection::Vector<std::shared_ptr<ColumnDefinition>>  ColumnDefinitionList;
        typedef System::Collection::Vector<std::shared_ptr<RowDefinition>>     RowDefinitionList;

        /// <summary>
        /// Defines the Column attached property.
        /// </summary>
        static XUI_DEPENDENCY_PROPERTY_DECL(int) ColumnProperty;

        /// <summary>
        /// Defines the ColumnSpan attached property.
        /// </summary>
        static XUI_DEPENDENCY_PROPERTY_DECL(int) ColumnSpanProperty;

        /// <summary>
        /// Defines the Row attached property.
        /// </summary>
        static XUI_DEPENDENCY_PROPERTY_DECL(int) RowProperty;

        /// <summary>
        /// Defines the RowSpan attached property.
        /// </summary>
        static XUI_DEPENDENCY_PROPERTY_DECL(int) RowSpanProperty;

        static void StaticClassInitializer();
        Grid();

    private:
        Meta(Disable) ColumnDefinitionList _columnDefinitions;
        Meta(Disable) RowDefinitionList _rowDefinitions;

        Segments _rowMatrix;
        Segments _colMatrix;

    public:
        /// <summary>
        /// Gets or sets the columns definitions for the grid.
        /// </summary>
        PROPERTY_(Grid::ColumnDefinitionList const&, ColumnDefinitions);
        Grid::ColumnDefinitionList& GetColumnDefinitions() { return _columnDefinitions; }
        void SetColumnDefinitions(Grid::ColumnDefinitionList const& value);

        /// <summary>
        /// Gets or sets the row definitions for the grid.
        /// </summary>
        PROPERTY_(Grid::RowDefinitionList const&, RowDefinitions);
        Grid::RowDefinitionList& GetRowDefinitions() { return _rowDefinitions; }
        void SetRowDefinitions(Grid::RowDefinitionList const& value);

        /// <summary>
        /// Gets the value of the Column attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <returns>The control's column.</returns>
        static int GetColumn(Core::Dependency::DependencyObject *element);

        /// <summary>
        /// Gets the value of the ColumnSpan attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <returns>The control's column span.</returns>
        static int GetColumnSpan(Core::Dependency::DependencyObject *element);

        /// <summary>
        /// Gets the value of the Row attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <returns>The control's row.</returns>
        static int GetRow(Core::Dependency::DependencyObject *element);

        /// <summary>
        /// Gets the value of the RowSpan attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <returns>The control's row span.</returns>
        static int GetRowSpan(Core::Dependency::DependencyObject *element);

        /// <summary>
        /// Sets the value of the Column attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <param name="value">The column value.</param>
        static void SetColumn(Core::Dependency::DependencyObject *element, int value);

        /// <summary>
        /// Sets the value of the ColumnSpan attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <param name="value">The column span value.</param>
        static void SetColumnSpan(Core::Dependency::DependencyObject *element, int value);

        /// <summary>
        /// Sets the value of the Row attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <param name="value">The row value.</param>
        static void SetRow(Core::Dependency::DependencyObject *element, int value);

        /// <summary>
        /// Sets the value of the RowSpan attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <param name="value">The row span value.</param>
        static void SetRowSpan(Core::Dependency::DependencyObject *element, int value);

        /// <summary>
        /// Measures the grid.
        /// </summary>
        /// <param name="constraint">The available size.</param>
        /// <returns>The desired size of the control.</returns>
    protected:
        Core::Media::Size MeasureOverride(Core::Media::Size const& constraint) override;

        /// <summary>
        /// Arranges the grid's children.
        /// </summary>
        /// <param name="finalSize">The size allocated to the control.</param>
        /// <returns>The space taken.</returns>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

    private:
        static double Clamp(double val, double min, double max);

        static int ValidateColumn(Core::Dependency::DependencyObject *o, int value);

        static int ValidateRow(Core::Dependency::DependencyObject *o, int value);

        void CreateMatrices(int rowCount, int colCount);

        void ExpandStarCols(Core::Media::Size const& availableSize);

        void ExpandStarRows(Core::Media::Size const& availableSize);

        Meta(Disable) void AssignSize(Segments* matrix, int start, int end, double &size, GridUnitType type, bool desiredSize);

        void AllocateDesiredSize(int rowCount, int colCount);

        void SaveMeasureResults();

        void RestoreMeasureResults();
    };
}


#endif	//#ifndef __XUI_GRID_H__
