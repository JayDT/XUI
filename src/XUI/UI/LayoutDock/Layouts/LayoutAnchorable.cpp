#include "StdAfx.h"
#include "LayoutAnchorable.h"
#include "LayoutContainer.h"
#include "LayoutRoot.h"
#include "LayoutAnchorGroup.h"
#include "LayoutAnchorablePane.h"
#include "LayoutDocumentPane.h"
#include "LayoutDocument.h"
#include "LayoutPanel.h"
#include "DockingManager.h"

XUI::UI::LayoutAnchorable::LayoutAnchorable()
{
}

XUI::UI::LayoutAnchorable::~LayoutAnchorable()
{
}

bool XUI::UI::LayoutAnchorable::GetIsVisible() const
{
    return GetParent() != nullptr && !GetParent()->Is<LayoutRoot>();
}

void XUI::UI::LayoutAnchorable::SetIsVisible(bool const & value)
{
    if (value)
    {
        Show();
    }
    else
    {
        Hide();
    }
}

bool XUI::UI::LayoutAnchorable::IsHidden() const
{
    return GetParent() == nullptr || GetParent()->Is<LayoutRoot>();
}

bool XUI::UI::LayoutAnchorable::IsAutoHidden() const
{
    return GetParent() != nullptr && GetParent()->Is<LayoutAnchorGroup>();
}

void XUI::UI::LayoutAnchorable::OnParentChanged(LayoutContainer * oldValue, LayoutContainer * newValue)
{
    UpdateParentVisibility();
    NotifyPropertyChanged::RaisePropertyChanged("IsVisible");
    NotifyIsVisibleChanged();
    NotifyPropertyChanged::RaisePropertyChanged("IsHidden");
    NotifyPropertyChanged::RaisePropertyChanged("IsAutoHidden");
    LayoutElement::OnParentChanged(oldValue, newValue);
}

void XUI::UI::LayoutAnchorable::UpdateParentVisibility()
{
    auto parentPane = GetParent();
    if (parentPane)
        parentPane->ComputeVisibility();
}

/// <summary>
/// Hide this contents
/// </summary>
/// <remarks>Add this content to <see cref="ILayoutRoot.Hidden"/> collection of parent root.</remarks>

void XUI::UI::LayoutAnchorable::Hide(bool cancelable)
{
    if (!IsVisible)
    {
        IsSelected = true;
        IsActive = true;
        return;
    }

    if (cancelable)
    {
        CancelEventArgs args;
        if (Hiding)
            Hiding(this, args);
        if (args.Cancel)
            return;
    }

    //if (Parent is ILayoutPane)
    {
        auto parentAsGroup = GetParent();
        PreviousContainer = parentAsGroup;
        PreviousContainerIndex = parentAsGroup->IndexOfChild(this);
    }
    GetRoot()->Hidden.Add(shared_from_base_static<LayoutAnchorable>());
    NotifyPropertyChanged::RaisePropertyChanged("IsVisible");
    NotifyPropertyChanged::RaisePropertyChanged("IsHidden");
    NotifyIsVisibleChanged();
}

/// <summary>
/// Show the content
/// </summary>
/// <remarks>Try to show the content where it was previously hidden.</remarks>

void XUI::UI::LayoutAnchorable::Show()
{
    if (IsVisible)
        return;
    
    if (!IsHidden())
        throw System::InvalidOperationException();
    
    bool added = false;
    auto root = GetRoot();
    if (root && root->GetManager())
    {
        if (root->GetManager()->LayoutUpdateStrategy)
            added = root->GetManager()->LayoutUpdateStrategy->BeforeInsertAnchorable(root, this, PreviousContainer);
    }
    
    if (!added && PreviousContainer)
    {
        auto previousContainerAsLayoutGroup = PreviousContainer;
        if (PreviousContainerIndex < previousContainerAsLayoutGroup->Children.size())
            previousContainerAsLayoutGroup->InsertChildAt(PreviousContainerIndex, this);
        else
            previousContainerAsLayoutGroup->InsertChildAt(previousContainerAsLayoutGroup->Children.size(), this);
        IsSelected = true;
        IsActive = true;
    }
    
    if (root && root->GetManager())
    {
        if (root->GetManager()->LayoutUpdateStrategy)
        {
            root->GetManager()->LayoutUpdateStrategy->AfterInsertAnchorable(root, this);
        }
    }
    
    PreviousContainer = nullptr;
    PreviousContainerIndex = -1;
    
    NotifyPropertyChanged::RaisePropertyChanged("IsVisible");
    NotifyPropertyChanged::RaisePropertyChanged("IsHidden");
    NotifyIsVisibleChanged();
}

