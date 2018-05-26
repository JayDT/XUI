#include "AnchorablePaneTabPanel.h"
#include "LayoutAnchorable.h"
#include "LayoutRoot.h"
#include "DockingManager.h"
#include "LayoutAnchorableTabItem.h"
#include "cpplinq/linq.hpp"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_POST_INITIALIZER(AnchorablePaneTabPanel, XUI::UI::AnchorablePaneTabPanel::StaticClassInitializer)

void XUI::UI::AnchorablePaneTabPanel::StaticClassInitializer()
{
    ItemsPanelProperty->OverrideDefaultValue<AnchorablePaneTabPanel>(TabStrip::DefaultPanel);
}

AnchorablePaneTabPanel::AnchorablePaneTabPanel()
{
    //FlowDirection = FlowDirection::LeftToRight;
}

std::shared_ptr<Generators::IItemContainerGenerator> AnchorablePaneTabPanel::CreateItemContainerGenerator()
{
    return std::make_shared<Generators::ItemContainerGenerator<LayoutAnchorableTabItem>>(this, LayoutAnchorableTabItem::ModelProperty.get(), Controls::ContentControl::ContentTemplateProperty.get());
}

//Core::Media::Size AnchorablePaneTabPanel::MeasureOverride(Core::Media::Size const& availableSize)
//{
//    double totWidth = 0;
//    double maxHeight = 0;
//    int count = 0;
//
//    for (auto child : Children.GetNativeEnumerator())
//    {
//        if (!child->IsVisible)
//            continue;
//
//        ++count;
//        child->Measure(Core::Media::Size(std::numeric_limits<double>::max(), availableSize.Height));
//        totWidth += child->DesiredSize.Width;
//        maxHeight = std::max(maxHeight, child->DesiredSize.Height);
//    }
//
//    if (totWidth > availableSize.Width)
//    {
//        double childFinalDesideredWidth = availableSize.Width / count;
//        for (auto child : Children.GetNativeEnumerator())
//        {
//            if (!child->IsVisible)
//                continue;
//
//            child->Measure(Core::Media::Size(childFinalDesideredWidth, availableSize.Height));
//        }
//    }
//
//    return Core::Media::Size(std::min(availableSize.Width, totWidth), maxHeight);
//}
//
//Core::Media::Size AnchorablePaneTabPanel::ArrangeOverride(Core::Media::Size const& finalSize)
//{
//    double finalWidth = finalSize.Width;
//    double desideredWidth = 0.0;
//    double offsetX = 0.0;
//    int count = 0;
//
//    for (auto child : Children.GetNativeEnumerator())
//    {
//        if (!child->IsVisible)
//            continue;
//
//        ++count;
//        desideredWidth += child->DesiredSize.Width;
//    }
//
//    if (finalWidth > desideredWidth)
//    {
//        for (auto child : Children.GetNativeEnumerator())
//        {
//            if (!child->IsVisible)
//                continue;
//
//            double childFinalWidth = child->DesiredSize.Width;
//            child->Arrange(Core::Media::Rect(offsetX, 0, childFinalWidth, finalSize.Height));
//
//            offsetX += childFinalWidth;
//        }
//    }
//    else
//    {
//        double childFinalWidth = finalWidth / count;
//        for (auto child : Children.GetNativeEnumerator())
//        {
//            if (!child->IsVisible)
//                continue;
//
//            child->Arrange(Core::Media::Rect(offsetX, 0, childFinalWidth, finalSize.Height));
//
//            offsetX += childFinalWidth;
//        }
//    }
//
//    return finalSize;
//}

void AnchorablePaneTabPanel::OnPointerLeave(Interfaces::PointerEventArgs& e)
{
    if (e.InputModifiers & Interfaces::InputModifiers::LeftMouseButton && LayoutAnchorableTabItem::IsDraggingItem())
    {
        LayoutAnchorable* contentModel = LayoutAnchorableTabItem::GetDraggingItem()->Model->Static_As<LayoutAnchorable>();
        auto manager = contentModel->Root->GetManager();
        LayoutAnchorableTabItem::ResetDraggingItem();

        manager->StartDraggingFloatingWindowForContent(contentModel);
    }

    TabStrip::OnPointerLeave(e);
}
