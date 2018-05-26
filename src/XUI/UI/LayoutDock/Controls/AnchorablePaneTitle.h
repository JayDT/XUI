#ifndef __XUI_ANCHORABLEPANETITLE_H__
#define __XUI_ANCHORABLEPANETITLE_H__

#include "UI/Controls/Control.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorable.h"
#include "UI/LayoutDock/Controls/LayoutItem.h"

namespace XUI::UI
{

    class AnchorablePaneTitle : public Controls::Control
    {
    private:
        bool _isMouseDown = false;

    public:

        /// <summary>
        /// Model Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ModelProperty;
        static XUI::Core::Dependency::RefDependencyProperty LayoutItemProperty;

        static void StaticClassInitializer();

        AnchorablePaneTitle();
        virtual ~AnchorablePaneTitle() {}

        /// <summary>
        /// Gets or sets the Model property.  This dependency property 
        /// indicates model attached to this view.
        /// </summary>
        PROPERTY_(std::shared_ptr<UI::LayoutAnchorable>, Model);
        std::shared_ptr<UI::LayoutAnchorable> GetModel() const { return GetValue<std::shared_ptr<UI::LayoutAnchorable>>(ModelProperty); }
        void SetModel(std::shared_ptr<UI::LayoutAnchorable> const& value)
        {
            SetValue(ModelProperty, value);
        }

        /// <summary>
        /// Gets the LayoutItem property.  This dependency property 
        /// indicates the LayoutItem attached to this tag item.
        /// </summary>
        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::LayoutItem>, LayoutItem)

    protected:
        static void _OnModelChanged(IDependencyObject *sender, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the Model property.
        /// </summary>
        virtual void OnModelChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);
        void OnHide();
        void OnToggleAutoHide();

        void OnPointerMoved(Interfaces::PointerEventArgs& e) override;
        void OnPointerLeave(Interfaces::PointerEventArgs& e) override;
        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;
        void OnPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;
    };
}


#endif	//#ifndef __XUI_ANCHORABLEPANETITLE_H__
