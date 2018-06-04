/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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

#ifndef _XUI_CONTROL_WINDOW_H__
#define _XUI_CONTROL_WINDOW_H__

#include "UI/TopLevel.h"
#include "UI/Controls/NameScope.h"
#include "UI/Controls/WindowIcon.h"
#include "Interfaces/IWindowImpl.h"
#include "Interfaces/ICommand.h"
#include "Core/Observer/RoutedCommand.h"

namespace XUI::UI
{
    class Window;

    enum class WindowEdge : uint8
    {
        //Please don't reorder stuff here, I was lazy to write proper conversion code
        //so the order of values is matching one from GTK
        NorthWest = 0,
        North,
        NorthEast,
        West,
        East,
        SouthWest,
        South,
        SouthEast,
    };

    /// <summary>
    /// Defines the minimized/maximized state of a <see cref="Window"/>.
    /// </summary>
    enum class WindowState : uint8
    {
        /// <summary>
        /// The window is neither minimized or maximized.
        /// </summary>
        Normal,

        /// <summary>
        /// The window is minimized.
        /// </summary>
        Minimized,

        /// <summary>
        /// The window is maximized.
        /// </summary>
        Maximized,
    };

    /// <summary>
    /// Determines how a <see cref="Window"/> will size itself to fit its content.
    /// </summary>
    enum class SizeToContent : uint8
    {
        /// <summary>
        /// The window will not automatically size itself to fit its content.
        /// </summary>
        Manual,

        /// <summary>
        /// The window will size itself horizontally to fit its content.
        /// </summary>
        Width,

        /// <summary>
        /// The window will size itself vertically to fit its content.
        /// </summary>
        Height,

        /// <summary>
        /// The window will size itself horizontally and vertically to fit its content.
        /// </summary>
        WidthAndHeight,
    };

