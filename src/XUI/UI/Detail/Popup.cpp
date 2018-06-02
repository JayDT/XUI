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

#include "UI/Detail/Popup.h"
#include "UI/Detail/PopupRoot.h"
#include "UI/Window.h"
#include "Core/SceneGraph/SceneNode.h"
#include "Platform/UIService.h"
#include "Platform/UIRenderer.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::Interfaces;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(Popup, Popup::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty Popup::ChildProperty;
XUI::Core::Dependency::RefDependencyProperty Popup::IsOpenProperty;
XUI::Core::Dependency::RefDependencyProperty Popup::PlacementModeProperty;
XUI::Core::Dependency::RefDependencyProperty Popup::HorizontalOffsetProperty;
XUI::Core::Dependency::RefDependencyProperty Popup::VerticalOffsetProperty;
XUI::Core::Dependency::RefDependencyProperty Popup::PlacementTargetProperty;
XUI::Core::Dependency::RefDependencyProperty Popup::StaysOpenProperty;
XUI::Core::Dependency::RefDependencyProperty Popup::IsEmbedProperty;

void Popup::StaticClassInitializer()
{
    ChildProperty = (Core::Dependency::BindProperty<std::shared_ptr<Controls::Control>>::Register<Popup>(nameof(Child)));
    IsOpenProperty = (Core::Dependency::DirectProperty<Popup, bool>::RegisterDirect(nameof(IsOpen),
                      [](Popup* x) { return x->IsOpen; },
                      [](Popup* x, bool v) { x->IsOpen = v; })
                      );
    PlacementModeProperty = (Core::Dependency::BindProperty<UI::PlacementMode>::Register<Popup>(nameof(PlacementMode), UI::PlacementMode::Bottom));
    HorizontalOffsetProperty = (Core::Dependency::BindProperty<double>::Register<Popup>(nameof(HorizontalOffset)));
    VerticalOffsetProperty = (Core::Dependency::BindProperty<double>::Register<Popup>(nameof(VerticalOffset)));
    PlacementTargetProperty = (Core::Dependency::BindProperty<std::shared_ptr<Controls::Control>>::Register<Popup>(nameof(PlacementTarget)));
    StaysOpenProperty = (Core::Dependency::BindProperty<bool>::Register<Popup>(nameof(StaysOpen), true));
    IsEmbedProperty = (Core::Dependency::BindProperty<bool>::Register<Popup>(nameof(IsEmbed), true));

    IsHitTestVisibleProperty->OverrideDefaultValue<Popup>(false);

    ChildProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        Popup* control = e.Sender->Static_As<Popup>();
        if (control)
            control->ChildChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });
    IsOpenProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        Popup* control = e.Sender->Static_As<Popup>();
        if (control)
            control->IsOpenChanged((XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&)e);
    });
}

std::shared_ptr<Controls::Control> Popup::GetChild() const
{
    return GetValue<std::shared_ptr<Controls::Control>>(ChildProperty);
}

void Popup::SetChild(std::shared_ptr<Controls::Control> const& value)
{
    SetValue(ChildProperty, value);
}

bool Popup::GetIsOpen() const
{
    return _isOpen;
}

void Popup::SetIsOpen(const bool &value)
{
    SetAndRaise(IsOpenProperty, _isOpen, value);
}

UI::PlacementMode Popup::GetPlacementMode() const
{
    return GetValue<UI::PlacementMode>(PlacementModeProperty);
}

void Popup::SetPlacementMode(UI::PlacementMode value)
{
    SetValue(PlacementModeProperty, value);
}

double Popup::GetHorizontalOffset() const
{
    return GetValue<double>(HorizontalOffsetProperty);
}

void Popup::SetHorizontalOffset(const double &value)
{
    SetValue(HorizontalOffsetProperty, value);
}

double Popup::GetVerticalOffset() const
{
    return GetValue<double>(VerticalOffsetProperty);
}

void Popup::SetVerticalOffset(const double &value)
{
    SetValue(VerticalOffsetProperty, value);
}

