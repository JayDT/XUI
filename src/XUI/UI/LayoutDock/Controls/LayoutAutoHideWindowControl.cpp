#include "LayoutAutoHideWindowControl.h"
#include "LayoutAnchorable.h"
#include "LayoutAnchorControl.h"
#include "LayoutAnchorableControl.h"
#include "LayoutGridResizerControl.h"
#include "LayoutContainer.h"
#include "LayoutAnchorSide.h"
#include "LayoutRoot.h"
#include "DockingManager.h"
#include "UI/Canvas.h"
#include "XamlCPP/DataBind/Binding.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutAutoHideWindowControl, XUI::UI::LayoutAutoHideWindowControl::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty LayoutAutoHideWindowControl::AnchorableStyleProperty;

void XUI::UI::LayoutAutoHideWindowControl::StaticClassInitializer()
{
    //FrameworkPropertyMetadata tempVar(LayoutAutoHideWindowControl::typeid);
    //DefaultStyleKeyProperty::OverrideMetadata(LayoutAutoHideWindowControl::typeid, &tempVar);
    //FrameworkPropertyMetadata tempVar2(true);
    //UIElement::FocusableProperty::OverrideMetadata(LayoutAutoHideWindowControl::typeid, &tempVar2);
    //FrameworkPropertyMetadata tempVar3(true);
    //Control->IsTabStopProperty.OverrideMetadata(LayoutAutoHideWindowControl::typeid, &tempVar3);
    //FrameworkPropertyMetadata tempVar4(Visibility::Hidden);
    //VisibilityProperty::OverrideMetadata(LayoutAutoHideWindowControl::typeid, &tempVar4);
    IsVisibleProperty->OverrideDefaultValue<LayoutAutoHideWindowControl>(false);
}

LayoutAutoHideWindowControl::LayoutAutoHideWindowControl()
{
}

void LayoutAutoHideWindowControl::Show(LayoutAnchorControl *anchor)
{
    if (Model != nullptr)
        throw System::InvalidOperationException();

    _anchor = anchor->shared_from_base_static<LayoutAnchorControl>();
    Model = anchor->GetModel()->shared_from_base_dynamic<LayoutAnchorable>();
    _side = (anchor->GetModel()->Parent->Parent->Static_As<LayoutAnchorSide>())->Side;
    _manager = Model->Root->GetManager();
    CreateInternalGrid();

    Model->PropertyChanged += System::make_eventhandler(&LayoutAutoHideWindowControl::_model_PropertyChanged, this);

    IsVisible = true;
    InvalidateMeasure();
    //UpdateWindowPos();
}

void LayoutAutoHideWindowControl::Hide()
{
    if (Model == nullptr)
    {
        return;
    }

    Model->PropertyChanged -= System::make_eventhandler(&LayoutAutoHideWindowControl::_model_PropertyChanged, this);

    RemoveInternalGrid();
    _anchor = nullptr;
    Model = nullptr;
    _manager = nullptr;
    IsVisible = false;
}

LayoutElement * XUI::UI::LayoutAutoHideWindowControl::GetModel()
{
    return Model.get();
}

Interfaces::PlatformHandle LayoutAutoHideWindowControl::BuildWindowCore(Interfaces::PlatformHandle const& hwndParent)
{
    //parentWindowHandle = hwndParent.Handle;
    //_internalHwndSource = new HwndSource(new HwndSourceParameters();
    //_internalHwndSource->ParentWindow = hwndParent.Handle;
    //_internalHwndSource->WindowStyle = Win32Helper::WS_CHILD | Win32Helper::WS_VISIBLE | Win32Helper::WS_CLIPSIBLINGS | Win32Helper::WS_CLIPCHILDREN;
    //_internalHwndSource->Width = 0;
    //_internalHwndSource->Height = 0;
    //
    ////_internalHost_ContentRendered = false;
    //_internalHwndSource->ContentRendered += _internalHwndSource_ContentRendered;
    //_internalHwndSource->RootVisual = _internalHostPresenter;
    //AddLogicalChild(_internalHostPresenter);
    //Win32Helper::BringWindowToTop(_internalHwndSource->Handle);
    //return HandleRef(this, _internalHwndSource->Handle);
    return Interfaces::PlatformHandle();
}

