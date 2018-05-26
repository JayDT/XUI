#ifndef __XUI_LAYOUTANCHORABLEFLOATINGWINDOWCONTROL_H__
#define __XUI_LAYOUTANCHORABLEFLOATINGWINDOWCONTROL_H__

#include "UI/LayoutDock/Controls/LayoutFloatingWindowControl.h"
#include "UI/LayoutDock/Layouts/LayoutAnchorableFloatingWindow.h"

namespace XUI::UI
{
    class LayoutItem;

    class LayoutAnchorableFloatingWindowControl : public LayoutFloatingWindowControl, public IOverlayWindowHost
    {
    private:
        std::shared_ptr<Interfaces::ICommand> m_HideWindowCommand;
        std::shared_ptr<OverlayWindow> _overlayWindow = nullptr;
        std::list<std::shared_ptr<IDropArea>> _dropAreas;

    public:

        /// <summary>
        /// SingleContentLayoutItem Dependency Property
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty SingleContentLayoutItemProperty;

        static void StaticClassInitializer();

        LayoutAnchorableFloatingWindowControl(LayoutAnchorableFloatingWindow *model);

        PROPERTY_(std::shared_ptr<Interfaces::ICommand>, HideWindowCommand);
        std::shared_ptr<Interfaces::ICommand> const& GetHideWindowCommand() const { return m_HideWindowCommand; }
        void SetHideWindowCommand(std::shared_ptr<Interfaces::ICommand> const& value) { m_HideWindowCommand = value; }

        DEPENDENCY_PROPERTY_(std::shared_ptr<UI::LayoutItem>, SingleContentLayoutItem)

        bool HitTest(Core::Media::Point const& dragPoint);

        OverlayWindow* ShowOverlayWindow(LayoutFloatingWindowControl *draggingWindow);

        void HideOverlayWindow();

        std::list<std::shared_ptr<IDropArea>> const& GetDropAreas(LayoutFloatingWindowControl *draggingWindow);

        DockingManager * GetManager() override;

        IOverlayWindowHost* ToIOverlayWindowHost() override { return this; }

    protected:

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        /// <summary>
        /// Handles changes to the SingleContentLayoutItem property.
        /// </summary>
        static void OnSingleContentLayoutItemChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs & e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the SingleContentLayoutItem property.
        /// </summary>
        virtual void OnSingleContentLayoutItemChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e);

        void OnWindowInitialized() override;
        void _model_PropertyChanged(void* sender, XamlCPP::Core::PropertyChangedEventArgs& e);
        void CreateOverlayWindow();
        void HandleClosed() override;
        //void OnClosing(CancelEventArgs& e) override;
        //void* FilterMessage(void* hwnd, int msg, void* wParam, void* lParam, bool &handled) override;
        bool OpenContextMenu();
        bool IsContextMenuOpen();
        bool CanExecuteHideWindowCommand(System::Reflection::Variant parameter);
        void OnExecuteHideWindowCommand(System::Reflection::Variant parameter);
    };
}


#endif	//#ifndef __XUI_LAYOUTANCHORABLEFLOATINGWINDOWCONTROL_H__
