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

#include "Grid.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(Grid, XUI::UI::Grid::StaticClassInitializer)

XUI_DEPENDENCY_PROPERTY_DECL(int) Grid::ColumnProperty;
XUI_DEPENDENCY_PROPERTY_DECL(int) Grid::ColumnSpanProperty;
XUI_DEPENDENCY_PROPERTY_DECL(int) Grid::RowProperty;
XUI_DEPENDENCY_PROPERTY_DECL(int) Grid::RowSpanProperty;

void XUI::UI::Grid::StaticClassInitializer()
{
    ColumnProperty = (Core::Dependency::BindProperty<int>::RegisterAttached<Grid, XUI::UI::Controls::Control>(nameof(Column), 0, false, Core::Dependency::BindingMode::OneWay, [](XamlCPP::Core::Dependency::IDependencyObject*, System::Reflection::meta::Variant const& value)
    {
        int _value = value.GetValue<int>();
        if (_value < 0)
            throw System::ArgumentException("Invalid Grid.Column value.");
        return value;
    }));

    ColumnSpanProperty  = (Core::Dependency::BindProperty<int>::RegisterAttached<Grid, XUI::UI::Controls::Control>(nameof(ColumnSpan), 1));
    RowProperty         = (Core::Dependency::BindProperty<int>::RegisterAttached<Grid, XUI::UI::Controls::Control>(nameof(Row), 0, false, Core::Dependency::BindingMode::OneWay, [](XamlCPP::Core::Dependency::IDependencyObject*, System::Reflection::meta::Variant const& value)
    {
        int _value = value.GetValue<int>();
        if (_value < 0)
            throw System::ArgumentException("Invalid Grid.Row value.");
        return value;
    }));
    RowSpanProperty     = (Core::Dependency::BindProperty<int>::RegisterAttached<Grid, XUI::UI::Controls::Control>(nameof(RowSpan), 1));
}

XUI::UI::Grid::Grid()
{
}

void Grid::SetColumnDefinitions(Grid::ColumnDefinitionList const& value)
{
    if (!_columnDefinitions.empty())
    {
        throw System::NotSupportedException("Reassigning ColumnDefinitions not yet implemented.");
    }

    for (auto col : value.GetNativeEnumerator())
        _columnDefinitions.Add(col);

    _columnDefinitions.CollectionChanged += ([&](void *_, System::Collection::NotifyCollectionChangedEventArgs const&)
    {
        InvalidateMeasure();
    });
}

void Grid::SetRowDefinitions(Grid::RowDefinitionList const& value)
{
    if (!_rowDefinitions.empty())
    {
        throw System::NotSupportedException("Reassigning RowDefinitions not yet implemented.");
    }

    for (auto row : value.GetNativeEnumerator())
        _rowDefinitions.Add(row);

    _rowDefinitions.CollectionChanged += ([&](void *_, System::Collection::NotifyCollectionChangedEventArgs const&)
    {
        InvalidateMeasure();
    });
}

int Grid::GetColumn(Core::Dependency::DependencyObject *element)
{
    return element->GetValue<int>(ColumnProperty.get());
}

int Grid::GetColumnSpan(Core::Dependency::DependencyObject *element)
{
    return element->GetValue<int>(ColumnSpanProperty.get());
}

int Grid::GetRow(Core::Dependency::DependencyObject *element)
{
    return element->GetValue<int>(RowProperty.get());
}

int Grid::GetRowSpan(Core::Dependency::DependencyObject *element)
{
    return element->GetValue<int>(RowSpanProperty.get());
}

void Grid::SetColumn(Core::Dependency::DependencyObject *element, int value)
{
    element->SetValue(ColumnProperty.get(), value);
}

void Grid::SetColumnSpan(Core::Dependency::DependencyObject *element, int value)
{
    element->SetValue(ColumnSpanProperty.get(), value);
}

void Grid::SetRow(Core::Dependency::DependencyObject *element, int value)
{
    element->SetValue(RowProperty.get(), value);
}

void Grid::SetRowSpan(Core::Dependency::DependencyObject *element, int value)
{
    element->SetValue(RowSpanProperty.get(), value);
}

