#include "StdAfx.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

XUI::UI::LayoutGridControl::LayoutGridControl(std::shared_ptr<LayoutGroupBase> const & model, UI::Orientation orientation)
    : Model(model)
    , _orientation(orientation)
    , _asyncRefreshCalled(ChildrenTreeChange::None)
{
    Initialized += System::make_eventhandler(&LayoutGridControl::OnControlInitialized, this);
}

XUI::UI::LayoutGridControl::~LayoutGridControl()
{
}

UI::Orientation XUI::UI::LayoutGridControl::Orientation() const
{
    return Model->GetOrientation();
}

LayoutElement * XUI::UI::LayoutGridControl::GetModel()
{
    return Model.get();
}

LayoutElement * XUI::UI::LayoutGridControl::GetLayoutModel()
{
    return Model.get();
}

void XUI::UI::LayoutGridControl::OnControlInitialized(void *, System::Events::EventArg & e)
{
    Model->ChildrenTreeChanged += System::make_eventhandler([this](void*, ChildrenTreeChangedEventArgs& e)
    {
        if (_asyncRefreshCalled != ChildrenTreeChange::None && _asyncRefreshCalled == e.Change)
            return;

        _asyncRefreshCalled = e.Change;
        Core::UIDispatcher::OnRender += [this]()
        {
            _asyncRefreshCalled = ChildrenTreeChange::None;
            this->UpdateChildren();
        };
    });

    LayoutUpdated += System::make_eventhandler(&LayoutGridControl::OnLayoutUpdated, this);
}

void XUI::UI::LayoutGridControl::OnLayoutUpdated(void * sender, System::Events::EventArg & e)
{
    auto modelWithAtcualSize = Model.get();
    modelWithAtcualSize->SetActualWidth(ActualWidth());
    modelWithAtcualSize->SetActualHeight(ActualHeight());

    if (!_initialized)
    {
        _initialized = true;
        UpdateChildren();
    }
}

void XUI::UI::LayoutGridControl::UpdateChildren()
{
    std::list<std::shared_ptr<ILayoutControl>> alreadyContainedChildren;
    for (auto const& child : Children.GetNativeEnumerator())
    {
        auto iLayout = child->shared_from_base_dynamic<ILayoutControl>();
        if (!iLayout)
            continue;

        alreadyContainedChildren.push_back(iLayout);
    }

    DetachOldSplitters();
    DetachPropertChangeHandler();

    Children.Clear();
    ColumnDefinitions.Clear();
    RowDefinitions.Clear();

    if (Model == nullptr ||
        Model->Root == nullptr)
        return;

    auto manager = Model->Root->GetManager();
    if (manager == nullptr)
        return;

    for (auto const& child : Model->Children.GetNativeEnumerator())
    {
        ILayoutControl* foundContainedChild = nullptr;
        for (auto oldChild : alreadyContainedChildren)
        {
            if (oldChild->GetLayoutModel() == child.get())
            {
                foundContainedChild = oldChild.get();
                break;
            }
        }
        if (foundContainedChild)
            Children.Add(dynamic_cast<Controls::Control*>(foundContainedChild)->shared_from_base_static<Controls::Control>());
        else
            Children.Add(manager->CreateUIElementForModel(child.get()));
    }

    CreateSplitters();

    UpdateRowColDefinitions();

    AttachNewSplitters();
    AttachPropertyChangeHandler();
}

