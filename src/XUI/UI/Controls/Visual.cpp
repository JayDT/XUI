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

#include "UI/Controls/Control.h"
#include "UI/Controls/Visual.h"
#include "XamlCPP/Core/LogInterface.h"

#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Core/Media/Transform.h"
#include "Core/Media/RelativePoint_t.h"
#include "Core/Media/Pen.h"
#include "Core/SceneGraph/SceneNode.h"
#include "Core/GlobalInitializerScheduler.h"

#include "Platform/UIService.h"
#include "Platform/UIRenderer.h"

#include <cpplinq/linq.hpp>

using namespace XUI;
using namespace XUI::UI::Controls;

STATIC_CLASS_PRE_INITIALIZER(Visual, Visual::StaticClassInitializer)

Core::Dependency::RefDependencyProperty    Visual::BoundsProperty;
Core::Dependency::RefDependencyProperty    Visual::ClipToBoundsProperty;
Core::Dependency::RefDependencyProperty    Visual::ClipProperty;
Core::Dependency::RefDependencyProperty    Visual::IsVisibleProperty;
Core::Dependency::RefDependencyProperty    Visual::OpacityProperty;
Core::Dependency::RefDependencyProperty    Visual::OpacityMaskProperty;
Core::Dependency::RefDependencyProperty    Visual::RenderTransformProperty;
Core::Dependency::RefDependencyProperty    Visual::RenderTransformOriginProperty;
Core::Dependency::RefDependencyProperty    Visual::VisualParentProperty;
Core::Dependency::RefDependencyProperty    Visual::ZIndexProperty;
Core::Dependency::RefDependencyProperty    Visual::DesiredSizeProperty;
Core::Dependency::RefDependencyProperty    Visual::WidthProperty;
Core::Dependency::RefDependencyProperty    Visual::HeightProperty;
Core::Dependency::RefDependencyProperty    Visual::MinWidthProperty;
Core::Dependency::RefDependencyProperty    Visual::MaxWidthProperty;
Core::Dependency::RefDependencyProperty    Visual::MinHeightProperty;
Core::Dependency::RefDependencyProperty    Visual::MaxHeightProperty;
Core::Dependency::RefDependencyProperty    Visual::MarginProperty;
Core::Dependency::RefDependencyProperty    Visual::HorizontalAlignmentProperty;
Core::Dependency::RefDependencyProperty    Visual::VerticalAlignmentProperty;
Core::Dependency::RefDependencyProperty    Visual::UseLayoutRoundingProperty;
Core::Dependency::RefDependencyProperty    Visual::TransformedBoundsProperty;

