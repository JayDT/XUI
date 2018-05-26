#ifndef __XUI_UI_DATAGRID_EVENTARGS_H__
#define __XUI_UI_DATAGRID_EVENTARGS_H__

#include "UI/Controls/Control.h"
#include "Core/Observer/RoutedEvent.h"

namespace XUI::UI
{
    class DataGridColumn;
    class DataGridRow;

    struct DataGridAutoGeneratingColumnEventArgs : System::Events::EventArg
    {
        virtual ~DataGridAutoGeneratingColumnEventArgs() {}

        DataGridColumn* Column;
        std::string PropertyName;
        System::Reflection::Type const* PropertyType;
        System::Reflection::Variant PropertyDescriptor;
        bool Cancel = false;
    };

    struct DataGridPreparingCellForEditEventArgs : System::Events::EventArg
    {
        DataGridColumn* Column = nullptr;
        DataGridRow* Row = nullptr;
        Core::Observer::RoutedEventArgs* EditingEventArgs = nullptr;
        Controls::Control* EditingElement = nullptr;
    };
}

#endif // !__XUI_UI_DATAGRID_EVENTARGS_H__