void XUI::UI::LayoutGridControl::OnChildModelPropertyChanged(void * sender, XamlCPP::Core::PropertyChangedEventArgs & e)
{
    if (_asyncRefreshCalled != ChildrenTreeChange::None)
        return;

    if (_fixingChildrenDockLengths.CanEnter() && !strcmp(e.PropertyName, "DockWidth") && Orientation() == UI::Orientation::Horizontal)
    {
        if (ColumnDefinitions.size() == Children.size())
        {
            LayoutGroupBase* changedElement = GENERIC_SAFE_CAST(LayoutGroupBase*, sender);
            std::shared_ptr<Controls::Control> childFromModel = nullptr;
            for (auto const& child : Children.GetNativeEnumerator())
            {
                auto iLayout = child->shared_from_base_dynamic<ILayoutControl>();
                if (!iLayout || iLayout->GetLayoutModel() != changedElement)
                    continue;

                childFromModel = child;
                break;
            }
            int indexOfChild = Children.IndexOf(childFromModel);
            ColumnDefinitions[indexOfChild]->Width = changedElement->GetDockWidth();
        }
    }
    else if (_fixingChildrenDockLengths.CanEnter() && !strcmp(e.PropertyName, "DockHeight") && Orientation() == UI::Orientation::Vertical)
    {
        if (RowDefinitions.size() == Children.size())
        {
            LayoutGroupBase* changedElement = GENERIC_SAFE_CAST(LayoutGroupBase*, sender);
            std::shared_ptr<Controls::Control> childFromModel = nullptr;
            for (auto const& child : Children.GetNativeEnumerator())
            {
                auto iLayout = child->shared_from_base_dynamic<ILayoutControl>();
                if (!iLayout || iLayout->GetLayoutModel() != changedElement)
                    continue;

                childFromModel = child;
                break;
            }
            int indexOfChild = Children.IndexOf(childFromModel);
            RowDefinitions[indexOfChild]->Height = changedElement->GetDockHeight();
        }
    }
    else if (!strcmp(e.PropertyName, "IsVisible"))
    {
        UpdateRowColDefinitions();
    }
}

void XUI::UI::LayoutGridControl::UpdateRowColDefinitions()
{
    LayoutRoot* root = Model->Root;
    if (root == nullptr)
        return;
    DockingManager* manager = root->GetManager();
    if (manager == nullptr)
        return;

    FixChildrenDockLengths();

    //#region Setup GridRows / Cols
    RowDefinitions.Clear();
    ColumnDefinitions.Clear();
    if (Orientation() == UI::Orientation::Horizontal)
    {
        int iColumn = 0;
        int iChild = 0;
        for (int iChildModel = 0; iChildModel < Model->Children.size(); iChildModel++, iColumn++, iChild++)
        {
            LayoutGroupBase* childModel = Model->GetChildElement(iChildModel)->Static_As<LayoutGroupBase>();
            std::shared_ptr<ColumnDefinition> colDefinitation = std::make_shared<ColumnDefinition>(
                childModel->GetIsVisible() ? childModel->GetDockWidth() : GridLength(0.0, GridUnitType::Pixel));
            colDefinitation->MinWidth = childModel->GetIsVisible() ? childModel->GetDockMinWidth() : 0.0;
            ColumnDefinitions.Add(colDefinitation);
            Grid::SetColumn(Children[iChild].get(), iColumn);

            //append column for splitter
            if (iChild < Children.size() - 1)
            {
                iChild++;
                iColumn++;

                bool nextChildModelVisibleExist = false;
                for (int i = iChildModel + 1; i < Model->Children.size(); i++)
                {
                    LayoutElement* nextChildModel = Model->GetChildElement(i);
                    if (nextChildModel->GetIsVisible())
                    {
                        nextChildModelVisibleExist = true;
                        break;
                    }
                }

                colDefinitation = std::make_shared<ColumnDefinition>(childModel->GetIsVisible() && nextChildModelVisibleExist ? GridLength(manager->GridSplitterWidth, GridUnitType::Pixel) : GridLength(0.0, GridUnitType::Pixel));
                ColumnDefinitions.Add(colDefinitation);
                Grid::SetColumn(Children[iChild].get(), iColumn);
            }
        }
    }
    else
    {
        int iRow = 0;
        int iChild = 0;
        for (int iChildModel = 0; iChildModel < Model->Children.size(); iChildModel++, iRow++, iChild++)
        {
            LayoutGroupBase* childModel = Model->GetChildElement(iChildModel)->Static_As<LayoutGroupBase>();
            std::shared_ptr<RowDefinition> rowDefinitation = std::make_shared<RowDefinition>(
                childModel->GetIsVisible() ? childModel->GetDockHeight() : GridLength(0.0, GridUnitType::Pixel));
            rowDefinitation->MinHeight = childModel->GetIsVisible() ? childModel->GetDockMinHeight() : 0.0;
            RowDefinitions.Add(rowDefinitation);
            Grid::SetRow(Children[iChild].get(), iRow);

            //if (RowDefinitions.Last().Height.Value == 0.0)
            //    System.Diagnostics.Debugger.Break();

            //append row for splitter (if necessary)
            if (iChild < Children.size() - 1)
            {
                iChild++;
                iRow++;

                bool nextChildModelVisibleExist = false;
                for (int i = iChildModel + 1; i < Model->Children.size(); i++)
                {
                    auto nextChildModel = Model->GetChildElement(i);
                    if (nextChildModel->GetIsVisible())
                    {
                        nextChildModelVisibleExist = true;
                        break;
                    }
                }

                rowDefinitation = std::make_shared<RowDefinition>(
                    childModel->GetIsVisible() && nextChildModelVisibleExist ? GridLength(manager->GridSplitterHeight, GridUnitType::Pixel) : GridLength(0.0, GridUnitType::Pixel));
                RowDefinitions.Add(rowDefinitation);

                //if (RowDefinitions.Last().Height.Value == 0.0)
                //    System.Diagnostics.Debugger.Break();
                Grid::SetRow(Children[iChild].get(), iRow);
            }
        }
    }
}

