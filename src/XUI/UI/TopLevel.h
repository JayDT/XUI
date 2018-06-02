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

#ifndef _XUI_UI_TOPLEVEL_H__
#define _XUI_UI_TOPLEVEL_H__

#include "UI/Controls/ContentControl.h"
#include "Interfaces/IInputDevices.h"
#include "Interfaces/IPlatformRenderInterface.h"
#include "Interfaces/IComponentConnector.h"

namespace XUI::Platform
{
    class UIRender;
}

namespace XUI::UI
{
    enum class WindowEdge : uint8;

    enum class WindowStyle : uint8
    {
        None,
        SingleBorderWindow = 1,
        ThreeDBorderWindow = 2,
        ToolWindow = 3,
        GUIWindow = 4,
    };

    struct PointEventArgs : System::Events::EventArg
    {
        PointEventArgs() {}

        PointEventArgs(Core::Media::Point const& point)
            : System::Events::EventArg()
            , Point(point)
        { }

        virtual ~PointEventArgs() {}

        Core::Media::Point Point = Core::Media::Point(0, 0);
    };

    /// <summary>
    /// Base class for top-level windows.
    /// </summary>
    class  TC_XUI_API TopLevel
        : public Controls::ContentControl
        , public Interfaces::UIControls::IInputRoot
        , public Interfaces::IRenderRoot
        , public Interfaces::ITopLevelInitializer
    {
        friend class XUI::Platform::UIRender;

    protected:
        std::shared_ptr<Interfaces::IWindowImpl> _windowImpl;

    public:
        /// <summary>
        /// Defines the <see cref="ClientSize"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ClientSizeProperty;

        /// <summary>
        /// Defines the <see cref="IsActive"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty IsActiveProperty;

        /// <summary>
        /// Defines the <see cref="IInputRoot.PointerOverElement"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty PointerOverElementProperty;

    protected:
        const std::shared_ptr<Interfaces::IInputMgr> _inputManager;
        const std::shared_ptr<Interfaces::IPlatformRenderInterface> _renderInterface;
        Core::Media::Size _clientSize;
        Core::Media::Size _maxClientSize;
        Core::Media::Point _position = Core::Media::Point(0, 0);
        float _layoutScaling = 1.f;
        bool m_AutoSizing = false;
        bool _isActive = false;
        bool _isRootMode = true;

    public:

        XUI::Interfaces::IDrawingContextImpl* GetDrawingContext() override
        {
            if (_windowImpl)
                return _windowImpl->GetDrawingContext();
            return nullptr;
        }

        Interfaces::IWindowImpl* GetPlatformImpl() const { return _windowImpl.get(); }
        virtual XUI::Interfaces::IWindowImpl* GetWindowImpl() const { return GetPlatformImpl(); }

        // Inherited Interfaces::UIControls::IInputRoot
        virtual XUI::UI::Controls::Visual * ToVisualBase() const override;
        virtual Core::Media::Point PointToClient(Core::Media::Point const& point) override;
        virtual Core::Media::Point PointToScreen(Core::Media::Point const& point) override;
        bool IsRootMode() const override { return _isRootMode; }

        // Inherited Interfaces::IRenderRoot
        virtual void Render(Interfaces::IDrawingContextImpl* context) override;
        void AddDirty(UI::Controls::Visual* context) override;
        void Invalidate(Core::Media::Rect const& rect) override;
        UI::Controls::Visual* ToVisual() override { return static_cast<UI::Controls::Visual*>(this); }
        UI::Controls::Control* HitTest(Core::Media::Point const& point, std::function<bool(UI::Controls::Control*)> const& filter) override;
        UI::Controls::Control* HitTest(Core::Media::Point const& point, UI::Controls::Control* root, std::function<bool(UI::Controls::Control*)> const& filter) override;
        UI::Controls::Visual* HitTestVisualTree(Core::Media::Point const& point, UI::Controls::Visual* root, std::function<bool(UI::Controls::Visual*)> const& filter) override;
        void HitTestChain(std::vector<UI::Controls::Visual*>& result, Core::Media::Point const& point, UI::Controls::Visual* root, std::function<bool(UI::Controls::Visual*)> const& filter) override;

        static void StaticClassInitializer();
        virtual void InstanceInitializer();

        TopLevel();
        virtual ~TopLevel() {}

        /// <summary>
        /// Fired when the window is activated.
        /// </summary>
        System::Events::EventHandler<> Activated;

        /// <summary>
        /// Fired when the window is closed.
        /// </summary>
        System::Events::EventHandler<> Closed;

        /// <summary>
        /// Fired when the window is deactivated.
        /// </summary>
        System::Events::EventHandler<> Deactivated;

        /// <summary>
        /// Fired when the window position is changed.
        /// </summary>
        System::Events::EventHandler<PointEventArgs> PositionChanged;

        /// <summary>
        /// Gets or sets the client size of the window.
        /// </summary>
        PROPERTY_(Core::Media::Size, ClientSize);
        Core::Media::Size const& GetClientSize() override
        {
            return _clientSize;
        }
    private:
        void SetClientSize(Core::Media::Size const& value);

    public:
        /// <summary>
        /// Gets a value that indicates whether the window is active.
        /// </summary>
        PROPERTY_(bool, IsActive);
        bool GetIsActive() const { return _isActive; }
    private:
        void SetIsActive(bool value);

    public:
        /// <summary>
        /// Gets or sets the window position in screen coordinates.
        /// </summary>
        PROPERTY_(Core::Media::Point, Position);
        Core::Media::Point GetPosition() const;
        void SetPosition(Core::Media::Point const& value);

        /// <summary>
        /// Gets or sets the input element that the pointer is currently over.
        /// </summary>
        PROPERTY_(std::weak_ptr<InputElement>, PointerOverElement);
        std::weak_ptr<InputElement> GetPointerOverElement() const;
        void SetPointerOverElement(std::weak_ptr<InputElement> const& value);

        /// <inheritdoc/>
        PROPERTY_(Core::Media::Size, MaxClientSize);
        Core::Media::Size const& GetMaxClientSize() const { return _maxClientSize; }
        void SetMaxClientSize(Core::Media::Size const& value) { _maxClientSize = value; }

        /// <inheritdoc/>
        PROPERTY_GET_ONLY(float, LayoutScaling);
        float GetLayoutScaling() const;

    protected:
        /// <summary>
        /// Whether an auto-size operation is in progress.
        /// </summary>
        PROPERTY_(bool, AutoSizing);
        bool GetAutoSizing() const { return m_AutoSizing; }
        void SetAutoSizing(bool value) { m_AutoSizing = value; }

        virtual void InitializePlatformWindow();
        virtual void InitializeWindowHooks();
        virtual void SetWindowDevice(std::shared_ptr<Interfaces::IWindowImpl> const& wnd);

        /// <summary>
        /// Handles a paint notification from <see cref="Window.Resized"/>.
        /// </summary>
        /// <param name="rect">The dirty area.</param>
        virtual void HandlePaint(Core::Media::Rect const& rect)
        {

        }

        /// <summary>
        /// Handles a closed notification from <see cref="Window.Closed"/>.
        /// </summary>
        virtual void HandleClosed();

        /// <summary>
        /// Handles a resize notification from <see cref="Window.Resized"/>.
        /// </summary>
        /// <param name="clientSize">The new client size.</param>
        virtual void HandleResized(Core::Media::Size const& clientSize);

        /// <summary>
        /// Handles input from <see cref="ITopLevelImpl.Input"/>.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void HandleInput(Interfaces::RawInputEventArgs* e);

        /// <summary>
        /// Handles a window scaling change notification from 
        /// <see cref="ITopLevelImpl.ScalingChanged"/>.
        /// </summary>
        /// <param name="scaling">The window scaling.</param>
        virtual void HandleScalingChanged(double scaling);

        /// <summary>
        /// Handles a window position change notification from 
        /// <see cref="IWindowBaseImpl.PositionChanged"/>.
        /// </summary>
        /// <param name="pos">The window position.</param>
        virtual void HandlePositionChanged(Core::Media::Point const& pos);

        /// <summary>
        /// Handles an activated notification from <see cref="IWindowBaseImpl.Activated"/>.
        /// </summary>
        virtual void HandleActivated();

        /// <summary>
        /// Handles a deactivated notification from <see cref="IWindowBaseImpl.Deactivated"/>.
        /// </summary>
        virtual void HandleDeactivated();

    public:
        /// <summary>
        /// Activates the window.
        /// </summary>
        virtual void Activate();

        /// <summary>
        /// Closes the window.
        /// </summary>
        virtual void Close()
        {
            HandleClosed();
        }

        /// <summary>
        /// Hides the window but does not close it.
        /// </summary>
        virtual void Hide()
        {

        }

        /// <summary>
        /// Shows the window.
        /// </summary>
        virtual void Show()
        {

        }

    protected:
        /// <summary>
        /// Begins an auto-resize operation.
        /// </summary>
        /// <returns>A disposable used to finish the operation.</returns>
        /// <remarks>
        /// When an auto-resize operation is in progress any resize events received will not be
        /// cause the new size to be written to the <see cref="Layoutable.Width"/> and
        /// <see cref="Layoutable.Height"/> properties.
        /// </remarks>
        void SetOnAutoSizing(bool on);

        /// <summary>
        /// Carries out the arrange pass of the window.
        /// </summary>
        /// <param name="finalSize">The final window size.</param>
        /// <returns>The <paramref name="finalSize"/> parameter unchanged.</returns>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

    private:

        void OnApplicationExiting(void* sender, System::Events::EventArg& args);

        /// <summary>
        /// Handles the application exiting, either from the last window closing, or a call to <see cref="IApplicationLifecycle.Exit"/>.
        /// </summary>
    protected:
        virtual void HandleApplicationExiting();

    public:
        /// <summary>
        /// Starts moving a window with left button being held. Should be called from left mouse button press event handler
        /// </summary>
        void BeginMoveDrag();

        /// <summary>
        /// Starts resizing a window. This function is used if an application has window resizing controls. 
        /// Should be called from left mouse button press event handler
        /// </summary>
        void BeginResizeDrag(WindowEdge edge);
    };
}

#endif //!_XUI_UI_TOPLEVEL_H__