XUI::Core::Media::Size Grid::MeasureOverride(XUI::Core::Media::Size const& constraint)
{
    XUI::Core::Media::Size totalSize = constraint;
    int colCount = ColumnDefinitions.size();
    int rowCount = RowDefinitions.size();
    double totalStarsX = 0;
    double totalStarsY = 0;
    bool emptyRows = rowCount == 0;
    bool emptyCols = colCount == 0;
    bool hasChildren = Children.size() > 0;

    if (emptyRows)
    {
        rowCount = 1;
    }

    if (emptyCols)
    {
        colCount = 1;
    }

    CreateMatrices(rowCount, colCount);

    if (emptyRows)
    {
        _rowMatrix[0][0] = Segment(0, 0, std::numeric_limits<double>::infinity(), GridUnitType::Star);
        _rowMatrix[0][0].Stars = 1.0;
        totalStarsY += 1.0;
    }
    else
    {
        for (int i = 0; i < rowCount; i++)
        {
            RowDefinition *rowdef = _rowDefinitions[i].get();
            GridLength *height = &rowdef->Height;

            rowdef->ActualHeight = std::numeric_limits<double>::infinity();
            _rowMatrix[i][i] = Segment(0, rowdef->MinHeight, rowdef->MaxHeight, height->GridUnitType);

            if (height->GridUnitType == GridUnitType::Pixel)
            {
                _rowMatrix[i][i].OfferedSize = Clamp(height->Value, _rowMatrix[i][i].Min, _rowMatrix[i][i].Max);
                _rowMatrix[i][i].DesiredSize = _rowMatrix[i][i].OfferedSize;
                rowdef->ActualHeight = _rowMatrix[i][i].OfferedSize;
            }
            else if (height->GridUnitType == GridUnitType::Star)
            {
                _rowMatrix[i][i].Stars = height->Value;
                totalStarsY += height->Value;
            }
            else if (height->GridUnitType == GridUnitType::Auto)
            {
                _rowMatrix[i][i].OfferedSize = Clamp(0, _rowMatrix[i][i].Min, _rowMatrix[i][i].Max);
                _rowMatrix[i][i].DesiredSize = _rowMatrix[i][i].OfferedSize;
            }
        }
    }

    if (emptyCols)
    {
        _colMatrix[0][0] = Segment(0, 0, std::numeric_limits<double>::infinity(), GridUnitType::Star);
        _colMatrix[0][0].Stars = 1.0;
        totalStarsX += 1.0;
    }
    else
    {
        for (int i = 0; i < colCount; i++)
        {
            ColumnDefinition *coldef = _columnDefinitions[i].get();
            GridLength *width = &coldef->Width;

            coldef->ActualWidth = std::numeric_limits<double>::infinity();
            _colMatrix[i][i] = Segment(0, coldef->MinWidth, coldef->MaxWidth, width->GridUnitType);

            if (width->GridUnitType == GridUnitType::Pixel)
            {
                _colMatrix[i][i].OfferedSize = Clamp(width->Value, _colMatrix[i][i].Min, _colMatrix[i][i].Max);
                _colMatrix[i][i].DesiredSize = _colMatrix[i][i].OfferedSize;
                coldef->ActualWidth = _colMatrix[i][i].OfferedSize;
            }
            else if (width->GridUnitType == GridUnitType::Star)
            {
                _colMatrix[i][i].Stars = width->Value;
                totalStarsX += width->Value;
            }
            else if (width->GridUnitType == GridUnitType::Auto)
            {
                _colMatrix[i][i].OfferedSize = Clamp(0, _colMatrix[i][i].Min, _colMatrix[i][i].Max);
                _colMatrix[i][i].DesiredSize = _colMatrix[i][i].OfferedSize;
            }
        }
    }

    std::vector<GridNode> sizes;
    GridNode node;
    GridNode separator = GridNode(nullptr, 0, 0, 0);
    int separatorIndex;

    sizes.push_back(separator);

    // Pre-process the grid children so that we know what types of elements we have so
    // we can apply our special measuring rules.
    GridWalker gridWalker(this, &_rowMatrix, &_colMatrix);

    for (int i = 0; i < 6; i++)
    {
        // These bools tell us which grid element type we should be measuring. i.e.
        // 'star/auto' means we should measure elements with a star row and auto col
        bool autoAuto = i == 0;
        bool starAuto = i == 1;
        bool autoStar = i == 2;
        bool starAutoAgain = i == 3;
        bool nonStar = i == 4;
        bool remainingStar = i == 5;

        if (hasChildren)
        {
            ExpandStarCols(totalSize);
            ExpandStarRows(totalSize);
        }

        for (auto child : Children.GetNativeEnumerator())
        {
            int col, row;
            int colspan, rowspan;
            double childSizeX = 0;
            double childSizeY = 0;
            bool starCol = false;
            bool starRow = false;
            bool autoCol = false;
            bool autoRow = false;

            col = std::min(GetColumn(child.get()), colCount - 1);
            row = std::min(GetRow(child.get()), rowCount - 1);
            colspan = std::min(GetColumnSpan(child.get()), colCount - col);
            rowspan = std::min(GetRowSpan(child.get()), rowCount - row);

            for (int r = row; r < row + rowspan; r++)
            {
                starRow |= _rowMatrix[r][r].Type == GridUnitType::Star;
                autoRow |= _rowMatrix[r][r].Type == GridUnitType::Auto;
            }

            for (int c = col; c < col + colspan; c++)
            {
                starCol |= _colMatrix[c][c].Type == GridUnitType::Star;
                autoCol |= _colMatrix[c][c].Type == GridUnitType::Auto;
            }

            // This series of if statements checks whether or not we should measure
            // the current element and also if we need to override the sizes
            // passed to the Measure call.

            // If the element has Auto rows and Auto columns and does not span Star
            // rows/cols it should only be measured in the auto_auto phase.
            // There are similar rules governing auto/star and star/auto elements.
            // NOTE: star/auto elements are measured twice. The first time with
            // an override for height, the second time without it.
            if (autoRow && autoCol && !starRow && !starCol)
            {
                if (!autoAuto)
                {
                    continue;
                }

                childSizeX = std::numeric_limits<double>::infinity();
                childSizeY = std::numeric_limits<double>::infinity();
            }
            else if (starRow && autoCol && !starCol)
            {
                if (!(starAuto || starAutoAgain))
                {
                    continue;
                }

                if (starAuto && gridWalker.HasAutoStar)
                {
                    childSizeY = std::numeric_limits<double>::infinity();
                }

                childSizeX = std::numeric_limits<double>::infinity();
            }
            else if (autoRow && starCol && !starRow)
            {
                if (!autoStar)
                {
                    continue;
                }

                childSizeY = std::numeric_limits<double>::infinity();
            }
            else if ((autoRow || autoCol) && !(starRow || starCol))
            {
                if (!nonStar)
                {
                    continue;
                }

                if (autoRow)
                {
                    childSizeY = std::numeric_limits<double>::infinity();
                }

                if (autoCol)
                {
                    childSizeX = std::numeric_limits<double>::infinity();
                }
            }
            else if (!(starRow || starCol))
            {
                if (!nonStar)
                {
                    continue;
                }
            }
            else
            {
                if (!remainingStar)
                {
                    continue;
                }
            }

            for (int r = row; r < row + rowspan; r++)
            {
                childSizeY += _rowMatrix[r][r].OfferedSize;
            }

            for (int c = col; c < col + colspan; c++)
            {
                childSizeX += _colMatrix[c][c].OfferedSize;
            }

            child->Measure(Core::Media::Size(childSizeX, childSizeY));
            XUI::Core::Media::Size desired = child->DesiredSize;

            // Elements distribute their height based on two rules:
            // 1) Elements with rowspan/colspan == 1 distribute their height first
            // 2) Everything else distributes in a LIFO manner.
            // As such, add all UIElements with rowspan/colspan == 1 after the separator in
            // the list and everything else before it. Then to process, just keep popping
            // elements off the end of the list.
            if (!starAuto)
            {
                node = GridNode(&_rowMatrix, row + rowspan - 1, row, desired.Height);
                separatorIndex = std::distance(sizes.begin(), std::find(sizes.begin(), sizes.end(), separator));
                sizes.insert(sizes.begin() + (node.Row == node.Column ? separatorIndex + 1 : separatorIndex), node);
            }

            node = GridNode(&_colMatrix, col + colspan - 1, col, desired.Width);

            separatorIndex = std::distance(sizes.begin(), std::find(sizes.begin(), sizes.end(), separator));
            sizes.insert(sizes.begin() + (node.Row == node.Column ? separatorIndex + 1 : separatorIndex), node);
        }

        sizes.erase(std::find(sizes.begin(), sizes.end(), separator));

        while (sizes.size() > 0)
        {
            node = sizes.back();
            (*node.Matrix)[node.Row][node.Column].DesiredSize = std::max((*node.Matrix)[node.Row][node.Column].DesiredSize, node.Size);
            AllocateDesiredSize(rowCount, colCount);
            sizes.erase(std::find(sizes.begin(), sizes.end(), node));
        }

        sizes.push_back(separator);
    }

    // Once we have measured and distributed all sizes, we have to store
    // the results. Every time we want to expand the rows/cols, this will
    // be used as the baseline.
    SaveMeasureResults();

    sizes.erase(std::find(sizes.begin(), sizes.end(), separator));

    double gridSizeX = 0;
    double gridSizeY = 0;

    for (int c = 0; c < colCount; c++)
    {
        gridSizeX += _colMatrix[c][c].DesiredSize;
    }

    for (int r = 0; r < rowCount; r++)
    {
        gridSizeY += _rowMatrix[r][r].DesiredSize;
    }

    return Core::Media::Size(gridSizeX, gridSizeY);
}

