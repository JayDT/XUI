#ifndef __XUI_LAYOUTANCHORABLETABITEM_H__
#define __XUI_LAYOUTANCHORABLETABITEM_H__

#include "UI/Detail/TabStripItem.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/DockingManager.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/DocumentPaneTabPanel.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutContent.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutContent;
    class LayoutItem;

    class LayoutAnchorableTabItem : public TabStripItem
    {
    private:
        bool _isMouseDown = false;
        static std::shared_ptr<LayoutAnchorableTabItem> _draggingItem;
        static bool _cancelMouseLeave;

    public:
        /// <summary>
        /// Model Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ModelProperty;
        static XUI::Core::Dependency::RefDependencyProperty LayoutItemProperty;

        LayoutAnchorableTabItem();
        virtual ~LayoutAnchorableTabItem() {}

        static void StaticClassInitializer();

        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::LayoutContent>, Model)
        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::LayoutItem>, LayoutItem)

        static bool IsDraggingItem();

        static LayoutAnchorableTabItem *GetDraggingItem();
        static void ResetDraggingItem();

        static void CancelMouseLeave();

    protected:

        /// <summary>
        /// Handles changes to the Model property.
        /// </summary>
        static void OnModelChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the Model property.
        /// </summary>
        virtual void OnModelChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void OnPointerMoved(Interfaces::PointerEventArgs& e) override;

        void OnPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;

        void OnPointerLeave(Interfaces::PointerEventArgs& e) override;

        void OnPointerEnter(Interfaces::PointerEventArgs& e) override;

        //void OnPreviewGotKeyboardFocus(KeyboardFocusChangedEventArgs *e) override;
    };
}


#endif	//#ifndef __XUI_LAYOUTANCHORABLETABITEM_H__
