#ifndef __DATAGRID_EXTENSIONS_H__
#define __DATAGRID_EXTENSIONS_H__

#include "standard/LinkedReference/Reference.h"
#include "standard/LinkedReference/RefManager.h"

namespace XUI::UI
{
    class DataGrid;
    class DataGridColumn;
    class DataGridColumnHeader;
    class DataGridColumnHeadersPresenter;
    class DataGridRow;
    class DataGridCell;

    struct DataGridHelper
    {

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
        static System::Reflection::Variant GetCoercedTransferPropertyValue(
            Core::Dependency::DependencyObject* baseObject,
            System::Reflection::Variant const& baseValue,
            Core::Dependency::DependencyProperty* baseProperty = nullptr,
            Core::Dependency::DependencyObject* parentObject = nullptr,
            Core::Dependency::DependencyProperty* parentProperty = nullptr,
            Core::Dependency::DependencyObject* grandParentObject = nullptr,
            Core::Dependency::DependencyProperty* grandParentProperty = nullptr);


        /// <summary>
        ///     Invalidates a cell's panel if its column's width changes sufficiently. 
        /// </summary>
        /// <param name="cell">The cell or header.</param>
        /// <param name="e"></param>
        static void OnColumnWidthChanged(Controls::Control* cell, Core::Dependency::DependencyPropertyChangedEventArgs& e);
    };

    class DataGridRowReference : public System::Collection::Reference<DataGridRow, DataGrid>
    {
    protected:
        void targetObjectBuildLink() override;
        void targetObjectDestroyLink() override;
        void sourceObjectDestroyLink() override;
    public:
        explicit DataGridRowReference() : System::Collection::Reference<DataGridRow, DataGrid>() {}
        virtual ~DataGridRowReference() { unlink(); }
    };

    class DataGridCellReference : public System::Collection::Reference<DataGridCell, DataGridRow>
    {
    protected:
        void targetObjectBuildLink() override;
        void targetObjectDestroyLink() override;
        void sourceObjectDestroyLink() override;
    public:
        explicit DataGridCellReference() : System::Collection::Reference<DataGridCell, DataGridRow>() {}
        virtual ~DataGridCellReference() { unlink(); }
    };

    class DataGridColumnHeaderReference : public System::Collection::Reference<DataGridColumnHeader, DataGridColumnHeadersPresenter>
    {
    protected:
        void targetObjectBuildLink() override;
        void targetObjectDestroyLink() override;
        void sourceObjectDestroyLink() override;
    public:
        explicit DataGridColumnHeaderReference() : System::Collection::Reference<DataGridColumnHeader, DataGridColumnHeadersPresenter>() {}
        virtual ~DataGridColumnHeaderReference() { unlink(); }
    };
}

#endif //! __DATAGRID_EXTENSIONS_H__