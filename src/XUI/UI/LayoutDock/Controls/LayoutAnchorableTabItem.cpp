#include "LayoutAnchorableTabItem.h"
#include "LayoutAnchorablePane.h"
#include "LayoutAnchorablePaneGroup.h"
#include "LayoutItem.h"
#include "LayoutRoot.h"
#include "DockingManager.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutAnchorableTabItem, XUI::UI::LayoutAnchorableTabItem::StaticClassInitializer)

std::shared_ptr<LayoutAnchorableTabItem> LayoutAnchorableTabItem::_draggingItem;
bool LayoutAnchorableTabItem::_cancelMouseLeave = false;

XUI::Core::Dependency::RefDependencyProperty LayoutAnchorableTabItem::ModelProperty;
XUI::Core::Dependency::RefDependencyProperty LayoutAnchorableTabItem::LayoutItemProperty;

void XUI::UI::LayoutAnchorableTabItem::StaticClassInitializer()
{
    ModelProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::LayoutContent>>::Register<SelectingItemsControl>(nameof(Model)));;
    LayoutItemProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::LayoutItem>>::Register<SelectingItemsControl>(nameof(LayoutItem)));;

    ModelProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        LayoutAnchorableTabItem* control = e.Sender->Static_As<LayoutAnchorableTabItem>();
        if (control)
        {
            UI::LayoutContent* content = static_cast<UI::LayoutContent*>(e.NewValue.ToObject());
            control->Header = content->Title;
        }
        static_cast<UI::Controls::Control*>(e.Sender)->OnTemplateChanged(e);
    });
}

LayoutAnchorableTabItem::LayoutAnchorableTabItem()
{
}

void LayoutAnchorableTabItem::OnModelChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutAnchorableTabItem*>(d))->OnModelChanged(e);
}

void LayoutAnchorableTabItem::OnModelChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (Model != nullptr)
    {
        SetLayoutItem(Model->Root->GetManager()->GetLayoutItemFromModel(Model.get())->shared_from_base_static<UI::LayoutItem>());
    }
    else
    {
        SetLayoutItem(nullptr);
    }
    //UpdateLogicalParent();
}

void LayoutAnchorableTabItem::OnPointerPressed(Interfaces::PointerPressedEventArgs& e)
{
    Control::OnPointerPressed(e);

    _isMouseDown = true;
    _draggingItem = shared_from_base_static<LayoutAnchorableTabItem>();
}

void LayoutAnchorableTabItem::OnPointerMoved(Interfaces::PointerEventArgs& e)
{
    Control::OnPointerMoved(e);

    if (~e.InputModifiers & Interfaces::InputModifiers::LeftMouseButton)
    {
        _isMouseDown = false;
        _draggingItem = nullptr;
    }
    else
    {
        _cancelMouseLeave = false;
    }
}

void LayoutAnchorableTabItem::OnPointerReleased(Interfaces::PointerReleasedEventArgs& e)
{
    _isMouseDown = false;

    Control::OnPointerReleased(e);

    Model->IsActive = true;
}

void LayoutAnchorableTabItem::OnPointerLeave(Interfaces::PointerEventArgs& e)
{
    Control::OnPointerLeave(e);

    if (_isMouseDown && e.InputModifiers & Interfaces::InputModifiers::LeftMouseButton)
    {
        // drag the item if the mouse leave is not canceled.
        // Mouse leave should be canceled when selecting a new tab to prevent automatic undock when Panel size is Auto.
        _draggingItem = !_cancelMouseLeave ? shared_from_base_static<LayoutAnchorableTabItem>() : nullptr;
    }

    _isMouseDown = false;
    _cancelMouseLeave = false;
}

void LayoutAnchorableTabItem::OnPointerEnter(Interfaces::PointerEventArgs& e)
{
    Control::OnPointerEnter(e);

    if (_draggingItem != nullptr && _draggingItem.get() != this && e.InputModifiers & Interfaces::InputModifiers::LeftMouseButton)
    {
        auto model = Model;
        auto container = model->Parent;
        auto containerPane = model->Parent;

        if (containerPane->Is<LayoutAnchorablePane>() && !containerPane->Static_As<LayoutAnchorablePane>()->CanRepositionItems)
            return;

        if (containerPane->Parent && containerPane->Parent->Is<LayoutAnchorablePaneGroup>() && !containerPane->Parent->Static_As<LayoutAnchorablePaneGroup>()->CanRepositionItems)
            return;

        containerPane->MoveChild(container->Children.IndexOf(_draggingItem->Model), container->Children.IndexOf(model));
    }
}

//void LayoutAnchorableTabItem::OnPreviewGotKeyboardFocus(KeyboardFocusChangedEventArgs *e)
//{
//    Control::OnPreviewGotKeyboardFocus(e);
//}

bool LayoutAnchorableTabItem::IsDraggingItem()
{
    return _draggingItem != nullptr;
}

LayoutAnchorableTabItem *LayoutAnchorableTabItem::GetDraggingItem()
{
    return _draggingItem.get();
}

void LayoutAnchorableTabItem::ResetDraggingItem()
{
    _draggingItem = nullptr;
}

void LayoutAnchorableTabItem::CancelMouseLeave()
{
    _cancelMouseLeave = true;
}
