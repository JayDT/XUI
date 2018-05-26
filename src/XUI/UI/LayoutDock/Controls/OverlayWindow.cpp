#include "StdAfx.h"
#include "OverlayWindow.h"
#include "Controls/DropArea.h"
#include "Controls/DocumentPaneDropTarget.h"
#include "Controls/DocumentPaneGroupDropTarget.h"
#include "Controls/LayoutDocumentTabItem.h"
#include "Controls/LayoutDocumentPaneControl.h"
#include "Controls/AnchorablePaneTitle.h"
#include "Controls/LayoutAnchorablePaneControl.h"
#include "Controls/DockingManagerDropTarget.h"
#include "Controls/AnchorablePaneDropTarget.h"
#include "Controls/LayoutAnchorableTabItem.h"
#include "Layouts/LayoutAnchorablePane.h"
#include "Layouts/LayoutDocumentPaneGroup.h"
#include "DockingManager.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_POST_INITIALIZER(OverlayWindow, XUI::UI::OverlayWindow::StaticClassInitializer);

void XUI::UI::OverlayWindow::StaticClassInitializer()
{
    //WindowStyleProperty->OverrideDefaultValue<OverlayWindow>(WindowStyle::None);
    IsVisibleProperty->OverrideDefaultValue<OverlayWindow>(false);
}

XUI::UI::OverlayWindow::OverlayWindow()
{
}

XUI::UI::OverlayWindow::~OverlayWindow()
{
}

std::shared_ptr<IOverlayWindowHost> FindLogicalAncestorIOverlayWindowHost(Controls::Control* control)
{
    Controls::Control* parent = control->Parent;
    if (!parent)
        return nullptr;

    if (parent->Is<IOverlayWindowHost>())
        return parent->shared_from_base_dynamic<IOverlayWindowHost>();

    return FindLogicalAncestorIOverlayWindowHost(parent);
}

void XUI::UI::OverlayWindow::OnTemplateApplied(Controls::TemplateAppliedEventArgs const & e)
{
    Controls::Control::OnTemplateApplied(e);

    _host = FindLogicalAncestorIOverlayWindowHost(this); // std::shared_ptr<IOverlayWindowHost> const& host
    UpdateThemeResources();

    _mainCanvasPanel = Template->FindNameByManagedType<Canvas>("PART_DropTargetsContainer", this);
    _gridDockingManagerDropTargets = Template->FindNameByManagedType<Grid>("PART_DockingManagerDropTargets", this);
    _gridAnchorablePaneDropTargets = Template->FindNameByManagedType<Grid>("PART_AnchorablePaneDropTargets", this);
    _gridDocumentPaneDropTargets = Template->FindNameByManagedType<Grid>("PART_DocumentPaneDropTargets", this);
    _gridDocumentPaneFullDropTargets = Template->FindNameByManagedType<Grid>("PART_DocumentPaneFullDropTargets", this);

    _gridDockingManagerDropTargets->IsVisible = false;
    _gridAnchorablePaneDropTargets->IsVisible = false;
    _gridDocumentPaneDropTargets->IsVisible = false;
    if (_gridDocumentPaneFullDropTargets)
        _gridDocumentPaneFullDropTargets->IsVisible = false;

    _dockingManagerDropTargetBottom = Template->FindNameByManagedType<Controls::Control>("PART_DockingManagerDropTargetBottom", this);
    _dockingManagerDropTargetTop = Template->FindNameByManagedType<Controls::Control>("PART_DockingManagerDropTargetTop", this);
    _dockingManagerDropTargetLeft = Template->FindNameByManagedType<Controls::Control>("PART_DockingManagerDropTargetLeft", this);
    _dockingManagerDropTargetRight = Template->FindNameByManagedType<Controls::Control>("PART_DockingManagerDropTargetRight", this);

    _anchorablePaneDropTargetBottom = Template->FindNameByManagedType<Controls::Control>("PART_AnchorablePaneDropTargetBottom", this);
    _anchorablePaneDropTargetTop = Template->FindNameByManagedType<Controls::Control>("PART_AnchorablePaneDropTargetTop", this);
    _anchorablePaneDropTargetLeft = Template->FindNameByManagedType<Controls::Control>("PART_AnchorablePaneDropTargetLeft", this);
    _anchorablePaneDropTargetRight = Template->FindNameByManagedType<Controls::Control>("PART_AnchorablePaneDropTargetRight", this);
    _anchorablePaneDropTargetInto = Template->FindNameByManagedType<Controls::Control>("PART_AnchorablePaneDropTargetInto", this);

    _documentPaneDropTargetBottom = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneDropTargetBottom", this);
    _documentPaneDropTargetTop = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneDropTargetTop", this);
    _documentPaneDropTargetLeft = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneDropTargetLeft", this);
    _documentPaneDropTargetRight = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneDropTargetRight", this);
    _documentPaneDropTargetInto = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneDropTargetInto", this);

    _documentPaneDropTargetBottomAsAnchorablePane = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneDropTargetBottomAsAnchorablePane", this);
    _documentPaneDropTargetTopAsAnchorablePane = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneDropTargetTopAsAnchorablePane", this);
    _documentPaneDropTargetLeftAsAnchorablePane = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneDropTargetLeftAsAnchorablePane", this);
    _documentPaneDropTargetRightAsAnchorablePane = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneDropTargetRightAsAnchorablePane", this);

    _documentPaneFullDropTargetBottom = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneFullDropTargetBottom", this);
    _documentPaneFullDropTargetTop = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneFullDropTargetTop", this);
    _documentPaneFullDropTargetLeft = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneFullDropTargetLeft", this);
    _documentPaneFullDropTargetRight = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneFullDropTargetRight", this);
    _documentPaneFullDropTargetInto = Template->FindNameByManagedType<Controls::Control>("PART_DocumentPaneFullDropTargetInto", this);

    _previewBox = Template->FindNameByManagedType<Shapes::Path>("PART_PreviewBox", this);
}

