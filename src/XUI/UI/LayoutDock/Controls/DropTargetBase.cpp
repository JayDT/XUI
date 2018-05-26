#include "StdAfx.h"
#include "DropTargetBase.h"

using namespace XUI;
using namespace XUI::UI;

Core::Dependency::RefDependencyProperty DropTargetBase::IsDraggingOverProperty;

/// <summary>
/// Initializes static members of the <see cref="DataGrid"/> class.
/// </summary>
void XUI::UI::DropTargetBase::StaticClassInitializer()
{
    IsDraggingOverProperty = (Core::Dependency::BindProperty<bool>::Register<DropTargetBase>(nameof(IsDraggingOver), false));
}

bool DropTargetBase::GetIsDraggingOver(DependencyObject *d)
{
    return d->GetValue<bool>(IsDraggingOverProperty);
}

void DropTargetBase::SetIsDraggingOver(DependencyObject *d, bool value)
{
    d->SetValue(IsDraggingOverProperty, value);
}
