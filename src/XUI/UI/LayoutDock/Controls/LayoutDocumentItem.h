#ifndef __XUI_LAYOUTDOCUMENTITEM_H__
#define __XUI_LAYOUTDOCUMENTITEM_H__

#include "UI/LayoutDock/Controls/LayoutItem.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/DockingManager.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutAnchorControl.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutDocument.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutDocumentItem : public LayoutItem
    {
    private:
        std::shared_ptr<LayoutDocument> _document;

    public:

        /// <summary>
        /// Description Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty DescriptionProperty;

        LayoutDocumentItem();
        virtual ~LayoutDocumentItem() {}

        DEPENDENCY_PROPERTY_(std::string, Description)

        void Attach(LayoutContent *model) override;
        void Detach() override;

    protected:
        void Close() override;

        /// <summary>
        /// Handles changes to the Description property.
        /// </summary>
        static void OnDescriptionChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs & e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the Description property.
        /// </summary>
        virtual void OnDescriptionChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e);
    };
}


#endif	//#ifndef __XUI_LAYOUTDOCUMENTITEM_H__
