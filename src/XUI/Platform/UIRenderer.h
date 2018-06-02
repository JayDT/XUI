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

#ifndef __XUI_UIRENDERER_H__
#define __XUI_UIRENDERER_H__

#include "standard/Platform/Define.h"
#include "standard/BasicPrimities.h"

#include "Interfaces/InputEventArgs.h"
#include "Interfaces/IInputDevices.h"
#include "Interfaces/IRenderer.h"
#include "Interfaces/IStandardCursorFactory.h"
#include "Interfaces/UIControls/IInputRoot.h"
#include "Interfaces/UIControls/IControl.h"

#include "UI/Controls/NameScope.h"
#include "Core/Input/KeyboardDevice.h"
#include "Core/Media/Size_t.h"
#include "XamlCPP/Core/Assembler/Settings.h"

#include <boost/uuid/uuid.hpp>
#include <rxcpp/rx.hpp>
#include <stack>
#include "sparse/sparsehash/dense_hash_set"

#include "UI/TopLevel.h"

#include <System/Uri.h>

MetaAssemblyBegin
Meta(modul: ReflInclude("XUI/Core/Input/MouseDevice.h"))
Meta(modul: ReflInclude("XamlCPP/DataBind/InstancedBinding.h"))
MetaAssemblyEnd

namespace XUI::Interfaces
{
    struct IWindowImpl;

    namespace UIControls
    {
        struct IFocusScope;
    }
}

//namespace XUI::UI::Controls
//{
//    class InputElement;
//}

namespace XUI::Core
{
    namespace Input
    {
        class MouseDevice;
        class KeyboardDevice;
    }

    namespace SceneGraph
    {
        class SceneNode;
    }
}

namespace XamlCPP::DataBind
{
    class InstancedBinding;
}

namespace XUI::Platform
{
    enum class UIRenderFlags
    {
        RELATIVE_RENDER = 0x01,
        RELATIVE_FIRST_IDENTIFY_MAT = 0x02,
    };

    inline bool operator & (UIRenderFlags lhs, UIRenderFlags rhs)
    {
        return (static_cast<std::underlying_type_t<UIRenderFlags>>(lhs) & static_cast<std::underlying_type_t<UIRenderFlags>>(rhs));
    }

    inline UIRenderFlags operator | (UIRenderFlags lhs, UIRenderFlags rhs)
    {
        return (UIRenderFlags)(static_cast<std::underlying_type_t<UIRenderFlags>>(lhs) | static_cast<std::underlying_type_t<UIRenderFlags>>(rhs));
    }