std::list<std::shared_ptr<DropTargetBase>> XUI::UI::OverlayWindow::GetTargets()
{
    std::list<std::shared_ptr<DropTargetBase>> _result;
    for (auto visibleArea : _visibleAreas)
    {
        switch (visibleArea->Type)
        {
        case DropAreaType::DockingManager:
        {
            auto dropAreaDockingManager = visibleArea->Static_As<DropArea<DockingManager>>();
            _result.push_back(std::make_shared<DockingManagerDropTarget>(dropAreaDockingManager->AreaElement, _dockingManagerDropTargetLeft->TransformedBounds, DropTargetType::DockingManagerDockLeft));
            _result.push_back(std::make_shared<DockingManagerDropTarget>(dropAreaDockingManager->AreaElement, _dockingManagerDropTargetTop->TransformedBounds, DropTargetType::DockingManagerDockTop));
            _result.push_back(std::make_shared<DockingManagerDropTarget>(dropAreaDockingManager->AreaElement, _dockingManagerDropTargetBottom->TransformedBounds, DropTargetType::DockingManagerDockBottom));
            _result.push_back(std::make_shared<DockingManagerDropTarget>(dropAreaDockingManager->AreaElement, _dockingManagerDropTargetRight->TransformedBounds, DropTargetType::DockingManagerDockRight));
        }
        break;
        case DropAreaType::AnchorablePane:
        {
            auto dropAreaAnchorablePane = visibleArea->Static_As<DropArea<LayoutAnchorablePaneControl>>();
            _result.push_back(std::make_shared<AnchorablePaneDropTarget>(dropAreaAnchorablePane->AreaElement.get(), _anchorablePaneDropTargetLeft->TransformedBounds, DropTargetType::AnchorablePaneDockLeft));
            _result.push_back(std::make_shared<AnchorablePaneDropTarget>(dropAreaAnchorablePane->AreaElement.get(), _anchorablePaneDropTargetTop->TransformedBounds, DropTargetType::AnchorablePaneDockTop));
            _result.push_back(std::make_shared<AnchorablePaneDropTarget>(dropAreaAnchorablePane->AreaElement.get(), _anchorablePaneDropTargetRight->TransformedBounds, DropTargetType::AnchorablePaneDockRight));
            _result.push_back(std::make_shared<AnchorablePaneDropTarget>(dropAreaAnchorablePane->AreaElement.get(), _anchorablePaneDropTargetBottom->TransformedBounds, DropTargetType::AnchorablePaneDockBottom));
            if (_anchorablePaneDropTargetInto->IsVisible)
                _result.push_back(std::make_shared<AnchorablePaneDropTarget>(dropAreaAnchorablePane->AreaElement.get(), _anchorablePaneDropTargetInto->TransformedBounds, DropTargetType::AnchorablePaneDockInside));

            auto parentPaneModel = dropAreaAnchorablePane->AreaElement->Model->Static_As<LayoutAnchorablePane>();
            LayoutAnchorableTabItem* lastAreaTabItem = nullptr;
            AnchorablePaneTitle* dropAreaTitle = nullptr;
            dropAreaAnchorablePane->AreaElement->CallVisualDescendents([&](Controls::Visual* v)
            {
                if (!dropAreaTitle && v->Is<AnchorablePaneTitle>())
                    dropAreaTitle = static_cast<AnchorablePaneTitle*>(v);

                LayoutAnchorableTabItem* dropAreaTabItem = v->Static_As<LayoutAnchorableTabItem>();
                if (!dropAreaTabItem)
                    return;

                auto tabItemModel = dropAreaTabItem->Model->Static_As<LayoutAnchorable>();
                lastAreaTabItem = lastAreaTabItem == nullptr || lastAreaTabItem->TransformedBounds.Right < dropAreaTabItem->TransformedBounds.Right ? dropAreaTabItem : lastAreaTabItem;
                int tabIndex = parentPaneModel->Children.IndexOf(tabItemModel->shared_from_base_static<LayoutElement>());
                _result.push_back(std::make_shared<AnchorablePaneDropTarget>(dropAreaAnchorablePane->AreaElement.get(), dropAreaTabItem->TransformedBounds, DropTargetType::AnchorablePaneDockInside, tabIndex));

            });

            if (lastAreaTabItem != nullptr)
            {
                auto lastAreaTabItemScreenArea = lastAreaTabItem->TransformedBounds;
                auto newAreaTabItemScreenArea = Core::Media::Rect(lastAreaTabItemScreenArea.TopRight, Core::Media::Point(lastAreaTabItemScreenArea.Width, lastAreaTabItemScreenArea.Bottom));
                if (newAreaTabItemScreenArea.Right < dropAreaAnchorablePane->AreaElement->TransformedBounds.Right)
                    _result.push_back(std::make_shared<AnchorablePaneDropTarget>(dropAreaAnchorablePane->AreaElement.get(), newAreaTabItemScreenArea, DropTargetType::AnchorablePaneDockInside, parentPaneModel->Children.size()));
            }

            if (dropAreaTitle)
                _result.push_back(std::make_shared<AnchorablePaneDropTarget>(dropAreaAnchorablePane->AreaElement.get(), dropAreaTitle->TransformedBounds, DropTargetType::AnchorablePaneDockInside));
        }
        break;
        case DropAreaType::DocumentPane:
        {
            bool isDraggingAnchorables = _floatingWindow->GetModel()->Static_As<LayoutAnchorableFloatingWindow>();
            if (isDraggingAnchorables && _gridDocumentPaneFullDropTargets)
            {
                auto dropAreaDocumentPane = visibleArea->Static_As<DropArea<LayoutDocumentPaneControl>>();
                if (_documentPaneFullDropTargetLeft->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneFullDropTargetLeft->TransformedBounds, DropTargetType::DocumentPaneDockLeft));
                if (_documentPaneFullDropTargetTop->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneFullDropTargetTop->TransformedBounds, DropTargetType::DocumentPaneDockTop));
                if (_documentPaneFullDropTargetRight->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneFullDropTargetRight->TransformedBounds, DropTargetType::DocumentPaneDockRight));
                if (_documentPaneFullDropTargetBottom->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneFullDropTargetBottom->TransformedBounds, DropTargetType::DocumentPaneDockBottom));
                if (_documentPaneFullDropTargetInto->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneFullDropTargetInto->TransformedBounds, DropTargetType::DocumentPaneDockInside));

                auto parentPaneModel = dropAreaDocumentPane->AreaElement->GetModel()->Static_As<LayoutDocumentPane>();
                LayoutDocumentTabItem* lastAreaTabItem = nullptr;
                dropAreaDocumentPane->AreaElement->CallVisualDescendents([&](Controls::Visual* v)
                {
                    LayoutDocumentTabItem* dropAreaTabItem = v->Static_As<LayoutDocumentTabItem>();
                    if (!dropAreaTabItem)
                        return;

                    auto tabItemModel = dropAreaTabItem->Model;
                    lastAreaTabItem = lastAreaTabItem == nullptr || lastAreaTabItem->TransformedBounds.Right < dropAreaTabItem->TransformedBounds.Right ? dropAreaTabItem : lastAreaTabItem;
                    int tabIndex = parentPaneModel->Children.IndexOf(tabItemModel->shared_from_base_static<LayoutElement>());
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), dropAreaTabItem->TransformedBounds, DropTargetType::DocumentPaneDockInside, tabIndex));
                });

                if (lastAreaTabItem)
                {
                    auto lastAreaTabItemScreenArea = lastAreaTabItem->TransformedBounds;
                    auto newAreaTabItemScreenArea = Core::Media::Rect(lastAreaTabItemScreenArea.TopRight, Core::Media::Point(lastAreaTabItemScreenArea.Width, lastAreaTabItemScreenArea.Height));
                    if (newAreaTabItemScreenArea.Right < dropAreaDocumentPane->AreaElement->TransformedBounds.Right)
                        _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), newAreaTabItemScreenArea, DropTargetType::DocumentPaneDockInside, parentPaneModel->Children.size()));
                }

                if (_documentPaneDropTargetLeftAsAnchorablePane->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneDropTargetLeftAsAnchorablePane->TransformedBounds, DropTargetType::DocumentPaneDockAsAnchorableLeft));
                if (_documentPaneDropTargetTopAsAnchorablePane->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneDropTargetTopAsAnchorablePane->TransformedBounds, DropTargetType::DocumentPaneDockAsAnchorableTop));
                if (_documentPaneDropTargetRightAsAnchorablePane->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneDropTargetRightAsAnchorablePane->TransformedBounds, DropTargetType::DocumentPaneDockAsAnchorableRight));
                if (_documentPaneDropTargetBottomAsAnchorablePane->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneDropTargetBottomAsAnchorablePane->TransformedBounds, DropTargetType::DocumentPaneDockAsAnchorableBottom));
            }
            else
            {

                auto dropAreaDocumentPane = visibleArea->Static_As<DropArea<LayoutDocumentPaneControl>>();
                if (_documentPaneDropTargetLeft->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneDropTargetLeft->TransformedBounds, DropTargetType::DocumentPaneDockLeft));
                if (_documentPaneDropTargetTop->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneDropTargetTop->TransformedBounds, DropTargetType::DocumentPaneDockTop));
                if (_documentPaneDropTargetRight->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneDropTargetRight->TransformedBounds, DropTargetType::DocumentPaneDockRight));
                if (_documentPaneDropTargetBottom->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneDropTargetBottom->TransformedBounds, DropTargetType::DocumentPaneDockBottom));
                if (_documentPaneDropTargetInto->IsVisible)
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneDropTargetInto->TransformedBounds, DropTargetType::DocumentPaneDockInside));

                auto parentPaneModel = dropAreaDocumentPane->AreaElement->Model->Static_As<LayoutDocumentPane>();
                LayoutDocumentTabItem* lastAreaTabItem = nullptr;
                dropAreaDocumentPane->AreaElement->CallVisualDescendents([&](Controls::Visual* v)
                {
                    LayoutDocumentTabItem* dropAreaTabItem = v->Static_As<LayoutDocumentTabItem>();
                    if (!dropAreaTabItem)
                        return;

                    auto tabItemModel = dropAreaTabItem->Model;
                    lastAreaTabItem = lastAreaTabItem == nullptr || lastAreaTabItem->TransformedBounds.Right < dropAreaTabItem->TransformedBounds.Right ? dropAreaTabItem : lastAreaTabItem;
                    int tabIndex = parentPaneModel->Children.IndexOf(tabItemModel->shared_from_base_static<LayoutElement>());
                    _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), dropAreaTabItem->TransformedBounds, DropTargetType::DocumentPaneDockInside, tabIndex));
                });

                if (lastAreaTabItem)
                {
                    auto lastAreaTabItemScreenArea = lastAreaTabItem->TransformedBounds;
                    auto newAreaTabItemScreenArea = Core::Media::Rect(lastAreaTabItemScreenArea.TopRight, Core::Media::Point(lastAreaTabItemScreenArea.Width, lastAreaTabItemScreenArea.Height));
                    if (newAreaTabItemScreenArea.Right < dropAreaDocumentPane->AreaElement->TransformedBounds.Right)
                        _result.push_back(std::make_shared<DocumentPaneDropTarget>(dropAreaDocumentPane->AreaElement.get(), newAreaTabItemScreenArea, DropTargetType::DocumentPaneDockInside, parentPaneModel->Children.size()));
                }
            }
        }
        break;
        case DropAreaType::DocumentPaneGroup:
        {
            auto dropAreaDocumentPane = visibleArea->Static_As<DropArea<LayoutDocumentPaneGroupControl>>();
            if (_documentPaneDropTargetInto->IsVisible)
                _result.push_back(std::make_shared<DocumentPaneGroupDropTarget>(dropAreaDocumentPane->AreaElement.get(), _documentPaneDropTargetInto->TransformedBounds, DropTargetType::DocumentPaneGroupDockInside));
        }
        break;
        }

    }

    return std::move(_result);
}