/// <summary>
/// Initializes static members of the <see cref="Decorator"/> class.
/// </summary>
void XUI::UI::Controls::Visual::StaticClassInitializer()
{
    BoundsProperty = (Core::Dependency::DirectProperty<Visual, Core::Media::Rect>::RegisterDirect(nameof(Bounds), [](XUI::UI::Controls::Visual* x) {return x->Bounds; }, nullptr));
    ClipToBoundsProperty = (Core::Dependency::BindProperty<bool>::Register<Visual>(nameof(ClipToBounds), true));
    ClipProperty = (Core::Dependency::BindProperty<Core::Media::Geometry*>::Register<Visual>(nameof(Clip), nullptr));
    IsVisibleProperty = (Core::Dependency::BindProperty<bool>::Register<Visual>(nameof(IsVisible), true));
    OpacityProperty = (Core::Dependency::BindProperty<double>::Register<Visual>(nameof(Opacity), 1.0));
    OpacityMaskProperty = (Core::Dependency::BindProperty<std::shared_ptr<XUI::Interfaces::IBrush> >::Register<Visual>(nameof(OpacityMask)));
    RenderTransformProperty = (Core::Dependency::BindProperty<std::shared_ptr<Core::Media::Transform>>::Register<Visual>(nameof(RenderTransform)));
    RenderTransformOriginProperty = (Core::Dependency::BindProperty<Core::Media::RelativePoint>::Register<Visual>(nameof(RenderTransformOrigin), Core::Media::RelativePoint::Center));
    VisualParentProperty = (Core::Dependency::DirectProperty<Visual, Visual*>::RegisterDirect(nameof(VisualParent), [](XUI::UI::Controls::Visual* x) {return x->VisualParent; }, nullptr));
    ZIndexProperty = (Core::Dependency::BindProperty<int>::Register<Visual>(nameof(ZIndex)));
    DesiredSizeProperty = (Core::Dependency::DirectProperty<Visual, Core::Media::Size>::RegisterDirect(nameof(DesiredSize), [](XUI::UI::Controls::Visual* x) {return x->DesiredSize; }, nullptr, Core::Media::Size(0, 0)));
    WidthProperty = (Core::Dependency::BindProperty<double>::Register<Visual>(nameof(Width), -1.0));
    HeightProperty = (Core::Dependency::BindProperty<double>::Register<Visual>(nameof(Height), -1.0));
    MinWidthProperty = (Core::Dependency::BindProperty<double>::Register<Visual>(nameof(MinWidth)));
    MaxWidthProperty = (Core::Dependency::BindProperty<double>::Register<Visual>(nameof(MaxWidth), std::numeric_limits<double>::max()));
    MinHeightProperty = (Core::Dependency::BindProperty<double>::Register<Visual>(nameof(MinHeight)));
    MaxHeightProperty = (Core::Dependency::BindProperty<double>::Register<Visual>(nameof(MaxHeight), std::numeric_limits<double>::max()));
    MarginProperty = (Core::Dependency::BindProperty<Core::Media::Thickness>::Register<Visual>(nameof(Margin), Core::Media::Thickness(0)));
    HorizontalAlignmentProperty = (Core::Dependency::BindProperty<UI::Controls::HorizontalAlignment>::Register<Visual>(nameof(HorizontalAlignment), HorizontalAlignment::Stretch));
    VerticalAlignmentProperty = (Core::Dependency::BindProperty<UI::Controls::VerticalAlignment>::Register<Visual>(nameof(VerticalAlignment), VerticalAlignment::Stretch));
    UseLayoutRoundingProperty = (Core::Dependency::BindProperty<bool>::Register<Visual>(nameof(UseLayoutRounding), true, true));
    TransformedBoundsProperty = (Core::Dependency::DirectProperty<Visual, Core::Media::Rect>::RegisterDirect(nameof(TransformedBounds), [](XUI::UI::Controls::Visual* x) {return x->TransformedBounds; }, nullptr));

    WidthProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
    HeightProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
    MinWidthProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
    MaxWidthProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
    MinHeightProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
    MaxHeightProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
    MarginProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
    HorizontalAlignmentProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
    VerticalAlignmentProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);
    IsVisibleProperty->Changed.get_observable().subscribe(&Visual::AffectsMeasureInvalidate);

    IsVisibleProperty->Changed.get_observable().subscribe(&Visual::AffectsVisualInvalidate);
    BoundsProperty->Changed.get_observable().subscribe(&Visual::AffectsVisualInvalidate);
    ClipProperty->Changed.get_observable().subscribe(&Visual::AffectsVisualInvalidate);
    ClipToBoundsProperty->Changed.get_observable().subscribe(&Visual::AffectsVisualInvalidate);
    OpacityProperty->Changed.get_observable().subscribe(&Visual::AffectsVisualInvalidate);

    RenderTransformProperty->Changed.get_observable().subscribe(&Visual::RenderTransformChanged);
}

Core::Media::Size ApplyLayoutConstraints(XUI::UI::Controls::Visual* control, Core::Media::Size constraints)
{
    double width = (control->Width > 0) ? control->Width : constraints.Width;
    double height = (control->Height > 0) ? control->Height : constraints.Height;
    width = std::min(width, control->MaxWidth);
    width = std::max(width, control->MinWidth);
    height = std::min(height, control->MaxHeight);
    height = std::max(height, control->MinHeight);
    return Core::Media::Size(width, height);
}

XUI::UI::Controls::Visual::Visual()
    : _measuring(false)
{
    m_renderNode = std::make_shared<Core::SceneGraph::SceneNode>(this);

    m_visualChildrens.CollectionChanged += System::make_eventhandler(&Control::VisualChildrenCollectionChanged, this);
}

XUI::UI::Controls::Visual::~Visual()
{
    if (m_renderNode)
        m_renderNode->Detached();
    m_renderNode = nullptr;
}

void XUI::UI::Controls::Visual::AddVisualChild(Visual * child)
{
    if (child->VisualParent == this)
        return;

    System::ThrowIfFailed<System::ArgumentException>(child->VisualParent == this || child->VisualParent == nullptr);

    m_visualChildrens.Add(child->shared_from_base_static<Visual>());
    //child->SetVisualParent(this);
}

void XUI::UI::Controls::Visual::RemoveVisualChild(Visual * child)
{
    System::ThrowIfFailed<System::ArgumentException>(child->VisualParent == this || child->VisualParent == nullptr);

    //child->SetVisualParent(nullptr);
    m_visualChildrens.Remove(child->shared_from_base_static<Visual>());
    //for (auto iChild = m_visualChildrens.GetContainer().begin(); iChild != m_visualChildrens.GetContainer().end(); ++iChild)
    //    if (iChild->get() == child)
    //    {
    //        m_visualChildrens.Remove(*iChild);
    //        break;
    //    }
}

