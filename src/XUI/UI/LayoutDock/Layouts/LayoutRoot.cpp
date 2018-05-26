#include "StdAfx.h"
#include "LayoutRoot.h"
#include "LayoutPanel.h"
#include "LayoutDocument.h"
#include "LayoutDocumentPane.h"
#include "LayoutAnchorablePaneGroup.h"
#include "LayoutDocumentPaneGroup.h"
#include "DockingManager.h"

XUI::UI::LayoutRoot::LayoutRoot()
{
}

XUI::UI::LayoutRoot::~LayoutRoot()
{
}

void XUI::UI::LayoutRoot::Initialize()
{
    RightSide = std::make_shared<LayoutAnchorSide>();
    LeftSide = std::make_shared<LayoutAnchorSide>();
    TopSide = std::make_shared<LayoutAnchorSide>();
    BottomSide = std::make_shared<LayoutAnchorSide>();
    RootPanel = std::make_shared<LayoutPanel>();
    auto pane = std::make_shared<LayoutDocumentPane>();
    RootPanel->AddChild(pane.get());

    _floatingWindows.CollectionChanged += System::make_eventhandler(&LayoutRoot::CollectionChanged_floatingWindows, this);
    _hidden.CollectionChanged += System::make_eventhandler(&LayoutRoot::CollectionChanged_hiddenAnchorables, this);
}

void XUI::UI::LayoutRoot::AddChild(LayoutElement * child)
{
    if (!child->Is<LayoutPanel>() || !m_children.empty())
        throw System::InvalidOperationException("Root layout can store directly element!");
    child->m_layoutElementReference.link(child, this);
    LayoutContainer::AddChild(child);
}

void XUI::UI::LayoutRoot::RemoveChild(LayoutElement * child)
{
    child->m_layoutElementReference.unlink();
    LayoutContainer::RemoveChild(child);
}

void XUI::UI::LayoutRoot::ReplaceChild(LayoutElement * oldchild, LayoutElement * newchild)
{
    // does not affect linked list
    LayoutContainer::ReplaceChild(oldchild, newchild);
}

void XUI::UI::LayoutRoot::CollectionChanged_hiddenAnchorables(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Remove ||
        e.Action == System::Collection::NotifyCollectionChangedAction::Replace)
    {
        auto const& element = _hidden[e.OldStartingIndex];
        if (element && element->Parent == this)
        {
            LayoutContainer::RemoveChild(element.get());
            element->Parent = nullptr;
        }
    }
    
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Add ||
        e.Action == System::Collection::NotifyCollectionChangedAction::Replace)
    {
        auto const& element = _hidden[e.NewStartingIndex];
        if (element && element->Parent != this)
        {
            if (element->Parent != nullptr)
                element->Parent->RemoveChild(element.get());
            element->Parent = this;
            LayoutContainer::AddChild(element.get());
        }
    }
}

void XUI::UI::LayoutRoot::CollectionChanged_floatingWindows(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Remove ||
        e.Action == System::Collection::NotifyCollectionChangedAction::Replace)
    {
        auto const& element = _floatingWindows[e.OldStartingIndex];
        if (element && element->Parent == this)
        {
            LayoutContainer::RemoveChild(element.get());
            element->Parent = nullptr;
        }
    }
    
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Add ||
        e.Action == System::Collection::NotifyCollectionChangedAction::Replace)
    {
        auto const& element = _floatingWindows[e.NewStartingIndex];
        if (element && element->Parent != this)
        {
            element->Parent = this;
            LayoutContainer::AddChild(element.get());
        }
    }
}

void XUI::UI::LayoutRoot::OnLayoutElementAdded(LayoutElement * element)
{
    if (ElementAdded)
    {
        LayoutElementEventArgs e(element);
        ElementAdded(this, e);
    }
}

void XUI::UI::LayoutRoot::OnLayoutElementRemoved(LayoutElement * element)
{
    if (LayoutContainer::ContainsChild(element->Static_As<LayoutContainer>(), [this](LayoutElement* x) -> bool { return x == LastFocusedDocument; }))
        LastFocusedDocument = nullptr;
    if (LayoutContainer::ContainsChild(element->Static_As<LayoutContainer>(), [this](LayoutElement* x) -> bool { return x == ActiveContent; }))
        ActiveContent = nullptr;

    if (ElementRemoved)
    {
        LayoutElementEventArgs e(element);
        ElementRemoved(this, e);
    }
}

