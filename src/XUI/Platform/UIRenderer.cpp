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

#include "UIRenderer.h"
#include "Interfaces/IWindowImpl.h"

#include "XUI/Platform/UIService.h"
#include "Brush.h"
#include "XamlCPP/Core/LogInterface.h"
#include "IRenderer.h"
#include "Transform.h"
#include "RelativePoint_t.h"
#include "Geometry.h"

#include "Core/Input/KeyboardDevice.h"
#include "Core/Input/MouseDevice.h"
#include "Core/SceneGraph/SceneNode.h"
#include "Core/GlobalInitializerScheduler.h"
#include "Core/TypeConverter/TypeConverters.h"
#include "Core/Application.h"

#include "UI/Controls/Control.h"
#include "UI/TopLevel.h"
#include "Interfaces/UIControls/IControl.h"
#include "Interfaces/IComponentConnector.h"
#include "XamlCPP/DataBind/InstancedBinding.h"
#include "XamlCPP/XamlCPP.h"
#include "XamlCPP/Core/ReflEx/TypeConverterRegistration.h"

#include "standard/Random/random_number.h"
#include "standard/events.h"

#include <System/Resource/ResourceManager.h>

#include "XUI/Core/GlobalInitializerScheduler.h"

//#include "UI/DataTemplates/DataTemplate.h"
//#include "Core/Media/MediaTypes.h"


void XUI::Platform::UIRender::PlatformMainInitializer()
{
    std::shared_ptr<XamlCPP::Interfaces::ILoader> xamlLoader = XamlCPP::Xaml::DefaultLoader(XUI::Platform::UIService::Instance());
    XUI::Core::TypeConverter::Initialize();
    XUI::Core::Application::InitializeDefaultResoures();
}

using namespace XUI;

XUI::Platform::UIRender::UIRender()
{
    XUI::Platform::UIService::Instance()->Add<XUI::Platform::UIRender>(static_cast<XUI::Platform::UIRender*>(this));
    XUI::Platform::UIService::Instance()->Add<Interfaces::IInputMgr>(static_cast<Interfaces::IInputMgr*>(this));
    XUI::Platform::UIService::Instance()->Add<Interfaces::UIControls::IGlobalDataTemplates>(static_cast<Interfaces::UIControls::IGlobalDataTemplates*>(this));
    XUI::Platform::UIService::Instance()->Add<XamlCPP::Interfaces::INameScope>(static_cast<XamlCPP::Interfaces::INameScope*>(this));
    XUI::Platform::UIService::Instance()->Add<XamlCPP::Interfaces::IInstancedBindingManager>(static_cast<XamlCPP::Interfaces::IInstancedBindingManager*>(this));
    XUI::Platform::UIService::Instance()->Add<Interfaces::IStandardCursorFactory>(static_cast<Interfaces::IStandardCursorFactory*>(this));

    OnApplicationExit += System::make_eventhandler(&UIRender::OnApplicationExiting, this);

    _mouseDevice = std::make_shared<Core::Input::MouseDevice>();
    _keyBoardDevice = std::make_shared<Core::Input::KeyboardDevice>();

    _isAttachedToLogicalTree = true;

    _bindings.set_empty_key(nullptr);

    //UI::Controls::InputElement::PointerPressedEvent->AddClassHandler(&typeof(UI::Controls::InputElement), System::make_eventhandler(&UIRender::OnPreviewPointerPressed, this), Core::Observer::RoutingStrategies::Tunnel);
}

XUI::Platform::UIRender::~UIRender()
{
    XUI::Platform::UIService::Instance()->Remove<XUI::Platform::UIRender>();
    XUI::Platform::UIService::Instance()->Remove<Interfaces::IInputMgr>();
    XUI::Platform::UIService::Instance()->Remove<Interfaces::UIControls::IGlobalDataTemplates>();
    XUI::Platform::UIService::Instance()->Remove<XamlCPP::Interfaces::INameScope>();
    XUI::Platform::UIService::Instance()->Remove<XamlCPP::Interfaces::IInstancedBindingManager>();
    XUI::Platform::UIService::Instance()->Remove<Interfaces::IStandardCursorFactory>();
}

