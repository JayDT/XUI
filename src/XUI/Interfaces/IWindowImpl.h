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

#ifndef __XUI_IWINDOW_IMPL_H__
#define __XUI_IWINDOW_IMPL_H__

#include "Core/Media/MediaTypes.h"
#include "Interfaces/InputEventArgs.h"
#include "Interfaces/IInputDevices.h"
#include "Interfaces/UIControls/IInputRoot.h"
#include "Interfaces/IStandardCursorFactory.h"
#include "Interfaces/IWindowIconImp.h"

namespace XUI::UI
{
    enum class WindowEdge : uint8;
    enum class WindowState : uint8;
}

namespace XUI::Interfaces
{
    struct IDrawingContextImpl;

    struct IWindowImpl : System::Reflection::Object
    {
        std::wstring _debugTitle;
        float _param0 = 0.0f;

        virtual XUI::Interfaces::IDrawingContextImpl* GetDrawingContext() = 0;
        virtual void SetBackgroundColor(Core::Media::Color const& color) = 0;

        virtual void Focus() const = 0;
        virtual void Unfocus() const = 0;
        virtual void Capture() const = 0;
        virtual void Uncapture() const = 0;
        virtual void SetActiveWindow() const = 0;
        virtual void SetParent(IWindowImpl*) const = 0;
        virtual void SetTimer(uint32 id, uint interval, std::function<void()> const& callback) = 0;
        virtual void UnsetTimer(uint32 id) = 0;

        /// <summary>
        /// Gets the client size of the toplevel.
        /// </summary>
        virtual Core::Media::Size GetClientSize() const = 0;

        /// <summary>
        /// Gets the scaling factor for the toplevel.
        /// </summary>
        virtual double GetScaling() const = 0;

        /// <summary>
        /// The list of native platform's surfaces that can be consumed by rendering subsystems.
        /// </summary>
        /// <remarks>
        /// Rendering platform will check that list and see if it can utilize one of them to output.
        /// It should be enough to expose a native window handle via IPlatformHandle
        /// and add support for framebuffer (even if it's emulated one) via IFramebufferPlatformSurface.
        /// If you have some rendering platform that's tied to your particular windowing platform,
        /// just expose some toolkit-specific object (e. g. Func&lt;Gdk.Drawable&gt; in case of GTK#+Cairo)
        /// </remarks>
        virtual std::vector<System::Reflection::RefObject> GetSurfaces() const = 0;

        /// <summary>
        /// Gets or sets a method called when the toplevel receives input.
        /// </summary>
        virtual std::function<void(RawInputEventArgs*)> GetInput() const = 0;
        virtual void SetInput(const std::function<void(RawInputEventArgs*)> &value) = 0;

        /// <summary>
        /// Gets or sets a method called when the toplevel requires painting.
        /// </summary>
        virtual std::function<void(Core::Media::Rect)> GetPaint() const = 0;
        virtual void SetPaint(const std::function<void(Core::Media::Rect)> &value) = 0;

        /// <summary>
        /// Gets or sets a method called when the toplevel is resized.
        /// </summary>
        virtual std::function<void(Core::Media::Size)> GetResized() const = 0;
        virtual void SetResized(const std::function<void(Core::Media::Size)> &value) = 0;

        /// <summary>
        /// Gets or sets a method called when the toplevel's scaling changes.
        /// </summary>
        virtual std::function<void(double)> GetScalingChanged() const = 0;
        virtual void SetScalingChanged(const std::function<void(double)> &value) = 0;

        /// <summary>
        /// Invalidates a rect on the toplevel.
        /// </summary>
        virtual void Invalidate(Core::Media::Rect const& rect) = 0;

        /// <summary>
        /// Sets the <see cref="IInputRoot"/> for the toplevel.
        /// </summary>
        virtual void SetInputRoot(Interfaces::UIControls::IInputRoot *inputRoot) = 0;

        /// <summary>
        /// Converts a point from screen to client coordinates.
        /// </summary>
        /// <param name="point">The point in screen coordinates.</param>
        /// <returns>The point in client coordinates.</returns>
        virtual Core::Media::Point PointToClient(Core::Media::Point const& point) = 0;