void XUI::UI::LayoutRoot::InternalSetActiveContent(LayoutContent * currentValue, LayoutContent * newActiveContent)
{
    if (currentValue)
        currentValue->IsActive = false;
    if (newActiveContent)
        _activeContent = newActiveContent->weak_from_base_static<LayoutContent>();
    else
        _activeContent.reset();
    currentValue = ActiveContent;
    if (currentValue)
        currentValue->IsActive = true;
    NotifyPropertyChanged::RaisePropertyChanged("ActiveContent");
    _activeContentSet = currentValue != nullptr;
    if (currentValue)
    {
        if (currentValue->GetParent()->Is<LayoutDocumentPane>() || currentValue->Is<LayoutDocument>())
            LastFocusedDocument = currentValue;
    }
    else
        LastFocusedDocument = nullptr;
}

void XUI::UI::LayoutRoot::UpdateActiveContentProperty()
{
    auto activeContent = ActiveContent;
    if (_activeContentSet && (activeContent == nullptr || activeContent->GetRoot() != this))
    {
        _activeContentSet = false;
        InternalSetActiveContent(activeContent, nullptr);
    }
}

void XUI::UI::LayoutRoot::SetRootPanel(std::shared_ptr<LayoutPanel> const & value)
{
    if (_rootPanel != value)
    {
        if (_rootPanel != nullptr)
            RemoveChild(_rootPanel.get());

        _rootPanel = value;
        if (_rootPanel == nullptr)
        {
            _rootPanel = std::make_shared<LayoutPanel>();
            auto child = std::make_shared<LayoutDocumentPane>();
            _rootPanel->AddChild(child.get());
        }

        if (_rootPanel != nullptr)
        {
            _rootPanel->SetParent(this);
            AddChild(_rootPanel.get());
        }

        NotifyPropertyChanged::RaisePropertyChanged("RootPanel");
    }
}