XUI::Core::Media::Size Grid::ArrangeOverride(XUI::Core::Media::Size const& finalSize)
{
    int colCount = ColumnDefinitions.size();
    int rowCount = RowDefinitions.size();
    int colMatrixDim = _colMatrix[0].size();
    int rowMatrixDim = _rowMatrix[0].size();

    RestoreMeasureResults();

    double totalConsumedX = 0;
    double totalConsumedY = 0;

    for (int c = 0; c < colMatrixDim; c++)
    {
        _colMatrix[c][c].OfferedSize = _colMatrix[c][c].DesiredSize;
        totalConsumedX += _colMatrix[c][c].OfferedSize;
    }

    for (int r = 0; r < rowMatrixDim; r++)
    {
        _rowMatrix[r][r].OfferedSize = _rowMatrix[r][r].DesiredSize;
        totalConsumedY += _rowMatrix[r][r].OfferedSize;
    }

    if (totalConsumedX != finalSize.Width)
    {
        ExpandStarCols(finalSize);
    }

    if (totalConsumedY != finalSize.Height)
    {
        ExpandStarRows(finalSize);
    }

    for (int c = 0; c < colCount; c++)
    {
        _columnDefinitions[c]->ActualWidth = _colMatrix[c][c].OfferedSize;
    }

    for (int r = 0; r < rowCount; r++)
    {
        _rowDefinitions[r]->ActualHeight = _rowMatrix[r][r].OfferedSize;
    }

    for (auto child : Children.GetNativeEnumerator())
    {
        int col = std::min(GetColumn(child.get()), colMatrixDim - 1);
        int row = std::min(GetRow(child.get()), rowMatrixDim - 1);
        int colspan = std::min(GetColumnSpan(child.get()), colMatrixDim - col);
        int rowspan = std::min(GetRowSpan(child.get()), rowMatrixDim - row);

        double childFinalX = 0;
        double childFinalY = 0;
        double childFinalW = 0;
        double childFinalH = 0;

        for (int c = 0; c < col; c++)
        {
            childFinalX += _colMatrix[c][c].OfferedSize;
        }

        for (int c = col; c < col + colspan; c++)
        {
            childFinalW += _colMatrix[c][c].OfferedSize;
        }

        for (int r = 0; r < row; r++)
        {
            childFinalY += _rowMatrix[r][r].OfferedSize;
        }

        for (int r = row; r < row + rowspan; r++)
        {
            childFinalH += _rowMatrix[r][r].OfferedSize;
        }

        child->Arrange(Core::Media::Rect::xywh(childFinalX, childFinalY, childFinalW, childFinalH));
    }

    return finalSize;
}

