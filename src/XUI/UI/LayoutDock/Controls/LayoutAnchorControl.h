#ifndef __XUI_LAYOUTANCHORCONTROL_H__
#define __XUI_LAYOUTANCHORCONTROL_H__

#include "UI/Controls/Control.h"
#include "UI/LayoutDock/Controls/ILayoutControl.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorable.h"
#include "Core/DispatcherTimer.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutItem.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutAnchorControl.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutAnchorable.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutAnchorControl : public Controls::Control, public ILayoutControl
    {
    private:
        std::shared_ptr<Core::DispatcherTimer> _openUpTimer;

    public:

        /// <summary>
        /// Side Read-Only Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty SideProperty;

        static void StaticClassInitializer();

        LayoutAnchorControl(LayoutAnchorable *model);
        virtual ~LayoutAnchorControl() {}

		std::shared_ptr<LayoutAnchorable> Model;

        virtual LayoutElement* GetModel() { return Model.get(); }
        LayoutElement* GetLayoutModel() override { return Model.get(); }

        DEPENDENCY_PROPERTY_(AnchorSide, Side)

        //protected override void OnVisualParentChanged(DependencyObject oldParent)
        //{
        //    base.OnVisualParentChanged(oldParent);

        //    var contentModel = _model;

        //    if (oldParent != null && contentModel != null && contentModel.Content is UIElement)
        //    {
        //        var oldParentPaneControl = oldParent.FindVisualAncestor<LayoutAnchorablePaneControl>();
        //        if (oldParentPaneControl != null)
        //        {
        //            ((ILogicalChildrenContainer)oldParentPaneControl).InternalRemoveLogicalChild(contentModel.Content);
        //        }
        //    }

        //    if (contentModel.Content != null && contentModel.Content is UIElement)
        //    {
        //        var oldLogicalParentPaneControl = LogicalTreeHelper.GetParent(contentModel.Content as UIElement)
        //            as ILogicalChildrenContainer;
        //        if (oldLogicalParentPaneControl != null)
        //            oldLogicalParentPaneControl.InternalRemoveLogicalChild(contentModel.Content);
        //    }

        //    if (contentModel != null && contentModel.Content != null && contentModel.Root != null && contentModel.Content is UIElement)
        //    {
        //        ((ILogicalChildrenContainer)contentModel.Root.Manager).InternalAddLogicalChild(contentModel.Content);
        //    }
        //}

    protected:
        void _model_IsSelectedChanged(void* sender, System::Events::EventArg& e);
        void _model_IsActiveChanged(void* sender, System::Events::EventArg& e);
        void _openUpTimer_Tick(void* sender, System::Events::EventArg& e);

        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void OnPointerEnter(Interfaces::PointerEventArgs& e) override;

        void OnPointerLeave(Interfaces::PointerEventArgs& e) override;
    };
}


#endif	//#ifndef __XUI_LAYOUTANCHORCONTROL_H__