void XUI::UI::Controls::Visual::VisualChildrenCollectionChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e)
{
    VisualChildList& list = *(VisualChildList*)sender;
    switch (e.Action)
    {
        case System::Collection::NotifyCollectionChangedAction::Add:
        {
            auto children = static_cast<Visual*>(e.NewValue.ToObject()); //list[e.NewStartingIndex].get();
            if (children && children->GetVisualParent() == nullptr)
                children->SetVisualParent(this);
            break;
        }
        case System::Collection::NotifyCollectionChangedAction::Remove:
        {
            auto children = list[e.OldStartingIndex].get();
            if (children && children->GetVisualParent() == this)
                children->SetVisualParent(nullptr);
            break;
        }
        case System::Collection::NotifyCollectionChangedAction::Replace:
        case System::Collection::NotifyCollectionChangedAction::Reset:
            throw System::NotSupportedException("Reset should not be signalled on LogicalChildren collection");
    }
}

void XUI::UI::Controls::Visual::SetVisualParent(Visual* parent)
{
    if (m_visualParent.lock().get() == parent)
        return;

    auto old = m_visualParent.lock();
    if (parent)
        m_visualParent = parent->shared_from_base_dynamic<Visual>();
    else
        m_visualParent.reset();

    if (old != nullptr && !_visualRoot.expired())
    {
        VisualTreeAttachmentEventArgs e(old.get(), VisualRoot);
        OnDetachedFromVisualTreeCore(e);
    }

    if (parent && parent->IsAttachedToVisualTree)
    {
        Interfaces::IRenderRoot* renderRootParent = Dynamic_As<Interfaces::IRenderRoot>();
        Visual* root = renderRootParent && renderRootParent->IsRootMode() ? this : FindRenderRoot(parent);

        if (root)
        {
            VisualTreeAttachmentEventArgs e(parent, dynamic_cast<Interfaces::IRenderRoot*>(root));
            OnAttachedToVisualTreeCore(e);
        }
    }

    OnVisualParentChanged(old.get(), parent);
}

void XUI::UI::Controls::Visual::OnVisualParentChanged(Visual* oldParent, Visual* newParent)
{
    RaisePropertyChanged(VisualParentProperty.get(), oldParent, newParent); //, BindingLevel::LocalValue
}

Visual * XUI::UI::Controls::Visual::FindRenderRoot(Visual * visual)
{
    Interfaces::IRenderRoot* renderRootParent = visual ? visual->Dynamic_As<Interfaces::IRenderRoot>() : nullptr;

    if (!renderRootParent || !renderRootParent->IsRootMode())
    {
        Interfaces::IRenderRoot* root = visual->VisualRoot;
        if (root && root->ToVisual())
            return root->ToVisual();
    }
    else
    {
        return renderRootParent->ToVisual();
    }

    if (visual && ((renderRootParent && !renderRootParent->IsRootMode()) || visual->IsAttachedToVisualTree))
    {
        std::list<Visual*> ancestors;

        Visual* root = renderRootParent && renderRootParent->IsRootMode() ? renderRootParent->ToVisual() : nullptr;
        if (!root)
        {
            visual->GetVisualAncestors(ancestors, [](Visual* p) -> bool
            {
                return p && p->Is<Interfaces::IRenderRoot>() && dynamic_cast<Interfaces::IRenderRoot*>(p)->IsRootMode();
            });
            root = ancestors.empty() ? nullptr : *ancestors.begin();
        }

        if (root)
            return root;
    }

    if (visual->Is<Control>())
    {
        std::list<Control*> ancestors;
        {
            static_cast<Control*>(visual)->GetLogicalAncestors(ancestors, [](Control* p) -> bool
            {
                return p && p->Is<Interfaces::IRenderRoot>() && dynamic_cast<Interfaces::IRenderRoot*>(p)->IsRootMode();
            });

            return ancestors.empty() ? nullptr : *ancestors.begin();
        }
    }

    return nullptr;
}

void XUI::UI::Controls::Visual::SetBounds(Core::Media::Rect const& value)
{
    SetAndRaise(BoundsProperty.get(), _bounds, value);
}

bool XUI::UI::Controls::Visual::GetClipToBounds() const
{
    return GetValue<bool>(ClipToBoundsProperty.get());
}

void XUI::UI::Controls::Visual::SetClipToBounds(bool value)
{
    SetValue(ClipToBoundsProperty.get(), value);
}

Core::Media::Geometry * XUI::UI::Controls::Visual::GetClip() const
{
    return GetValue<Core::Media::Geometry *>(ClipProperty.get());
}

void XUI::UI::Controls::Visual::SetClip(Core::Media::Geometry * value)
{
    SetValue(ClipProperty.get(), value);
}

bool XUI::UI::Controls::Visual::GetIsAttachedToVisualTree() const
{
    return VisualRoot != nullptr ? true : false;
}

