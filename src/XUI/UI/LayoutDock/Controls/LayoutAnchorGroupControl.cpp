#include "LayoutAnchorGroupControl.h"
#include "LayoutAnchorGroup.h"
#include "LayoutAnchorControl.h"
#include "LayoutRoot.h"
#include "DockingManager.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutAnchorGroupControl, XUI::UI::LayoutAnchorGroupControl::StaticClassInitializer);

void XUI::UI::LayoutAnchorGroupControl::StaticClassInitializer()
{
    //FrameworkPropertyMetadata tempVar(LayoutAnchorGroupControl::typeid);
    //DefaultStyleKeyProperty::OverrideMetadata(LayoutAnchorGroupControl::typeid, &tempVar);
}

LayoutAnchorGroupControl::LayoutAnchorGroupControl(LayoutAnchorGroup *model)
{
    Model = model->shared_from_base_static<LayoutAnchorGroup>();
    CreateChildrenViews();

    Model->Children.CollectionChanged += System::make_eventhandler([this](void* s, System::Collection::NotifyCollectionChangedEventArgs& e)
    {
        OnModelChildrenCollectionChanged(e);
    });
}

void LayoutAnchorGroupControl::CreateChildrenViews()
{
    auto manager = Model->Root->GetManager();
    for (auto childModel : Model->Children.GetNativeEnumerator())
    {
        auto child = std::make_shared<LayoutAnchorControl>(childModel->Static_As<LayoutAnchorable>());
        child->Template = manager->AnchorTemplate;
        _childViews.Add(child);
    }
}

void LayoutAnchorGroupControl::OnModelChildrenCollectionChanged(System::Collection::NotifyCollectionChangedEventArgs& e)
{
    if (e.Action == System::Collection::NotifyCollectionChangedAction::Remove || e.Action == System::Collection::NotifyCollectionChangedAction::Replace)
    {
        auto oldItem = Model->Children[e.OldStartingIndex];
        if (oldItem != nullptr)
        {
            for (auto itr = _childViews.begin(); itr != _childViews.end(); ++itr)
            {
                if ((*itr)->GetModel() == oldItem.get())
                {
                    _childViews.Remove((*itr));
                    break;
                }
            }
        }
    }

    if (e.Action == System::Collection::NotifyCollectionChangedAction::Reset)
    {
        _childViews.Clear();
    }

    if (e.Action == System::Collection::NotifyCollectionChangedAction::Add || e.Action == System::Collection::NotifyCollectionChangedAction::Replace)
    {
        auto newItem = Model->Children[e.NewStartingIndex];
        if (newItem != nullptr)
        {
            auto manager = Model->Root->GetManager();
            {
                int insertIndex = e.NewStartingIndex;
                auto child = std::make_shared<LayoutAnchorControl>(newItem->Static_As<LayoutAnchorable>());
                child->Template = manager->AnchorTemplate;
                _childViews.Insert(insertIndex++, child);
            }
        }
    }
}

