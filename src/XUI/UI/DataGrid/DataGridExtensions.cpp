#include "StdAfx.h"
#include "DataGridExtensions.h"
#include "DataGrid/DataGridControl.h"
#include "DataGrid/DataGridRow.h"
#include "DataGrid/DataGridCell.h"
#include "DataGrid/DataGridColumnHeader.h"
#include "DataGrid/DataGridColumnHeaderPresenter.h"
#include "Core/Dependencies/BindValue.h"

/// <summary>
///     Computes the value of a given property based on the DataGrid property transfer rules.
/// </summary>
/// <remarks>
///     This is intended to be called from within the coercion of the baseProperty.
/// </remarks>
/// <param name="baseObject">The target object which recieves the transferred property</param>
/// <param name="baseValue">The baseValue that was passed into the coercion delegate</param>
/// <param name="baseProperty">The property that is being coerced</param>
/// <param name="parentObject">The object that contains the parentProperty</param>
/// <param name="parentProperty">A property who's value should be transfered (via coercion) to the baseObject if it has a higher precedence.</param>
/// <param name="grandParentObject">Same as parentObject but evaluated at a lower presedece for a given BaseValueSource</param>
/// <param name="grandParentProperty">Same as parentProperty but evaluated at a lower presedece for a given BaseValueSource</param>
/// <returns></returns>

System::Reflection::Variant XUI::UI::DataGridHelper::GetCoercedTransferPropertyValue(Core::Dependency::DependencyObject * baseObject, System::Reflection::Variant const & baseValue, Core::Dependency::DependencyProperty * baseProperty, Core::Dependency::DependencyObject * parentObject, Core::Dependency::DependencyProperty * parentProperty, Core::Dependency::DependencyObject * grandParentObject, Core::Dependency::DependencyProperty * grandParentProperty)
{
    // Transfer Property Coercion rules:
    //
    // Determine if this is a 'Transfer Property Coercion'.  If so:
    //   We can safely get the BaseValueSource because the property change originated from another
    //   property, and thus this BaseValueSource wont be stale.
    //   Pick a value to use based on who has the greatest BaseValueSource
    // If not a 'Transfer Property Coercion', simply return baseValue.  This will cause a property change if the value changes, which
    // will trigger a 'Transfer Property Coercion', and we will no longer have a stale BaseValueSource
    auto coercedValue = baseValue;

    //if (IsPropertyTransferEnabled(baseObject, baseProperty))
    {
        auto propertySource = baseObject->GetValueSource(baseProperty);
        auto maxBaseValueSource = propertySource ? propertySource->ValueLevel : Core::Dependency::BindingLevel::Unset;

        if (parentObject != nullptr)
        {
            auto parentPropertySource = parentObject->GetValueSource(parentProperty);
            auto parentBaseValueSource = parentPropertySource ? parentPropertySource->ValueLevel : Core::Dependency::BindingLevel::Unset;

            if (parentProperty->IsDirect || parentBaseValueSource <= maxBaseValueSource)
            {
                coercedValue = parentObject->GetValue(parentProperty);
                if (coercedValue.toPointer() == nullptr)
                    coercedValue = baseValue;
                maxBaseValueSource = parentBaseValueSource;
            }
        }

        if (grandParentObject != nullptr)
        {
            auto grandParentPropertySource = grandParentObject->GetValueSource(grandParentProperty);
            auto grandParentBaseValueSource = grandParentPropertySource ? grandParentPropertySource->ValueLevel : Core::Dependency::BindingLevel::Unset;

            if (grandParentProperty->IsDirect || grandParentBaseValueSource <= maxBaseValueSource)
            {
                coercedValue = grandParentObject->GetValue(grandParentProperty);
                if (coercedValue.toPointer() == nullptr)
                    coercedValue = baseValue;
                //maxBaseValueSource = grandParentPropertySource->ValueLevel;
            }
        }
    }

    return coercedValue;
}

/// <summary>
///     Invalidates a cell's panel if its column's width changes sufficiently. 
/// </summary>
/// <param name="cell">The cell or header.</param>
/// <param name="e"></param>

void XUI::UI::DataGridHelper::OnColumnWidthChanged(Controls::Control * cell, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    ASSERT((cell->Is<DataGridCell>()) || (cell->Is<DataGridColumnHeader>()), "provideColumn should be one of the cell or header containers.");

    bool isColumnHeader = false;
    DataGridColumn* column = nullptr;
    if (cell->Is<DataGridCell>())
        column = static_cast<DataGridCell*>(cell)->Column.get();
    else if (cell->Is<DataGridColumnHeader>())
    {
        isColumnHeader = true;
        column = static_cast<DataGridColumnHeader*>(cell)->Column();
    }

    if (column != nullptr)
    {
        // determine the desired value of width for auto kind columns
        GridLength width = column->Width;
        if (width.IsAuto ||
            (!isColumnHeader && width.IsSizeToCells) ||
            (isColumnHeader && width.IsSizeToHeader))
        {
            GridLength oldWidth = e.OldValue.GetValue<GridLength>();
            double desiredWidth = 0.0;
            if (oldWidth.GetGridUnitType() != width.GetGridUnitType())
            {
                double constraintWidth = column->GetConstraintWidth(isColumnHeader);
                if (cell->DesiredSize.Width != constraintWidth)
                {
                    cell->InvalidateMeasure();
                    cell->Measure(Core::Media::Size(constraintWidth, std::numeric_limits<double>::max()));
                }

                desiredWidth = cell->DesiredSize.Width;
            }
            else
            {
                desiredWidth = column->DesiredWidth; // oldWidth.DesiredValue;
            }

            if (std::isnan(column->DesiredWidth) || column->DesiredWidth > 0.0 ||
                column->DesiredWidth < desiredWidth)
            {
                column->SetWidthInternal(width, column->ActualWidth, desiredWidth);
            }
        }
    }
}

void XUI::UI::DataGridRowReference::targetObjectBuildLink()
{
    getSource()->GetRows().insertLast(this);
}

void XUI::UI::DataGridRowReference::targetObjectDestroyLink()
{
}

void XUI::UI::DataGridRowReference::sourceObjectDestroyLink()
{
}

void XUI::UI::DataGridCellReference::targetObjectBuildLink()
{
    getSource()->GetCells().insertLast(this);
}

void XUI::UI::DataGridCellReference::targetObjectDestroyLink()
{
}

void XUI::UI::DataGridCellReference::sourceObjectDestroyLink()
{
}

void XUI::UI::DataGridColumnHeaderReference::targetObjectBuildLink()
{
    getSource()->GetHeaders().insertLast(this);
}

void XUI::UI::DataGridColumnHeaderReference::targetObjectDestroyLink()
{
}

void XUI::UI::DataGridColumnHeaderReference::sourceObjectDestroyLink()
{
}
