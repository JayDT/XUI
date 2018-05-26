#include "LayoutDocumentFloatingWindowControl.h"
#include "LayoutItem.h"
#include "DockingManager.h"
#include "LayoutRoot.h"
#include "LayoutDocumentFloatingWindow.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

//STATIC_CLASS_INITIALIZER(LayoutDocumentFloatingWindowControl, XUI::UI::LayoutDocumentFloatingWindowControl::StaticClassInitializer);

//LayoutDocumentFloatingWindowControl::StaticConstructor::StaticConstructor()
//{
//  FrameworkPropertyMetadata tempVar(LayoutDocumentFloatingWindowControl::typeid);
//  DefaultStyleKeyProperty::OverrideMetadata(LayoutDocumentFloatingWindowControl::typeid, &tempVar);
//}

LayoutDocumentFloatingWindowControl::LayoutDocumentFloatingWindowControl(LayoutDocumentFloatingWindow *model)
    : LayoutFloatingWindowControl(model)
{
    //UpdateThemeResources();
}

LayoutItem *LayoutDocumentFloatingWindowControl::GetRootDocumentLayoutItem() const
{
    return Model->Root->GetManager()->GetLayoutItemFromModel(static_cast<LayoutDocumentFloatingWindow*>(Model.get())->RootDocument.get());
}

void LayoutDocumentFloatingWindowControl::OnWindowInitialized()
{
    LayoutFloatingWindowControl::OnWindowInitialized();

    if (static_cast<LayoutDocumentFloatingWindow*>(Model.get())->RootDocument == nullptr)
    {
        InternalClose();
    }
    else
    {
        auto manager = Model->Root->GetManager();

        Content = manager->CreateUIElementForModel(static_cast<LayoutDocumentFloatingWindow*>(Model.get())->RootDocument.get());

        static_cast<LayoutDocumentFloatingWindow*>(Model.get())->RootDocumentChanged += System::make_eventhandler(&LayoutDocumentFloatingWindowControl::_model_RootDocumentChanged, this);
    }
}

void LayoutDocumentFloatingWindowControl::_model_RootDocumentChanged(void* sender, System::Events::EventArg& e)
{
    if (static_cast<LayoutDocumentFloatingWindow*>(Model.get())->RootDocument == nullptr)
    {
        InternalClose();
    }
}

bool LayoutDocumentFloatingWindowControl::OpenContextMenu()
{
    //auto ctxMenu = Model->Root.Manager.DocumentContextMenu;
    //if (ctxMenu != nullptr && RootDocumentLayoutItem != nullptr)
    //{
    //    ctxMenu->PlacementTarget = nullptr;
    //    ctxMenu->Placement = PlacementMode::MousePoint;
    //    ctxMenu->DataContext = RootDocumentLayoutItem;
    //    ctxMenu->IsOpen = true;
    //    return true;
    //}

    return false;
}

void LayoutDocumentFloatingWindowControl::HandleClosed()
{
    auto root = Model->Root;
    root->GetManager()->RemoveFloatingWindow(this);
    root->CollectGarbage();

    LayoutFloatingWindowControl::HandleClosed();

    if (!GetCloseInitiatedByUser())
        root->FloatingWindows.Remove(Model->shared_from_base_static<LayoutDocumentFloatingWindow>());

    static_cast<LayoutDocumentFloatingWindow*>(Model.get())->RootDocumentChanged -= System::make_eventhandler(&LayoutDocumentFloatingWindowControl::_model_RootDocumentChanged, this);
}