void XUI::UI::LayoutAnchorable::InternalDock()
{
    auto root = GetRoot();
    LayoutAnchorablePane* anchorablePane = nullptr;
    
    if (root->ActiveContent != nullptr &&
        root->ActiveContent != this)
    {
        //look for active content parent pane
        anchorablePane = root->ActiveContent->GetParent()->Static_As<LayoutAnchorablePane>();
    }
    
    if (anchorablePane == nullptr)
    {
        //look for a pane on the right side
        LayoutContainer::ContainsChild(root, [&anchorablePane](LayoutElement* x)
        {
            return anchorablePane = LayoutElement::GetSide(x) == AnchorSide::Right ? x->Static_As<LayoutAnchorablePane>() : nullptr;
        });
    }
    
    if (anchorablePane == nullptr)
    {
        //look for an available pane
        LayoutContainer::ContainsChild(root, [&anchorablePane](LayoutElement* x)
        {
            return anchorablePane = x->Static_As<LayoutAnchorablePane>();
        });
    }
    
    
    bool added = false;
    if (root->GetManager()->LayoutUpdateStrategy)
    {
        added = root->GetManager()->LayoutUpdateStrategy->BeforeInsertAnchorable(root, this, anchorablePane);
    }
    
    if (!added)
    {
        if (anchorablePane)
        {
            auto mainLayoutPanel = std::make_shared<LayoutPanel>();
            mainLayoutPanel->Orientation = Orientation::Horizontal;
            if (root->RootPanel)
                mainLayoutPanel->AddChild(root->RootPanel.get());
    
            root->SetRootPanel(mainLayoutPanel);
            auto _anchorablePane = std::make_shared<LayoutAnchorablePane>();
            anchorablePane = _anchorablePane.get();
            anchorablePane->DockWidth = GridLength(200.0, GridUnitType::Pixel);
            mainLayoutPanel->AddChild(anchorablePane);
        }
    
        anchorablePane->AddChild(this);
        added = true;
    }
    
    if (root->GetManager()->LayoutUpdateStrategy)
    {
        root->GetManager()->LayoutUpdateStrategy->AfterInsertAnchorable(root, this);
    }
    
    LayoutContent::InternalDock();
}

/// <summary>
/// Add the anchorable to a DockingManager layout
/// </summary>
/// <param name="manager"></param>
/// <param name="strategy"></param>

