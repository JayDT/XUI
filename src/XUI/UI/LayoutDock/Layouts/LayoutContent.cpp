#include "StdAfx.h"
#include "LayoutContent.h"
#include "LayoutFloatingWindow.h"
#include "LayoutRoot.h"
#include "LayoutPanel.h"
#include "LayoutDocumentPane.h"
#include "DockingManager.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutContent, XUI::UI::LayoutContent::StaticClassInitializer)

Core::Dependency::RefDependencyProperty LayoutContent::TitleProperty;
Core::Dependency::RefDependencyProperty LayoutContent::IconSourceProperty;

void XUI::UI::LayoutContent::StaticClassInitializer()
{
    TitleProperty = (Core::Dependency::BindProperty<std::wstring>::Register<LayoutContent>(nameof(Title)));
    IconSourceProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBitmapImpl>>::Register<LayoutContent>(nameof(IconSource)));
}

XUI::UI::LayoutContent::LayoutContent()
{
}

XUI::UI::LayoutContent::~LayoutContent()
{
}

void XUI::UI::LayoutContent::SetIsSelected(bool value)
{
    if (_isSelected != value)
    {
        bool oldValue = _isSelected;
        _isSelected = value;
        auto parentSelector = GetParent() ? GetParent()->Static_As<LayoutGroupBase>() : nullptr;
        if (parentSelector)
            parentSelector->SetSelectedContentIndex(_isSelected ? parentSelector->IndexOf(this) : -1);
        OnIsSelectedChanged(oldValue, value);
        NotifyPropertyChanged::RaisePropertyChanged("IsSelected");
    }
}

void XUI::UI::LayoutContent::SetIsActive(bool value)
{
    if (_isActive != value)
    {
        bool oldValue = _isActive;
        _isActive = value;

        auto root = GetRoot();
        if (root && _isActive)
            root->ActiveContent = this;

        if (_isActive)
            IsSelected = true;

        OnIsActiveChanged(oldValue, value);
        NotifyPropertyChanged::RaisePropertyChanged("IsActive");
    }
}

void XUI::UI::LayoutContent::SetPreviousContainer(LayoutContainer * value)
{
    if (_previousContainer.get() != value)
    {
        if (value)
            _previousContainer = value->shared_from_base_static<LayoutContainer>();
        else
            _previousContainer.reset();
        NotifyPropertyChanged::RaisePropertyChanged("PreviousContainer");

        //auto paneSerializable = _previousContainer->Static_As<LayoutPaneSerializable>();
        //if (paneSerializable &&
        //    paneSerializable->Id.empty())
        //    paneSerializable->Id = Guid.NewGuid().ToString();
    }
}

bool XUI::UI::LayoutContent::IsFloating()
{
    return LayoutElement::FindParent<LayoutFloatingWindow>(this);
}

bool XUI::UI::LayoutContent::CompareTo(LayoutContent * other)
{
    if (!other)
        return false;

    if (Content == other->Content)
        return true;

    return Title == other->Title;
}

/// <summary>
/// Test if the content can be closed
/// </summary>
/// <returns></returns>

bool XUI::UI::LayoutContent::TestCanClose()
{
    CancelEventArgs args;

    if (Closing)
        Closing(this, args);

    if (args.Cancel)
        return false;

    return true;
}

void XUI::UI::LayoutContent::CloseInternal()
{
    auto root = GetRoot();
    auto parentAsContainer = GetParent() ? GetParent()->Static_As<LayoutContainer>() : nullptr;
    if (parentAsContainer)
        parentAsContainer->RemoveChild(this);
    if (root)
        root->CollectGarbage();

    OnClosed();
}

/// <summary>
/// Float the content in a popup window
/// </summary>

void XUI::UI::LayoutContent::Float()
{
    if (PreviousContainer &&
        LayoutContainer::FindParent<LayoutFloatingWindow>(PreviousContainer))
    {
        auto currentContainer = GetParent();
        auto currentContainerIndex = currentContainer->IndexOfChild(this);
        auto previousContainerAsLayoutGroup = PreviousContainer;
    
        if (PreviousContainerIndex < previousContainerAsLayoutGroup->Children.size())
            previousContainerAsLayoutGroup->InsertChildAt(PreviousContainerIndex, this);
        else
            previousContainerAsLayoutGroup->InsertChildAt(previousContainerAsLayoutGroup->Children.size(), this);
    
        PreviousContainer = currentContainer;
        PreviousContainerIndex = currentContainerIndex;
    
        IsSelected = true;
        IsActive = true;
    
        GetRoot()->CollectGarbage();
    }
    else
    {
        GetRoot()->GetManager()->StartDraggingFloatingWindowForContent(this, false);
    
        IsSelected = true;
        IsActive = true;
    }
}

