#include "LayoutDocumentTabItem.h"
#include "DocumentPaneTabPanel.h"
#include "LayoutContent.h"
#include "LayoutRoot.h"
#include "LayoutDocumentPane.h"
#include "LayoutDocumentPaneGroup.h"
#include "DockingManager.h"
#include "UI/TabItem.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutDocumentTabItem, XUI::UI::LayoutDocumentTabItem::StaticClassInitializer);

XUI::Core::Dependency::RefDependencyProperty LayoutDocumentTabItem::ModelProperty;
XUI::Core::Dependency::RefDependencyProperty LayoutDocumentTabItem::LayoutItemProperty;

void LayoutDocumentTabItem::StaticClassInitializer()
{
    ModelProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::LayoutContent>>::Register<SelectingItemsControl>(nameof(Model)));;
    LayoutItemProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::LayoutItem>>::Register<SelectingItemsControl>(nameof(LayoutItem)));;

    ModelProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        LayoutDocumentTabItem* control = e.Sender->Static_As<LayoutDocumentTabItem>();
        if (control)
        {
            UI::LayoutContent* content = static_cast<UI::LayoutContent*>(e.NewValue.ToObject());
            control->Header = content->Title;
        }
        static_cast<UI::Controls::Control*>(e.Sender)->OnTemplateChanged(e);
    });
}

LayoutDocumentTabItem::LayoutDocumentTabItem()
{
}

void LayoutDocumentTabItem::OnModelChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs &e)
{
    (static_cast<LayoutDocumentTabItem*>(d))->OnModelChanged(e);
}

void LayoutDocumentTabItem::OnModelChanged(Core::Dependency::DependencyPropertyChangedEventArgs &e)
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

void LayoutDocumentTabItem::UpdateDragDetails()
{
    _parentDocumentTabPanel = FindLogicalAncestor<DocumentPaneTabPanel>()->shared_from_base_static<DocumentPaneTabPanel>();
}

void LayoutDocumentTabItem::OnPointerPressed(Interfaces::PointerPressedEventArgs& e)
{
    TabStripItem::OnPointerPressed(e);

    if (e.MouseButton == Interfaces::MouseButton::Left)
    {
        Model->IsActive = true;
    
        if (e.ClickCount == 1)
        {
            _mouseDownPoint = e.GetPosition(this);
            _isMouseDown = true;
        }
    }
    
    if (e.MouseButton == Interfaces::MouseButton::Middle)
    {
        if (LayoutItem->CloseCommand->CanExecute(nullptr, this))
        {
            LayoutItem->CloseCommand->Execute(nullptr, this);
        }
    }
}

void LayoutDocumentTabItem::OnPointerMoved(Interfaces::PointerEventArgs& e)
{
    TabStripItem::OnPointerMoved(e);

    if (_isMouseDown)
    {
        Core::Media::Point ptMouseMove = e.GetPosition(this);
    
        if (std::abs(ptMouseMove.x - _mouseDownPoint.x) > 2.0 || std::abs(ptMouseMove.y - _mouseDownPoint.y) > 2.0)
        {
            UpdateDragDetails();
            e.Device->Capture(this);
            _isMouseDown = false;
        }
    }
    
    if (e.Device->Captured == this && _parentDocumentTabPanel)
    {
        auto mousePosInScreenCoord = e.GetPosition(VisualRoot->ToVisual()); // this->PointToScreenDPI(
        auto parentRect = _parentDocumentTabPanel->GetTransformedBounds();
        if (!parentRect.contains(mousePosInScreenCoord))
        {
            e.Device->Capture(nullptr);
            auto manager = Model->Root->GetManager();
            manager->StartDraggingFloatingWindowForContent(Model.get());
        }
        else
        {
            Controls::Control* tabItem = nullptr;
            int indexOfTabItemWithMouseOver = -1;
            for (auto const& child : _parentDocumentTabPanel->GetItemContainerGenerator()->GetContainers()) //for (int i = 0; i < _otherTabsScreenArea.size(); ++i)
            {
                tabItem = child.second.ContainerControl.get();
                if (tabItem->GetTransformedBounds().contains(mousePosInScreenCoord))
                {
                    indexOfTabItemWithMouseOver = child.second.Index;
                    break;
                }
            }
    
            if (indexOfTabItemWithMouseOver >= 0)
            {
                LayoutContent* targetModel = static_cast<LayoutDocumentTabItem*>(tabItem)->Model.get();
                auto container = Model->Parent;
                auto containerPane = Model->Parent;
    
                if (containerPane->Is<LayoutDocumentPane>() && !static_cast<LayoutDocumentPane*>(containerPane)->CanRepositionItems)
                {
                    return;
                }
                if ((containerPane->Parent != nullptr) && containerPane->Parent->Is<LayoutDocumentPaneGroup>() && !(static_cast<LayoutDocumentPaneGroup*>(containerPane->Parent))->CanRepositionItems)
                {
                    return;
                }
    
                containerPane->MoveChild(container->Children.IndexOf(Model), container->Children.IndexOf(targetModel->shared_from_base_static<LayoutElement>()));
                Model->IsActive = true;
                _parentDocumentTabPanel->InvalidateArrange();
                //UpdateDragDetails();
            }
        }
    }

}

void LayoutDocumentTabItem::OnPointerReleased(Interfaces::PointerReleasedEventArgs& e)
{
    if (e.Device->Captured == this)
    {
        e.Device->Capture(nullptr);
    }

    _parentDocumentTabPanel = nullptr;
    _isMouseDown = false;

    Control::OnPointerReleased(e);
}

void LayoutDocumentTabItem::OnPointerLeave(Interfaces::PointerEventArgs& e)
{
    Control::OnPointerLeave(e);

    _isMouseDown = false;
}

void LayoutDocumentTabItem::OnPointerEnter(Interfaces::PointerEventArgs& e)
{
    Control::OnPointerEnter(e);
    _isMouseDown = false;
}