void XUI::UI::LayoutAnchorable::AddToLayout(DockingManager * manager, AnchorableShowStrategy strategy)
{
    if (IsVisible || IsHidden())
        throw System::InvalidOperationException();
    
    
    bool most = AnchorableShowStrategy(strategy & AnchorableShowStrategy::Most) == AnchorableShowStrategy::Most;
    bool left = AnchorableShowStrategy(strategy & AnchorableShowStrategy::Left) == AnchorableShowStrategy::Left;
    bool right = AnchorableShowStrategy(strategy & AnchorableShowStrategy::Right) == AnchorableShowStrategy::Right;
    bool top = AnchorableShowStrategy(strategy & AnchorableShowStrategy::Top) == AnchorableShowStrategy::Top;
    bool bottom = AnchorableShowStrategy(strategy & AnchorableShowStrategy::Bottom) == AnchorableShowStrategy::Bottom;
    
    if (!most)
    {
        auto side = AnchorSide::Left;
        if (left)
            side = AnchorSide::Left;
        if (right)
            side = AnchorSide::Right;
        if (top)
            side = AnchorSide::Top;
        if (bottom)
            side = AnchorSide::Bottom;
    
        LayoutAnchorablePane* anchorablePane = nullptr;
        LayoutContainer::ContainsChild(manager->Layout.get(), [side, &anchorablePane](LayoutElement* p) { return (LayoutElement::GetSide(p) == side) ? (anchorablePane = p->Static_As<LayoutAnchorablePane>()) != nullptr : false; });
        if (anchorablePane)
            anchorablePane->AddChild(this);
        else
            most = true;
    }
    
    
    if (most)
    {
        if (!manager->Layout->RootPanel)
        {
            manager->Layout->RootPanel = std::make_shared<LayoutPanel>();
            manager->Layout->RootPanel->Orientation = (left || right ? Orientation::Horizontal : Orientation::Vertical);
        }
    
        if (left || right)
        {
            if (manager->Layout->RootPanel->Orientation == Orientation::Vertical &&
                manager->Layout->RootPanel->Children.size() > 1)
            {
                manager->Layout->RootPanel = std::make_shared<LayoutPanel>(manager->Layout->RootPanel.get());
            }
    
            manager->Layout->RootPanel->Orientation = Orientation::Horizontal;
    
            auto pane = std::make_shared<LayoutAnchorablePane>();
            pane->AddChild(this);

            if (left)
                manager->Layout->RootPanel->InsertChildAt(0, pane);
            else
                manager->Layout->RootPanel->AddChild(pane);
        }
        else
        {
            if (manager->Layout->RootPanel->Orientation == Orientation::Horizontal &&
                manager->Layout->RootPanel->Children.size() > 1)
            {
                manager->Layout->RootPanel = std::make_shared<LayoutPanel>(manager->Layout->RootPanel.get());
            }
    
            manager->Layout->RootPanel->Orientation = Orientation::Vertical;

            auto pane = std::make_shared<LayoutAnchorablePane>();
            pane->AddChild(this);

            if (top)
                manager->Layout->RootPanel->InsertChildAt(0, pane);
            else
                manager->Layout->RootPanel->AddChild(pane);
        }
    
    }
}