std::shared_ptr<Controls::Control> Popup::GetPlacementTarget() const
{
    return GetValue<std::shared_ptr<Controls::Control>>(PlacementTargetProperty);
}

void Popup::SetPlacementTarget(std::shared_ptr<Controls::Control> const& value)
{
    SetValue(PlacementTargetProperty, value);
}

UI::PopupRoot *Popup::GetPopupRoot() const
{
    return _popupRoot.get();
}

bool Popup::GetStaysOpen() const
{
    return GetValue<bool>(StaysOpenProperty);
}

void Popup::SetStaysOpen(const bool value)
{
    SetValue(StaysOpenProperty, value);
}

void XUI::UI::Popup::SetInternalStaysOpen(const bool value)
{
    if (value)
        SetValue(StaysOpenProperty, true, Core::Dependency::BindingLevel::StyleTrigger);
    else
        SetValue(StaysOpenProperty, Core::Dependency::DependencyPropertyMgr::UnsetValue, Core::Dependency::BindingLevel::StyleTrigger);
}

bool XUI::UI::Popup::GetIsEmbed() const
{
    return GetValue<bool>(IsEmbedProperty);
}

void XUI::UI::Popup::SetIsEmbed(const bool value)
{
    SetValue(IsEmbedProperty, value);
}

Controls::Visual * Popup::GetRoot() const
{
    return GetPopupRoot();
}

void Popup::Open()
{
    if (!_popupRoot)
    {
        _popupRoot = std::make_shared<UI::PopupRoot>();

        _popupRoot->BeginInit();
        _popupRoot->SetParent(this);
        _popupRoot->SetEmbed(IsEmbed);

        AddDirectBind(ChildProperty.get(), [this](const System::Reflection::Variant& value)
        {
            if (_popupRoot)
                _popupRoot->Content = value.GetValue<System::Reflection::RefObject>();
        });
        AddDirectBind(WidthProperty.get(), [this](const System::Reflection::Variant& value)
        {
            if (_popupRoot)
                _popupRoot->Width = value.GetValue<double>();
        });
        AddDirectBind(HeightProperty.get(), [this](const System::Reflection::Variant& value)
        {
            if (_popupRoot)
                _popupRoot->Height = value.GetValue<double>();
        });
        AddDirectBind(MinWidthProperty.get(), [this](const System::Reflection::Variant& value)
        {
            if (_popupRoot)
                _popupRoot->MinWidth = value.GetValue<double>();
        });
        AddDirectBind(MaxWidthProperty.get(), [this](const System::Reflection::Variant& value)
        {
            if (_popupRoot)
                _popupRoot->MaxWidth = value.GetValue<double>();
        });
        AddDirectBind(MinHeightProperty.get(), [this](const System::Reflection::Variant& value)
        {
            if (_popupRoot)
                _popupRoot->MinHeight = value.GetValue<double>();
        });
        AddDirectBind(MaxHeightProperty.get(), [this](const System::Reflection::Variant& value)
        {
            if (_popupRoot)
                _popupRoot->MaxHeight = value.GetValue<double>();
        });
        AddDirectBind(IsEmbedProperty.get(), [this](const System::Reflection::Variant& value)
        {
            if (_popupRoot)
                _popupRoot->SetEmbed(value.GetValue<bool>());
        });
    }
    else
    {
        _popupRoot->BeginInit();
    }
    
    _popupRoot->Position = GetPosition();
    
    if (!_topLevel && PlacementTarget)
    {
        std::list<Visual*> ascentors;
        PlacementTarget->GetVisualAncestors(ascentors, [](Visual* v) -> bool
        {
            return v->Is<TopLevel>();
        });

        if (!ascentors.empty())
            _topLevel = (*ascentors.begin())->shared_from_base_static<TopLevel>();
        else
            _topLevel = nullptr;
    }
    
    if (_topLevel)
    {
        _topLevel->Deactivated += System::make_eventhandler(&Popup::WindowDeactivated, this);

        _nonPointerPressSubcribe = _topLevel->AddHandler(PointerPressedEvent.get(), System::make_eventhandler(&Popup::PointerPressedOutside, this), Core::Observer::RoutingStrategies::Tunnel);

        _nonClientListener = XUI::Platform::UIService::Instance()->GetCache<XUI::Interfaces::IInputMgr>()->Process.get_observable().subscribe([this](Interfaces::RawInputEventArgs & e)
        {
            ListenForNonClientClick(e);
        });
    }

    if (PopupRootCreated)
    {
        System::Events::EventArg e;
        PopupRootCreated(this, e);
    }
    
    _popupRoot->Show();

    _ignoreIsOpenChanged = true;
    IsOpen = true;
    _ignoreIsOpenChanged = false;
    
    if (Opened)
    {
        System::Events::EventArg e;
        Opened(this, e);
    }
}

