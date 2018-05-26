#ifndef __XUI_LAYOUTANCHORSIDECONTROL_H__
#define __XUI_LAYOUTANCHORSIDECONTROL_H__

#include "UI/LayoutDock/Controls/ILayoutControl.h"
#include "Controls/Control.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutAnchorControl.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutAnchorGroupControl.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutAnchorSide.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutAnchorSide;
    class LayoutAnchorGroupControl;

    class LayoutAnchorSideControl : public Controls::Control, public ILayoutControl
    {
    private:

		System::Collection::List<std::shared_ptr<LayoutAnchorGroupControl>> _childViews;
        std::shared_ptr<LayoutAnchorSide> _model;

    public:
        static XUI::Core::Dependency::RefDependencyProperty IsLeftSideProperty;
        static XUI::Core::Dependency::RefDependencyProperty IsTopSideProperty;
        static XUI::Core::Dependency::RefDependencyProperty IsRightSideProperty;
        static XUI::Core::Dependency::RefDependencyProperty IsBottomSideProperty;

        static void StaticClassInitializer();

        LayoutAnchorSideControl();
        virtual ~LayoutAnchorSideControl();

        void Initialize(LayoutAnchorSide *model);

        PROPERTY_(std::shared_ptr<UI::LayoutAnchorSide>, Model);
        std::shared_ptr<UI::LayoutAnchorSide> const& GetModel() const { return _model; }
        void SetModel(std::shared_ptr<UI::LayoutAnchorSide> const& value);

        LayoutElement* GetLayoutModel() override { return Model.get(); }

        System::Collection::List<std::shared_ptr<LayoutAnchorGroupControl>>& GetChildren();

        void OnModelChanged(void* s, System::Collection::NotifyCollectionChangedEventArgs& e);

        DEPENDENCY_PROPERTY_(bool, IsLeftSide)
        DEPENDENCY_PROPERTY_(bool, IsTopSide)
        DEPENDENCY_PROPERTY_(bool, IsRightSide)
        DEPENDENCY_PROPERTY_(bool, IsBottomSide)

    protected:

        void CreateChildrenViews();
        void OnModelChildrenCollectionChanged(System::Collection::NotifyCollectionChangedEventArgs& e);
        void UpdateSide();
    };
}


#endif	//#ifndef __XUI_LAYOUTANCHORSIDECONTROL_H__