void XUI::UI::LayoutGridControl::FixChildrenDockLengths()
{
    auto guard = _fixingChildrenDockLengths.Enter();
    OnFixChildrenDockLengths();
}

void XUI::UI::LayoutGridControl::AttachPropertyChangeHandler()
{
    for (auto const& child : Children)
    {
        ILayoutControl* iLayout = child->Dynamic_As<ILayoutControl>();
        if (!iLayout)
            continue;

        iLayout->GetLayoutModel()->PropertyChanged += System::make_eventhandler(&LayoutGridControl::OnChildModelPropertyChanged, this);
    }
}

void XUI::UI::LayoutGridControl::DetachPropertChangeHandler()
{
    for (auto const& child : Children)
    {
        ILayoutControl* iLayout = child->Dynamic_As<ILayoutControl>();
        if (!iLayout)
            continue;

        iLayout->GetLayoutModel()->PropertyChanged -= System::make_eventhandler(&LayoutGridControl::OnChildModelPropertyChanged, this);
    }
}

void XUI::UI::LayoutGridControl::CreateSplitters()
{
    for (int iChild = 1; iChild < Children.size(); iChild++)
    {
        auto splitter = std::make_shared<LayoutGridResizerControl>();
        splitter->Cursor = Orientation() == UI::Orientation::Horizontal ? std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType::SizeWE) : std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType::SizeNS);
        Children.Insert(iChild, splitter);
        iChild++;
    }
}

void XUI::UI::LayoutGridControl::DetachOldSplitters()
{
    for (auto const& child : Children.GetNativeEnumerator())
    {
        LayoutGridResizerControl* splitter = child->Static_As<LayoutGridResizerControl>();
        if (splitter)
        {
            splitter->DragStarted -= System::make_eventhandler(&LayoutGridControl::OnSplitterDragStarted, this);
            splitter->DragDelta -= System::make_eventhandler(&LayoutGridControl::OnSplitterDragDelta, this);
            splitter->DragCompleted -= System::make_eventhandler(&LayoutGridControl::OnSplitterDragCompleted, this);
        }
    }
}

void XUI::UI::LayoutGridControl::AttachNewSplitters()
{
    for (auto const& child : Children.GetNativeEnumerator())
    {
        LayoutGridResizerControl* splitter = child->Static_As<LayoutGridResizerControl>();
        if (splitter)
        {
            splitter->DragStarted += System::make_eventhandler(&LayoutGridControl::OnSplitterDragStarted, this);
            splitter->DragDelta += System::make_eventhandler(&LayoutGridControl::OnSplitterDragDelta, this);
            splitter->DragCompleted += System::make_eventhandler(&LayoutGridControl::OnSplitterDragCompleted, this);
        }
    }
}

void XUI::UI::LayoutGridControl::OnSplitterDragStarted(void * sender, Interfaces::VectorEventArgs & e)
{
    auto resizer = GENERIC_SAFE_CAST(LayoutGridResizerControl*, sender);
    ShowResizerOverlayWindow(resizer);
}

void XUI::UI::LayoutGridControl::OnSplitterDragDelta(void * sender, Interfaces::VectorEventArgs & e)
{
    LayoutGridResizerControl* splitter = GENERIC_SAFE_CAST(LayoutGridResizerControl*, sender);
    auto rootVisual = VisualRoot->ToVisual();

    //auto trToWnd = TransformToAncestor(rootVisual);
    //Vector transformedDelta = trToWnd.Transform(new Point(e.HorizontalChange, e.VerticalChange)) - trToWnd.Transform(new Point());
    Core::Media::Vector transformedDelta = e.Vector;

    if (Orientation() == UI::Orientation::Horizontal)
    {
        Canvas::SetLeft(_resizerGhost.get(), std::min(std::max(_initialStartPoint.x + transformedDelta.x, 0.0f), float(_resizerHost->ActualWidth() - _resizerGhost->ActualWidth())));
    }
    else
    {
        Canvas::SetTop(_resizerGhost.get(), std::min(std::max(_initialStartPoint.y + transformedDelta.y, 0.0f), float(_resizerHost->ActualHeight() - _resizerGhost->ActualHeight())));
    }
}