void Popup::Close()
{
    if (_popupRoot != nullptr)
    {
        if (_topLevel != nullptr)
        {
            if (_nonPointerPressSubcribe.is_subscribed())
                _nonPointerPressSubcribe.unsubscribe();

            _topLevel->Deactivated -= System::make_eventhandler(&Popup::WindowDeactivated, this);

            if (_nonClientListener.is_subscribed())
                _nonClientListener.unsubscribe();
        }
    
        _popupRoot->Hide();
    }
    
    IsOpen = false;

    if (Closed)
    {
        System::Events::EventArg e;
        Closed(this, e);
    }
}

Core::Media::Size Popup::MeasureCore(Core::Media::Size const& availableSize)
{
    return Core::Media::Size::Empty;
}

void Popup::OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e)
{
    Control::OnAttachedToLogicalTree(e);
    if (!e.Root.expired())
        _topLevel = e.Root.lock()->shared_from_base_dynamic<TopLevel>();
}

void Popup::OnDetachedFromLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e)
{
    Control::OnDetachedFromLogicalTree(e);
    _topLevel = nullptr;

    if (_popupRoot != nullptr)
    {
        _popupRoot->SetParent(nullptr);
        _popupRoot = nullptr;
    }
}

void XUI::UI::Popup::SetTemplatedParent(std::shared_ptr<Core::Dependency::DependencyObject> const & value)
{
    Controls::Control::SetTemplatedParent(value);

    if (Child && !IsInitialized)
        Child->SetTemplatedParent(value);
}

void Popup::IsOpenChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (!_ignoreIsOpenChanged)
    {
        if (e.NewValue.GetValue<bool>())
        {
            Open();
        }
        else
        {
            Close();
        }
    }
}

void Popup::ChildChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
}

Core::Media::Point Popup::GetPosition()
{
    return GetPosition(PlacementTarget ? PlacementTarget.get() : GetVisualParent()->Static_As<Controls::Control>(), PlacementMode, PopupRoot, HorizontalOffset, VerticalOffset);
}

