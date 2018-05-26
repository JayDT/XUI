#ifndef __XUI_DROPDOWNCONTROLAREA_H__
#define __XUI_DROPDOWNCONTROLAREA_H__

#include "UI/UserControl.h"
#include "UI/ContextMenu.h"

namespace XUI::UI
{
    class DropDownControlArea : public UserControl
    {
    public:

        static void StaticClassInitializer();

        DropDownControlArea();
        virtual ~DropDownControlArea() {}

        /// <summary>
        /// DropDownContextMenu Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty DropDownContextMenuProperty;

        /// <summary>
        /// DropDownContextMenuDataContext Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty DropDownContextMenuDataContextProperty;

        /// <summary>
        /// Gets or sets the DropDownContextMenu property.  This dependency property 
        /// indicates context menu to show when a right click is detected over the area occpied by the control.
        /// </summary>
        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::ContextMenu>, DropDownContextMenu)

        /// <summary>
        /// Gets or sets the DropDownContextMenuDataContext property.  This dependency property 
        /// indicates data context to attach when context menu is shown.
        /// </summary>
        PROPERTY_(System::Reflection::Variant  , DropDownContextMenuDataContext);
        System::Reflection::Variant GetDropDownContextMenuDataContext() const;
        void SetDropDownContextMenuDataContext(System::Reflection::Variant value);

        #pragma endregion

    protected:
        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void OnPreviewPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;
    };
}


#endif	//#ifndef __XUI_DROPDOWNCONTROLAREA_H__
