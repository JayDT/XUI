#include "LayoutAnchorSideControl.h"
#include "LayoutAnchorSide.h"
#include "LayoutAnchorControl.h"
#include "LayoutAnchorGroupControl.h"
#include "LayoutRoot.h"
#include "DockingManager.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutAnchorSideControl, XUI::UI::LayoutAnchorSideControl::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty LayoutAnchorSideControl::IsLeftSideProperty;
XUI::Core::Dependency::RefDependencyProperty LayoutAnchorSideControl::IsTopSideProperty;
XUI::Core::Dependency::RefDependencyProperty LayoutAnchorSideControl::IsRightSideProperty;
XUI::Core::Dependency::RefDependencyProperty LayoutAnchorSideControl::IsBottomSideProperty;

void XUI::UI::LayoutAnchorSideControl::StaticClassInitializer()
{
    IsLeftSideProperty = (Core::Dependency::BindProperty<bool>::Register<LayoutAnchorSideControl>(nameof(IsLeftSide)));
    IsTopSideProperty = (Core::Dependency::BindProperty<bool>::Register<LayoutAnchorSideControl>(nameof(IsTopSide)));
    IsRightSideProperty = (Core::Dependency::BindProperty<bool>::Register<LayoutAnchorSideControl>(nameof(IsRightSide)));
    IsBottomSideProperty = (Core::Dependency::BindProperty<bool>::Register<LayoutAnchorSideControl>(nameof(IsBottomSide)));

    //DefaultStyleKeyProperty::OverrideMetadata();
}

LayoutAnchorSideControl::LayoutAnchorSideControl()
{

}

LayoutAnchorSideControl::~LayoutAnchorSideControl()
{
    Model = nullptr;
}

void LayoutAnchorSideControl::Initialize(LayoutAnchorSide *model)
{
    if (model == nullptr)
        throw std::invalid_argument("model");

    Model = model->shared_from_base_static<LayoutAnchorSide>();

    CreateChildrenViews();

    UpdateSide();
}

void LayoutAnchorSideControl::SetModel(std::shared_ptr<UI::LayoutAnchorSide> const& value)
{
    if (_model)
        Model->Children.CollectionChanged -= System::make_eventhandler(&LayoutAnchorSideControl::OnModelChanged, this);

    _model = value;

    if (_model)
        _model->Children.CollectionChanged += System::make_eventhandler(&LayoutAnchorSideControl::OnModelChanged, this);
}

void LayoutAnchorSideControl::OnModelChanged(void* s, System::Collection::NotifyCollectionChangedEventArgs& e)
{
    OnModelChildrenCollectionChanged(e);
}

void LayoutAnchorSideControl::CreateChildrenViews()
{
    auto manager = Model->Root->GetManager();
    for (auto childModel : Model->Children.GetNativeEnumerator())
    {
        auto child = manager->CreateUIElementForModel(childModel.get())->Static_As<LayoutAnchorGroupControl>();
        if (child)
            _childViews.Add(child->shared_from_base_static<LayoutAnchorGroupControl>());
    }
}

void LayoutAnchorSideControl::OnModelChildrenCollectionChanged(System::Collection::NotifyCollectionChangedEventArgs& e)
{
    if ((e.Action == System::Collection::NotifyCollectionChangedAction::Remove || e.Action == System::Collection::NotifyCollectionChangedAction::Replace))
    {
        auto oldItem = Model->Children[e.OldStartingIndex];
        for (auto cv = _childViews.GetNativeEnumerator().begin(); cv != _childViews.GetNativeEnumerator().end(); ++cv)
        {
            if ((*cv)->GetModel() == oldItem.get())
            {
                _childViews.Remove(*cv);
                break;
            }
        }
    }

    if (e.Action == System::Collection::NotifyCollectionChangedAction::Reset)
    {
        _childViews.Clear();
    }

    if ((e.Action == System::Collection::NotifyCollectionChangedAction::Add || e.Action == System::Collection::NotifyCollectionChangedAction::Replace))
    {
        auto manager = Model->Root->GetManager();
        auto newItem = Model->Children[e.OldStartingIndex];
        _childViews.Insert(e.NewStartingIndex, manager->CreateUIElementForModel(newItem.get())->shared_from_base_static<LayoutAnchorGroupControl>());
    }
}

System::Collection::List<std::shared_ptr<LayoutAnchorGroupControl>>& LayoutAnchorSideControl::GetChildren()
{
    return _childViews;
}

void LayoutAnchorSideControl::UpdateSide()
{
    switch (Model->Side)
    {
    case AnchorSide::Left:
        SetIsLeftSide(true);
        break;
    case AnchorSide::Top:
        SetIsTopSide(true);
        break;
    case AnchorSide::Right:
        SetIsRightSide(true);
        break;
    case AnchorSide::Bottom:
        SetIsBottomSide(true);
        break;
    }
}