double Grid::Clamp(double val, double min, double max)
{
    if (val < min)
    {
        return min;
    }
    else if (val > max)
    {
        return max;
    }
    else
    {
        return val;
    }
}

int Grid::ValidateColumn(Core::Dependency::DependencyObject *o, int value)
{
    if (value < 0)
    {
        throw std::invalid_argument("Invalid Grid.Column value.");
    }

    return value;
}

int Grid::ValidateRow(Core::Dependency::DependencyObject *o, int value)
{
    if (value < 0)
    {
        throw std::invalid_argument("Invalid Grid.Row value.");
    }

    return value;
}

void Grid::CreateMatrices(int rowCount, int colCount)
{
    _rowMatrix.clear();
    _rowMatrix.resize(rowCount);
    for (auto& _row : _rowMatrix)
        _row.resize(rowCount);

    _colMatrix.clear();
    _colMatrix.resize(colCount);
    for (auto& _col : _colMatrix)
        _col.resize(colCount);
}

void Grid::ExpandStarCols(XUI::Core::Media::Size const& availableSize)
{
    int matrixCount = _colMatrix[0].size();
    int columnsCount = ColumnDefinitions.size();
    double width = availableSize.Width;

    for (int i = 0; i < matrixCount; i++)
    {
        if (_colMatrix[i][i].Type == GridUnitType::Star)
        {
            _colMatrix[i][i].OfferedSize = 0;
        }
        else
        {
            width = std::max(width - _colMatrix[i][i].OfferedSize, 0.0);
        }
    }

    AssignSize(&_colMatrix, 0, matrixCount - 1, width, GridUnitType::Star, false);
    width = std::max(0.0, width);

    if (columnsCount > 0)
    {
        for (int i = 0; i < matrixCount; i++)
        {
            if (_colMatrix[i][i].Type == GridUnitType::Star)
            {
                _columnDefinitions[i]->ActualWidth = _colMatrix[i][i].OfferedSize;
            }
        }
    }
}