/// <summary>
/// Find any LayoutDocument or LayoutAnchorable from a given source (e.g. LayoutDocumentPane, LayoutAnchorableFloatingWindow, etc.)
/// </summary>
/// <param name="source">The given source to search in</param>
/// <returns>A list of all LayoutContent's</returns>

void XUI::UI::OverlayWindow::GetAllLayoutContents(System::Reflection::Object * source, std::list<LayoutContent*>& result)
{
    auto documentFloatingWindow = source->Static_As<LayoutDocumentFloatingWindow>();
    if (documentFloatingWindow)
    {
        for (auto const& layoutElement : documentFloatingWindow->Children.GetNativeEnumerator())
        {
            GetAllLayoutContents(layoutElement.get(), result);
        }
    }

    auto anchorableFloatingWindow = source->Static_As<LayoutAnchorableFloatingWindow>();
    if (anchorableFloatingWindow)
    {
        for (auto const&layoutElement : anchorableFloatingWindow->Children.GetNativeEnumerator())
        {
            GetAllLayoutContents(layoutElement.get(), result);
        }
    }

    auto documentPaneGroup = source->Static_As<LayoutDocumentPaneGroup>();
    if (documentPaneGroup)
    {
        for (auto const& layoutDocumentPane : documentPaneGroup->Children.GetNativeEnumerator())
        {
            GetAllLayoutContents(layoutDocumentPane.get(), result);
        }
    }

    auto anchorablePaneGroup = source->Static_As<LayoutAnchorablePaneGroup>();
    if (anchorablePaneGroup)
    {
        for (auto const&layoutDocumentPane : anchorablePaneGroup->Children.GetNativeEnumerator())
        {
            GetAllLayoutContents(layoutDocumentPane.get(), result);
        }
    }

    auto documentPane = source->Static_As<LayoutDocumentPane>();
    if (documentPane)
    {
        for (auto const&layoutContent : documentPane->Children.GetNativeEnumerator())
        {
            result.push_back(layoutContent->Static_As<LayoutContent>());
        }
    }

    auto anchorablePane = source->Static_As<LayoutAnchorablePane>();
    if (anchorablePane)
    {
        for (auto const&layoutContent : anchorablePane->Children.GetNativeEnumerator())
        {
            result.push_back(layoutContent->Static_As<LayoutContent>());
        }
    }

    auto document = source->Static_As<LayoutDocument>();
    if (document)
    {
        result.push_back(document);
    }

    auto anchorable = source->Static_As<LayoutAnchorable>();
    if (anchorable)
    {
        result.push_back(anchorable);
    }
}