/// <summary>
/// Dock the content as document
/// </summary>

void XUI::UI::LayoutContent::DockAsDocument()
{
    auto root = GetRoot();
    if (!root)
        throw System::InvalidOperationException();
    if (GetParent()->Is<LayoutDocumentPane>())
        return;
    
    if (PreviousContainer->Is<LayoutDocumentPane>())
    {
        Dock();
        return;
    }
    
    LayoutDocumentPane* newParentPane;
    if (root->LastFocusedDocument)
    {
        newParentPane = root->LastFocusedDocument->GetParent()->Static_As<LayoutDocumentPane>();
    }
    else
    {
        newParentPane = nullptr;
        LayoutContainer::ContainsChild(root, [&newParentPane](LayoutElement* x)
        {
            return newParentPane = x->Static_As<LayoutDocumentPane>();
        });
    }
    
    if (newParentPane)
    {
        newParentPane->AddChild(this);
        root->CollectGarbage();
    }
    
    IsSelected = true;
    IsActive = true;
}

/// <summary>
/// Re-dock the content to its previous container
/// </summary>

void XUI::UI::LayoutContent::Dock()
{
    if (PreviousContainer)
    {
        auto currentContainer = GetParent();
        auto currentContainerIndex = currentContainer ? currentContainer->IndexOfChild(this) : -1;
        auto previousContainerAsLayoutGroup = PreviousContainer;
    
        if (PreviousContainerIndex < previousContainerAsLayoutGroup->Children.size())
            previousContainerAsLayoutGroup->InsertChildAt(PreviousContainerIndex, this);
        else
            previousContainerAsLayoutGroup->InsertChildAt(previousContainerAsLayoutGroup->Children.size(), this);
    
        if (currentContainerIndex > -1)
        {
            PreviousContainer = currentContainer;
            PreviousContainerIndex = currentContainerIndex;
        }
        else
        {
            PreviousContainer = nullptr;
            PreviousContainerIndex = 0;
        }
    
        IsSelected = true;
        IsActive = true;
    }
    else
    {
        InternalDock();
    }
    
    
    GetRoot()->CollectGarbage();
}

void XUI::UI::LayoutContent::InternalDock()
{

}

void XUI::UI::LayoutContent::OnClosed()
{
    static System::Events::EventArg e;
    if (Closed)
        Closed(this, e);
}

void XUI::UI::LayoutContent::OnIsSelectedChanged(bool oldValue, bool newValue)
{
    static System::Events::EventArg e;
    if (IsSelectedChanged)
        IsSelectedChanged(this, e);
}

void XUI::UI::LayoutContent::OnIsActiveChanged(bool oldValue, bool newValue)
{
    if (newValue)
        _lastActivationTimeStamp = std::chrono::system_clock::now();

    if (IsActiveChanged)
    {
        static System::Events::EventArg e;
        IsActiveChanged(this, e);
    }
}

void XUI::UI::LayoutContent::OnParentChanging(LayoutContainer * oldValue, LayoutContainer * newValue)
{
    auto root = GetRoot();

    if (oldValue)
        IsSelected = false;

    //if (root != null && _isActive && newValue == null)
    //    root.ActiveContent = null;

    LayoutElement::OnParentChanging(oldValue, newValue);
}

void XUI::UI::LayoutContent::OnParentChanged(LayoutContainer * oldValue, LayoutContainer * newValue)
{
    if (IsSelected && GetParent())
    {
        auto parentSelector = GetParent()->Static_As<LayoutGroupBase>();
        if (parentSelector)
            parentSelector->SetSelectedContentIndex(parentSelector->IndexOf(this));
    }

    //var root = Root;
    //if (root != null && _isActive)
    //    root.ActiveContent = this;

    LayoutElement::OnParentChanged(oldValue, newValue);
}
