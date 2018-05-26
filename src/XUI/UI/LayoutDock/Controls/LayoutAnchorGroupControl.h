#ifndef __XUI_LAYOUTANCHORGROUPCONTROL_H__
#define __XUI_LAYOUTANCHORGROUPCONTROL_H__

#include "Controls/Control.h"
#include "UI/LayoutDock/Controls/ILayoutControl.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorGroup.h"
#include "Core/DispatcherTimer.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutItem.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutAnchorControl.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutAnchorGroup.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutAnchorControl;

    class LayoutAnchorGroupControl : public Controls::Control, public ILayoutControl
    {
    private:

        System::Collection::List<std::shared_ptr<LayoutAnchorControl>> _childViews;

    public:

        static void StaticClassInitializer();

        LayoutAnchorGroupControl(LayoutAnchorGroup *model);
        virtual ~LayoutAnchorGroupControl() {}

		std::shared_ptr<LayoutAnchorGroup> Model;

        virtual LayoutElement* GetModel() { return Model.get(); }
        virtual LayoutElement* GetLayoutModel() override { return Model.get(); }

        void OnModelChildrenCollectionChanged(System::Collection::NotifyCollectionChangedEventArgs& e);

        PROPERTY_GET_ONLY(System::Collection::List<std::shared_ptr<LayoutAnchorControl>>, Children);
        System::Collection::List<std::shared_ptr<LayoutAnchorControl>>& GetChildren() { return _childViews; }

    private:
        void CreateChildrenViews();

    };
}


#endif	//#ifndef __XUI_LAYOUTANCHORGROUPCONTROL_H__