bool XUI::UI::Controls::Visual::GetIsEffectivelyVisible() const
{
    return cpplinq::from(VisualChildren.GetContainer()).all([](std::shared_ptr<Visual> const& control) -> bool
    {
        auto visual = control.get();
        if (visual)
            return visual->IsVisible;
        return true;
    });
}

bool XUI::UI::Controls::Visual::GetIsVisible() const
{
    return GetValue<bool>(IsVisibleProperty.get());
}

void XUI::UI::Controls::Visual::SetIsVisible(bool value)
{
    SetValue(IsVisibleProperty.get(), value);
}

double XUI::UI::Controls::Visual::GetOpacity() const
{
    return GetValue<double>(OpacityProperty.get());
}

void XUI::UI::Controls::Visual::SetOpacity(double value)
{
    SetValue(OpacityProperty.get(), value);
}

std::shared_ptr<XUI::Interfaces::IBrush>  XUI::UI::Controls::Visual::GetOpacityMask() const
{
    return GetValue<std::shared_ptr<XUI::Interfaces::IBrush> >(OpacityMaskProperty.get());
}

void XUI::UI::Controls::Visual::SetOpacityMask(std::shared_ptr<XUI::Interfaces::IBrush>  value)
{
    SetValue(OpacityMaskProperty.get(), value);
}

std::shared_ptr<Core::Media::Transform> XUI::UI::Controls::Visual::GetRenderTransform() const
{
    return GetValue<std::shared_ptr<Core::Media::Transform>>(RenderTransformProperty);
}

void XUI::UI::Controls::Visual::SetRenderTransform(std::shared_ptr<Core::Media::Transform> const& value)
{
    SetValue(RenderTransformProperty, value);
}

Core::Media::RelativePoint XUI::UI::Controls::Visual::GetRenderTransformOrigin() const
{
    return GetValue<Core::Media::RelativePoint>(RenderTransformOriginProperty.get());
}

void XUI::UI::Controls::Visual::SetRenderTransformOrigin(Core::Media::RelativePoint value)
{
    SetValue(RenderTransformOriginProperty.get(), value);
}

Visual * XUI::UI::Controls::Visual::GetVisualParent() const
{
    return m_visualParent.lock().get();
}

Interfaces::IRenderRoot * XUI::UI::Controls::Visual::GetVisualRoot() const
{
    return _visualRoot.lock().get();
}

int XUI::UI::Controls::Visual::GetZIndex() const
{
    return GetValue<int>(ZIndexProperty.get());
}

void XUI::UI::Controls::Visual::SetZIndex(int value)
{
    SetValue(ZIndexProperty.get(), value);
}

double XUI::UI::Controls::Visual::GetWidth() const
{
    return GetValue<double>(WidthProperty.get());
}

void XUI::UI::Controls::Visual::SetWidth(double value)
{
    SetValue(WidthProperty.get(), value);
}

double XUI::UI::Controls::Visual::GetHeight() const
{
    return GetValue<double>(HeightProperty.get());
}

void XUI::UI::Controls::Visual::SetHeight(double value)
{
    SetValue(HeightProperty.get(), value);
}

double XUI::UI::Controls::Visual::GetMinWidth() const
{
    return GetValue<double>(MinWidthProperty.get());
}

void XUI::UI::Controls::Visual::SetMinWidth(double value)
{
    SetValue(MinWidthProperty.get(), value);
}

double XUI::UI::Controls::Visual::GetMaxWidth() const
{
    return GetValue<double>(MaxWidthProperty.get());
}

void XUI::UI::Controls::Visual::SetMaxWidth(double value)
{
    SetValue(MaxWidthProperty.get(), value);
}

double XUI::UI::Controls::Visual::GetMinHeight() const
{
    return GetValue<double>(MinHeightProperty.get());
}

void XUI::UI::Controls::Visual::SetMinHeight(double value)
{
    SetValue(MinHeightProperty.get(), value);
}

double XUI::UI::Controls::Visual::GetMaxHeight() const
{
    return GetValue<double>(MaxHeightProperty.get());
}

void XUI::UI::Controls::Visual::SetMaxHeight(double value)
{
    SetValue(MaxHeightProperty.get(), value);
}

Core::Media::Thickness XUI::UI::Controls::Visual::GetMargin() const
{
    return GetValue<Core::Media::Thickness>(MarginProperty.get());
}

void XUI::UI::Controls::Visual::SetMargin(Core::Media::Thickness const & value)
{
    SetValue(MarginProperty.get(), value);
}

UI::Controls::HorizontalAlignment XUI::UI::Controls::Visual::GetHorizontalAlignment() const
{
    return GetValue<UI::Controls::HorizontalAlignment>(HorizontalAlignmentProperty.get());
}

