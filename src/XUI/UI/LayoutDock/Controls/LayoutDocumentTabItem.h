#ifndef __XUI_LAYOUTDOCUMENTTABITEM_H__
#define __XUI_LAYOUTDOCUMENTTABITEM_H__

#include "UI/Detail/TabStripItem.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/DockingManager.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/DocumentPaneTabPanel.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutContent.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DocumentPaneTabPanel;
    class TabItem;
    class LayoutContent;
    class LayoutItem;

    class LayoutDocumentTabItem : public TabStripItem
    {
    private:
        std::shared_ptr<DocumentPaneTabPanel> _parentDocumentTabPanel;
        Core::Media::Point _mouseDownPoint;
        bool _isMouseDown = false;

    public:

        /// <summary>
        /// Model Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ModelProperty;
        static XUI::Core::Dependency::RefDependencyProperty LayoutItemProperty;

        static void StaticClassInitializer();

        LayoutDocumentTabItem();
        virtual ~LayoutDocumentTabItem() {}

        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::LayoutContent>, Model)
        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::LayoutItem>, LayoutItem)

    protected:
        static void OnModelChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs & e);


        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the Model property.
        /// </summary>
        virtual void OnModelChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e);

        void UpdateDragDetails();

        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void OnPointerMoved(Interfaces::PointerEventArgs&  e) override;

        void OnPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;

        void OnPointerLeave(Interfaces::PointerEventArgs& e) override;

        void OnPointerEnter(Interfaces::PointerEventArgs& e) override;
    };
}


#endif	//#ifndef __XUI_LAYOUTDOCUMENTTABITEM_H__