void LayoutAutoHideWindowControl::_internalHwndSource_ContentRendered(void* sender, System::Events::EventArg& e)
{
    //    _internalHost_ContentRendered = true;
}

void LayoutAutoHideWindowControl::DestroyWindowCore(Interfaces::PlatformHandle const& hwnd)
{
    //if (_internalHwndSource != nullptr)
    //{
    //    _internalHwndSource->ContentRendered -= _internalHwndSource_ContentRendered;
    //    delete _internalHwndSource;
    //    _internalHwndSource = nullptr;
    //}
}

void LayoutAutoHideWindowControl::OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e)
{
    Controls::Control::OnTemplateApplied(e);


}

void LayoutAutoHideWindowControl::_model_PropertyChanged(void* sender, XamlCPP::Core::PropertyChangedEventArgs& e)
{
    if (e.PropertyName == "IsAutoHidden")
    {
        if (!Model->IsAutoHidden())
        {
            _manager->HideAutoHideWindow(_anchor.get());
        }
    }
}

void LayoutAutoHideWindowControl::CreateInternalGrid()
{
    _internalGrid = std::make_shared<Grid>();
    //_internalGrid->FlowDirection = Controls::FlowDirection::LeftToRight;

    std::shared_ptr<XamlCPP::DataBind::Binding> bindDataContext = std::make_shared<XamlCPP::DataBind::Binding>("Background", XUI::Core::Dependency::BindingMode::OneWay);
    bindDataContext->Source = this;
    _viewBindings.add(XUI::Core::Dependency::DependencyObject::Bind(_internalGrid.get(), Grid::BackgroundProperty.get(), bindDataContext.get(), nullptr));

    _internalHost = std::make_shared<LayoutAnchorableControl>();
    _internalHost->Model = Model;
    //_internalHost->Style = AnchorableStyle;

    //KeyboardNavigation::SetTabNavigation(_internalGrid, KeyboardNavigationMode::Cycle);

    _resizer = std::make_shared<LayoutGridResizerControl>();

    _resizer->DragStarted += System::make_eventhandler(&LayoutAutoHideWindowControl::OnResizerDragStarted, this);
    _resizer->DragDelta += System::make_eventhandler(&LayoutAutoHideWindowControl::OnResizerDragDelta, this);
    _resizer->DragCompleted += System::make_eventhandler(&LayoutAutoHideWindowControl::OnResizerDragCompleted, this);

    if (_side == AnchorSide::Right)
    {
        auto col = std::make_shared<ColumnDefinition>();
        col->Width = GridLength(_manager->GridSplitterWidth);
        _internalGrid->ColumnDefinitions.Add(col);
        col = std::make_shared<ColumnDefinition>();
        col->Width = Model->AutoHideWidth == 0.0 ? GridLength(Model->AutoHideMinWidth) : GridLength(Model->AutoHideWidth, GridUnitType::Pixel);
        _internalGrid->ColumnDefinitions.Add(col);

        Grid::SetColumn(_resizer.get(), 0);
        Grid::SetColumn(_internalHost.get(), 1);

        //_resizer->Cursor = Cursors::SizeWE;

        HorizontalAlignment = Controls::HorizontalAlignment::Right;
        VerticalAlignment = Controls::VerticalAlignment::Stretch;
    }
    else if (_side == AnchorSide::Left)
    {
        auto col = std::make_shared<ColumnDefinition>();
        col->Width = Model->AutoHideWidth == 0.0 ? GridLength(Model->AutoHideMinWidth) : GridLength(Model->AutoHideWidth, GridUnitType::Pixel);
        _internalGrid->ColumnDefinitions.Add(col);
        col = std::make_shared<ColumnDefinition>();
        col->Width = GridLength(_manager->GridSplitterWidth);
        _internalGrid->ColumnDefinitions.Add(col);

        Grid::SetColumn(_internalHost.get(), 0);
        Grid::SetColumn(_resizer.get(), 1);

        //_resizer->Cursor = Cursors::SizeWE;

        HorizontalAlignment = Controls::HorizontalAlignment::Left;
        VerticalAlignment = Controls::VerticalAlignment::Stretch;
    }
    else if (_side == AnchorSide::Top)
    {
        auto row = std::make_shared<RowDefinition>();
        row->Height = Model->AutoHideHeight == 0.0 ? GridLength(Model->AutoHideMinHeight) : GridLength(Model->AutoHideHeight, GridUnitType::Pixel);
        _internalGrid->RowDefinitions.Add(row);
        row = std::make_shared<RowDefinition>();
        row->Height = GridLength(_manager->GridSplitterHeight);
        _internalGrid->RowDefinitions.Add(row);

        Grid::SetRow(_internalHost.get(), 0);
        Grid::SetRow(_resizer.get(), 1);

        //_resizer->Cursor = Cursors::SizeNS;

        VerticalAlignment = Controls::VerticalAlignment::Top;
        HorizontalAlignment = Controls::HorizontalAlignment::Stretch;

    }
    else if (_side == AnchorSide::Bottom)
    {
        auto row = std::make_shared<RowDefinition>();
        row->Height = GridLength(_manager->GridSplitterHeight);
        _internalGrid->RowDefinitions.Add(row);
        row = std::make_shared<RowDefinition>();
        row->Height = Model->AutoHideHeight == 0.0 ? GridLength(Model->AutoHideMinHeight) : GridLength(Model->AutoHideHeight, GridUnitType::Pixel);
        _internalGrid->RowDefinitions.Add(row);

        Grid::SetRow(_resizer.get(), 0);
        Grid::SetRow(_internalHost.get(), 1);

        //_resizer->Cursor = Cursors::SizeNS;

        VerticalAlignment = Controls::VerticalAlignment::Bottom;
        HorizontalAlignment = Controls::HorizontalAlignment::Stretch;
    }


    _internalGrid->Children.Add(_resizer);
    _internalGrid->Children.Add(_internalHost);
    _internalHostPresenter->Content = _internalGrid;
}