void XUI::Platform::UIRender::SetWindowDevice(std::shared_ptr<Interfaces::IWindowImpl> const& wnd)
{
    if (GetWindowImpl())
    {
        //GetWindowImpl()->SetInputRoot(this);
        _clientSize = GetWindowImpl()->GetClientSize();
        GetWindowImpl()->SetInputRoot(static_cast<Interfaces::UIControls::IInputRoot *>(this));
        auto background = Background;
        GetWindowImpl()->SetBackgroundColor(Core::Media::Color(0xFF, 0xFF, 0xFF, 0xFF * (background ? background->GetOpacity() : 1.0)));

        InitializeWindowHooks();
    }
}

System::Reflection::RefObject XUI::Platform::UIRender::InternalLoadXaml(XamlCPP::Core::Assembler::Settings& settings, System::URI const & uri, std::string* raw /*= nullptr*/)
{
    std::ostringstream stream;
    if (raw)
    {
        stream << *raw;
    }
    else
    {
        System::Resource::ResourceObject* resource = System::Resource::ResourceManager::Instance()->FindResource(uri);
        if (!resource)
            return nullptr;

        stream << resource->ToString();
    }
    
    std::shared_ptr<XamlCPP::Interfaces::ILoader> xamlLoader = XamlCPP::Xaml::DefaultLoader(XUI::Platform::UIService::Instance());
    System::Reflection::RefObject result = xamlLoader->Load(stream, &settings);

    Interfaces::IComponentConnector* connector = result->Dynamic_As<Interfaces::IComponentConnector>();
    if (connector)
        connector->InitializeComponent();

    Interfaces::ITopLevelInitializer* initializer = result->Dynamic_As<Interfaces::ITopLevelInitializer>();
    if (initializer)
        initializer->InstanceInitializer();

    return result;
}

bool XUI::Platform::UIRender::ExecuteMeasurePass()
{
    while (!_invalidatedMeasure.empty())
    {
        std::shared_ptr<XUI::UI::Controls::Visual> visual = _invalidatedMeasure.front().lock();
        _invalidatedMeasure.pop_front();
        if (!visual)
            continue;

        if (!visual->IsMeasureValid)
            Measure(visual.get());
    }

    return true;
}

bool XUI::Platform::UIRender::ExecuteArrangePass()
{
    while (!_invalidatedArrange.empty() && _invalidatedMeasure.empty())
    {
        std::shared_ptr<XUI::UI::Controls::Visual> visual = _invalidatedArrange.front().lock();
        _invalidatedArrange.pop_front();
        if (!visual)
            continue;

        if (!visual->IsArrangeValid)
            Arrange(visual.get());
    }

    return _invalidatedMeasure.empty();
}

bool XUI::Platform::UIRender::ExecuteScenePass()
{
    while (!_invalidatedVisual.empty() && _invalidatedMeasure.empty() && _invalidatedArrange.empty())
    {
        std::shared_ptr<XUI::UI::Controls::Visual> visual = _invalidatedVisual.front().lock();
        _invalidatedVisual.pop_front();
        if (!visual)
            continue;

        if (visual->RenderScene() && visual->RenderScene()->InvalidatedVisual())
            SceneUpdate(visual.get());
    }

    return _invalidatedMeasure.empty() && _invalidatedArrange.empty();
}

void XUI::Platform::UIRender::ExecuteLayoutPass()
{
    while (true)
    {
        if (ExecuteMeasurePass())
            if (ExecuteArrangePass())
                if (ExecuteScenePass())
                    break;
    }
}

void XUI::Platform::UIRender::Render(Interfaces::IDrawingContextImpl * context)
{
    UI::Controls::Visual::Render(context);
}

