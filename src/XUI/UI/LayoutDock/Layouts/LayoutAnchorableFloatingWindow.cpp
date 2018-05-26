#include "StdAfx.h"
#include "LayoutAnchorableFloatingWindow.h"

XUI::UI::LayoutAnchorableFloatingWindow::LayoutAnchorableFloatingWindow()
{
}

XUI::UI::LayoutAnchorableFloatingWindow::~LayoutAnchorableFloatingWindow()
{
}

void XUI::UI::LayoutAnchorableFloatingWindow::SetRootPanel(std::shared_ptr<LayoutAnchorablePaneGroup> const & value)
{
    if (_rootPanel != value)
    {
        if (_rootPanel)
            _rootPanel->ChildrenTreeChanged -= System::make_eventhandler(&LayoutAnchorableFloatingWindow::ChildrenTreeChanged_rootPanel, this);

        _rootPanel = value;
        if (_rootPanel)
            _rootPanel->SetParent(this);

        if (_rootPanel)
            _rootPanel->ChildrenTreeChanged += System::make_eventhandler(&LayoutAnchorableFloatingWindow::ChildrenTreeChanged_rootPanel, this);

        NotifyPropertyChanged::RaisePropertyChanged("RootPanel");
        NotifyPropertyChanged::RaisePropertyChanged("IsSinglePane");
        NotifyPropertyChanged::RaisePropertyChanged("SinglePane");
        NotifyPropertyChanged::RaisePropertyChanged("Children");
        NotifyPropertyChanged::RaisePropertyChanged("ChildrenCount");
        ComputeVisibility();
    }
}

void XUI::UI::LayoutAnchorableFloatingWindow::SetIsVisible(bool const & value)
{
    if (_isVisible != value)
    {
        _isVisible = value;
        NotifyPropertyChanged::RaisePropertyChanged("IsVisible");
        if (IsVisibleChanged)
        {
            static System::Events::EventArg e;
            IsVisibleChanged(this, e);
        }
    }
}

void XUI::UI::LayoutAnchorableFloatingWindow::ChildrenTreeChanged_rootPanel(void * sender, ChildrenTreeChangedEventArgs & e)
{
    NotifyPropertyChanged::RaisePropertyChanged("IsSinglePane");
    NotifyPropertyChanged::RaisePropertyChanged("SinglePane");
}

bool XUI::UI::LayoutAnchorableFloatingWindow::IsSinglePane() const
{
    return _rootPanel != nullptr && LayoutContainer::ContainsChild(const_cast<LayoutAnchorableFloatingWindow*>(this), [](LayoutElement* x) -> bool { return x->GetIsVisible(); });
}

bool XUI::UI::LayoutAnchorableFloatingWindow::IsValid()
{
    return _rootPanel != nullptr;
}

XUI::UI::LayoutAnchorablePane * XUI::UI::LayoutAnchorableFloatingWindow::SinglePane()
{
    if (_rootPanel == nullptr)
        return nullptr;

    LayoutAnchorablePane* selectedPane = nullptr;
    LayoutContainer::ContainsChild(const_cast<LayoutAnchorableFloatingWindow*>(this), [&selectedPane](LayoutElement* x) -> bool
    {
        if (x->GetIsVisible())
        {
            selectedPane = x->Static_As<LayoutAnchorablePane>();
            return selectedPane != nullptr;
        }
        return false;
    });

    if (!selectedPane)
        return nullptr;

    selectedPane->UpdateIsDirectlyHostedInFloatingWindow();
    return selectedPane;
}

void XUI::UI::LayoutAnchorableFloatingWindow::AddChild(LayoutElement * child)
{
    if (!child->Is<LayoutAnchorablePaneGroup>() || _rootPanel != nullptr)
        throw System::ArgumentException();

    RootPanel = child->shared_from_base_static<LayoutAnchorablePaneGroup>();
    LayoutFloatingWindow::AddChild(child);
}

void XUI::UI::LayoutAnchorableFloatingWindow::RemoveChild(LayoutElement * child)
{
    ASSERT(child == RootPanel.get() && child);
    RootPanel = nullptr;
    LayoutFloatingWindow::RemoveChild(child);
}

void XUI::UI::LayoutAnchorableFloatingWindow::ReplaceChild(LayoutElement * oldElement, LayoutElement * newElement)
{
    if (!newElement->Is<LayoutAnchorablePaneGroup>() || (oldElement != nullptr && _rootPanel.get() != oldElement))
        throw System::ArgumentException();

    RootPanel = newElement->shared_from_base_static<LayoutAnchorablePaneGroup>();
    LayoutFloatingWindow::ReplaceChild(oldElement, newElement);
}

void XUI::UI::LayoutAnchorableFloatingWindow::ComputeVisibility()
{
    if (RootPanel)
        IsVisible = RootPanel->IsVisible;
    else
        IsVisible = false;
}