void XUI::UI::OverlayWindow::SetFloatingWindow(LayoutFloatingWindowControl* wnd)
{
    _floatingWindow = wnd ? wnd->shared_from_base_static<LayoutFloatingWindowControl>() : nullptr;
}

void XUI::UI::OverlayWindow::DragEnter(LayoutFloatingWindowControl * floatingWindow)
{
    _floatingWindow = floatingWindow->shared_from_base_static<LayoutFloatingWindowControl>();
    EnableDropTargets();
}

void XUI::UI::OverlayWindow::DragLeave(LayoutFloatingWindowControl * floatingWindow)
{
    IsVisible = false;
    _floatingWindow = nullptr;
}

void XUI::UI::OverlayWindow::SetDropTargetIntoVisibility(LayoutGroupBase *positionableElement)
{
    if (positionableElement->Is<LayoutAnchorablePane>())
    {
        _anchorablePaneDropTargetInto->IsVisible = true;
    }
    else if (positionableElement->Is<LayoutDocumentPane>())
    {
        _documentPaneDropTargetInto->IsVisible = true;
    }

    if (positionableElement == nullptr || _floatingWindow->GetModel() == nullptr || positionableElement->GetAllowDuplicateContent())
        return;

    // Find all content layouts in the anchorable pane (object to drop on)
    std::list<LayoutContent*> contentLayoutsOnPositionableElementPane;
    GetAllLayoutContents(positionableElement, contentLayoutsOnPositionableElementPane);

    // Find all content layouts in the floating window (object to drop)
    std::list<LayoutContent*> contentLayoutsOnFloatingWindow;
    GetAllLayoutContents(_floatingWindow->GetModel().get(), contentLayoutsOnFloatingWindow);

    // If any of the content layouts is present in the drop area, then disable the DropTargetInto button.
    for (auto content : contentLayoutsOnFloatingWindow)
    {
        bool has_same = false;
        for (auto pos_content : contentLayoutsOnPositionableElementPane)
        {
            if (pos_content->Title == content->Title /*&& pos_content->contentid == content->contentid*/)
            {
                has_same = true;
                break;
            }
        }

        if (!has_same)
            continue;

        if (positionableElement->Is<LayoutAnchorablePane>())
        {
            _anchorablePaneDropTargetInto->IsVisible = true;
        }
        else if (positionableElement->Is<LayoutDocumentPane>())
        {
            _documentPaneDropTargetInto->IsVisible = true;
        }
        break;
    }
}

