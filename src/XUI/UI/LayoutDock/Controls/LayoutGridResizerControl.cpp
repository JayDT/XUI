#include "StdAfx.h"
#include "LayoutGridResizerControl.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutGridResizerControl, XUI::UI::LayoutGridResizerControl::StaticClassInitializer);

Core::Dependency::RefDependencyProperty LayoutGridResizerControl::BackgroundWhileDraggingProperty;
Core::Dependency::RefDependencyProperty LayoutGridResizerControl::OpacityWhileDraggingProperty;

void XUI::UI::LayoutGridResizerControl::StaticClassInitializer()
{
    BackgroundWhileDraggingProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<LayoutGridResizerControl>(nameof(GridSplitterWidth), Core::Media::Brush::Parse("Black")));
    OpacityWhileDraggingProperty = (Core::Dependency::BindProperty<double>::Register<LayoutGridResizerControl>(nameof(GridSplitterWidth), 0.5));
}

XUI::UI::LayoutGridResizerControl::LayoutGridResizerControl()
{
}

XUI::UI::LayoutGridResizerControl::~LayoutGridResizerControl()
{
}