void XUI::UI::LayoutRoot::CollectGarbage()
{
    bool exitFlag = true;

#pragma region collect empty panes
    do
    {
        exitFlag = true;

        //for each content that references via PreviousContainer a disconnected Pane set the property to null
        LayoutContainer::ContainsChild(this, [&](LayoutElement* x)
        {
            auto content = x->Static_As<LayoutContent>();
            if (content && content->PreviousContainer != nullptr && (content->PreviousContainer->Parent == nullptr || content->PreviousContainer->Parent->Root != this))
                content->PreviousContainer = nullptr;
            return false;
        });

        //for each pane that is empty
        LayoutContainer::ContainsChild(this, [&](LayoutElement* x)
        {
            auto emptyPane = x->Static_As<LayoutGroupBase>();
            if (!emptyPane || emptyPane == _rootPanel.get() || !emptyPane->Children.empty())
                return false;

            LayoutContainer::ContainsChild(this, [&](LayoutElement* x)
            {
                auto content = x->Static_As<LayoutContent>();
                if (content && content->PreviousContainer == emptyPane && !content->IsFloating())
                {
                    if (content->Is<LayoutAnchorable>() && !content->GetIsVisible())
                        return false;

                    content->PreviousContainer = nullptr;
                    content->PreviousContainerIndex = -1;
                }
                return false;
            });

            //...if this pane is the only documentpane present in the layout than skip it
            if (emptyPane->Is<LayoutDocumentPane>() && LayoutContainer::ContainsChild(this, [&](LayoutElement* x) { return x == emptyPane; }))
            {
                // this.Descendents().OfType<LayoutDocumentPane>().Count(c = > c != emptyPane) == 0)
                return false;
            }

            //...if this empty panes is not referenced by anyone, than removes it from its parent container
            if (!LayoutContainer::ContainsChild(this, [&](LayoutElement* x)
            {
                auto content = x->Static_As<LayoutContent>();
                return content && content->PreviousContainer == emptyPane;
            })) // !this.Descendents().OfType<ILayoutPreviousContainer>().Any(c = > c.PreviousContainer == emptyPane)
            {
                auto parentGroup = emptyPane->Parent->Static_As<LayoutContainer>();
                parentGroup->RemoveChild(emptyPane);
                exitFlag = false;
                return true;
            }
            return false;
        });

        if (!exitFlag)
        {
            //removes any empty anchorable pane group
            LayoutContainer::ContainsChild(this, [&](LayoutElement* x) // this.Descendents().OfType<LayoutAnchorablePaneGroup>().Where(p = > p.Children.size() == 0)
            {
                auto content = x->Static_As<LayoutGroupBase>();
                if (content && content != _rootPanel.get() && content->Children.empty())
                {
                    auto parentGroup = content->Parent->Static_As<LayoutContainer>();
                    parentGroup->RemoveChild(content);
                    exitFlag = false;
                    return true;
                }
                return false;
            });
        }

        if (!exitFlag)
        {
            //removes any empty layout panel
            LayoutContainer::ContainsChild(this, [&](LayoutElement* x) // this.Descendents().OfType<LayoutPanel>().Where(p = > p.Children.size() == 0)
            {
                auto content = x->Static_As<LayoutPanel>();
                if (content && content != _rootPanel.get() && content->Children.empty())
                {
                    auto parentGroup = content->Parent->Static_As<LayoutContainer>();
                    parentGroup->RemoveChild(content);
                    exitFlag = false;
                    return true;
                }
                return false;
            });
        }

        if (!exitFlag)
        {
            //removes any empty floating window
            LayoutContainer::ContainsChild(this, [&](LayoutElement* x) // this.Descendents().OfType<LayoutFloatingWindow>().Where(p = > p.Children.size() == 0)
            {
                auto content = x->Static_As<LayoutFloatingWindow>();
                if (content && content->Children.empty())
                {
                    auto parentGroup = content->Parent->Static_As<LayoutContainer>();
                    parentGroup->RemoveChild(content);
                    exitFlag = false;
                    return true;
                }
                return false;
            });
        }

        if (!exitFlag)
        {
            //removes any empty anchor group
            LayoutContainer::ContainsChild(this, [&](LayoutElement* x) // this.Descendents().OfType<LayoutFloatingWindow>().Where(p = > p.Children.size() == 0)
            {
                auto content = x->Static_As<LayoutAnchorGroup>();
                if (content && content->Children.empty())
                {
                    auto parentGroup = content->Parent->Static_As<LayoutContainer>();
                    parentGroup->RemoveChild(content);
                    exitFlag = false;
                    return true;
                }
                return false;
            });
        }

    } while (!exitFlag);
#pragma endregion

#pragma region collapse single child anchorable pane groups
    do
    {
        exitFlag = true;
        //for each pane that is empty
        LayoutContainer::ContainsChild(this, [&](LayoutElement* x) // this.Descendents().OfType<LayoutFloatingWindow>().Where(p = > p.Children.size() == 0)
        {
            auto content = x->Static_As<LayoutAnchorablePaneGroup>();
            if (content && content->Children.size() == 1 && (*content->Children.GetNativeEnumerator().begin())->Is<LayoutAnchorablePaneGroup>())
            {
                auto singleChild = (*content->Children.GetNativeEnumerator().begin())->Static_As<LayoutAnchorablePaneGroup>();
                content->SetOrientation(singleChild->Orientation);
                content->RemoveChild(singleChild);
                while (singleChild->Children.size() > 0)
                {
                    content->InsertChildAt(
                        content->Children.size(), (*singleChild->Children.GetNativeEnumerator().begin()).get());
                }

                exitFlag = false;
                return true;
            }
            return false;
        });
    } while (!exitFlag);
#pragma endregion

#pragma region collapse single child document pane groups
    do
    {
        exitFlag = true;
        //for each pane that is empty
        LayoutContainer::ContainsChild(this, [&](LayoutElement* x) // this.Descendents().OfType<LayoutFloatingWindow>().Where(p = > p.Children.size() == 0)
        {
            auto content = x->Static_As<LayoutDocumentPaneGroup>();
            if (content && content->GetChildren().size() == 1 && (*content->GetChildren().GetNativeEnumerator().begin())->Is<LayoutDocumentPaneGroup>())
            {
                auto singleChild = (*content->GetChildren().GetNativeEnumerator().begin())->shared_from_base_static<LayoutDocumentPaneGroup>();
                content->SetOrientation(singleChild->Orientation);
                content->RemoveChild(singleChild.get());
                while (!singleChild->GetChildren().empty())
                {
                    content->InsertChildAt(
                        content->GetChildren().size(), (*singleChild->GetChildren().GetNativeEnumerator().begin()).get());
                }

                exitFlag = false;
                return true;
            }
            return false;
        });
    } while (!exitFlag);
#pragma endregion

    //removes any empty floating window
    for(std::shared_ptr<LayoutFloatingWindow> const& wnd : this->_floatingWindows.GetNativeEnumerator())
    {
        if (wnd->Children.empty())
        {
            auto parentGroup = wnd->Parent->Static_As<LayoutContainer>();
            parentGroup->RemoveChild(wnd.get());
            exitFlag = false;
            break;
        }
    }

#pragma region Update ActiveContent and LastFocusedDocument properties
    UpdateActiveContentProperty();
#pragma endregion

}