void XUI::UI::OverlayWindow::DragEnter(IDropArea * area)
{
    _visibleAreas.push_back(area->shared_from_base_static<IDropArea>());

    Controls::Control* areaElement = nullptr;
    switch (area->Type)
    {
    case DropAreaType::DockingManager:
        areaElement = _gridDockingManagerDropTargets.get();
        break;
    case DropAreaType::AnchorablePane:
    {
        areaElement = _gridAnchorablePaneDropTargets.get();

        auto dropAreaAnchorablePaneGroup = area->Static_As<DropArea<LayoutAnchorablePaneControl>>();
        auto layoutAnchorablePane = dropAreaAnchorablePaneGroup->AreaElement->GetModel()->Static_As<LayoutAnchorablePane>();
        SetDropTargetIntoVisibility(layoutAnchorablePane);
        break;
    }
    case DropAreaType::DocumentPaneGroup:
    {
        areaElement = _gridDocumentPaneDropTargets.get();
        auto dropAreaDocumentPaneGroup = area->Static_As<DropArea<LayoutDocumentPaneGroupControl>>();
        auto layoutDocumentPane = (*dropAreaDocumentPaneGroup->AreaElement->GetModel()->Static_As<LayoutDocumentPaneGroup>()->Children.GetNativeEnumerator().begin())->Static_As<LayoutDocumentPane>();
        auto parentDocumentPaneGroup = layoutDocumentPane->Parent->Static_As<LayoutDocumentPaneGroup>();

        _documentPaneDropTargetLeft->IsVisible = false;
        _documentPaneDropTargetRight->IsVisible = false;
        _documentPaneDropTargetTop->IsVisible = false;
        _documentPaneDropTargetBottom->IsVisible = false;
    }
    break;
    case DropAreaType::DocumentPane:
    default:
    {
        bool isDraggingAnchorables = _floatingWindow->GetModel()->Is<LayoutAnchorableFloatingWindow>();
        if (isDraggingAnchorables && _gridDocumentPaneFullDropTargets != nullptr)
        {
            areaElement = _gridDocumentPaneFullDropTargets.get();
            auto dropAreaDocumentPaneGroup = area->Static_As<DropArea<LayoutDocumentPaneControl>>();
            auto layoutDocumentPane = dropAreaDocumentPaneGroup->AreaElement->GetModel()->Static_As<LayoutDocumentPane>();
            auto parentDocumentPaneGroup = layoutDocumentPane->Parent->Static_As<LayoutDocumentPaneGroup>();

            SetDropTargetIntoVisibility(layoutDocumentPane);

            if (parentDocumentPaneGroup && LayoutContainer::ContainsChild(parentDocumentPaneGroup, [](LayoutElement* x) { return x->GetIsVisible(); }, false))
            {
                auto manager = parentDocumentPaneGroup->Root->GetManager();
                if (!manager->AllowMixedOrientation)
                {
                    _documentPaneFullDropTargetLeft->IsVisible = parentDocumentPaneGroup->Orientation == UI::Orientation::Horizontal ? true : false;
                    _documentPaneFullDropTargetRight->IsVisible = parentDocumentPaneGroup->Orientation == UI::Orientation::Horizontal ? true : false;
                    _documentPaneFullDropTargetTop->IsVisible = parentDocumentPaneGroup->Orientation == UI::Orientation::Vertical ? true : false;
                    _documentPaneFullDropTargetBottom->IsVisible = parentDocumentPaneGroup->Orientation == UI::Orientation::Vertical ? true : false;
                }
                else
                {
                    _documentPaneFullDropTargetLeft->IsVisible = true;
                    _documentPaneFullDropTargetRight->IsVisible = true;
                    _documentPaneFullDropTargetTop->IsVisible = true;
                    _documentPaneFullDropTargetBottom->IsVisible = true;
                }
            }
            else if (!parentDocumentPaneGroup &&
                layoutDocumentPane &&
                layoutDocumentPane->Children.empty())
            {
                _documentPaneFullDropTargetLeft->IsVisible = false;
                _documentPaneFullDropTargetRight->IsVisible = false;
                _documentPaneFullDropTargetTop->IsVisible = false;
                _documentPaneFullDropTargetBottom->IsVisible = false;
            }
            else
            {
                _documentPaneFullDropTargetLeft->IsVisible = true;
                _documentPaneFullDropTargetRight->IsVisible = true;
                _documentPaneFullDropTargetTop->IsVisible = true;
                _documentPaneFullDropTargetBottom->IsVisible = true;
            }

            if (parentDocumentPaneGroup && LayoutContainer::ContainsChild(parentDocumentPaneGroup, [](LayoutElement* x) { return x->GetIsVisible(); }, false))
            {
                int indexOfDocumentPane = parentDocumentPaneGroup->Children.IndexOf(layoutDocumentPane->shared_from_base_static<LayoutElement>());
                bool isFirstChild = indexOfDocumentPane == 0;
                bool isLastChild = indexOfDocumentPane == parentDocumentPaneGroup->Children.size() - 1;

                auto manager = parentDocumentPaneGroup->Root->GetManager();
                if (!manager->AllowMixedOrientation)
                {
                    _documentPaneDropTargetBottomAsAnchorablePane->IsVisible =
                        parentDocumentPaneGroup->Orientation == UI::Orientation::Vertical ?
                        (isLastChild ? true : false) :
                        false;
                    _documentPaneDropTargetTopAsAnchorablePane->IsVisible =
                        parentDocumentPaneGroup->Orientation == UI::Orientation::Vertical ?
                        (isFirstChild ? true : false) :
                        false;

                    _documentPaneDropTargetLeftAsAnchorablePane->IsVisible =
                        parentDocumentPaneGroup->Orientation == UI::Orientation::Horizontal ?
                        (isFirstChild ? true : false) :
                        false;


                    _documentPaneDropTargetRightAsAnchorablePane->IsVisible =
                        parentDocumentPaneGroup->Orientation == UI::Orientation::Horizontal ?
                        (isLastChild ? true : false) :
                        false;
                }
                else
                {
                    _documentPaneDropTargetBottomAsAnchorablePane->IsVisible = true;
                    _documentPaneDropTargetLeftAsAnchorablePane->IsVisible = true;
                    _documentPaneDropTargetRightAsAnchorablePane->IsVisible = true;
                    _documentPaneDropTargetTopAsAnchorablePane->IsVisible = true;
                }
            }
            else
            {
                _documentPaneDropTargetBottomAsAnchorablePane->IsVisible = true;
                _documentPaneDropTargetLeftAsAnchorablePane->IsVisible = true;
                _documentPaneDropTargetRightAsAnchorablePane->IsVisible = true;
                _documentPaneDropTargetTopAsAnchorablePane->IsVisible = true;
            }
        }
        else
        {
            areaElement = _gridDocumentPaneDropTargets.get();
            auto dropAreaDocumentPaneGroup = area->Static_As<DropArea<LayoutDocumentPaneControl>>();
            auto layoutDocumentPane = dropAreaDocumentPaneGroup->AreaElement->GetModel()->Static_As<LayoutDocumentPane>();
            auto parentDocumentPaneGroup = layoutDocumentPane->Parent->Static_As<LayoutDocumentPaneGroup>();

            SetDropTargetIntoVisibility(layoutDocumentPane);

            if (parentDocumentPaneGroup && LayoutContainer::ContainsChild(parentDocumentPaneGroup, [](LayoutElement* x) { return x->GetIsVisible(); }, false))
            {
                auto manager = parentDocumentPaneGroup->Root->GetManager();
                if (!manager->AllowMixedOrientation)
                {
                    _documentPaneDropTargetLeft->IsVisible = parentDocumentPaneGroup->Orientation == UI::Orientation::Horizontal ? true : false;
                    _documentPaneDropTargetRight->IsVisible = parentDocumentPaneGroup->Orientation == UI::Orientation::Horizontal ? true : false;
                    _documentPaneDropTargetTop->IsVisible = parentDocumentPaneGroup->Orientation == UI::Orientation::Vertical ? true : false;
                    _documentPaneDropTargetBottom->IsVisible = parentDocumentPaneGroup->Orientation == UI::Orientation::Vertical ? true : false;
                }
                else
                {
                    _documentPaneDropTargetLeft->IsVisible = true;
                    _documentPaneDropTargetRight->IsVisible = true;
                    _documentPaneDropTargetTop->IsVisible = true;
                    _documentPaneDropTargetBottom->IsVisible = true;
                }

            }
            else if (!parentDocumentPaneGroup &&
                layoutDocumentPane &&
                layoutDocumentPane->Children.empty())
            {
                _documentPaneDropTargetLeft->IsVisible = false;
                _documentPaneDropTargetRight->IsVisible = false;
                _documentPaneDropTargetTop->IsVisible = false;
                _documentPaneDropTargetBottom->IsVisible = false;
            }
            else
            {
                _documentPaneDropTargetLeft->IsVisible = true;
                _documentPaneDropTargetRight->IsVisible = true;
                _documentPaneDropTargetTop->IsVisible = true;
                _documentPaneDropTargetBottom->IsVisible = true;
            }
        }
    }
    break;
    }

    Canvas::SetLeft(areaElement, area->GetDetectionRect().Left - TransformedBounds.Left);
    Canvas::SetTop(areaElement, area->GetDetectionRect().Top - TransformedBounds.Top);
    areaElement->Width = area->GetDetectionRect().Width;
    areaElement->Height = area->GetDetectionRect().Height;
    areaElement->IsVisible = true;
}