void XUI::UI::Controls::Visual::SetHorizontalAlignment(UI::Controls::HorizontalAlignment value)
{
    SetValue(HorizontalAlignmentProperty.get(), value);
}

UI::Controls::VerticalAlignment XUI::UI::Controls::Visual::GetVerticalAlignment() const
{
    return GetValue<UI::Controls::VerticalAlignment>(VerticalAlignmentProperty.get());
}

void XUI::UI::Controls::Visual::SetVerticalAlignment(UI::Controls::VerticalAlignment value)
{
    SetValue(VerticalAlignmentProperty.get(), value);
}

bool XUI::UI::Controls::Visual::GetUseLayoutRounding() const
{
    return GetValue<bool>(UseLayoutRoundingProperty.get());
}

void XUI::UI::Controls::Visual::SetUseLayoutRounding(bool value)
{
    SetValue(UseLayoutRoundingProperty.get(), value);
}

void XUI::UI::Controls::Visual::InvalidateVisual()
{
    if (VisualRoot)
        VisualRoot->AddDirty(this);
}

/// <summary>
/// Invalidates the measurement of the control and queues a new layout pass.
/// </summary>
void XUI::UI::Controls::Visual::InvalidateMeasure()
{
    if (IsMeasureValid)
    {
        XamlCPP::Platform::ILog::Verbose(XamlCPP::Platform::LogArea::Layout, nullptr, "Invalidated measure");

        IsMeasureValid = false;
        IsArrangeValid = false;
        //LayoutManager.Instance ? .InvalidateMeasure(this);
        InvalidateVisual();
        if (m_renderNode)
            m_renderNode->InvalidateRender();
    }
}

void XUI::UI::Controls::Visual::InvalidateArrange()
{
    if (IsArrangeValid)
    {
        XamlCPP::Platform::ILog::Verbose(XamlCPP::Platform::LogArea::Layout, nullptr, "Invalidated arrange");

        IsArrangeValid = false;
        //LayoutManager.Instance ? .InvalidateMeasure(this);
        InvalidateVisual();
        if (m_renderNode)
            m_renderNode->InvalidateRender();
    }
}

void XUI::UI::Controls::Visual::Render(Interfaces::IDrawingContextImpl * context)
{
    if (GetDebugFlag() & 1)
    {
        static auto color = std::make_shared<XUI::Core::Media::SolidColorBrush>(XUI::Core::Media::Color(255, 255, 255, 255));
        Core::Media::Rect rect = Core::Media::Rect::xywh(0, 0, Bounds.width(), Bounds.height());
        Core::Media::Pen pen(color.get(), 1.0);
        context->DrawRectangle(pen, rect, 0.0);
    }
}

Core::Media::Matrix XUI::UI::Controls::Visual::TransformToVisual(Visual * visual)
{
    auto common = FindCommonVisualAncestor(visual);

    if (common != nullptr)
    {
        auto thisOffset = common->GetOffsetFrom(this);
        auto thatOffset = common->GetOffsetFrom(visual);

        return Core::Media::Matrix::CreateTranslation(-thatOffset.x, -thatOffset.y, 0.0f) * Core::Media::Matrix::CreateTranslation(thisOffset.x, thisOffset.y, 0.0f);
    }

    return Core::Media::Matrix::Identity();
}

Visual * XUI::UI::Controls::Visual::FindCommonVisualAncestor(Visual * target)
{
    Visual* visual = this;
    while (visual)
    {
        if (visual == target)
            return target;

        Visual* target_visual = target;
        while (target_visual)
        {
            if (target_visual == this || target_visual == visual)
                return target_visual;

            target_visual = target_visual->VisualParent;
        }

        visual = visual->VisualParent;
    }

    //for (auto const& control : VisualChildren)
    //{
    //    Visual* visual = control.lock().get();
    //    if (visual == target)
    //        return target;
    //
    //    for (auto const& target_control : target->VisualChildren)
    //    {
    //        Visual* target_visual = target_control.lock().get();
    //        if (target_visual == this)
    //            return this;
    //
    //        if (visual == target_visual)
    //            return visual;
    //    }
    //}
    //
    //for (auto const& control : VisualChildren)
    //{
    //    Visual* visual = control.lock().get();
    //    if (visual)
    //    {
    //        visual = visual->FindCommonVisualAncestor(target);
    //        if (visual)
    //            return visual;
    //    }
    //}
    //
    //for (auto const& control : target->VisualChildren)
    //{
    //    Visual* visual = control.lock().get();
    //    if (visual)
    //    {
    //        visual = visual->FindCommonVisualAncestor(this);
    //        if (visual)
    //            return visual;
    //    }
    //}

    return nullptr;
}

/// <summary>
/// Gets the visual offset from the specified ancestor.
/// </summary>
/// <param name="ancestor">The ancestor visual.</param>
/// <param name="visual">The visual.</param>
/// <returns>The visual offset.</returns>

