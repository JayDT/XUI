#ifndef __XUI_LAYOUTDOCUMENTCONTROL_H__
#define __XUI_LAYOUTDOCUMENTCONTROL_H__

#include "Controls/ContentControl.h"
#include "UI/LayoutDock/Controls/ILayoutControl.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorGroup.h"
#include "Core/DispatcherTimer.h"

namespace XUI::UI
{
    class LayoutItem;

    class LayoutDocumentControl : public Controls::ContentControl, public ILayoutControl
    {
    private:

    public:
        /// <summary>
        /// Model Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ModelProperty;
        static XUI::Core::Dependency::RefDependencyProperty LayoutItemProperty;

        static void StaticClassInitializer();

        LayoutDocumentControl();
        virtual ~LayoutDocumentControl() {}

        /// <summary>
        /// Gets or sets the Model property.  This dependency property 
        /// indicates the model attached to this view.
        /// </summary>
        PROPERTY_(std::shared_ptr<UI::LayoutContent>, Model);
        std::shared_ptr<UI::LayoutContent> GetModel() const { return GetValue<std::shared_ptr<UI::LayoutContent>>(ModelProperty); }
        void SetModel(std::shared_ptr<UI::LayoutContent> const& value)
        {
            SetValue(ModelProperty, value);
        }

        LayoutElement* GetLayoutModel() override { return Model.get(); }

        /// <summary>
        /// Gets the LayoutItem property.  This dependency property 
        /// indicates the LayoutItem attached to this tag item.
        /// </summary>
        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::LayoutItem>, LayoutItem)

    protected:

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the Model property.
        /// </summary>
        virtual void OnModelChanged(Core::Dependency::DependencyPropertyChangedEventArgs &e);

        /// <summary>
        /// Handles changes to the Model property.
        /// </summary>
        static void OnModelChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs &e);

        void Model_PropertyChanged(void* sender, XamlCPP::Core::PropertyChangedEventArgs& e);

        //void OnPreviewGotKeyboardFocus(KeyboardFocusChangedEventArgs *e) override;

        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void SetIsActive();
    };
}


#endif	//#ifndef __XUI_LAYOUTDOCUMENTCONTROL_H__