Core::Media::Point Popup::GetPosition(Controls::Control *target, UI::PlacementMode placement, XUI::UI::PopupRoot *popupRoot, double horizontalOffset, double verticalOffset)
{
    auto mode = placement;

    if (target == nullptr ? nullptr : target->GetVisualRoot() == nullptr)
    {
        mode = PlacementMode::Pointer;
    }

    switch (mode)
    {
        case PlacementMode::Pointer:
            if (popupRoot)
            {
                Core::Media::Point position;
                auto mouseDevice = XUI::Platform::UIService::Instance()->Get<Interfaces::IMouseDevice>();

                // Scales the Horizontal and Vertical offset to screen co-ordinates.
                auto screenOffset = Core::Media::Point(horizontalOffset * popupRoot->LayoutScaling, verticalOffset * popupRoot->LayoutScaling);
                if (target && (popupRoot && popupRoot->IsEmbed()))
                    position = mouseDevice->GetRelativePosition(target->VisualRoot->ToVisual()) + screenOffset;
                position = mouseDevice->Position + screenOffset;

                if (target->RenderScene() && (target->GetVisualRoot()->ToVisual() == Platform::UIService::Instance()->Get<Platform::UIRender>()->ToVisual() || (popupRoot && popupRoot->IsEmbed())))
                    position += (glm::vec2)target->RenderScene()->ClipRect.TopLeft;

                return position;
            }

            return Core::Media::Point::Zero();

        case PlacementMode::Bottom:
        {
            if (target)
            {
                if (target->GetVisualRoot())
                {
                    Core::Media::Point position;
                    if (target->RenderScene() && (popupRoot && popupRoot->IsEmbed()))
                    {
                        position.x = horizontalOffset;
                        position.y = target->Bounds.height() + verticalOffset;
                    }
                    else
                        position = target->GetVisualRoot()->PointToScreen(Core::Media::Point(0 + horizontalOffset, target->Bounds.height() + verticalOffset));

                    if (target->RenderScene() && (target->GetVisualRoot()->ToVisual() == Platform::UIService::Instance()->Get<Platform::UIRender>()->ToVisual() || (popupRoot && popupRoot->IsEmbed())))
                        position += (glm::vec2)target->RenderScene()->ClipRect.TopLeft;

                    if (popupRoot && popupRoot->IsEmbed())
                    {
                        UI::TopLevel* toplevel = target->GetParent()->GetVisualRoot()->ToVisual()->Static_As<UI::TopLevel>();
                        if (toplevel)
                        {
                            if (auto popupRoot = toplevel->Static_As<UI::PopupRoot>())
                                position += (glm::vec2)toplevel->GetPosition();
                        }
                    }

                    return position;
                }
            }
            return Core::Media::Point::Zero();
        }
        case PlacementMode::Right:
        {
            if (target)
            {
                if (target->GetVisualRoot())
                {
                    Core::Media::Point position;
                    if (target->RenderScene() && (popupRoot && popupRoot->IsEmbed()))
                    {
                        position.x = target->Bounds.width() + horizontalOffset;
                        position.y = verticalOffset;
                    }
                    else
                        position = target->GetVisualRoot()->PointToScreen(Core::Media::Point(target->Bounds.width() + horizontalOffset, 0 + verticalOffset));

                    if (target->RenderScene() && (target->GetVisualRoot()->ToVisual() == Platform::UIService::Instance()->Get<Platform::UIRender>()->ToVisual() || (popupRoot && popupRoot->IsEmbed())))
                        position += (glm::vec2)target->RenderScene()->ClipRect.TopLeft;

                    if (popupRoot && popupRoot->IsEmbed())
                    {
                        UI::TopLevel* toplevel = target->GetParent()->GetVisualRoot()->ToVisual()->Static_As<UI::TopLevel>();
                        if (toplevel)
                        {
                            if (auto popupRoot = toplevel->Static_As<UI::PopupRoot>())
                                position += (glm::vec2)toplevel->GetPosition();
                        }
                    }

                    return position;
                }
            }
            return Core::Media::Point::Zero();
        }
        default:
            throw System::InvalidOperationException("Invalid value for Popup.PlacementMode");
    }
}

void Popup::ListenForNonClientClick(Interfaces::RawInputEventArgs& e)
{
    auto mouse = dynamic_cast<RawMouseEventArgs*>(&e);

    if (!StaysOpen && mouse && mouse->Type == RawMouseEventType::NonClientLeftButtonDown)
    {
        Close();
    }
}

void Popup::PointerPressedOutside(void* sender, PointerPressedEventArgs& e)
{
    if (!StaysOpen)
    {
        auto root = e.Source->GetVisualRoot();

        if (root != this->PopupRoot)
        {
            Close();
            e.Handled = true;
        }
    }
}

void Popup::WindowDeactivated(void* sender, System::Events::EventArg& e)
{
    if (!StaysOpen)
    {
        Platform::UIRender* UIService = Platform::UIService::Instance()->Get<Platform::UIRender>();
        auto control = UIService->GetHoverControl();
        if (!control.expired() && control.lock()->VisualRoot && control.lock()->VisualRoot->ToVisual()->Is<UI::PopupRoot>())
            return;
        Close();
    }
}