void XUI::UI::LayoutGridControl::OnSplitterDragCompleted(void * sender, Interfaces::VectorEventArgs & e)
{
    LayoutGridResizerControl* splitter = GENERIC_SAFE_CAST(LayoutGridResizerControl*, sender);
    auto rootVisual = VisualRoot->ToVisual();

    //var trToWnd = TransformToAncestor(rootVisual);
    //Vector transformedDelta = trToWnd.Transform(new Point(e.HorizontalChange, e.VerticalChange)) - trToWnd.Transform(new Point());
    Core::Media::Vector transformedDelta = e.Vector;

    double delta;
    if (Orientation() == UI::Orientation::Horizontal)
        delta = Canvas::GetLeft(_resizerGhost.get()) - _initialStartPoint.x;
    else
        delta = Canvas::GetTop(_resizerGhost.get()) - _initialStartPoint.y;

    int indexOfResizer = Children.IndexOf(splitter->shared_from_base_static<LayoutGridResizerControl>());

    Controls::Control* prevChild = Children[indexOfResizer - 1].get();
    Controls::Control* nextChild = GetNextVisibleChild(indexOfResizer);

    auto prevChildActualSize = prevChild->TransformedBounds.GetSize();
    auto nextChildActualSize = nextChild->TransformedBounds.GetSize();

    auto prevChildModel = static_cast<LayoutGroupBase*>(prevChild->Dynamic_As<ILayoutControl>()->GetLayoutModel());
    auto nextChildModel = static_cast<LayoutGroupBase*>(nextChild->Dynamic_As<ILayoutControl>()->GetLayoutModel());

    if (Orientation() == UI::Orientation::Horizontal)
    {
        if (prevChildModel->GetDockWidth().IsStar)
        {
            prevChildModel->SetDockWidth(GridLength(prevChildModel->GetDockWidth().Value * (prevChildActualSize.Width + delta) / prevChildActualSize.Width, GridUnitType::Star));
        }
        else
        {
            auto width = (prevChildModel->GetDockWidth().IsAuto) ? prevChildActualSize.Width : prevChildModel->GetDockWidth().Value;
            prevChildModel->SetDockWidth(GridLength(width + delta, GridUnitType::Pixel));
        }

        if (nextChildModel->GetDockWidth().IsStar)
        {
            nextChildModel->SetDockWidth(GridLength(nextChildModel->GetDockWidth().Value * (nextChildActualSize.Width - delta) / nextChildActualSize.Width, GridUnitType::Star));
        }
        else
        {
            auto width = (nextChildModel->GetDockWidth().IsAuto) ? nextChildActualSize.Width : nextChildModel->GetDockWidth().Value;
            nextChildModel->SetDockWidth(GridLength(width - delta, GridUnitType::Pixel));
        }
    }
    else
    {
        if (prevChildModel->GetDockHeight().IsStar)
        {
            prevChildModel->SetDockHeight(GridLength(prevChildModel->GetDockHeight().Value * (prevChildActualSize.Height + delta) / prevChildActualSize.Height, GridUnitType::Star));
        }
        else
        {
            auto height = (prevChildModel->GetDockHeight().IsAuto) ? prevChildActualSize.Height : prevChildModel->GetDockHeight().Value;
            prevChildModel->SetDockHeight(GridLength(height + delta, GridUnitType::Pixel));
        }

        if (nextChildModel->GetDockHeight().IsStar)
        {
            nextChildModel->SetDockHeight(GridLength(nextChildModel->GetDockHeight().Value * (nextChildActualSize.Height - delta) / nextChildActualSize.Height, GridUnitType::Star));
        }
        else
        {
            auto height = (nextChildModel->GetDockHeight().IsAuto) ? nextChildActualSize.Height : nextChildModel->GetDockHeight().Value;
            nextChildModel->SetDockHeight(GridLength(height - delta, GridUnitType::Pixel));
        }
    }

    HideResizerOverlayWindow();
    InvalidateMeasure();
}

