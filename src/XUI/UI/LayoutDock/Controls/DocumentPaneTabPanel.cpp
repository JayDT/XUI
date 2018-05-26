#include "DocumentPaneTabPanel.h"
#include "LayoutDocumentTabItem.h"
#include "UI/TabItem.h"
#include "UI/LayoutDock/Layouts/LayoutContainer.h"
#include "UI/LayoutDock/Layouts/LayoutContent.h"
#include "UI/LayoutDock/Layouts/LayoutGroupBase.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_POST_INITIALIZER(DocumentPaneTabPanel, XUI::UI::DocumentPaneTabPanel::StaticClassInitializer)

void XUI::UI::DocumentPaneTabPanel::StaticClassInitializer()
{
    ItemsPanelProperty->OverrideDefaultValue<DocumentPaneTabPanel>(TabStrip::DefaultPanel);
}

DocumentPaneTabPanel::DocumentPaneTabPanel()
{
    //FlowDirection = System::Windows::FlowDirection::LeftToRight;
}

std::shared_ptr<Generators::IItemContainerGenerator> DocumentPaneTabPanel::CreateItemContainerGenerator()
{
    return std::make_shared<Generators::ItemContainerGenerator<LayoutDocumentTabItem>>(this, LayoutDocumentTabItem::ModelProperty.get(), Controls::ContentControl::ContentTemplateProperty.get());
}

//Core::Media::Size DocumentPaneTabPanel::MeasureOverride(Core::Media::Size const& availableSize)
//{
//    Core::Media::Size desideredSize;
//    for (auto const& child : Children)
//    {
//        child->Measure(Core::Media::Size(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()));
//        desideredSize.Width += child->DesiredSize.Width;
//    
//        desideredSize.Height = std::max(desideredSize.Height, child->DesiredSize.Height);
//    }
//    
//    return Core::Media::Size(std::min(desideredSize.Width, availableSize.Width), desideredSize.Height);
//}
//
//Core::Media::Size DocumentPaneTabPanel::ArrangeOverride(Core::Media::Size const& finalSize)
//{
//    auto offset = 0.0;
//    auto skipAllOthers = false;
//    for (auto const& child : Children)
//    {
//        if (!child->IsVisible)
//            continue;
//    
//        UI::TabItem* doc = child->Static_As<UI::TabItem>();
//        if (!doc)
//            continue;
//    
//        auto layoutContent = doc->Content.ToObject()->Static_As<LayoutContent>();
//        if (skipAllOthers || offset + doc->DesiredSize.Width > finalSize.Width)
//        {
//            if (layoutContent->IsSelected && !doc->IsVisible)
//            {
//                auto parentContainer = layoutContent->Parent;
//                auto parentSelector = layoutContent->Parent->Static_As<LayoutGroupBase>();
//                auto parentPane = layoutContent->Parent;
//                int contentIndex = parentSelector->IndexOf(layoutContent);
//                if (contentIndex > 0 && parentContainer->Children.size() > 1)
//                {
//                    parentPane->MoveChild(contentIndex, 0);
//                    parentSelector->SetSelectedContentIndex(0);
//                    return ArrangeOverride(finalSize);
//                }
//            }
//            doc->IsVisible = false;
//            skipAllOthers = true;
//        }
//        else
//        {
//            doc->IsVisible = true;
//            doc->Arrange(Core::Media::Rect(offset, 0.0, doc->DesiredSize.Width, finalSize.Height));
//            offset += doc->ActualWidth() + doc->Margin.Left + doc->Margin.Right;
//        }
//    }
//    return finalSize;
//}

void DocumentPaneTabPanel::OnPointerLeave(Interfaces::PointerEventArgs& e)
{
    //if (e.InputModifiers & Interfaces::InputModifiers::LeftMouseButton && LayoutDocumentTabItem::IsDraggingItem())
    //{
    //    auto contentModel = LayoutDocumentTabItem::GetDraggingItem()->Model;
    //    auto manager = contentModel->Root->GetManager();
    //    LayoutDocumentTabItem::ResetDraggingItem();
    //    //System.Diagnostics.Trace.WriteLine("OnMouseLeave()");
    //    manager->StartDraggingFloatingWindowForContent(contentModel);
    //}

    TabStrip::OnPointerLeave(e);
}