void XUI::Platform::UIRender::Render(Interfaces::IDrawingContextImpl * context, Visual* visual, Core::Media::Rect const& clipRect, Platform::UIRenderFlags flags /*= 0*/)
{
    Core::SceneGraph::SceneNode* scene = visual->RenderScene();
    if (!scene)
        return; // not render object

    bool relative = flags & Platform::UIRenderFlags::RELATIVE_RENDER;
    if (!relative && scene->InvalidatedVisual())
        scene->UpdateTransformation(context->GetTransform(), clipRect);

    if (visual->IsVisible && scene->Opacity > 0)
    {
        const Core::Media::Matrix* forRelative;
        if (relative)
        {
            forRelative = &context->GetTransform();
            if (flags & Platform::UIRenderFlags::RELATIVE_FIRST_IDENTIFY_MAT)
            {
                flags = Platform::UIRenderFlags(uint32(flags) & ~uint32(Platform::UIRenderFlags::RELATIVE_FIRST_IDENTIFY_MAT));
                context->PushTransform(Core::Media::Matrix::Identity(), true);
            }
            else
            {
                context->PushTransform(scene->RelativeTransformation, true);
            }
        }
        else
            context->PushTransform(scene->AbsoluteTransformation, false);
        context->PushOpacity(scene->Opacity);
        if (scene->ClipToBounds)
            context->PushClip(scene->RelativeClipRect);
        if (visual->Clip != nullptr)
            context->PushGeometryClip(visual->Clip);
        if (visual->OpacityMask != nullptr)
            context->PushOpacityMask(visual->OpacityMask.get(), scene->Bounds);

        {
            visual->Render(context);

            for(auto const& child : visual->VisualChildren.GetContainer())
            {
                Visual* child_visual = child.get();
                if (!child_visual)
                    continue;

                Core::SceneGraph::SceneNode* child_scene = child_visual->RenderScene();
                if (!child_scene)
                    continue;

                if (relative)
                {
                    if (!child_scene->ClipToBounds || scene->GetClipRect(*forRelative).intersectsOrTouches(child_scene->GetClipRect(context->GetTransform())))
                    {
                        Render(context, child_visual, scene->GetClipRect(*forRelative), flags);
                    }
                }
                else
                {
                    if (child_scene->InvalidatedVisual())
                        child_scene->UpdateTransformation(context->GetTransform(), scene->ClipRect);

                    if (!child_scene->ClipToBounds || scene->ClipRect.intersectsOrTouches(child_scene->ClipRect))
                    {
                        Render(context, child_visual, scene->ClipRect, flags);
                    }
                }
            }
        }

        context->PopTransform();
        context->PopOpacity();
        if (scene->ClipToBounds)
            context->PopClip();
        if (visual->Clip != nullptr)
            context->PopGeometryClip();
        if (visual->OpacityMask != nullptr)
            context->PopOpacityMask();
    }
}

void XUI::Platform::UIRender::RenderFps(XUI::Interfaces::IWindowImpl * context, Core::Media::Rect const & clientRect, bool incrementFrameCount)
{
    //var now = _stopwatch.Elapsed;
    //var elapsed = now - _lastFpsUpdate;
    //
    //if (incrementFrameCount)
    //{
    //    ++_framesThisSecond;
    //}
    //
    //if (elapsed.TotalSeconds > 1)
    //{
    //    _fps = (int)(_framesThisSecond / elapsed.TotalSeconds);
    //    _framesThisSecond = 0;
    //    _lastFpsUpdate = now;
    //}
    //
    //_fpsText.Text = string.Format("FPS: {0:000}", _fps);
    //var size = _fpsText.Measure();
    //var rect = new Rect(clientRect.Right - size.Width, 0, size.Width, size.Height);
    //
    //context.Transform = Matrix.Identity;
    //context.FillRectangle(Brushes.Black, rect);
    //context.DrawText(Brushes.White, rect.TopLeft, _fpsText.PlatformImpl);
}

void XUI::Platform::UIRender::Paint(UI::TopLevel* hnd, Core::Media::Rect const & rect)
{
    XUI::Interfaces::IDrawingContextImpl* context = hnd->GetWindowImpl()->GetDrawingContext();
    try
    {
        ExecuteLayoutPass();

        //if (_invalidatedRender) // ToDo: move to top level
        {
            _invalidatedRender = false;

            Render(context, hnd, hnd->Bounds);

            //for (auto const& child : VisualChildren.GetContainer())
            //{
            //    Visual* child_visual = child.get();
            //    if (!child_visual)
            //        continue;
            //
            //    XUI::Interfaces::IDrawingContextImpl* childContext = child_visual->GetDrawingContext();
            //    if (childContext && childContext != context)
            //        throw System::InvalidOperationException("");
            //
            //    Render(context, child_visual, Bounds);
            //}
        }

        if (GetDebugFlag() & Core::Dependency::RenderDebugFlags::RDF_DRAW_DIRTY_RECTS)
        {
            auto color = Core::Media::Color(urand(0, 0xff), urand(0, 0xff), urand(0, 0xff), 0x44);
            Core::Media::SolidColorBrush brush;
            brush.Color = color;
            brush.Opacity = 1.0f;

            context->FillRectangle(
                &brush,
                rect);
        }

        if (GetDebugFlag() & Core::Dependency::RenderDebugFlags::RDF_FPS)
        {
            RenderFps(hnd->GetWindowImpl(), Bounds, true);
        }
    }
    catch (std::exception const& ex)
    {
        XamlCPP::Platform::ILog::Error("Renderer", nullptr, "Render target was corrupted. Exception: {0}", ex.what());
    }
}