Controls::Control * XUI::UI::LayoutGridControl::GetNextVisibleChild(int index)
{
    for (int i = index + 1; i < Children.size(); i++)
    {
        if (Children[i]->Is<LayoutGridResizerControl>())
            continue;

        if (Orientation() == UI::Orientation::Horizontal)
        {
            if (ColumnDefinitions[i]->Width.IsStar || ColumnDefinitions[i]->Width.Value > 0)
                return Children[i].get();
        }
        else
        {
            if (RowDefinitions[i]->Height.IsStar || RowDefinitions[i]->Height.Value > 0)
                return Children[i].get();
        }
    }

    return nullptr;
}

void XUI::UI::LayoutGridControl::ShowResizerOverlayWindow(LayoutGridResizerControl * splitter)
{
    HideResizerOverlayWindow();

    _resizerGhost = std::make_shared<Border>();
    _resizerGhost->Background = splitter->BackgroundWhileDragging;
    _resizerGhost->Opacity = splitter->OpacityWhileDragging;

    int indexOfResizer = Children.IndexOf(splitter->shared_from_base_static<LayoutGridResizerControl>());

    Controls::Control* prevChild = Children[indexOfResizer - 1].get();
    Controls::Control* nextChild = GetNextVisibleChild(indexOfResizer);

    auto prevChildActualSize = prevChild->TransformedBounds.GetSize();
    auto nextChildActualSize = nextChild->TransformedBounds.GetSize();

    auto prevChildModel = static_cast<LayoutGroupBase*>(prevChild->Dynamic_As<ILayoutControl>()->GetLayoutModel());
    auto nextChildModel = static_cast<LayoutGroupBase*>(nextChild->Dynamic_As<ILayoutControl>()->GetLayoutModel());
    nextChild->GID = 512;

    Core::Media::Point ptTopLeftScreen = prevChild->TransformedBounds.TopLeft; //PointToScreenDPIWithoutFlowDirection(new Point());

    Core::Media::Size actualSize;

    if (Orientation() == UI::Orientation::Horizontal)
    {
        actualSize = Core::Media::Size(prevChildActualSize.Width + splitter->ActualWidth() + nextChildActualSize.Width, nextChildActualSize.Height);

        _resizerGhost->Width = splitter->ActualWidth();
        _resizerGhost->Height = actualSize.Height;

        //ptTopLeftScreen.Offset(prevChildModel->DockMinWidth, 0.0);
    }
    else
    {
        actualSize = Core::Media::Size(prevChildActualSize.Width, prevChildActualSize.Height + splitter->ActualHeight() + nextChildActualSize.Height);

        _resizerGhost->Height = splitter->ActualHeight();
        _resizerGhost->Width = actualSize.Width;

        //ptTopLeftScreen.Offset(0.0, prevChildModel->DockMinHeight);
    }

    _initialStartPoint = splitter->TransformedBounds.TopLeft - ptTopLeftScreen;

    if (Orientation() == UI::Orientation::Horizontal)
    {
        Canvas::SetLeft(_resizerGhost.get(), _initialStartPoint.x);
    }
    else
    {
        Canvas::SetTop(_resizerGhost.get(), _initialStartPoint.y);
    }

    _resizerHost = std::make_shared<Canvas>();
    _resizerHost->HorizontalAlignment = Controls::HorizontalAlignment::Stretch;
    _resizerHost->VerticalAlignment = Controls::VerticalAlignment::Stretch;

    _resizerHost->Children.Add(_resizerGhost);

    _resizerHost->GID = 1012;
    _resizerHost->Width = actualSize.Width;
    _resizerHost->Height = actualSize.Height;
    //auto margin = _resizerHost->Margin;
    //margin.Left = ptTopLeftScreen.x - TransformedBounds.TopLeft.x;
    //margin.Top = ptTopLeftScreen.y - TransformedBounds.TopLeft.y;
    _resizerHost->SetDebugFlag(0x1);
    //_resizerHost->Margin = margin;
    _resizerHost->Measure(_desiredSize);
    _resizerHost->Arrange(Core::Media::Rect(_previousMeasure));

    AddVisualChild(_resizerHost.get());
    AddLogicalChild(_resizerHost.get());
}

void XUI::UI::LayoutGridControl::HideResizerOverlayWindow()
{
    if (_resizerHost != nullptr)
    {
        RemoveVisualChild(_resizerHost.get());
        RemoveLogicalChild(_resizerHost.get());
        _resizerHost = nullptr;
    }
}
