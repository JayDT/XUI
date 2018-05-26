#ifndef __XUI_LAYOUTANCHORABLEITEM_H__
#define __XUI_LAYOUTANCHORABLEITEM_H__

#include "UI/LayoutDock/Controls/LayoutItem.h"
#include "UI/LayoutDock/Layouts/LayoutContent.h"

namespace XUI::UI
{
    class LayoutAnchorableItem : public LayoutItem
    {
    private:
        std::shared_ptr<LayoutAnchorable> _anchorable;

        std::shared_ptr<Interfaces::ICommand> _defaultHideCommand;
        std::shared_ptr<Interfaces::ICommand> _defaultAutoHideCommand;
        std::shared_ptr<Interfaces::ICommand> _defaultDockCommand;

        ReentrantFlag _visibilityReentrantFlag;

    public:
        static XUI::Core::Dependency::RefDependencyProperty HideCommandProperty;
        static XUI::Core::Dependency::RefDependencyProperty AutoHideCommandProperty;
        static XUI::Core::Dependency::RefDependencyProperty DockCommandProperty;
        static XUI::Core::Dependency::RefDependencyProperty CanHideProperty;

        static void StaticClassInitializer();

        LayoutAnchorableItem();

        void Attach(LayoutContent *model) override;

        void Detach() override;

        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, HideCommand)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, AutoHideCommand)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, DockCommand)
        DEPENDENCY_PROPERTY_(bool, CanHide)

    protected:
        void InitDefaultCommands() override;

        void ClearDefaultBindings() override;

        void SetDefaultBindings() override;

        void Close() override;

        /// <summary>
        /// Handles changes to the HideCommand property.
        /// </summary>
        static void OnHideCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs &e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the HideCommand property.
        /// </summary>
        virtual void OnHideCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs &e);

        /// <summary>
        /// Coerces the HideCommand value.
        /// </summary>
        static System::Reflection::Variant CoerceHideCommandValue(DependencyObject *d, System::Reflection::Variant value);

        bool CanExecuteHideCommand(System::Reflection::Variant parameter);

        void ExecuteHideCommand(System::Reflection::Variant parameter);

        /// <summary>
        /// Handles changes to the AutoHideCommand property.
        /// </summary>
        static void OnAutoHideCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs &e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the AutoHideCommand property.
        /// </summary>
        virtual void OnAutoHideCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs &e);

        /// <summary>
        /// Coerces the AutoHideCommand value.
        /// </summary>
        static System::Reflection::Variant CoerceAutoHideCommandValue(DependencyObject *d, System::Reflection::Variant value);

        bool CanExecuteAutoHideCommand(System::Reflection::Variant parameter);

        void ExecuteAutoHideCommand(System::Reflection::Variant parameter);

        /// <summary>
        /// Handles changes to the DockCommand property.
        /// </summary>
        static void OnDockCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs &e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the DockCommand property.
        /// </summary>
        virtual void OnDockCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs &e);

        /// <summary>
        /// Coerces the DockCommand value.
        /// </summary>
        static System::Reflection::Variant CoerceDockCommandValue(DependencyObject *d, System::Reflection::Variant value);

        bool CanExecuteDockCommand(System::Reflection::Variant parameter);

        void ExecuteDockCommand(System::Reflection::Variant parameter);

        void OnVisibilityChanged() override;

        void _anchorable_IsVisibleChanged(void* sender, System::Events::EventArg& e);

        /// <summary>
        /// Handles changes to the CanHide property.
        /// </summary>
        static void OnCanHideChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs &e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the CanHide property.
        /// </summary>
        virtual void OnCanHideChanged(Core::Dependency::DependencyPropertyChangedEventArgs &e);
    };
}


#endif	//#ifndef __XUI_LAYOUTANCHORABLEITEM_H__