void XUI::Platform::UIRender::AddDirty(UI::Controls::Visual * visual)
{
    auto weakVisual = visual->weak_from_base_static<UI::Controls::Visual>();

    if (!visual->IsMeasureValid)
        _invalidatedMeasure.push_back(weakVisual);
    if (!visual->IsArrangeValid)
        _invalidatedArrange.push_back(weakVisual);
    if (visual->RenderScene() && visual->RenderScene()->InvalidatedVisual())
        _invalidatedVisual.push_back(weakVisual);

    if (visual->Bounds.width() != 0 && visual->Bounds.height() != 0)
    {
        //Core::Media::Matrix m =  //TransformToVisual(this);
        //
        //Core::Media::Rect bounds(visual->Bounds.wh());
        //bounds = bounds * m;

        Invalidate(visual->RenderScene()->ClipRect);
    }
}

void XUI::Platform::UIRender::AddTopLevelHandle(UI::TopLevel * hnd)
{
    _topLevelHnd.push_back(hnd->shared_from_base_static<UI::TopLevel>());
}

void XUI::Platform::UIRender::RemoveTopLevelHandle(UI::TopLevel * hnd)
{
    auto itr = std::find(_topLevelHnd.begin(), _topLevelHnd.end(), hnd->shared_from_base_static<UI::TopLevel>());
    if (itr != _topLevelHnd.end())
        _topLevelHnd.erase(itr);
}

UI::Controls::Control* XUI::Platform::UIRender::HitTest(Core::Media::Point const& point, std::function<bool(UI::Controls::Control*)> const& filter)
{
    bool containsPoint;
    if (RenderScene() && RenderScene()->AbsoluteTransformationBoundingBox)
        containsPoint = RenderScene()->AbsoluteTransformationBoundingBox->Contains(point);
    else
        containsPoint = TransformedBounds.intersectsOrTouches(point);

    if ((containsPoint || (RenderScene() && !RenderScene()->ClipToBounds)) && GetVisualChildren().size() > 0)
    {
        for (auto child = GetVisualChildren().GetContainer().rbegin(); child != GetVisualChildren().GetContainer().rend(); ++child)
        {
            UI::Controls::Control* childControl = (*child)->Static_As<UI::Controls::Control>();
            if (!childControl)
                continue;

            UI::Controls::Control* hit = HitTest(point, childControl, filter);
            if (hit)
                return hit;
        }
    }

    return nullptr;
}

UI::Controls::Control* XUI::Platform::UIRender::HitTest(Core::Media::Point const& point, UI::Controls::Control* control, std::function<bool(UI::Controls::Control*)> const& filter)
{
    if (control && control->IsEnabledCore && (!control->RenderScene() || control->RenderScene()->HitTestVisible) && (!filter || filter(control) != false))
    {
        bool containsPoint;
        if (control->RenderScene() && !control->RenderScene()->InvalidatedVisual() && control->RenderScene()->AbsoluteTransformationBoundingBox)
            containsPoint = control->RenderScene()->AbsoluteTransformationBoundingBox->Contains(point);
        else
            containsPoint = control->TransformedBounds.contains(point);

        if ((containsPoint || !control->ClipToBounds) && control->GetLogicalChildren().size() > 0)
        {
            for (auto const& child : control->GetLogicalChildren().GetContainer())
            {
                UI::Controls::Control* hit = HitTest(point, child.get(), filter);
                if (hit)
                    return hit;
            }
        }

        if (containsPoint && control->IsHitTestVisible)
        {
            return control;
        }
    }

    return nullptr;
}