void LayoutAutoHideWindowControl::RemoveInternalGrid()
{
    _resizer->DragStarted -= System::make_eventhandler(&LayoutAutoHideWindowControl::OnResizerDragStarted, this);
    _resizer->DragDelta -= System::make_eventhandler(&LayoutAutoHideWindowControl::OnResizerDragDelta, this);
    _resizer->DragCompleted -= System::make_eventhandler(&LayoutAutoHideWindowControl::OnResizerDragCompleted, this);

    _internalHostPresenter->Content = nullptr;
}

bool LayoutAutoHideWindowControl::HasFocusWithinCore()
{
    return false;
}

void LayoutAutoHideWindowControl::ShowResizerOverlayWindow(LayoutGridResizerControl *splitter)
{
    _resizerGhost = std::make_shared<Border>();
    _resizerGhost->Background = splitter->BackgroundWhileDragging;
    _resizerGhost->Opacity = splitter->OpacityWhileDragging;

    auto areaElement = _manager->GetAutoHideAreaElement();
    auto modelControlActualSize = _internalHost->DesiredSize;

    auto ptTopLeftScreen = areaElement->Bounds.GetPosition();

    auto managerSize = areaElement->DesiredSize;

    Core::Media::Size windowSize;

    if (_side == AnchorSide::Right || _side == AnchorSide::Left)
    {
        windowSize = Core::Media::Size(managerSize.Width - 25.0 + splitter->ActualWidth(), managerSize.Height);

        _resizerGhost->Width = splitter->ActualWidth();
        _resizerGhost->Height = windowSize.Height;
        ptTopLeftScreen.x += 25.0;
    }
    else
    {
        windowSize = Core::Media::Size(managerSize.Width, managerSize.Height - Model->AutoHideMinHeight - 25.0 + splitter->ActualHeight());

        _resizerGhost->Height = splitter->ActualHeight();
        _resizerGhost->Width = windowSize.Width;
        ptTopLeftScreen.y += 25.0;
    }

    _initialStartPoint = splitter->Bounds.GetPosition() - ptTopLeftScreen;

    if (_side == AnchorSide::Right || _side == AnchorSide::Left)
    {
        Canvas::SetLeft(_resizerGhost.get(), _initialStartPoint.x);
    }
    else
    {
        Canvas::SetTop(_resizerGhost.get(), _initialStartPoint.y);
    }

    Canvas *panelHostResizer = new Canvas();
    panelHostResizer->HorizontalAlignment = Controls::HorizontalAlignment::Stretch;
    panelHostResizer->VerticalAlignment = Controls::VerticalAlignment::Stretch;

    panelHostResizer->Children.Add(_resizerGhost);


    _resizerWindowHost = std::make_shared<Window>();
    //_resizerWindowHost->ResizeMode = ResizeMode::NoResize;
    _resizerWindowHost->WindowStyle = WindowStyle::None;
    //_resizerWindowHost->ShowInTaskbar = false;
    //_resizerWindowHost->AllowsTransparency = true;
    _resizerWindowHost->Background = nullptr;
    _resizerWindowHost->Width = windowSize.Width;
    _resizerWindowHost->Height = windowSize.Height;
    _resizerWindowHost->Left = ptTopLeftScreen.x;
    _resizerWindowHost->Top = ptTopLeftScreen.y;
    //_resizerWindowHost->ShowActivated = false;
    //_resizerWindowHost->Owner = Window::GetWindow(this);
    _resizerWindowHost->Content = panelHostResizer;

    _resizerWindowHost->Show();
}