Core::Media::Vector XUI::UI::Controls::Visual::GetOffsetFrom(Visual * visual)
{
    Core::Media::Vector result = Core::Media::Vector(0, 0);

    while (visual != this)
    {
        result = Core::Media::Vector(result.x + visual->Bounds.x0(), result.y + visual->Bounds.y0());
        visual = visual->VisualParent;

        if (visual == nullptr)
        {
            throw System::ArgumentException("'visual' is not a descendent of 'ancestor'.");
        }
    }

    return result;
}

/// <summary>
/// Calls the <see cref="OnAttachedToVisualTree(VisualTreeAttachmentEventArgs)"/> method 
/// for this control and all of its visual descendents.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::Visual::OnAttachedToVisualTreeCore(VisualTreeAttachmentEventArgs const& e)
{
    XamlCPP::Platform::ILog::Verbose(XamlCPP::Platform::LogArea::Visual, nullptr, "Attached to visual tree");
    
    _visualRoot = dynamic_cast<XUI::UI::Controls::Visual*>(e.Root)->shared_from_base_dynamic<Interfaces::IRenderRoot>();
    
    if (RenderTransform != nullptr)
    {
        RenderTransform->PropertyChanged += System::make_eventhandler(&Visual::RenderTransformCoreChanged, std::weak_ptr<Visual>(shared_from_base_static<Visual>()));
    }
    
    OnAttachedToVisualTree(e);
    InvalidateVisual();
    
    if (!VisualChildren.empty())
    {
        for(std::shared_ptr<Visual> const& child : VisualChildren.GetContainer())
        {
            auto visual = child.get();
            if (visual)
                visual->OnAttachedToVisualTreeCore(e);
        }
    }
}

/// <summary>
/// Calls the <see cref="OnDetachedFromVisualTree(VisualTreeAttachmentEventArgs)"/> method 
/// for this control and all of its visual descendents.
/// </summary>
/// <param name="e">The event args.</param>
void XUI::UI::Controls::Visual::OnDetachedFromVisualTreeCore(VisualTreeAttachmentEventArgs const& e)
{
    XamlCPP::Platform::ILog::Verbose(XamlCPP::Platform::LogArea::Visual, nullptr, "Detached from visual tree");
    
    _visualRoot.reset();
    
    if (RenderTransform != nullptr)
    {
        RenderTransform->PropertyChanged -= System::make_eventhandler(&Visual::RenderTransformCoreChanged, std::weak_ptr<Visual>(shared_from_base_static<Visual>()));
    }
    
    OnDetachedFromVisualTree(e);

    if (e.Root)
        e.Root->AddDirty(this);

    if (!VisualChildren.empty())
    {
        for (std::shared_ptr<Visual> const& child : VisualChildren.GetContainer())
        {
            auto visual = child.get();
            if (visual)
                visual->OnDetachedFromVisualTreeCore(e);
        }
    }
}

void Visual::OnAttachedToVisualTree(VisualTreeAttachmentEventArgs const& e)
{
    if (AttachedToVisualTree)
        AttachedToVisualTree(this, const_cast<VisualTreeAttachmentEventArgs&>(e));
}

void Visual::OnDetachedFromVisualTree(VisualTreeAttachmentEventArgs const& e)
{
    if (DetachedFromVisualTree)
        DetachedFromVisualTree(this, const_cast<VisualTreeAttachmentEventArgs&>(e));
}

void Visual::ClearVisualChild()
{
    while (!m_visualChildrens.empty())
    {
        auto child = m_visualChildrens.GetContainer().begin();
        //if (!child->expired())
        child->get()->SetVisualParent(nullptr); //m_visualParent.reset();
        m_visualChildrens.GetContainer().erase(m_visualChildrens.GetContainer().begin());
    }
}

Core::Media::Size Visual::MeasureOverride(Core::Media::Size const& availableSize)
{
    double width = 0;
    double height = 0;

    for (auto visual : m_visualChildrens.GetNativeEnumerator())
    {
        visual->Measure(availableSize);
        width = std::max(width, visual->DesiredSize.Width);
        height = std::max(height, visual->DesiredSize.Height);
    }

    return Core::Media::Size(width, height);
}