    /// <summary>
    /// class for top-level widgets.
    /// </summary>
    class
        TC_XUI_API
        UIRender
        : public UI::TopLevel
        , public Interfaces::IInputMgr
        //, public Interfaces::IRenderRoot
        , public UI::Controls::NameScope
        , public XamlCPP::Interfaces::IInstancedBindingManager
        //, public Interfaces::UIControls::IInputRoot
        , public Interfaces::UIControls::IGlobalDataTemplates
        , public Interfaces::IStandardCursorFactory
    {
    public:

        typedef std::map<std::string, System::Reflection::meta::Object*> NameScopeMap;
        typedef google::dense_hash_set<std::shared_ptr<XamlCPP::DataBind::InstancedBinding>> InstancedBindingSet;
        typedef std::deque<std::weak_ptr<XUI::UI::Controls::Visual>> InvalidatedVisuals;
        typedef std::list<std::shared_ptr<UI::TopLevel>> TopLevelList;

    protected:
        IInputMgr::InputObservable _preProcess;
        IInputMgr::InputObservable _process;
        IInputMgr::InputObservable _postProcess;

        std::shared_ptr<Core::Input::MouseDevice>    _mouseDevice;
        std::shared_ptr<Core::Input::KeyboardDevice> _keyBoardDevice;

        float _currentFPS = 0.f;
        bool _invalidatedRender = false;

        Core::Media::Size _clientSize;

        std::weak_ptr<XUI::UI::Controls::Visual> m_focusedElement;
        std::weak_ptr<XUI::UI::Controls::Visual> m_hoveredElement;

        System::Collection::List<std::shared_ptr<UI::DataTemplates::FrameworkTemplate>> _FrameworkTemplates;

        NameScopeMap _namescope;
        InstancedBindingSet _bindings;
        InvalidatedVisuals _invalidatedMeasure;
        InvalidatedVisuals _invalidatedArrange;
        InvalidatedVisuals _invalidatedVisual;
        TopLevelList _topLevelHnd;

        static System::Reflection::RefObject InternalLoadXaml(XamlCPP::Core::Assembler::Settings& settings, System::URI const& uri, std::string* raw = nullptr);

        bool ExecuteMeasurePass();
        bool ExecuteArrangePass();
        bool ExecuteScenePass();

        virtual void SetWindowDevice(std::shared_ptr<Interfaces::IWindowImpl> const& wnd) override;
        void SetFPS(float fps)
        {
            _currentFPS = fps;
        }

    public:
        //// {B15147E6-5C0A-4445-82F3-446F69AADB96}
        static constexpr boost::uuids::uuid __uuid = { 0xb1, 0x51, 0x47, 0xe6, 0x5c, 0x0a, 0x44, 0x45, 0x82, 0xf3, 0x44, 0x6f, 0x69, 0xaa, 0xdb, 0x96 };

        static void PlatformMainInitializer();

        UIRender();
        virtual ~UIRender();

        inline static System::Reflection::RefObject LoadXaml(System::URI const& uri, std::string* raw = nullptr, const System::Reflection::meta::ReflectionModule * m_defaultAssembly = System::Reflection::meta::ReflectionModulManager::Instance().GetCurrentAssembly())
        {
            XamlCPP::Core::Assembler::Settings settings(m_defaultAssembly);
            return InternalLoadXaml(settings, uri, raw);
        }

        // Return backend handle
        virtual void* InitializeInlineMainWindow() = 0;

        void ExecuteLayoutPass();

        PROPERTY_T_G(Core::Media::Size const&, _clientSize, ClientSize);
        PROPERTY_G(_invalidatedRender, InvalidatedRender);

        PROPERTY_GS(m_focusedElement, FocusControl);
        PROPERTY_GS(m_hoveredElement, HoverControl);

        void SetFocusedElement(XUI::UI::Controls::InputElement*,
            Interfaces::NavigationMethod method = Interfaces::NavigationMethod::Unspecified,
            Interfaces::InputModifiers modifiers = Interfaces::InputModifiers::None);

        void Hover(XUI::UI::Controls::InputElement*);

        /// <summary>
        /// Focuses a control.
        /// </summary>
        /// <param name="control">The control to focus.</param>
        /// <param name="method">The method by which focus was changed.</param>
        /// <param name="modifiers">Any input modifiers active at the time of focus.</param>
        void Focus(
            UI::Controls::InputElement* control,
            Interfaces::NavigationMethod method = Interfaces::NavigationMethod::Unspecified,
            Interfaces::InputModifiers modifiers = Interfaces::InputModifiers::None);

        /// <summary>
        /// Gets the focus scope ancestors of the specified control, traversing popups.
        /// </summary>
        /// <param name="control">The control.</param>
        /// <returns>The focus scopes.</returns>
        static Interfaces::UIControls::IFocusScope* GetFocusScopeAncestors(UI::Controls::InputElement* control);

        /// <summary>
        /// Checks if the specified element can be focused.
        /// </summary>
        /// <param name="e">The element.</param>
        /// <returns>True if the element can be focused.</returns>
        static bool CanFocus(XUI::UI::Controls::InputElement* e);

        UI::Controls::Visual* ToVisualBase() const override final { return const_cast<UI::Controls::Visual*>(static_cast<const UI::Controls::Visual*>(this)); }
        virtual UI::Controls::Visual* ToVisual() override { return static_cast<UI::Controls::Visual*>(this); }

        System::Events::EventHandler<> OnApplicationExit;

        void Render(Interfaces::IDrawingContextImpl* context) override;
        static void Render(Interfaces::IDrawingContextImpl* context, Visual* visual, Core::Media::Rect const& clipRect, Platform::UIRenderFlags flags = Platform::UIRenderFlags(0));
        virtual void RenderFps(XUI::Interfaces::IWindowImpl* context, Core::Media::Rect const& clientRect, bool incrementFrameCount);

        void Paint(UI::TopLevel* hnd, Core::Media::Rect const& rect);

        void AddDirty(UI::Controls::Visual* context);
        void AddTopLevelHandle(UI::TopLevel* hnd);
        void RemoveTopLevelHandle(UI::TopLevel* hnd);

        UI::Controls::Control* HitTest(Core::Media::Point const& point, std::function<bool(UI::Controls::Control*)> const& filter) override;
        UI::Controls::Control* HitTest(Core::Media::Point const& point, UI::Controls::Control* root, std::function<bool(UI::Controls::Control*)> const& filter) override;
        UI::Controls::Visual* HitTestVisualTree(Core::Media::Point const& point, UI::Controls::Visual* root, std::function<bool(UI::Controls::Visual*)> const& filter) override;
        void HitTestChain(std::vector<UI::Controls::Visual*>& result, Core::Media::Point const& point, UI::Controls::Visual* root, std::function<bool(UI::Controls::Visual*)> const& filter) override;

        /// <summary>
        /// Handles a paint notification from <see cref="ITopLevelImpl.Resized"/>.
        /// </summary>
        /// <param name="rect">The dirty area.</param>
        virtual void HandlePaint(Core::Media::Rect const& rect) override;

        /// <summary>
        /// Handles a closed notification from <see cref="ITopLevelImpl.Closed"/>.
        /// </summary>
        virtual void HandleClosed();

        /// <summary>
        /// Handles a resize notification from <see cref="ITopLevelImpl.Resized"/>.
        /// </summary>
        /// <param name="clientSize">The new client size.</param>
        virtual void HandleResized(Core::Media::Size const& clientSize);

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
        void HandlePositionChanged(Core::Media::Point const& pos)
        {
        }

        /// <summary>
        /// Handles an activated notification from <see cref="IWindowBaseImpl.Activated"/>.
        /// </summary>
        void HandleActivated()
        {
            XUI::UI::TopLevel::HandleActivated();
        }

        /// <summary>
        /// Handles a deactivated notification from <see cref="IWindowBaseImpl.Deactivated"/>.
        /// </summary>
        void HandleDeactivated()
        {
            XUI::UI::TopLevel::HandleDeactivated();
        }

        void OnAttachedToVisualTreeCore(XUI::UI::Controls::VisualTreeAttachmentEventArgs const& e) override;

        void OnApplicationExiting(void* sender, System::Events::EventArg& args);

        /// <summary>
        /// Handles the application exiting, either from the last window closing, or a call to <see cref="IApplicationLifecycle.Exit"/>.
        /// </summary>
        virtual void HandleApplicationExiting();

        /// <summary>
        /// Handles input from <see cref="ITopLevelImpl.Input"/>.
        /// </summary>
        /// <param name="e">The event args.</param>
        void HandleInput(Interfaces::RawInputEventArgs* e)
        {
            ProcessInput(*e);
        }

        /// <summary>
        /// Global handler for pointer pressed events.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
        void OnPreviewPointerPressed(Interfaces::PointerPressedEventArgs & e);

        void AddInstancedBinding(XamlCPP::DataBind::InstancedBinding*);
        void RemoveInstancedBinding(XamlCPP::DataBind::InstancedBinding*);

        /// <inheritdoc/>
        virtual void Invalidate(Core::Media::Rect const& rect) = 0;
        virtual Core::Media::Point PointToClient(Core::Media::Point const& point) override { throw System::NotImplementedException(""); }
        virtual Core::Media::Point PointToScreen(Core::Media::Point const& point) override { throw System::NotImplementedException(""); }
        virtual XUI::Interfaces::IWindowImpl* GetWindowImpl() const override { return UI::TopLevel::GetWindowImpl(); }
        virtual XUI::Interfaces::IWindowImpl* GetFirstActiveWindowImpl() const = 0;

        // Inherited via IInputMgr
        virtual void ProcessInput(Interfaces::RawInputEventArgs & e) override;
        virtual XUI::Interfaces::IInputMgr::InputObservable & GetPreProcess();
        virtual XUI::Interfaces::IInputMgr::InputObservable & GetProcess();
        virtual XUI::Interfaces::IInputMgr::InputObservable & GetPostProcess();

        // Inherited via IGlobalDataTemplates
        void AddTemplates(std::shared_ptr<UI::DataTemplates::FrameworkTemplate> const& t) override
        {
            _FrameworkTemplates.Add(t);
        }
        void RemoveTemplates(std::shared_ptr<UI::DataTemplates::FrameworkTemplate> const& t) override
        {
            _FrameworkTemplates.Remove(t);
        }
        System::Collection::List<std::shared_ptr<UI::DataTemplates::FrameworkTemplate>>& GlobalTemplates() override { return _FrameworkTemplates; }

        void ExecuteInitialLayoutPass(XUI::UI::Controls::Visual* control);

        void Measure(XUI::UI::Controls::Visual* control);
        void Arrange(XUI::UI::Controls::Visual* control);
        bool SceneUpdate(XUI::UI::Controls::Visual* control, Core::Media::Matrix* contextTransform = nullptr, Core::Media::Rect* availableSize = nullptr);

        virtual void InstanceInitializer() override;
    };
}

#endif //!__XUI_UIRENDERER_H__