UI::Controls::Visual* XUI::Platform::UIRender::HitTestVisualTree(Core::Media::Point const& point, UI::Controls::Visual* visual, std::function<bool(UI::Controls::Visual*)> const& filter)
{
    if (visual && (!visual->RenderScene() || visual->RenderScene()->HitTestVisible) && (!filter || filter(visual) != false))
    {
        bool containsPoint;
        if (visual->RenderScene() && visual->RenderScene()->AbsoluteTransformationBoundingBox)
            containsPoint = visual->RenderScene()->AbsoluteTransformationBoundingBox->Contains(point);
        else
            containsPoint = visual->TransformedBounds.contains(point);

        if ((containsPoint || !visual->ClipToBounds) && visual->GetVisualChildren().size() > 0)
        {
            for (auto child = visual->GetVisualChildren().GetContainer().rbegin(); child != visual->GetVisualChildren().GetContainer().rend(); ++child)
            {
                UI::Controls::Visual* hit = HitTestVisualTree(point, child->get(), filter);
                if (hit)
                    return hit;
            }
        }

        if (containsPoint)
        {
            return visual;
        }
    }

    return nullptr;
}

void XUI::Platform::UIRender::HitTestChain(std::vector<UI::Controls::Visual*>& result, Core::Media::Point const& point, UI::Controls::Visual * visual, std::function<bool(UI::Controls::Visual*)> const & filter)
{
    std::vector<UI::Controls::Visual*> ret;

    if (visual && (!filter || filter(visual) != false))
    {
        bool containsPoint = visual->TransformedBounds.contains(point) == true;

        if ((containsPoint || !visual->ClipToBounds) && visual->VisualChildren.size() > 0)
        {
            for (auto child = visual->GetVisualChildren().GetContainer().rbegin(); child != visual->GetVisualChildren().GetContainer().rend(); ++child)
            {
                HitTestChain(result, point, child->get(), filter);
            }
        }

        if (containsPoint)
        {
            return result.push_back(visual);
        }
    }

    return ;
}

/// <summary>
/// Handles a paint notification from <see cref="ITopLevelImpl.Resized"/>.
/// </summary>
/// <param name="rect">The dirty area.</param>

void XUI::Platform::UIRender::HandlePaint(Core::Media::Rect const& rect)
{
    XUI::UI::TopLevel::HandlePaint(rect);
    //Paint(rect);
}

/// <summary>
/// Handles a closed notification from <see cref="ITopLevelImpl.Closed"/>.
/// </summary>

void XUI::Platform::UIRender::HandleClosed()
{
    XUI::UI::TopLevel::HandleClosed();
    OnApplicationExit -= System::make_eventhandler(&UIRender::OnApplicationExiting, this);
}

/// <summary>
/// Handles a resize notification from <see cref="ITopLevelImpl.Resized"/>.
/// </summary>
/// <param name="clientSize">The new client size.</param>

void XUI::Platform::UIRender::HandleResized(Core::Media::Size const & clientSize)
{
    XUI::UI::TopLevel::HandleResized(clientSize);

    Width = clientSize.Width;
    Height = clientSize.Height;
    _clientSize = clientSize;

    InvalidateMeasure();
}

/// <inheritdoc/>


/// <summary>
/// Handles a window scaling change notification from 
/// <see cref="ITopLevelImpl.ScalingChanged"/>.
/// </summary>
/// <param name="scaling">The window scaling.</param>

void XUI::Platform::UIRender::HandleScalingChanged(double scaling)
{
    XUI::UI::TopLevel::HandleScalingChanged(scaling);

    for (std::weak_ptr<Visual> control : VisualChildren.GetContainer())
    {
        auto visual = control.lock().get();
        if (visual)
            visual->InvalidateMeasure();
    }
}

void XUI::Platform::UIRender::OnAttachedToVisualTreeCore(XUI::UI::Controls::VisualTreeAttachmentEventArgs const & e)
{
    throw new System::InvalidOperationException("Control '{GetType().Name}' is a top level control and cannot be added as a child.");
}

void XUI::Platform::UIRender::OnApplicationExiting(void * sender, System::Events::EventArg & args)
{
    HandleApplicationExiting();
}

/// <summary>
/// Handles the application exiting, either from the last window closing, or a call to <see cref="IApplicationLifecycle.Exit"/>.
/// </summary>