void Grid::ExpandStarRows(XUI::Core::Media::Size const& availableSize)
{
    int matrixCount = _rowMatrix[0].size();
    int rowCount = RowDefinitions.size();
    double height = availableSize.Height;

    // When expanding star rows, we need to zero out their height before
    // calling AssignSize. AssignSize takes care of distributing the
    // available size when there are Mins and Maxs applied.
    for (int i = 0; i < matrixCount; i++)
    {
        if (_rowMatrix[i][i].Type == GridUnitType::Star)
        {
            _rowMatrix[i][i].OfferedSize = 0.0;
        }
        else
        {
            height = std::max(height - _rowMatrix[i][i].OfferedSize, 0.0);
        }
    }

    AssignSize(&_rowMatrix, 0, matrixCount - 1, height, GridUnitType::Star, false);

    if (rowCount > 0)
    {
        for (int i = 0; i < matrixCount; i++)
        {
            if (_rowMatrix[i][i].Type == GridUnitType::Star)
            {
                _rowDefinitions[i]->ActualHeight = _rowMatrix[i][i].OfferedSize;
            }
        }
    }
}

void Grid::AssignSize(Segments* matrix, int start, int end, double &size, GridUnitType type, bool desiredSize)
{
    double count = 0;
    bool assigned;

    // Count how many segments are of the correct type. If we're measuring Star rows/cols
    // we need to count the number of stars instead.
    for (int i = start; i <= end; i++)
    {
        double segmentSize = desiredSize ? (*matrix)[i][i].DesiredSize : (*matrix)[i][i].OfferedSize;
        if (segmentSize < (*matrix)[i][i].Max)
        {
            count += type == GridUnitType::Star ? (*matrix)[i][i].Stars : 1;
        }
    }

    do
    {
        double contribution = size / count;

        assigned = false;

        for (int i = start; i <= end; i++)
        {
            double segmentSize = desiredSize ? (*matrix)[i][i].DesiredSize : (*matrix)[i][i].OfferedSize;

            if (!((*matrix)[i][i].Type == type && segmentSize < (*matrix)[i][i].Max))
            {
                continue;
            }

            double newsize = segmentSize;
            newsize += contribution * (type == GridUnitType::Star ? (*matrix)[i][i].Stars : 1);
            newsize = std::min(newsize, (*matrix)[i][i].Max);
            assigned |= newsize > segmentSize;
            size -= newsize - segmentSize;

            if (desiredSize)
            {
                (*matrix)[i][i].DesiredSize = newsize;
            }
            else
            {
                (*matrix)[i][i].OfferedSize = newsize;
            }
        }
    } while (assigned);
}