void LayoutAutoHideWindowControl::HideResizerOverlayWindow()
{
    if (_resizerWindowHost != nullptr)
    {
        _resizerWindowHost->Close();
        _resizerWindowHost = nullptr;
    }
}

void LayoutAutoHideWindowControl::OnResizerDragCompleted(void* sender, Interfaces::VectorEventArgs& e)
{
    LayoutGridResizerControl *splitter = GENERIC_SAFE_CAST(LayoutGridResizerControl*, sender);
    //auto transformedDelta = e;

    double delta;
    if (_side == AnchorSide::Right || _side == AnchorSide::Left)
    {
        delta = Canvas::GetLeft(_resizerGhost.get()) - _initialStartPoint.x;
    }
    else
    {
        delta = Canvas::GetTop(_resizerGhost.get()) - _initialStartPoint.y;
    }

    if (_side == AnchorSide::Right)
    {
        if (Model->AutoHideWidth == 0.0)
        {
            Model->AutoHideWidth = _internalHost->ActualWidth() - delta;
        }
        else
        {
            Model->SetAutoHideWidth(Model->AutoHideWidth - delta);
        }

        _internalGrid->ColumnDefinitions[1]->Width = GridLength(Model->AutoHideWidth, GridUnitType::Pixel);
    }
    else if (_side == AnchorSide::Left)
    {
        if (Model->AutoHideWidth == 0.0)
        {
            Model->AutoHideWidth = _internalHost->ActualWidth() + delta;
        }
        else
        {
            Model->SetAutoHideWidth(Model->AutoHideWidth + delta);
        }

        _internalGrid->ColumnDefinitions[0]->Width = GridLength(Model->AutoHideWidth, GridUnitType::Pixel);
    }
    else if (_side == AnchorSide::Top)
    {
        if (Model->AutoHideHeight == 0.0)
        {
            Model->AutoHideHeight = _internalHost->ActualHeight() + delta;
        }
        else
        {
            Model->SetAutoHideHeight(Model->AutoHideHeight + delta);
        }

        _internalGrid->RowDefinitions[0]->Height = GridLength(Model->AutoHideHeight, GridUnitType::Pixel);
    }
    else if (_side == AnchorSide::Bottom)
    {
        if (Model->AutoHideHeight == 0.0)
        {
            Model->AutoHideHeight = _internalHost->ActualHeight() - delta;
        }
        else
        {
            Model->SetAutoHideHeight(Model->AutoHideHeight - delta);
        }

        _internalGrid->RowDefinitions[1]->Height = GridLength(Model->AutoHideHeight, GridUnitType::Pixel);
    }

    HideResizerOverlayWindow();

    m_IsResizing = false;
    InvalidateMeasure();
}

