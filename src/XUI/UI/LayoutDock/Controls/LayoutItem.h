/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __XUI_UI_LAYOUT_DOCK_LAYOUT_ITEM_H__
#define __XUI_UI_LAYOUT_DOCK_LAYOUT_ITEM_H__

#include "UI/Controls/Control.h"
#include "UI/LayoutDock/DockingDefines.h"
#include "UI/LayoutDock/DockingEventArgs.h"
#include "Interfaces/ICommand.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/LayoutDock/Controls/OverlayWindow.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutItem.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Controls/LayoutAnchorControl.h"))
Meta(modul: ReflInclude("UI/LayoutDock/Layouts/LayoutAnchorGroup.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    class LayoutContent;

    class LayoutItem : public Controls::Control
    {
    private:

        std::shared_ptr<LayoutContent> _LayoutElement;
        System::Reflection::RefObject _Model;

        mutable std::shared_ptr<Presenters::ContentPresenter> _view;
        rxcpp::composite_subscription _viewBindings;
        ReentrantFlag _isActiveReentrantFlag;
        ReentrantFlag _isSelectedReentrantFlag;

        std::shared_ptr<Interfaces::ICommand> _defaultCloseCommand;
        std::shared_ptr<Interfaces::ICommand> _defaultFloatCommand;
        std::shared_ptr<Interfaces::ICommand> _defaultDockAsDocumentCommand;
        std::shared_ptr<Interfaces::ICommand> _defaultCloseAllButThisCommand;
        std::shared_ptr<Interfaces::ICommand> _defaultCloseAllCommand;
        std::shared_ptr<Interfaces::ICommand> _defaultActivateCommand;
        std::shared_ptr<Interfaces::ICommand> _defaultNewVerticalTabGroupCommand;
        std::shared_ptr<Interfaces::ICommand> _defaultNewHorizontalTabGroupCommand;
        std::shared_ptr<Interfaces::ICommand> _defaultMoveToNextTabGroupCommand;
        std::shared_ptr<Interfaces::ICommand> _defaultMoveToPreviousTabGroupCommand;

    public:
        static Core::Dependency::RefDependencyProperty TitleProperty;
        static Core::Dependency::RefDependencyProperty IconSourceProperty;
        static Core::Dependency::RefDependencyProperty ContentIdProperty;
        static Core::Dependency::RefDependencyProperty IsSelectedProperty;
        static Core::Dependency::RefDependencyProperty IsActiveProperty;
        static Core::Dependency::RefDependencyProperty CanCloseProperty;
        static Core::Dependency::RefDependencyProperty CanFloatProperty;
        static Core::Dependency::RefDependencyProperty CloseCommandProperty;
        static Core::Dependency::RefDependencyProperty FloatCommandProperty;
        static Core::Dependency::RefDependencyProperty DockAsDocumentCommandProperty;
        static Core::Dependency::RefDependencyProperty CloseAllButThisCommandProperty;
        static Core::Dependency::RefDependencyProperty CloseAllCommandProperty;
        static Core::Dependency::RefDependencyProperty ActivateCommandProperty;
        static Core::Dependency::RefDependencyProperty NewVerticalTabGroupCommandProperty;
        static Core::Dependency::RefDependencyProperty NewHorizontalTabGroupCommandProperty;
        static Core::Dependency::RefDependencyProperty MoveToNextTabGroupCommandProperty;
        static Core::Dependency::RefDependencyProperty MoveToPreviousTabGroupCommandProperty;

        static void StaticClassInitializer();

        LayoutItem();
        virtual ~LayoutItem();

        DEPENDENCY_PROPERTY_(std::wstring, Title)
        DEPENDENCY_PROPERTY_(bool, IconSource)
        DEPENDENCY_PROPERTY_(int, ContentId)
        DEPENDENCY_PROPERTY_(bool, IsSelected)
        DEPENDENCY_PROPERTY_(bool, IsActive)
        DEPENDENCY_PROPERTY_(bool, CanClose)
        DEPENDENCY_PROPERTY_(bool, CanFloat)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, CloseCommand)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, FloatCommand)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, DockAsDocumentCommand)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, CloseAllButThisCommand)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, CloseAllCommand)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, ActivateCommand)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, NewVerticalTabGroupCommand)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, NewHorizontalTabGroupCommand)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, MoveToNextTabGroupCommand)
        DEPENDENCY_PROPERTY_(std::shared_ptr<Interfaces::ICommand>, MoveToPreviousTabGroupCommand)

        PROPERTY_(LayoutContent * , Element);
        LayoutContent *GetElement() const { return _LayoutElement.get(); }
        void SetElement(LayoutContent *value);

        PROPERTY_(System::Reflection::RefObject, Model);
        System::Reflection::RefObject const& GetModel() const { return _Model; }
        void SetModel(System::Reflection::RefObject const& value) { _Model = value; }

        virtual void Attach(LayoutContent *model);
        virtual void Detach();

        void _ClearDefaultBindings();
        void _SetDefaultBindings();
        bool IsViewExists();

        Presenters::ContentPresenter *GetView() const;

        void LayoutElement_IsActiveChanged(void* sender, System::Events::EventArg& e);
        void LayoutElement_IsSelectedChanged(void* sender, System::Events::EventArg& e);

    protected:

        virtual void InitDefaultCommands();
        virtual void ClearDefaultBindings();
        virtual void SetDefaultBindings();

        /// <summary>
        /// Handles changes to the Title property.
        /// </summary>
        static void OnTitleChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the Title property.
        /// </summary>
        virtual void OnTitleChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        static void OnToolTipChanged(DependencyObject *s, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        void OnToolTipChanged();

        /// <summary>
        /// Handles changes to the IconSource property.
        /// </summary>
        static void OnIconSourceChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the IconSource property.
        /// </summary>
        virtual void OnIconSourceChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        static void OnVisibilityChanged(DependencyObject *s, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        virtual void OnVisibilityChanged();

        /// <summary>
        /// Handles changes to the ContentId property.
        /// </summary>
        static void OnContentIdChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the ContentId property.
        /// </summary>
        virtual void OnContentIdChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Handles changes to the IsSelected property.
        /// </summary>
        static void OnIsSelectedChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the IsSelected property.
        /// </summary>
        virtual void OnIsSelectedChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Handles changes to the IsActive property.
        /// </summary>
        static void OnIsActiveChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the IsActive property.
        /// </summary>
        virtual void OnIsActiveChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Handles changes to the CanClose property.
        /// </summary>
        static void OnCanCloseChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the CanClose property.
        /// </summary>
        virtual void OnCanCloseChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Handles changes to the CanFloat property.
        /// </summary>
        static void OnCanFloatChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the CanFloat property.
        /// </summary>
        virtual void OnCanFloatChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Handles changes to the CloseCommand property.
        /// </summary>
        static void OnCloseCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the CloseCommand property.
        /// </summary>
        virtual void OnCloseCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Coerces the CloseCommand value.
        /// </summary>
        static System::Reflection::Variant CoerceCloseCommandValue(DependencyObject *d, System::Reflection::Variant value);

        bool CanExecuteCloseCommand(System::Reflection::Variant parameter);
        void ExecuteCloseCommand(System::Reflection::Variant parameter);

        virtual void Close() = 0;

        /// <summary>
        /// Handles changes to the FloatCommand property.
        /// </summary>
        static void OnFloatCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the FloatCommand property.
        /// </summary>
        virtual void OnFloatCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Coerces the FloatCommand value.
        /// </summary>
        static System::Reflection::Variant CoerceFloatCommandValue(DependencyObject *d, System::Reflection::Variant value);

        bool CanExecuteFloatCommand(System::Reflection::Variant anchorable);
        void ExecuteFloatCommand(System::Reflection::Variant parameter);

        virtual void Float();

        /// <summary>
        /// Handles changes to the DockAsDocumentCommand property.
        /// </summary>
        static void OnDockAsDocumentCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the DockAsDocumentCommand property.
        /// </summary>
        virtual void OnDockAsDocumentCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Coerces the DockAsDocumentCommand value.
        /// </summary>
        static System::Reflection::Variant CoerceDockAsDocumentCommandValue(DependencyObject *d, System::Reflection::Variant value);

        bool CanExecuteDockAsDocumentCommand(System::Reflection::Variant parameter);
        void ExecuteDockAsDocumentCommand(System::Reflection::Variant parameter);

        /// <summary>
        /// Handles changes to the CloseAllButThisCommand property.
        /// </summary>
        static void OnCloseAllButThisCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the CloseAllButThisCommand property.
        /// </summary>
        virtual void OnCloseAllButThisCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Coerces the CloseAllButThisCommand value.
        /// </summary>
        static System::Reflection::Variant CoerceCloseAllButThisCommandValue(DependencyObject *d, System::Reflection::Variant value);

        bool CanExecuteCloseAllButThisCommand(System::Reflection::Variant parameter);
        void ExecuteCloseAllButThisCommand(System::Reflection::Variant parameter);

        /// <summary>
        /// Handles changes to the CloseAllCommand property.
        /// </summary>
        static void OnCloseAllCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the CloseAllCommand property.
        /// </summary>
        virtual void OnCloseAllCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Coerces the CloseAllCommand value.
        /// </summary>
        static System::Reflection::Variant CoerceCloseAllCommandValue(DependencyObject *d, System::Reflection::Variant value);

        bool CanExecuteCloseAllCommand(System::Reflection::Variant parameter);
        void ExecuteCloseAllCommand(System::Reflection::Variant parameter);

        /// <summary>
        /// Handles changes to the ActivateCommand property.
        /// </summary>
        static void OnActivateCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the ActivateCommand property.
        /// </summary>
        virtual void OnActivateCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Coerces the ActivateCommand value.
        /// </summary>
        static System::Reflection::Variant CoerceActivateCommandValue(DependencyObject *d, System::Reflection::Variant value);

        bool CanExecuteActivateCommand(System::Reflection::Variant parameter);
        void ExecuteActivateCommand(System::Reflection::Variant parameter);

        /// <summary>
        /// Handles changes to the NewVerticalTabGroupCommand property.
        /// </summary>
        static void OnNewVerticalTabGroupCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the NewVerticalTabGroupCommand property.
        /// </summary>
        virtual void OnNewVerticalTabGroupCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        bool CanExecuteNewVerticalTabGroupCommand(System::Reflection::Variant parameter);
        void ExecuteNewVerticalTabGroupCommand(System::Reflection::Variant parameter);

        /// <summary>
        /// Handles changes to the NewHorizontalTabGroupCommand property.
        /// </summary>
        static void OnNewHorizontalTabGroupCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the NewHorizontalTabGroupCommand property.
        /// </summary>
        virtual void OnNewHorizontalTabGroupCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        bool CanExecuteNewHorizontalTabGroupCommand(System::Reflection::Variant parameter);
        void ExecuteNewHorizontalTabGroupCommand(System::Reflection::Variant parameter);

        /// <summary>
        /// Handles changes to the MoveToNextTabGroupCommand property.
        /// </summary>
        static void OnMoveToNextTabGroupCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the MoveToNextTabGroupCommand property.
        /// </summary>
        virtual void OnMoveToNextTabGroupCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        bool CanExecuteMoveToNextTabGroupCommand(System::Reflection::Variant parameter);
        void ExecuteMoveToNextTabGroupCommand(System::Reflection::Variant parameter);

        /// <summary>
        /// Handles changes to the MoveToPreviousTabGroupCommand property.
        /// </summary>
        static void OnMoveToPreviousTabGroupCommandChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs&e);

        /// <summary>
        /// Provides derived classes an opportunity to handle changes to the MoveToPreviousTabGroupCommand property.
        /// </summary>
        virtual void OnMoveToPreviousTabGroupCommandChanged(Core::Dependency::DependencyPropertyChangedEventArgs&e);

        bool CanExecuteMoveToPreviousTabGroupCommand(System::Reflection::Variant parameter);
        void ExecuteMoveToPreviousTabGroupCommand(System::Reflection::Variant parameter);
    };
}

#endif //!__XUI_UI_LAYOUT_DOCK_LAYOUT_ITEM_H__