void XUI::Platform::UIRender::HandleApplicationExiting()
{
    XUI::UI::TopLevel::HandleApplicationExiting();
}

/// <summary>
/// Global handler for pointer pressed events.
/// </summary>
/// <param name="sender">The event sender.</param>
/// <param name="e">The event args.</param>
void XUI::Platform::UIRender::OnPreviewPointerPressed(Interfaces::PointerPressedEventArgs & e)
{
    if (e.Source)
    {
        auto& ev = (Interfaces::PointerPressedEventArgs&)e;
        UI::Controls::InputElement* element = ev.Device ? ev.Device->Captured : nullptr;
        if (!element)
            element = e.Source;

        if (element && !CanFocus(element))
            element = nullptr;

        if (element == nullptr)
        {
            UI::Controls::Control* control = e.Source->Static_As<UI::Controls::Control>();
            while (control != nullptr)
            {
                if (control && CanFocus(control))
                    break;
                control = control->Parent;
            }

            if (control && CanFocus(control))
                element = control;
        }

        if (element != nullptr && element != GetFocusControl().lock().get())
        {
            Focus(element, Interfaces::NavigationMethod::Pointer, ev.InputModifiers);
            if (GetWindowImpl())
                GetWindowImpl()->Focus();
        }
    }
}

void XUI::Platform::UIRender::AddInstancedBinding(XamlCPP::DataBind::InstancedBinding * bind)
{
    _bindings.insert(bind->shared_from_this());
}

void XUI::Platform::UIRender::RemoveInstancedBinding(XamlCPP::DataBind::InstancedBinding * bind)
{
    _bindings.erase(bind->shared_from_this());
}

void XUI::Platform::UIRender::ProcessInput(Interfaces::RawInputEventArgs & e)
{
    if (_preProcess.has_observers())
        _preProcess.get_observer().on_next(e); //.get_subscriber().on_next(e);
    if (_process.has_observers())
        _process.get_observer().on_next(e); //.get_subscriber().on_next(e);
    if (_postProcess.has_observers())
        _postProcess.get_observer().on_next(e); //.get_subscriber().on_next(e);
}

XUI::Interfaces::IInputMgr::InputObservable & XUI::Platform::UIRender::GetPreProcess()
{
    return _preProcess;
}

XUI::Interfaces::IInputMgr::InputObservable & XUI::Platform::UIRender::GetProcess()
{
    return _process;
}

XUI::Interfaces::IInputMgr::InputObservable & XUI::Platform::UIRender::GetPostProcess()
{
    return _postProcess;
}

void XUI::Platform::UIRender::SetFocusedElement(XUI::UI::Controls::InputElement * control, Interfaces::NavigationMethod method, Interfaces::InputModifiers modifiers)
{
    if (control)
    {
        if (control != m_focusedElement.lock().get())
        {
            auto scope = control->VisualRoot;
            if (scope && scope->ToVisual())
            {
                auto inputRoot = scope->ToVisual()->shared_from_base_dynamic<Interfaces::UIControls::IInputRoot>();
                _keyBoardDevice->SetOwner(inputRoot);
            }
            m_focusedElement = control->shared_from_base_static<UI::Controls::Visual>();
        }
    }
    else
        m_focusedElement.reset();

    _keyBoardDevice->SetFocusedElement(control, method, modifiers);
}

void XUI::Platform::UIRender::Hover(XUI::UI::Controls::InputElement * control)
{
    if (control)
        m_hoveredElement = control->shared_from_base_static<UI::Controls::Visual>();
    else
        m_hoveredElement.reset();
}

/// <summary>
/// Focuses a control.
/// </summary>
/// <param name="control">The control to focus.</param>
/// <param name="method">The method by which focus was changed.</param>
/// <param name="modifiers">Any input modifiers active at the time of focus.</param>
void XUI::Platform::UIRender::Focus(UI::Controls::InputElement * control, Interfaces::NavigationMethod method, Interfaces::InputModifiers modifiers)
{
    if (control != nullptr)
    {
        //auto scope = GetFocusScopeAncestors(control);
        //
        //if (scope != nullptr)
        //{
        //    Scope = scope;
        //    SetFocusedElement(scope, control, method, modifiers);
        //}
        SetFocusedElement(control, method, modifiers);
    }
    else if (!FocusControl.expired())
    {
        SetFocusedElement(nullptr, method, modifiers);

        // If control is null, set focus to the topmost focus scope.
        //foreach(var scope in GetFocusScopeAncestors(Current).Reverse().ToList())
        //{
        //    IInputElement element;
        //
        //    if (_focusScopes.TryGetValue(scope, out element) && element != null)
        //    {
        //        Focus(element, method);
        //        return;
        //    }
        //}
        //
        //// Couldn't find a focus scope, clear focus.
        //SetFocusedElement(Scope, null);
    }
}

