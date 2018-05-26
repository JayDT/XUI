#include "AnchorablePaneTitle.h"
#include "DockingManager.h"
#include "LayoutRoot.h"
#include "LayoutAnchorable.h"
#include "LayoutAnchorablePane.h"
#include "LayoutAnchorablePaneControl.h"
#include "LayoutAnchorableFloatingWindow.h"
#include "LayoutFloatingWindowControl.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(AnchorablePaneTitle, XUI::UI::AnchorablePaneTitle::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty AnchorablePaneTitle::ModelProperty;
XUI::Core::Dependency::RefDependencyProperty AnchorablePaneTitle::LayoutItemProperty;

/// <summary>
/// Initializes static members of the <see cref="DataGrid"/> class.
/// </summary>
void XUI::UI::AnchorablePaneTitle::StaticClassInitializer()
{
    ModelProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::LayoutContent>>::Register<AnchorablePaneTitle>(nameof(Model), &AnchorablePaneTitle::_OnModelChanged, nullptr));
    LayoutItemProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::LayoutItem>>::Register<AnchorablePaneTitle>(nameof(LayoutItem)));

    //DependencyProperty *const AnchorablePaneTitle::ModelProperty = DependencyProperty::Register("Model", LayoutAnchorable::typeid, AnchorablePaneTitle::typeid, new FrameworkPropertyMetadata(static_cast<LayoutAnchorable*>(nullptr), new PropertyChangedCallback(_OnModelChanged)));
    //DependencyPropertyKey *const AnchorablePaneTitle::LayoutItemPropertyKey = DependencyProperty::RegisterReadOnly("LayoutItem", LayoutItem::typeid, AnchorablePaneTitle::typeid, new FrameworkPropertyMetadata(static_cast<LayoutItem*>(nullptr)));
    //DependencyProperty *const AnchorablePaneTitle::LayoutItemProperty = LayoutItemPropertyKey->DependencyProperty;

    //FrameworkPropertyMetadata tempVar(true);
    //IsHitTestVisibleProperty::OverrideMetadata(AnchorablePaneTitle::typeid, &tempVar);
    //FrameworkPropertyMetadata tempVar2(false);
    //FocusableProperty::OverrideMetadata(AnchorablePaneTitle::typeid, &tempVar2);
    //FrameworkPropertyMetadata tempVar3(AnchorablePaneTitle::typeid);
    //DefaultStyleKeyProperty::OverrideMetadata(AnchorablePaneTitle::typeid, &tempVar3);
}

AnchorablePaneTitle::AnchorablePaneTitle()
{
}

void AnchorablePaneTitle::_OnModelChanged(IDependencyObject * prop, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<AnchorablePaneTitle*>(e.Sender))->OnModelChanged(e);
}

void AnchorablePaneTitle::OnModelChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (Model != nullptr)
    {
        SetLayoutItem(Model->Root->GetManager()->GetLayoutItemFromModel(Model.get())->shared_from_base_static<UI::LayoutItem>());
    }
    else
    {
        SetLayoutItem(nullptr);
    }
}

void AnchorablePaneTitle::OnHide()
{
    Model->Hide();
}

void AnchorablePaneTitle::OnToggleAutoHide()
{
    Model->ToggleAutoHide();
}

void AnchorablePaneTitle::OnPointerMoved(Interfaces::PointerEventArgs& e)
{
    if (e.InputModifiers & (Interfaces::InputModifiers::LeftMouseButton | Interfaces::InputModifiers::RightMouseButton))
    {
        _isMouseDown = false;
    }

    Control::OnPointerMoved(e);
}

void AnchorablePaneTitle::OnPointerLeave(Interfaces::PointerEventArgs& e)
{
    Control::OnPointerLeave(e);

    if (_isMouseDown && e.InputModifiers & (Interfaces::InputModifiers::LeftMouseButton | Interfaces::InputModifiers::RightMouseButton))
    {
        auto pane = static_cast<LayoutAnchorablePaneControl*>(VisitVisualAncestors([](Controls::Visual* v) -> bool { return v->Is<LayoutAnchorablePaneControl>(); }));
        if (pane != nullptr)
        {
            auto paneModel = pane->Model->Static_As<LayoutAnchorablePane>();
            auto manager = paneModel->Root->GetManager();

            manager->StartDraggingFloatingWindowForPane(paneModel);
        }
    }

    _isMouseDown = false;
}

void AnchorablePaneTitle::OnPointerPressed(Interfaces::PointerPressedEventArgs& e)
{
    Control::OnPointerPressed(e);

    if (!e.Handled)
    {
        bool attachFloatingWindow = false;
        auto parentFloatingWindow = LayoutElement::FindParent<LayoutAnchorableFloatingWindow>(Model.get());
        if (parentFloatingWindow != nullptr)
        {
            attachFloatingWindow = LayoutContainer::ContainsChild(parentFloatingWindow, [](LayoutElement* x) { return x->Is<LayoutAnchorablePane>(); });
        }

        if (attachFloatingWindow)
        {
            //the pane is hosted inside a floating window that contains only an anchorable pane so drag the floating window itself
            LayoutFloatingWindowControl* floatingWndControl = nullptr;
            for (auto const& wnd : Model->Root->GetManager()->GetFloatingWindows())
            {
                if (wnd->GetModel().get() == parentFloatingWindow)
                {
                    floatingWndControl = wnd.get();
                    break;
                }
            }
            floatingWndControl->AttachDrag(false);
        }
        else
        {
            _isMouseDown = true; //normal drag
        }
    }
}

void AnchorablePaneTitle::OnPointerReleased(Interfaces::PointerReleasedEventArgs&  e)
{
    _isMouseDown = false;
    Control::OnPointerReleased(e);

    if (Model != nullptr)
    {
        Model->IsActive = true; //FocusElementManager.SetFocusOnLastElement(Model);
    }
}
