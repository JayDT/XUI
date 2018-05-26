#ifndef __XUI_LAYOUTAUTOHIDEWINDOWCONTROL_H__
#define __XUI_LAYOUTAUTOHIDEWINDOWCONTROL_H__

#include "UI/LayoutDock/Controls/ILayoutControl.h"
#include "UI/Controls/Control.h"
#include "UI/Border.h"
#include "UI/Grid.h"
#include "UI/Window.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutItem.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutAnchorControl.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutAnchorableControl.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutAnchorable.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutAnchorGroup.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutAnchorable;
    class LayoutAnchorControl;
    class LayoutAnchorableControl;
    class LayoutGridResizerControl;

    class LayoutAutoHideWindowControl : public Controls::Control, public ILayoutControl
    {
    private:
        bool m_IsResizing = false;
        rxcpp::composite_subscription _viewBindings;
        std::shared_ptr<LayoutAnchorControl> _anchor;
        //HwndSource *_internalHwndSource = nullptr;
        void* parentWindowHandle;
        std::shared_ptr<Presenters::ContentPresenter> _internalHostPresenter;
        std::shared_ptr<Grid> _internalGrid = nullptr;
        std::shared_ptr<LayoutAnchorableControl> _internalHost = nullptr;
        std::shared_ptr<LayoutGridResizerControl> _resizer = nullptr;
        DockingManager *_manager = nullptr;
        std::shared_ptr<Border> _resizerGhost = nullptr;
        std::shared_ptr<Window> _resizerWindowHost = nullptr;
        AnchorSide _side;
        Core::Media::Vector _initialStartPoint;

    public:

        /// <summary>
        /// AnchorableStyle Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty AnchorableStyleProperty;

        static void StaticClassInitializer();

        LayoutAutoHideWindowControl();
        virtual ~LayoutAutoHideWindowControl() {}

        void Show(LayoutAnchorControl *anchor);

        void Hide();

		std::shared_ptr<LayoutAnchorable> Model;

        LayoutElement* GetModel();
        LayoutElement* GetLayoutModel() override { return Model.get(); }

    protected:
        Interfaces::PlatformHandle BuildWindowCore(Interfaces::PlatformHandle const& hwndParent);

        void _internalHwndSource_ContentRendered(void* sender, System::Events::EventArg& e);
        void DestroyWindowCore(Interfaces::PlatformHandle const& hwnd);
        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        void _model_PropertyChanged(void* sender, XamlCPP::Core::PropertyChangedEventArgs& e);

        void CreateInternalGrid();

        void RemoveInternalGrid();

        bool HasFocusWithinCore();

        void ShowResizerOverlayWindow(LayoutGridResizerControl *splitter);

        void HideResizerOverlayWindow();

        void OnResizerDragCompleted(void* sender, Interfaces::VectorEventArgs& e);

        void OnResizerDragDelta(void* sender, Interfaces::VectorEventArgs& e);

        void OnResizerDragStarted(void* sender, Interfaces::VectorEventArgs& e);

        Core::Media::Size MeasureOverride(Core::Media::Size const& constraint) override;

        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;
    };
}


#endif	//#ifndef __XUI_LAYOUTAUTOHIDEWINDOWCONTROL_H__