        /// <summary>
        /// Converts a point from client to screen coordinates.
        /// </summary>
        /// <param name="point">The point in client coordinates.</param>
        /// <returns>The point in screen coordinates.</returns>
        virtual Core::Media::Point PointToScreen(Core::Media::Point const& point) = 0;

        /// <summary>
        /// Sets the cursor associated with the toplevel.
        /// </summary>
        /// <param name="cursor">The cursor. Use null for default cursor</param>
        virtual void SetCursor(PlatformHandle const& cursor) = 0;

        /// <summary>
        /// Gets or sets a method called when the underlying implementation is destroyed.
        /// </summary>
        virtual std::function<void()> GetClosed() const = 0;
        virtual void SetClosed(const std::function<void()> &value) = 0;

        /// <summary>
        /// Gets a mouse device associated with toplevel
        /// </summary>
        virtual IMouseDevice *GetMouseDevice() const = 0;

        /// <summary>
        /// Shows the toplevel.
        /// </summary>
        virtual void Show() = 0;

        /// <summary>
        /// Hides the window.
        /// </summary>
        virtual void Hide() = 0;

        /// <summary>
        /// Starts moving a window with left button being held. Should be called from left mouse button press event handler.
        /// </summary>
        virtual void BeginMoveDrag() = 0;

        /// <summary>
        /// Starts resizing a window. This function is used if an application has window resizing controls. 
        /// Should be called from left mouse button press event handler
        /// </summary>
        virtual void BeginResizeDrag(UI::WindowEdge edge) = 0;

        /// <summary>
        /// Gets position of the window relatively to the screen
        /// </summary>
        virtual Core::Media::Point GetPosition() const = 0;
        virtual void SetPosition(Core::Media::Point const& value) = 0;

        /// <summary>
        /// Gets or sets a method called when the window's position changes.
        /// </summary>
        virtual std::function<void(Core::Media::Point const&)> GetPositionChanged() const = 0;
        virtual void SetPositionChanged(const std::function<void(Core::Media::Point const&)> &value) = 0;

        /// <summary>
        /// Activates the window.
        /// </summary>
        virtual void Activate() = 0;

        /// <summary>
        /// Gets or sets a method called when the window is deactivated (loses focus).
        /// </summary>
        virtual std::function<void()> GetDeactivated() const = 0;
        virtual void SetDeactivated(const std::function<void()> &value) = 0;

        /// <summary>
        /// Gets or sets a method called when the window is activated (receives focus).
        /// </summary>
        virtual std::function<void()> GetActivated() const = 0;
        virtual void SetActivated(const std::function<void()> &value) = 0;

        /// <summary>
        /// Gets the platform window handle.
        /// </summary>
        virtual PlatformHandle const& GetHandle() const = 0;

        /// <summary>
        /// Gets the maximum size of a window on the system.
        /// </summary>
        virtual Core::Media::Size GetMaxClientSize() const = 0;

        /// <summary>
        /// Sets the client size of the toplevel.
        /// </summary>
        virtual void Resize(Core::Media::Size const& clientSize) = 0;

        /// <summary>
        /// Gets or sets the minimized/maximized state of the window.
        /// </summary>
        virtual UI::WindowState GetWindowState() const = 0;
        virtual void SetWindowState(UI::WindowState value) = 0;

        /// <summary>
        /// Sets the title of the window.
        /// </summary>
        /// <param name="title">The title.</param>
        virtual void SetTitle(const std::string &title) = 0;
        virtual void SetTitle(const std::wstring & wtitle) = 0;

        /// <summary>
        /// Shows the window as a dialog.
        /// </summary>
        /// <returns>
        /// An <see cref="IDisposable"/> that should be used to close the window.
        /// </returns>
        virtual rxcpp::subscription ShowDialog() = 0;

        /// <summary>
        /// Enables or disables system window decorations (title bar, buttons, etc)
        /// </summary>
        virtual void SetSystemDecorations(bool enabled) = 0;

        /// <summary>
        /// Sets the icon of this window.
        /// </summary>
        virtual void SetIcon(std::shared_ptr<IWindowIconImpl> const& icon) = 0;

        /// <summary>
        /// Enables or disables the taskbar icon
        /// </summary>
        virtual void ShowTaskbarIcon(bool value) = 0;
    };
}

#endif //!__XUI_IWINDOW_IMPL_H__