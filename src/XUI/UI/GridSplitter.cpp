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

#include "GridSplitter.h"

#include "Detail/ColumnDefinition.h"
#include "Detail/RowDefinition.h"

#include "ICursor.h"

using namespace XUI;
using namespace XUI::UI;

void GridSplitter::GetDeltaConstraints(double &min, double &max)
{
    auto prevDefinitionLen = GetActualLength(_prevDefinition.get());
    auto prevDefinitionMin = GetMinLength(_prevDefinition.get());
    auto prevDefinitionMax = GetMaxLength(_prevDefinition.get());

    auto nextDefinitionLen = GetActualLength(_nextDefinition.get());
    auto nextDefinitionMin = GetMinLength(_nextDefinition.get());
    auto nextDefinitionMax = GetMaxLength(_nextDefinition.get());

    // Determine the minimum and maximum the columns can be resized
    min = -std::min(prevDefinitionLen - prevDefinitionMin, nextDefinitionMax - nextDefinitionLen);
    max = std::min(prevDefinitionMax - prevDefinitionLen, nextDefinitionLen - nextDefinitionMin);
}

void GridSplitter::OnDragDelta(Interfaces::VectorEventArgs& e)
{
    auto delta = _orientation == Orientation::Vertical ? e.Vector.x : e.Vector.y;
    double max;
    double min;
    GetDeltaConstraints(min, max);
    delta = std::min(std::max(delta, float(min)), float(max));
    for (auto definition : _definitions)
    {
        if (definition == _prevDefinition)
        {
            SetLengthInStars(_prevDefinition.get(), GetActualLength(_prevDefinition.get()) + delta);
        }
        else if (definition == _nextDefinition)
        {
            SetLengthInStars(_nextDefinition.get(), GetActualLength(_nextDefinition.get()) - delta);
        }
        else if (IsStar(definition.get()))
        {
            SetLengthInStars(definition.get(), GetActualLength(definition.get())); // same size but in stars.
        }
    }
}

double GridSplitter::GetActualLength(DefinitionBase *definition)
{
    if (definition == nullptr)
        return 0.0;

    auto columnDefinition = definition->As<ColumnDefinition>();
    if (columnDefinition)
        return columnDefinition->ActualWidth;

    auto rowDefinition = definition->As<RowDefinition>();
    if (rowDefinition)
        return rowDefinition->ActualHeight;

    return 0.0;
}

double GridSplitter::GetMinLength(DefinitionBase *definition)
{
    if (definition == nullptr)
        return 0.0;

    auto columnDefinition = definition->As<ColumnDefinition>();
    if (columnDefinition)
        return columnDefinition->MinWidth;

    auto rowDefinition = definition->As<RowDefinition>();
    if (rowDefinition)
        return rowDefinition->MinHeight;

    return 0.0;
}

double GridSplitter::GetMaxLength(DefinitionBase *definition)
{
    if (definition == nullptr)
        return 0.0;

    auto columnDefinition = definition->As<ColumnDefinition>();
    if (columnDefinition)
        return columnDefinition->MaxWidth;

    auto rowDefinition = definition->As<RowDefinition>();
    if (rowDefinition)
        return rowDefinition->MaxHeight;

    return 0.0;
}

bool GridSplitter::IsStar(DefinitionBase *definition)
{
    if (definition == nullptr)
        return false;

    auto columnDefinition = definition->As<ColumnDefinition>();
    if (columnDefinition)
        return columnDefinition->Width.IsStar;

    auto rowDefinition = definition->As<RowDefinition>();
    if (rowDefinition)
        return rowDefinition->Height.IsStar;

    return false;
}

void GridSplitter::SetLengthInStars(DefinitionBase *definition, double value)
{

    auto columnDefinition = definition->As<ColumnDefinition>();
    if (columnDefinition)
    {
        columnDefinition->Width = GridLength(value, GridUnitType::Star);
        return;
    }

    auto rowDefinition = definition->As<RowDefinition>();
    if (rowDefinition)
        rowDefinition->Height = GridLength(value, GridUnitType::Star);
}

void GridSplitter::OnAttachedToVisualTree(XUI::UI::Controls::VisualTreeAttachmentEventArgs const& e)
{
    Thumb::OnAttachedToVisualTree(e);
    _grid = GetVisualParent<Grid>()->shared_from_base_static<Grid>();

    _orientation = DetectOrientation();

    //int defenitionIndex; //row or col
    //if (_orientation == Orientation::Vertical)
    //{
    //    Cursor = new Cursor(Interfaces::StandardCursorType::SizeWestEast);
    //    _definitions = _grid->ColumnDefinitions.Cast<DefinitionBase>().ToList();
    //    defenitionIndex = GetValue(Grid::ColumnProperty);
    //    PseudoClasses->Add(":vertical");
    //}
    //else
    //{
    //    Cursor = new Cursor(Interfaces::StandardCursorType::SizeNorthSouth);
    //    defenitionIndex = GetValue(Grid::RowProperty);
    //    _definitions = _grid->RowDefinitions.Cast<DefinitionBase*>().ToList();
    //    PseudoClasses->Add(":horizontal");
    //}

    //if (defenitionIndex > 0)
    //{
    //    _prevDefinition = _definitions[defenitionIndex - 1];
    //}
    //
    //if (defenitionIndex < _definitions.size() - 1)
    //{
    //    _nextDefinition = _definitions[defenitionIndex + 1];
    //}
}

Orientation GridSplitter::DetectOrientation()
{
    //if (!_grid->ColumnDefinitions.Any())
    //{
    //    return Orientation::Horizontal;
    //}
    //if (!_grid->RowDefinitions.Any())
    //{
    //    return Orientation::Vertical;
    //}
    //
    //auto col = GetValue(Grid::ColumnProperty);
    //auto row = GetValue(Grid::RowProperty);
    //auto width = _grid->ColumnDefinitions[col]->Width;
    //auto height = _grid->RowDefinitions[row]->Height;
    //if (width.IsAuto && !height.IsAuto)
    //{
    //    return Orientation::Vertical;
    //}
    //if (!width.IsAuto && height.IsAuto)
    //{
    //    return Orientation::Horizontal;
    //}
    //if (_grid->Children->OfType<Control*>().Where([&](void *c)
    //{
    //    return Grid::GetColumn(c) == col;
    //})->Any(c = > c->GetType() != GridSplitter::typeid)) // Decision based on other controls in the same column
    //{
    //    return Orientation::Horizontal;
    //}
    return Orientation::Vertical;
}