void Visual::ArrangeCore(Core::Media::Rect const& finalRect)
{
    if (IsVisible)
    {
        auto margin = Margin;
        auto originX = finalRect.x0() + margin.Left;
        auto originY = finalRect.y0() + margin.Top;
        auto availableSizeMinusMargins = Core::Media::Size(
            std::max(0.0, finalRect.width() - margin.Left - margin.Right),
            std::max(0.0, finalRect.height() - margin.Top - margin.Bottom));
        auto horizontalAlignment = HorizontalAlignment;
        auto verticalAlignment = VerticalAlignment;
        auto size = availableSizeMinusMargins;
        auto scale = 1.0f;

        if (horizontalAlignment != HorizontalAlignment::Stretch)
        {
            size = size.WithWidth(std::min(size.Width, DesiredSize.Width - margin.Left - margin.Right));
        }

        if (verticalAlignment != VerticalAlignment::Stretch)
        {
            size = size.WithHeight(std::min(size.Height, DesiredSize.Height - margin.Top - margin.Bottom));
        }

        size = ApplyLayoutConstraints(this, size);

        if (UseLayoutRounding)
        {
            scale = GetLayoutScale();
            if (VisualRoot)
            {
                Visual* Root = VisualRoot->ToVisual();
                if (Root)
                    scale = Root->GetLayoutScale();
            }

            size = Core::Media::Size(
                ceil(size.Width * scale) / scale,
                ceil(size.Height * scale) / scale);
            availableSizeMinusMargins = Core::Media::Size(
                ceil(availableSizeMinusMargins.Width * scale) / scale,
                ceil(availableSizeMinusMargins.Height * scale) / scale);
        }

        size = ArrangeOverride(size).Constrain(size);

        switch (horizontalAlignment)
        {
            case HorizontalAlignment::Center:
            case HorizontalAlignment::Stretch:
                originX += (availableSizeMinusMargins.Width - size.Width) / 2.0;
                break;
            case HorizontalAlignment::Right:
                originX += availableSizeMinusMargins.Width - size.Width;
                break;
        }

        switch (verticalAlignment)
        {
            case VerticalAlignment::Center:
            case VerticalAlignment::Stretch:
                originY += (availableSizeMinusMargins.Height - size.Height) / 2.0;
                break;
            case VerticalAlignment::Bottom:
                originY += availableSizeMinusMargins.Height - size.Height;
                break;
        }

        if (UseLayoutRounding)
        {
            originX = floor(originX * scale) / scale;
            originY = floor(originY * scale) / scale;
        }

        Bounds = Core::Media::Rect::xywh(originX, originY, size.Width, size.Height);
    }
}

Core::Media::Size Visual::ArrangeOverride(Core::Media::Size const& finalSize)
{
    auto finalRect = Core::Media::Rect::xywh(0, 0, finalSize.Width, finalSize.Height);
    for (auto const& child : GetVisualChildren().GetNativeEnumerator())
    {
        child->Arrange(finalRect);
    }

    return finalSize;
}

