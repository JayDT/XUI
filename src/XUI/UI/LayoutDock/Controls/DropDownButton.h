#ifndef __XUI_DROPDOWNBUTTON_H__
#define __XUI_DROPDOWNBUTTON_H__

#include "UI/Detail/ToggleButton.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/DockingManager.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class DropDownButton : public ToggleButton
    {
    public:
        /// <summary>
        /// DropDownContextMenu Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty DropDownContextMenuProperty;

        /// <summary>
        /// DropDownContextMenuDataContext Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty DropDownContextMenuDataContextProperty;

        static void StaticClassInitializer();

        DropDownButton();
        virtual ~DropDownButton() {}

        /// <summary>
        /// Gets or sets the DropDownContextMenu property.  This dependency property 
        /// indicates drop down menu to show up when user click on an anchorable menu pin.
        /// </summary>
        PROPERTY_(std::shared_ptr<UI::ContextMenu> , DropDownContextMenu);
        std::shared_ptr<UI::ContextMenu> GetDropDownContextMenu() const;
        void SetDropDownContextMenu(std::shared_ptr<UI::ContextMenu> const& value);

        /// <summary>
        /// Gets or sets the DropDownContextMenuDataContext property.  This dependency property 
        /// indicates data context to set for drop down context menu.
        /// </summary>
        PROPERTY_(System::Reflection::Variant, DropDownContextMenuDataContext);
        System::Reflection::Variant GetDropDownContextMenuDataContext() const;
        void SetDropDownContextMenuDataContext(System::Reflection::Variant const& value);

    protected:
        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the DropDownContextMenu property.
        /// </summary>
        virtual void OnDropDownContextMenuChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void OnClick(Core::Observer::RoutedEventArgs & e) override;

    private:
        /// <summary>
        /// Handles changes to the DropDownContextMenu property.
        /// </summary>
        static void OnDropDownContextMenuChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void OnContextMenuClosed(void* sender, System::Events::EventArg & e);

        void DropDownButton_Unloaded(void* sender, Controls::VisualTreeAttachmentEventArgs& e);


    };
}


#endif	//#ifndef __XUI_DROPDOWNBUTTON_H__
