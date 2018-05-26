#include "LayoutAnchorableFloatingWindowControl.h"
#include "LayoutAnchorableItem.h"
#include "Core/Observer/RoutedCommand.h"
#include "LayoutDocument.h"
#include "LayoutRoot.h"
#include "LayoutDocumentFloatingWindow.h"
#include "LayoutAnchorablePaneControl.h"
#include "LayoutDocumentPaneControl.h"
#include "LayoutDock/Controls/DropArea.h"
#include "OverlayWindow.h"
#include "DockingManager.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutAnchorableFloatingWindowControl, XUI::UI::LayoutAnchorableFloatingWindowControl::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty LayoutAnchorableFloatingWindowControl::SingleContentLayoutItemProperty;

void LayoutAnchorableFloatingWindowControl::StaticClassInitializer()
{
    //FrameworkPropertyMetadata tempVar(LayoutAnchorableFloatingWindowControl::typeid);
    //DefaultStyleKeyProperty::OverrideMetadata(LayoutAnchorableFloatingWindowControl::typeid, &tempVar);
}

LayoutAnchorableFloatingWindowControl::LayoutAnchorableFloatingWindowControl(LayoutAnchorableFloatingWindow *model)
    : LayoutFloatingWindowControl(model)
{
    HideWindowCommand = std::make_shared<Core::Observer::EventHandleCommand>(System::make_shared_eventhandler([this](void*, Core::Observer::ExecutedRoutedEventArgs& e) { OnExecuteHideWindowCommand(e.Parameter); }), System::make_shared_eventhandler([this](void*, Core::Observer::CanExecuteRoutedEventArgs& e) { e.CanExecute = CanExecuteHideWindowCommand(e.Parameter); }));
    //UpdateThemeResources();
}

void XUI::UI::LayoutAnchorableFloatingWindowControl::OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e)
{
    LayoutFloatingWindowControl::OnTemplateApplied(e);

    _overlayWindow = Template->FindNameByManagedType<OverlayWindow>("PART_OverlayWindow", this);
}

void LayoutAnchorableFloatingWindowControl::OnSingleContentLayoutItemChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    (static_cast<LayoutAnchorableFloatingWindowControl*>(d))->OnSingleContentLayoutItemChanged(e);
}

void LayoutAnchorableFloatingWindowControl::OnSingleContentLayoutItemChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
}

void LayoutAnchorableFloatingWindowControl::OnWindowInitialized()
{
    LayoutFloatingWindowControl::OnInitialized();

    auto manager = Model->Root->GetManager();

    Content = manager->CreateUIElementForModel(static_cast<LayoutAnchorableFloatingWindow*>(Model.get())->RootPanel.get());

    //SetBinding(VisibilityProperty, new Binding("IsVisible") { Source = _model, Converter = new BoolToVisibilityConverter(), Mode = BindingMode.OneWay, ConverterParameter = Visibility.Hidden });

    //Issue: http://avalondock.codeplex.com/workitem/15036
    //IsVisibleChanged += [&](s, args)
    //{
    //    auto visibilityBinding = GetBindingExpression(VisibilityProperty);
    //    if (IsVisible && (visibilityBinding == nullptr))
    //    {
    //        Binding *tempVar = new Binding("IsVisible");
    //        tempVar->Source = _model;
    //        tempVar->Converter = new BoolToVisibilityConverter();
    //        tempVar->Mode = BindingMode::OneWay;
    //        tempVar->ConverterParameter = Visibility::Hidden;
    //        SetBinding(VisibilityProperty, tempVar);
    //    }
    //};

    //Binding *tempVar2 = new Binding("Model.SinglePane.SelectedContent");
    //tempVar2->Source = this;
    //tempVar2->Converter = new LayoutItemFromLayoutModelConverter();
    //SetBinding(SingleContentLayoutItemProperty, tempVar2);
    
    Model->PropertyChanged += System::make_eventhandler(&LayoutAnchorableFloatingWindowControl::_model_PropertyChanged, this);
}

void LayoutAnchorableFloatingWindowControl::_model_PropertyChanged(void* sender, XamlCPP::Core::PropertyChangedEventArgs& e)
{
    if (!strncmp(e.PropertyName, "RootPanel", 9) && static_cast<LayoutAnchorableFloatingWindow*>(Model.get())->RootPanel == nullptr)
    {
        InternalClose();
    }
}

bool LayoutAnchorableFloatingWindowControl::HitTest(Core::Media::Point const& dragPoint)
{
    //Point tempVar();
    Core::Media::Rect detectionRect = TransformedBounds; //new Rect(this->PointToScreenDPIWithoutFlowDirection(&tempVar), this->TransformActualSizeToAncestor());
    return detectionRect.contains(dragPoint);
}

DockingManager *LayoutAnchorableFloatingWindowControl::GetManager()
{
    return Model->Root->GetManager();
}

void LayoutAnchorableFloatingWindowControl::CreateOverlayWindow()
{
    if (!_overlayWindow)
        return;

    Core::Media::Rect rectWindow = TransformedBounds;
    _overlayWindow->Width = rectWindow.Width;
    _overlayWindow->Height = rectWindow.Height;
}

UI::OverlayWindow *LayoutAnchorableFloatingWindowControl::ShowOverlayWindow(LayoutFloatingWindowControl *draggingWindow)
{
    if (draggingWindow)
    {
        CreateOverlayWindow();
        if (!_overlayWindow->IsVisible)
        {
            _overlayWindow->EnableDropTargets();
            _overlayWindow->IsVisible = true;
        }
    }
    return _overlayWindow.get();
}