void Grid::AllocateDesiredSize(int rowCount, int colCount)
{
    // First allocate the heights of the RowDefinitions, then allocate
    // the widths of the ColumnDefinitions.
    for (int i = 0; i < 2; i++)
    {
        Segments *matrix = i == 0 ? &_rowMatrix : &_colMatrix;
        int count = i == 0 ? rowCount : colCount;

        for (int row = count - 1; row >= 0; row--)
        {
            for (int col = row; col >= 0; col--)
            {
                bool spansStar = false;
                for (int j = row; j >= col; j--)
                {
                    spansStar |= (*matrix)[j][j].Type == GridUnitType::Star;
                }

                // This is the amount of pixels which must be available between the grid rows
                // at index 'col' and 'row'. i.e. if 'row' == 0 and 'col' == 2, there must
                // be at least 'matrix [row][col].size' pixels of height allocated between
                // all the rows in the range col -> row.
                double current = (*matrix)[row][col].DesiredSize;

                // Count how many pixels have already been allocated between the grid rows
                // in the range col -> row. The amount of pixels allocated to each grid row/column
                // is found on the diagonal of the matrix.
                double totalAllocated = 0;

                for (int k = row; k >= col; k--)
                {
                    totalAllocated += (*matrix)[k][k].DesiredSize;
                }

                // If the size requirement has not been met, allocate the additional required
                // size between 'pixel' rows, then 'star' rows, finally 'auto' rows, until all
                // height has been assigned.
                if (totalAllocated < current)
                {
                    double additional = current - totalAllocated;

                    if (spansStar)
                    {
                        AssignSize(matrix, col, row, additional, GridUnitType::Star, true);
                    }
                    else
                    {
                        AssignSize(matrix, col, row, additional, GridUnitType::Pixel, true);
                        AssignSize(matrix, col, row, additional, GridUnitType::Auto, true);
                    }
                }
            }
        }
    }

    int rowMatrixDim = _rowMatrix[0].size();
    int colMatrixDim = _colMatrix[0].size();

    for (int r = 0; r < rowMatrixDim; r++)
    {
        _rowMatrix[r][r].OfferedSize = _rowMatrix[r][r].DesiredSize;
    }

    for (int c = 0; c < colMatrixDim; c++)
    {
        _colMatrix[c][c].OfferedSize = _colMatrix[c][c].DesiredSize;
    }
}

void Grid::SaveMeasureResults()
{
    int rowMatrixDim = _rowMatrix[0].size();
    int colMatrixDim = _colMatrix[0].size();

    for (int i = 0; i < rowMatrixDim; i++)
    {
        for (int j = 0; j < rowMatrixDim; j++)
        {
            _rowMatrix[i][j].OriginalSize = _rowMatrix[i][j].OfferedSize;
        }
    }

    for (int i = 0; i < colMatrixDim; i++)
    {
        for (int j = 0; j < colMatrixDim; j++)
        {
            _colMatrix[i][j].OriginalSize = _colMatrix[i][j].OfferedSize;
        }
    }
}

void Grid::RestoreMeasureResults()
{
    int rowMatrixDim = _rowMatrix[0].size();
    int colMatrixDim = _colMatrix[0].size();

    for (int i = 0; i < rowMatrixDim; i++)
    {
        for (int j = 0; j < rowMatrixDim; j++)
        {
            _rowMatrix[i][j].OfferedSize = _rowMatrix[i][j].OriginalSize;
        }
    }

    for (int i = 0; i < colMatrixDim; i++)
    {
        for (int j = 0; j < colMatrixDim; j++)
        {
            _colMatrix[i][j].OfferedSize = _colMatrix[i][j].OriginalSize;
        }
    }
}

Grid::Segment::Segment(double offeredSize, double min, double max, GridUnitType type)
{
    OriginalSize = 0;
    Min = min;
    Max = max;
    DesiredSize = 0;
    OfferedSize = offeredSize;
    Stars = 0;
    Type = type;
}

Grid::GridNode::GridNode(Segments* matrix, int row, int col, double size)
    : Row(row), Column(col), Size(size), Matrix(matrix)
{
}

Grid::GridWalker::GridWalker(Grid *grid, Segments *rowMatrix, Segments *colMatrix)
{
    int rowMatrixDim = rowMatrix[0].size();
    int colMatrixDim = colMatrix[0].size();

    for (auto child : grid->Children.GetNativeEnumerator())
    {
        bool starCol = false;
        bool starRow = false;
        bool autoCol = false;
        bool autoRow = false;

        int col = std::min(GetColumn(child.get()), colMatrixDim - 1);
        int row = std::min(GetRow(child.get()), rowMatrixDim - 1);
        int colspan = std::min(GetColumnSpan(child.get()), colMatrixDim - 1);
        int rowspan = std::min(GetRowSpan(child.get()), rowMatrixDim - 1);

        for (int r = row; r < row + rowspan; r++)
        {
            starRow |= (*rowMatrix)[r][r].Type == GridUnitType::Star;
            autoRow |= (*rowMatrix)[r][r].Type == GridUnitType::Auto;
        }

        for (int c = col; c < col + colspan; c++)
        {
            starCol |= (*colMatrix)[c][c].Type == GridUnitType::Star;
            autoCol |= (*colMatrix)[c][c].Type == GridUnitType::Auto;
        }

        m_HasAutoAuto |= autoRow && autoCol && !starRow && !starCol;
        m_HasStarAuto |= starRow && autoCol;
        m_HasAutoStar |= autoRow && starCol;
    }
}