/// <summary>
/// Calls <see cref="InvalidateMeasure"/> on the control on which a property changed.
/// </summary>
/// <param name="e">The event args.</param>
void Visual::AffectsMeasureInvalidate(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
{
    UI::Controls::Visual* control = e.Sender->Static_As<UI::Controls::Visual>();
    if (control)
        control->InvalidateMeasure();
}

void Visual::AffectsArrangeInvalidate(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
{
    UI::Controls::Visual* control = e.Sender->Static_As<UI::Controls::Visual>();
    if (control)
        control->InvalidateArrange();
}

void Visual::AffectsVisualInvalidate(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
{
    UI::Controls::Visual* control = e.Sender->Static_As<UI::Controls::Visual>();
    if (control)
        control->InvalidateVisual();
}

/// <summary>
/// Called when a visual's <see cref="RenderTransform"/> changes.
/// </summary>
/// <param name="e">The event args.</param>
void Visual::RenderTransformChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
{
    auto sender = static_cast<Visual*>(e.Sender);

    if (sender->VisualRoot != nullptr)
    {
        auto oldValue = e.OldValue.TryGetValue<std::shared_ptr<Core::Media::Transform>>();
        auto newValue = e.NewValue.TryGetValue<std::shared_ptr<Core::Media::Transform>>();

        if (oldValue != nullptr)
        {
            oldValue->PropertyChanged -= System::make_eventhandler(&Visual::RenderTransformCoreChanged, sender->weak_from_base_static<Visual>());
        }

        if (newValue != nullptr)
        {
            auto event = System::make_eventhandler(&Visual::RenderTransformCoreChanged, sender->weak_from_base_static<Visual>());
            newValue->PropertyChanged += event;
        }

        sender->InvalidateVisual();
    }
}

/// <summary>
/// Carries out a measure of the control.
/// </summary>
/// <param name="availableSize">The available size for the control.</param>
void Visual::Measure(Core::Media::Size const & availableSize)
{
    if (std::isnan(availableSize.Width) || std::isnan(availableSize.Height))
    {
        throw System::InvalidOperationException("Cannot call Measure using a size with NaN values.");
    }

    if (!IsMeasureValid || _previousMeasure != availableSize)
    {
        Core::Media::Size previousDesiredSize = DesiredSize;
        Core::Media::Size desiredSize;

        IsMeasureValid = true;

        try
        {
            _measuring = true;
            desiredSize = MeasureCore(availableSize).Constrain(availableSize);
        }
        catch(...)
        {
            XamlCPP::Platform::ILog::Verbose(XamlCPP::Platform::LogArea::Layout, nullptr, "Measure encountered error", DesiredSize);
        }

        _measuring = false;

        if (IsInvalidSize(desiredSize))
        {
            throw System::InvalidOperationException("Invalid size returned for Measure.");
        }

        _desiredSize = desiredSize;
        _previousMeasure = availableSize;

        XamlCPP::Platform::ILog::Verbose(XamlCPP::Platform::LogArea::Layout, nullptr, "Measure requested {DesiredSize}", DesiredSize);

        if (DesiredSize != previousDesiredSize && VisualParent)
        {
            VisualParent->ChildDesiredSizeChanged(this);
        }
    }
}

void Visual::ChildDesiredSizeChanged(Visual* control)
{
    if (!_measuring)
        InvalidateMeasure();
}

/// <summary>
/// Arranges the control and its children.
/// </summary>
/// <param name="rect">The control's new bounds.</param>
void Visual::Arrange(Core::Media::Rect const & rect)
{
    if (IsInvalidRect(rect))
    {
        throw System::InvalidOperationException("Invalid Arrange rectangle.");
    }

    if (!IsMeasureValid)
    {
        //auto parent = VisualParent;
        Measure(/*parent ? parent->_previousMeasure :*/ _previousMeasure);
    }

    if (!IsArrangeValid || _previousArrange != rect)
    {
        XamlCPP::Platform::ILog::Verbose(XamlCPP::Platform::LogArea::Layout, nullptr, "Arrange to {Rect} ", rect);

        IsArrangeValid = true;
        ArrangeCore(rect);
        _previousArrange = rect;

        if (LayoutUpdated)
            LayoutUpdated(this, System::Events::EventArg());
    }
}

/// <summary>
/// Tests whether any of a <see cref="Rect"/>'s properties incude nagative values,
/// a NaN or Infinity.
/// </summary>
/// <param name="rect">The rect.</param>
/// <returns>True if the rect is invalid; otherwise false.</returns>
bool Visual::IsInvalidRect(Core::Media::Rect const & rect)
{
    return rect.width() < 0 || rect.height() < 0 ||
        !std::isfinite(rect.x0()) || !std::isfinite(rect.y0()) ||
        !std::isfinite(rect.width()) || !std::isfinite(rect.height());
}

bool Visual::IsInvalidSize(Core::Media::Size const& size)
{
    return size.Width < 0 || size.Height < 0 ||
        !std::isfinite(size.Width) || !std::isfinite(size.Height);
}

Core::Media::Size Visual::ApplyLayoutConstraints(Visual* control, Core::Media::Size const& constraints)
{
    double width = (control->Width != -1) ? control->Width : constraints.Width;
    double height = (control->Height != -1) ? control->Height : constraints.Height;
    width = std::min(width, control->MaxWidth);
    width = std::max(width, control->MinWidth);
    height = std::min(height, control->MaxHeight);
    height = std::max(height, control->MinHeight);
    return Core::Media::Size(width, height);
}

/// <summary>
/// The default implementation of the control's measure pass.
/// </summary>
/// <param name="availableSize">The size available to the control.</param>
/// <returns>The desired size for the control.</returns>
/// <remarks>
/// This method calls <see cref="MeasureOverride(Size)"/> which is probably the method you
/// want to override in order to modify a control's arrangement.
/// </remarks>
Core::Media::Size Visual::MeasureCore(Core::Media::Size const & availableSize)
{
    if (IsVisible)
    {
        auto margin = Margin;

        ApplyTemplate();

        auto _availableSize = availableSize.Deflate(margin);
        auto constrained = ApplyLayoutConstraints(this, _availableSize);

        auto measured = MeasureOverride(constrained);

        auto width = measured.Width;
        auto height = measured.Height;

        if (Width != -1.0)
        {
            width = Width;
        }

        width = std::min(width, MaxWidth);
        width = std::max(width, MinWidth);

        if (Height != -1.0)
        {
            height = Height;
        }

        height = std::min(height, MaxHeight);
        height = std::max(height, MinHeight);

        if (UseLayoutRounding)
        {
            float scale = GetLayoutScale();
            if (!_visualRoot.expired())
            {
                Visual* Root = VisualRoot->ToVisual();
                if (Root)
                    scale = Root->GetLayoutScale();
            }

            width = ceil(width * scale) / scale;
            height = ceil(height * scale) / scale;
        }

        return Core::Media::Size(width, height).Inflate(margin);
    }
    else
    {
        return Core::Media::Size();
    }
}