void XUI::UI::LayoutAnchorable::ToggleAutoHide()
{
    // Anchorable is already auto hidden
    if (IsAutoHidden())
    {
        auto parentGroup = GetParent() ? GetParent()->Static_As<LayoutAnchorGroup>() : nullptr;
        auto parentSide = parentGroup ? parentGroup->GetParent()->Static_As<LayoutAnchorSide>() : nullptr;
        auto previousContainer = parentGroup && parentGroup->PreviousContainer ? parentGroup->PreviousContainer->shared_from_base_static<LayoutAnchorablePane>() : nullptr;

        if (previousContainer == nullptr)
        {
            AnchorSide side = parentSide->Side;
            switch (side)
            {
                case AnchorSide::Right:
                    if (parentGroup->GetRoot()->RootPanel->Orientation == Orientation::Horizontal)
                    {
                        previousContainer = std::make_shared<LayoutAnchorablePane>();
                        previousContainer->DockMinWidth = AutoHideMinWidth;
                        parentGroup->GetRoot()->RootPanel->AddChild(previousContainer.get());
                    }
                    else
                    {
                        previousContainer = std::make_shared<LayoutAnchorablePane>();
                        auto panel = std::make_shared<LayoutPanel>();
                        panel->Orientation = Orientation::Horizontal;
                        LayoutRoot* root = parentGroup->GetRoot();
                        auto oldRootPanel = parentGroup->GetRoot()->RootPanel;
                        root->RootPanel = panel;
                        panel->AddChild(oldRootPanel.get());
                        panel->AddChild(previousContainer.get());
                    }
                    break;
                case AnchorSide::Left:
                    if (parentGroup->GetRoot()->RootPanel->Orientation == Orientation::Horizontal)
                    {
                        previousContainer = std::make_shared<LayoutAnchorablePane>();
                        previousContainer->DockMinWidth = AutoHideMinWidth;
                        parentGroup->GetRoot()->RootPanel->InsertChildAt(0, previousContainer.get());
                    }
                    else
                    {
                        previousContainer = std::make_shared<LayoutAnchorablePane>();
                        auto panel = std::make_shared<LayoutPanel>();
                        panel->Orientation = Orientation::Horizontal;
                        LayoutRoot* root = parentGroup->GetRoot();
                        auto oldRootPanel = parentGroup->GetRoot()->RootPanel;
                        root->RootPanel = panel;
                        panel->AddChild(previousContainer.get());
                        panel->AddChild(oldRootPanel.get());
                    }
                    break;
                case AnchorSide::Top:
                    if (parentGroup->GetRoot()->RootPanel->Orientation == Orientation::Vertical)
                    {
                        previousContainer = std::make_shared<LayoutAnchorablePane>();
                        previousContainer->DockMinHeight = AutoHideMinHeight;
                        parentGroup->GetRoot()->RootPanel->InsertChildAt(0, previousContainer);
                    }
                    else
                    {
                        previousContainer = std::make_shared<LayoutAnchorablePane>();
                        auto panel = std::make_shared<LayoutPanel>();
                        panel->Orientation = Orientation::Vertical;
                        LayoutRoot* root = parentGroup->GetRoot();
                        auto oldRootPanel = parentGroup->GetRoot()->RootPanel;
                        root->RootPanel = panel;
                        panel->AddChild(previousContainer.get());
                        panel->AddChild(oldRootPanel.get());
                    }
                    break;
                case AnchorSide::Bottom:
                    if (parentGroup->GetRoot()->RootPanel->Orientation == Orientation::Vertical)
                    {
                        previousContainer = std::make_shared<LayoutAnchorablePane>();
                        previousContainer->DockMinHeight = AutoHideMinHeight;
                        parentGroup->GetRoot()->RootPanel->AddChild(previousContainer);
                    }
                    else
                    {
                        previousContainer = std::make_shared<LayoutAnchorablePane>();
                        auto panel = std::make_shared<LayoutPanel>();
                        panel->Orientation = Orientation::Vertical;
                        LayoutRoot* root = parentGroup->GetRoot();
                        auto oldRootPanel = parentGroup->GetRoot()->RootPanel;
                        root->RootPanel = panel;
                        panel->AddChild(previousContainer.get());
                        panel->AddChild(oldRootPanel.get());
                    }
                    break;
            }
        }
        else
        {
            //I'm about to remove parentGroup, redirect any content (ie hidden contents) that point to it
            //to previousContainer
            LayoutRoot* root = parentGroup->GetRoot();
            LayoutContainer::ContainsChild(root, [&](LayoutElement* x)
            {
                x->SetPreviousContainer(previousContainer.get());
                return false;
            });
        }


        for(auto const& anchorableToToggle : parentGroup->Children.GetNativeEnumerator())
        {
            previousContainer->AddChild(anchorableToToggle.get());
        }

        parentSide->RemoveChild(parentGroup);
    }
    // Anchorable is docked
    else if (GetParent()->Is<LayoutAnchorablePane>())
    {
        auto root = GetRoot();
        auto parentPane = GetParent()->Static_As<LayoutAnchorablePane>();

        auto newAnchorGroup = std::make_shared<LayoutAnchorGroup>();

        newAnchorGroup->PreviousContainer = parentPane;

        for(auto const& anchorableToImport : parentPane->Children.GetNativeEnumerator())
            newAnchorGroup->AddChild(anchorableToImport.get());

        //detect anchor side for the pane
        auto anchorSide = LayoutElement::GetSide(parentPane);

        switch (anchorSide)
        {
            case AnchorSide::Right:
                root->RightSide->AddChild(newAnchorGroup);
                break;
            case AnchorSide::Left:
                root->LeftSide->AddChild(newAnchorGroup);
                break;
            case AnchorSide::Top:
                root->TopSide->AddChild(newAnchorGroup);
                break;
            case AnchorSide::Bottom:
                root->BottomSide->AddChild(newAnchorGroup);
                break;
        }
    }
}

void XUI::UI::LayoutAnchorable::Close()
{
    CloseAnchorable();
}

void XUI::UI::LayoutAnchorable::CloseAnchorable()
{
    if (TestCanClose())
    {
        if (IsAutoHidden())
            ToggleAutoHide();

        CloseInternal();
    }
}