void XUI::UI::OverlayWindow::DragLeave(IDropArea * area)
{
    _visibleAreas.remove(area->shared_from_base_static<IDropArea>());

    Controls::Control* areaElement = nullptr;
    switch (area->Type)
    {
    case DropAreaType::DockingManager:
        areaElement = _gridDockingManagerDropTargets.get();
        break;
    case DropAreaType::AnchorablePane:
        areaElement = _gridAnchorablePaneDropTargets.get();
        break;
    case DropAreaType::DocumentPaneGroup:
        areaElement = _gridDocumentPaneDropTargets.get();
        break;
    case DropAreaType::DocumentPane:
    default:
    {
        bool isDraggingAnchorables = _floatingWindow->GetModel()->Static_As<LayoutAnchorableFloatingWindow>();
        if (isDraggingAnchorables && _gridDocumentPaneFullDropTargets)
            areaElement = _gridDocumentPaneFullDropTargets.get();
        else
            areaElement = _gridDocumentPaneDropTargets.get();
    }
    break;
    }

    areaElement->IsVisible = false;
}

void XUI::UI::OverlayWindow::DragEnter(DropTargetBase * target)
{
    auto previewBoxPath = target->GetPreviewPath(this, _floatingWindow->GetModel()->Static_As<LayoutFloatingWindow>());
    if (previewBoxPath)
    {
        _previewBox->Data = previewBoxPath;
        _previewBox->IsVisible = true;
    }
}

void XUI::UI::OverlayWindow::DragLeave(DropTargetBase * target)
{
    _previewBox->IsVisible = false;
}

void XUI::UI::OverlayWindow::DragDrop(DropTargetBase * target)
{
    target->Drop(_floatingWindow->GetModel()->Static_As<LayoutFloatingWindow>());
}

void XUI::UI::OverlayWindow::OnClosing(CancelEventArgs & e)
{
}