void LayoutAnchorableFloatingWindowControl::HideOverlayWindow()
{
    if (_overlayWindow)
    {
        _dropAreas.clear();
        _overlayWindow->HideDropTargets();
        _overlayWindow->IsVisible = false;
        _overlayWindow->SetFloatingWindow(nullptr);
    }
}

std::list<std::shared_ptr<IDropArea>> const& LayoutAnchorableFloatingWindowControl::GetDropAreas(LayoutFloatingWindowControl *draggingWindow)
{
    if (_dropAreas.size() > 0)
        return _dropAreas;
    
    if (draggingWindow->Is<LayoutDocumentFloatingWindow>() && draggingWindow->Model != nullptr)
        return _dropAreas;
    
    auto rootVisual = this; // VisualRoot->ToVisual(); //(dynamic_cast<FloatingWindowContentHost*>(Content))->

    rootVisual->VisitVisualDescendents([&](Controls::Visual* visual)
    {
        LayoutAnchorablePaneControl* areaHost = visual->Static_As<LayoutAnchorablePaneControl>();
        if (areaHost)
        {
            auto dropArea = std::make_shared<DropArea<LayoutAnchorablePaneControl>>(areaHost->shared_from_base_static<LayoutAnchorablePaneControl>(), DropAreaType::AnchorablePane);
            _dropAreas.push_back(dropArea);
        }
        return false;
    });

    rootVisual->VisitVisualDescendents([&](Controls::Visual* visual)
    {
        LayoutDocumentPaneControl* areaHost = visual->Static_As<LayoutDocumentPaneControl>();
        if (areaHost)
        {
            auto dropArea = std::make_shared<DropArea<LayoutAnchorablePaneControl>>(areaHost->shared_from_base_static<LayoutAnchorablePaneControl>(), DropAreaType::DocumentPane);
            _dropAreas.push_back(dropArea);
        }
        return false;
    });

    return _dropAreas;
}

void LayoutAnchorableFloatingWindowControl::HandleClosed()
{
    auto root = Model->Root;
    root->GetManager()->RemoveFloatingWindow(this);
    root->CollectGarbage();
    //if (_overlayWindow != nullptr)
    //{
    //    _overlayWindow->Close();
    //    _overlayWindow = nullptr;
    //}

    LayoutFloatingWindowControl::HandleClosed();

    if (!GetCloseInitiatedByUser())
    {
        root->FloatingWindows.Remove(Model->shared_from_base_static<LayoutAnchorableFloatingWindow>());
    }

    Model->PropertyChanged -= System::make_eventhandler(&LayoutAnchorableFloatingWindowControl::_model_PropertyChanged, this);
}

//void LayoutAnchorableFloatingWindowControl::OnClosing(CancelEventArgs& e)
//{
//    if (CloseInitiatedByUser && !KeepContentVisibleOnClose)
//    {
//        e->Cancel = true;
//        _model->Descendents().OfType<LayoutAnchorable*>()->ToArray()->ForEach<LayoutAnchorable*>([&](a)
//        {
//            a->Hide();
//        });
//    }
//
//    LayoutFloatingWindowControl::OnClosing(e);
//}

bool LayoutAnchorableFloatingWindowControl::OpenContextMenu()
{
    //auto ctxMenu = _model->Root.Manager.AnchorableContextMenu;
    //if (ctxMenu != nullptr && SingleContentLayoutItem != nullptr)
    //{
    //    ctxMenu->PlacementTarget = nullptr;
    //    ctxMenu->Placement = PlacementMode::MousePoint;
    //    ctxMenu->DataContext = SingleContentLayoutItem;
    //    ctxMenu->IsOpen = true;
    //    return true;
    //}

    return false;
}

bool LayoutAnchorableFloatingWindowControl::IsContextMenuOpen()
{
    //auto ctxMenu = _model->Root.Manager.AnchorableContextMenu;
    //if (ctxMenu != nullptr && SingleContentLayoutItem != nullptr)
    //{
    //    return ctxMenu->IsOpen;
    //}

    return false;
}

bool LayoutAnchorableFloatingWindowControl::CanExecuteHideWindowCommand(System::Reflection::Variant parameter)
{
    if (Model == nullptr)
    {
        return false;
    }

    auto root = Model->Root;
    if (root == nullptr)
    {
        return false;
    }

    auto manager = root->GetManager();
    if (manager == nullptr)
    {
        return false;
    }

    bool canExecute = false;
    //for (auto anchorable : _model->Descendents().OfType<LayoutAnchorable*>()->ToArray())
    //{
    //    if (!anchorable->CanHide)
    //    {
    //        canExecute = false;
    //        break;
    //    }
    //
    //    auto anchorableLayoutItem = dynamic_cast<LayoutAnchorableItem*>(manager->GetLayoutItemFromModel(anchorable));
    //    if (anchorableLayoutItem == nullptr || anchorableLayoutItem->HideCommand == nullptr || !anchorableLayoutItem->HideCommand->CanExecute(parameter))
    //    {
    //        canExecute = false;
    //        break;
    //    }
    //
    //    canExecute = true;
    //}

    return canExecute;
}

void LayoutAnchorableFloatingWindowControl::OnExecuteHideWindowCommand(System::Reflection::Variant parameter)
{
    //auto manager = Model->Root.Manager;
    //for (auto anchorable : this->Model->Descendents().OfType<LayoutAnchorable*>()->ToArray())
    //{
    //    auto anchorableLayoutItem = dynamic_cast<LayoutAnchorableItem*>(manager->GetLayoutItemFromModel(anchorable));
    //    anchorableLayoutItem->HideCommand->Execute(parameter);
    //}
}