/// <summary>
/// Gets the focus scope ancestors of the specified control, traversing popups.
/// </summary>
/// <param name="control">The control.</param>
/// <returns>The focus scopes.</returns>
Interfaces::UIControls::IFocusScope * XUI::Platform::UIRender::GetFocusScopeAncestors(UI::Controls::InputElement * control)
{
    UI::Controls::Visual* visual = control;
    while (visual != nullptr)
    {
        auto scope = dynamic_cast<Interfaces::UIControls::IFocusScope*>(visual);

        if (scope != nullptr)
        {
            return scope;
        }

        visual = visual->VisualParent;
    }

    return nullptr;
}

/// <summary>
/// Checks if the specified element can be focused.
/// </summary>
/// <param name="e">The element.</param>
/// <returns>True if the element can be focused.</returns>
bool XUI::Platform::UIRender::CanFocus(XUI::UI::Controls::InputElement * e)
{
    return e->Focusable && e->IsEnabledCore && e->IsVisible;
}

void XUI::Platform::UIRender::ExecuteInitialLayoutPass(XUI::UI::Controls::Visual* control)
{
    Measure(control);
    Arrange(control);
    SceneUpdate(control);
}

void XUI::Platform::UIRender::Measure(XUI::UI::Controls::Visual* control)
{
    auto parent = control->VisualParent;
    if (parent)
        Measure(parent);

    if (!control->IsMeasureValid && (control->IsAttachedToVisualTree || control->Is<Interfaces::IRenderRoot>()))
    {
        if (control->Is<UI::TopLevel>() || control == ToVisualBase())
        {
            control->Measure(Core::Media::Size(std::numeric_limits<double>::max(), std::numeric_limits<double>::max()));
        }
        else
        {
            control->Measure(control->_previousMeasure);
        }
    }
}

void XUI::Platform::UIRender::Arrange(XUI::UI::Controls::Visual* control)
{
    auto parent = control->VisualParent;
    if (parent)
        Arrange(parent);

    if (!control->IsArrangeValid && (control->IsAttachedToVisualTree || control->Is<Interfaces::IRenderRoot>()))
    {
        if (control->Is<UI::TopLevel>() || control == ToVisualBase())
        {
            control->Arrange(Core::Media::Rect::xywh(0, 0, control->DesiredSize.Width, control->DesiredSize.Height));
        }
        else
        {
            control->Arrange(control->_previousArrange);
        }
    }
}

bool XUI::Platform::UIRender::SceneUpdate(XUI::UI::Controls::Visual* control, Core::Media::Matrix* contextTransform, Core::Media::Rect* availableSize)
{
    Core::Media::Matrix _contextTransform(Core::Media::Matrix::Identity());
    Core::Media::Rect   _availableSize;

    if (control->Is<UI::TopLevel>() || control->Is<Interfaces::IRenderRoot>())
        _availableSize = Core::Media::Rect::xywh(0, 0, control->DesiredSize.Width, control->DesiredSize.Height);

    auto parent = control->VisualParent;
    if (parent && !SceneUpdate(parent, &_contextTransform, &_availableSize))
        return false;

    if (!control->IsMeasureValid || !control->IsArrangeValid)
        return false;

    Core::SceneGraph::SceneNode* scene = control->RenderScene();
    if (!scene)
        return true; // not render object

    if (scene->InvalidatedVisual())
        scene->UpdateTransformation(_contextTransform, _availableSize);

    if (contextTransform)
    {
        *contextTransform = scene->AbsoluteTransformation;
        *availableSize = scene->ClipRect;
    }

    return true;
}

void XUI::Platform::UIRender::InstanceInitializer()
{
    XUI::UI::TopLevel::InstanceInitializer();
}