    class TC_XUI_API Window
        : public TopLevel
        , public Controls::NameScope
    {
    protected:

        enum EGUI_ACTION_MODE_FLAGS
        {
            // moving the selected element
            EAMF_MOVE = 0x00000001,
            // resizing the selected element
            EAMF_RESIZE_TL = 0x00000002,
            EAMF_RESIZE_T = 0x00000004,
            EAMF_RESIZE_TR = 0x00000008,
            EAMF_RESIZE_R = 0x00000010,
            EAMF_RESIZE_BR = 0x00000020,
            EAMF_RESIZE_B = 0x00000040,
            EAMF_RESIZE_BL = 0x00000080,
            EAMF_RESIZE_L = 0x00000100,

            // can resize the selected element
            EAMF_CAN_RESIZE_TL = 0x00000200,
            EAMF_CAN_RESIZE_T = 0x00000400,
            EAMF_CAN_RESIZE_TR = 0x00000800,
            EAMF_CAN_RESIZE_R = 0x00001000,
            EAMF_CAN_RESIZE_BR = 0x00002000,
            EAMF_CAN_RESIZE_B = 0x00004000,
            EAMF_CAN_RESIZE_BL = 0x00008000,
            EAMF_CAN_RESIZE_L = 0x00010000,

            EAMF_RESIZE_ALL = 0x000001FE,
            EAMF_CAN_RESIZE_ALL = 0x0001FE00,
            EAMF_ALL_CONTROL = 0x000001FF
        };

        XUI::UI::WindowState _windowState = XUI::UI::WindowState::Normal;

        System::Reflection::RefObject _dialogResult;
        std::shared_ptr<UI::Window> _owner;
        std::shared_ptr<Controls::Control> _titleBar;
        Core::Media::Size _maxPlatformClientSize;
        Core::Media::Point _lastMousePosition = Core::Media::Point(0, 0);
        uint32 m_actionModeFlags = 0;
        UI::WindowStyle m_windowStyle = UI::WindowStyle::None;
        bool _platformImplInitialized = false;
        bool _inDragMove = false;
        bool _inResizing = false;
        bool _isMaxialized = false;
        bool _isMinialized = false;
        bool _isRootWindow = false;

        void InitializePlatformWindow();

        bool HasActionModeFlags(uint32 flag) const
        {
            return (m_actionModeFlags & flag) != 0;
        }

    public:

        /// <summary>
        /// Defines the <see cref="SizeToContent"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty SizeToContentProperty;

        /// <summary>
        /// Enables of disables system window decorations (title bar, buttons, etc)
        /// </summary>
        static Core::Dependency::RefDependencyProperty HasSystemDecorationsProperty;

        /// <summary>
        /// Sets if the window should cover the taskbar when maximized. Only applies to Windows 
        /// with HasSystemDecorations = false.
        /// </summary>
        static Core::Dependency::RefDependencyProperty CoverTaskbarOnMaximizeProperty;

        /// <summary>
        /// Defines the <see cref="Title"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty TitleProperty;

        /// <summary>
        /// Defines the <see cref="Icon"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty IconProperty;

        static Core::Dependency::RefDependencyProperty WindowStyleProperty;

        static Core::Observer::RefRoutedCommand MinimizeWindowCommand;
        static Core::Observer::RefRoutedCommand MaximizeWindowCommand;
        static Core::Observer::RefRoutedCommand CloseWindowCommand;
        static Core::Observer::RefRoutedCommand RestoreWindowCommand;

        static void StaticClassInitializer();
        virtual void InstanceInitializer() override;

        Window();
        virtual ~Window();

        System::Events::EventHandler<Interfaces::CancelEventArgs> Closing;
        System::Events::EventHandler<> LocationChanged;

        /// <inheritdoc/>
        void EndInit() override final;

        /// <summary>
        /// Gets or sets a value indicating how the window will size itself to fit its content.
        /// </summary>
        PROPERTY_(XUI::UI::SizeToContent, SizeToContent);
        XUI::UI::SizeToContent GetSizeToContent() const;
        void SetSizeToContent(XUI::UI::SizeToContent value);

        /// <summary>
        /// Gets or sets the title of the window.
        /// </summary>
        PROPERTY_(std::wstring, Title);
        std::wstring GetTitle() const;
        void SetTitle(std::wstring value);

        /// <summary>
        /// Enables of disables system window decorations (title bar, buttons, etc)
        /// </summary>
        /// 
        PROPERTY_(bool, HasSystemDecorations);
        bool GetHasSystemDecorations() const;
        void SetHasSystemDecorations(bool value);

        /// <summary>
        /// Sets if the window should cover the taskbar when maximized. Only applies to Windows 
        /// with HasSystemDecorations = false.
        /// </summary>
        PROPERTY_(bool, CoverTaskbarOnMaximize);
        bool GetCoverTaskbarOnMaximize() const;
        void SetCoverTaskbarOnMaximize(bool value);

        /// <summary>
        /// Gets or sets the minimized/maximized state of the window.
        /// </summary>
        PROPERTY_(XUI::UI::WindowState, WindowState);
        XUI::UI::WindowState GetWindowState() const;
        void SetWindowState(XUI::UI::WindowState value);

        /// <summary>
        /// Gets or sets the icon of the window.
        /// </summary>
        PROPERTY_(std::shared_ptr<WindowIcon>, Icon);
        std::shared_ptr<WindowIcon> GetIcon() const;
        void SetIcon(std::shared_ptr<WindowIcon> value);

        /// <inheritdoc/>
        PROPERTY_(Core::Media::Size, MaxClientSize);
        Core::Media::Size GetMaxClientSize() const;
        void SetMaxClientSize(Core::Media::Size value);

        /// <inheritdoc/>
        PROPERTY_(System::Reflection::Type const*, StyleKey);
        System::Reflection::Type const* GetStyleKey() const;
        void SetStyleKey(System::Reflection::Type const* value);

        /// <summary>
        /// Gets or sets the position of the window's left edge.
        /// </summary>
        PROPERTY_(double, Left);
        double GetLeft() const;
        void SetLeft(double value);

        /// <summary>
        /// Gets or sets the position of the window's top edge.
        /// </summary>
        PROPERTY_(double, Top);
        double GetTop() const;
        void SetTop(double value);

        PROPERTY_(UI::WindowStyle, WindowStyle);
        UI::WindowStyle GetWindowStyle() const;
        void SetWindowStyle(UI::WindowStyle value);

        PROPERTY_(std::shared_ptr<UI::Window>, Owner);
        std::shared_ptr<UI::Window> const& GetOwner() const { return _owner; }
        void SetOwner(std::shared_ptr<UI::Window> const& value) { _owner = value; }

        /// <summary>
        /// Closes the window.
        /// </summary>
        void Close() override;

        /// <summary>
        /// Hides the window but does not close it.
        /// </summary>
        void Hide() override;

        /// <summary>
        /// Shows the window.
        /// </summary>
        void Show() override;

        /// <summary>
        /// Shows the window as a dialog.
        /// </summary>
        /// <typeparam name="TResult">
        /// The type of the result produced by the dialog.
        /// </typeparam>
        /// <returns>.
        /// A task that can be used to retrive the result of the dialog when it closes.
        /// </returns>
        template<typename TResult>
        std::shared_ptr<TResult> ShowDialog()
        {
            return nullptr;
        }

        void DragMove();

        void __OnTitle_Click(void * sender, XUI::Core::Observer::RoutedEventArgs& e);
        void __OnTitle_PointerLeave(void * sender, XUI::Core::Observer::RoutedEventArgs& e);

        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        static void OnExecutedMinimizeWindow(void * sender, Core::Observer::ExecutedRoutedEventArgs & e);
        static void OnExecutedMaximizeWindow(void * sender, Core::Observer::ExecutedRoutedEventArgs & e);
        static void OnCanExecuteCloseWindow(void * sender, Core::Observer::CanExecuteRoutedEventArgs & e);
        static void OnExecutedCloseWindow(void * sender, Core::Observer::ExecutedRoutedEventArgs & e);
        static void OnExecutedRestoreWindow(void * sender, Core::Observer::ExecutedRoutedEventArgs & e);

    protected:

        virtual void OnWindowInitialized();

        /// <summary>
        /// Handles a paint notification from <see cref="Window.Resized"/>.
        /// </summary>
        /// <param name="rect">The dirty area.</param>
        virtual void HandlePaint(Core::Media::Rect const& rect) override;

        /// <summary>
        /// Handles a closed notification from <see cref="Window.Closed"/>.
        /// </summary>
        virtual void HandleClosed() override;

        /// <summary>
        /// Handles a resize notification from <see cref="Window.Resized"/>.
        /// </summary>
        /// <param name="clientSize">The new client size.</param>
        void HandleResized(Core::Media::Size const& clientSize) override;

        /// <summary>
        /// Handles input from <see cref="ITopLevelImpl.Input"/>.
        /// </summary>
        /// <param name="e">The event args.</param>
        void HandleInput(Interfaces::RawInputEventArgs* e) override;

        /// <summary>
        /// Handles a window scaling change notification from 
        /// <see cref="ITopLevelImpl.ScalingChanged"/>.
        /// </summary>
        /// <param name="scaling">The window scaling.</param>
        virtual void HandleScalingChanged(double scaling) override;

        /// <summary>
        /// Handles a window position change notification from 
        /// <see cref="IWindowBaseImpl.PositionChanged"/>.
        /// </summary>
        /// <param name="pos">The window position.</param>
        void HandlePositionChanged(Core::Media::Point const& pos) override;

        /// <summary>
        /// Handles an activated notification from <see cref="IWindowBaseImpl.Activated"/>.
        /// </summary>
        void HandleActivated() override;

        /// <summary>
        /// Handles a deactivated notification from <see cref="IWindowBaseImpl.Deactivated"/>.
        /// </summary>
        void HandleDeactivated() override;

        /// <inheritdoc/>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        virtual void OnPointerMoved(Interfaces::PointerEventArgs& e) override;
        virtual void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;
        virtual void OnPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;

        virtual void OnPreviewPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

    private:
        void EnsureInitialized();
    };
}
#endif // !_XUI_CONTROL_WINDOW_H__