void LayoutAutoHideWindowControl::OnResizerDragDelta(void* sender, Interfaces::VectorEventArgs& e)
{
    LayoutGridResizerControl *splitter = GENERIC_SAFE_CAST(LayoutGridResizerControl*, sender);
    auto transformedDelta = e.Vector;

    if (_side == AnchorSide::Right || _side == AnchorSide::Left)
    {
        //if (FrameworkElement::GetFlowDirection(_internalHost) == Controls::FlowDirection::RightToLeft)
        //{
        //    transformedDelta->X = -transformedDelta->X;
        //}
        Canvas::SetLeft(_resizerGhost.get(), std::min((double)std::max(_initialStartPoint.x + transformedDelta.x, 0.0f), _resizerWindowHost->Width - _resizerGhost->Width));
    }
    else
    {
        Canvas::SetTop(_resizerGhost.get(), std::min((double)std::max(_initialStartPoint.y + transformedDelta.y, 0.0f), _resizerWindowHost->Height - _resizerGhost->Height));
    }
}

void LayoutAutoHideWindowControl::OnResizerDragStarted(void* sender, Interfaces::VectorEventArgs& e)
{
    LayoutGridResizerControl *resizer = GENERIC_SAFE_CAST(LayoutGridResizerControl*, sender);
    ShowResizerOverlayWindow(resizer);
    m_IsResizing = true;
}

Core::Media::Size LayoutAutoHideWindowControl::MeasureOverride(Core::Media::Size const& constraint)
{
    if (_internalHostPresenter == nullptr)
    {
        return Controls::Control::MeasureOverride(constraint);
    }

    _internalHostPresenter->Measure(constraint);
    //return base.MeasureOverride(constraint);
    return _internalHostPresenter->DesiredSize;
}

Core::Media::Size LayoutAutoHideWindowControl::ArrangeOverride(Core::Media::Size const& finalSize)
{
    if (_internalHostPresenter == nullptr)
    {
        return Controls::Control::ArrangeOverride(finalSize);
    }

    _internalHostPresenter->Arrange(finalSize);
    return Controls::Control::ArrangeOverride(finalSize); // new Size(_internalHostPresenter.ActualWidth, _internalHostPresenter.ActualHeight);
}

//bool LayoutAutoHideWindowControl::GetIsWin32MouseOver() const
//{
//    auto ptMouse = new Win32Helper::Win32Point();
//    if (!Win32Helper::GetCursorPos(ptMouse))
//    {
//        return false;
//    }
//
//    Point tempVar();
//    Point *location = this->PointToScreenDPI(&tempVar);
//
//    Rect *rectWindow = this->GetScreenArea();
//    Point tempVar2(ptMouse->X, ptMouse->Y);
//    if (rectWindow->Contains(&tempVar2))
//    {
//        return true;
//    }
//
//    auto manager = Model->Root.Manager;
//    auto anchor = manager->FindVisualChildren<LayoutAnchorControl*>().Where([&](System::Reflection::Variant c)
//    {
//        return c->Model == Model;
//    })->FirstOrDefault();
//
//    if (anchor == nullptr)
//    {
//        return false;
//    }
//
//    Point tempVar3();
//    location = anchor->PointToScreenDPI(&tempVar3);
//
//    if (anchor->IsMouseOver)
//    {
//        return true;
//    }
//
//    return false;
//}
