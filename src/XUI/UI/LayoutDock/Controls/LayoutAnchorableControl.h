#ifndef __XUI_LAYOUTANCHORABLECONTROL_H__
#define __XUI_LAYOUTANCHORABLECONTROL_H__

#include "UI/Controls/ContentControl.h"
#include "UI/LayoutDock/Controls/ILayoutControl.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutItem.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutAnchorable.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutAnchorable;
    class LayoutItem;

    class LayoutAnchorableControl : public Controls::ContentControl, public ILayoutControl
    {
    private:

    public:

        /// <summary>
        /// LayoutItem Read-Only Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty LayoutItemProperty;

        /// <summary>
        /// Model Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ModelProperty;

        static void StaticClassInitializer();

        LayoutAnchorableControl();
        virtual ~LayoutAnchorableControl() {}

        void Initialize();

        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::LayoutAnchorable>, Model)
        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::LayoutItem>, LayoutItem)

        LayoutElement* GetLayoutModel() override { return Model.get(); }

    protected:

        /// <summary>
        /// Handles changes to the Model property.
        /// </summary>
        static void OnModelChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the Model property.
        /// </summary>
        virtual void OnModelChanged( Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void Model_PropertyChanged(void* sender, XamlCPP::Core::PropertyChangedEventArgs& e);
        void OnGotFocus(Interfaces::GotFocusEventArgs& e) override;
        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;
    };
}


#endif	//#ifndef __XUI_LAYOUTANCHORABLECONTROL_H__
