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

#ifndef __XUI_UI_CONTROLS_VISUAL_H__
#define __XUI_UI_CONTROLS_VISUAL_H__

#include <standard/events.h>
#include <standard/collection.h>
#include "Core/Media/MediaTypes.h"
#include "Core/Media/Geometry.h"
#include "Core/Media/RelativePoint_t.h"
#include "Interfaces/IBrush.h"
#include "Interfaces/IDrawingContext.h"
#include "Interfaces/IRenderer.h"

#include "Core/Dependencies/DependencyObject.h"
#include "Core/Dependencies/Models/DataProperty.h"
#include "Core/Dependencies/Models/DirectProperty.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("Core/Media/Transform.h"))
MetaAssemblyEnd

namespace System::Reflection::Assembly
{
    class AssemblyModulXUI;
}

namespace XamlCPP
{
    namespace Core
    {
        struct PropertyChangedEventArgs;
    }
}

namespace XUI
{
    namespace Core
    {
        namespace Media
        {
            //class Geometry;
            struct Transform;
            struct RelativePoint;
        }

        namespace SceneGraph
        {
            class SceneNode;
        }
    }

    namespace Platform
    {
        class UIRender;
    }
}

namespace XUI::UI::Controls
{
    class Visual;

    /// <summary>
    /// Defines how a control aligns itself horizontally in its parent control.
    /// </summary>
    enum class HorizontalAlignment
    {
        /// <summary>
        /// The control stretches to fill the width of the parent control.
        /// </summary>
        Stretch,

        /// <summary>
        /// The control aligns itself to the left of the parent control.
        /// </summary>
        Left,

        /// <summary>
        /// The control centers itself in the parent control.
        /// </summary>
        Center,

        /// <summary>
        /// The control aligns itself to the right of the parent control.
        /// </summary>
        Right,
    };

    /// <summary>
    /// Defines how a control aligns itself vertically in its parent control.
    /// </summary>
    enum class VerticalAlignment
    {
        /// <summary>
        /// The control stretches to fill the height of the parent control.
        /// </summary>
        Stretch,

        /// <summary>
        /// The control aligns itself to the top of the parent control.
        /// </summary>
        Top,

        /// <summary>
        /// The control centers itself within the parent control.
        /// </summary>
        Center,

        /// <summary>
        /// The control aligns itself to the bottom of the parent control.
        /// </summary>
        Bottom,
    };

    /// <summary>
    /// Holds the event arguments for the <see cref="Visual.AttachedToVisualTree"/> and 
    /// <see cref="Visual.DetachedFromVisualTree"/> events.
    /// </summary>
    struct VisualTreeAttachmentEventArgs : System::Events::EventArg
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="VisualTreeAttachmentEventArgs"/> class.
        /// </summary>
        /// <param name="parent">The parent that the visual is being attached to or detached from.</param>
        /// <param name="root">The root visual.</param>
        VisualTreeAttachmentEventArgs(Visual* parent, Interfaces::IRenderRoot* root)
        {
            System::ThrowIfFailed<System::ArgumentNullException>(parent != nullptr);
            System::ThrowIfFailed<System::ArgumentNullException>(root != nullptr);

            Parent = parent;
            Root = root;
        }

        /// <summary>
        /// Gets the parent that the visual is being attached to or detached from.
        /// </summary>
        Visual* Parent;

        /// <summary>
        /// Gets the root of the visual tree that the visual is being attached to or detached from.
        /// </summary>
        Interfaces::IRenderRoot* Root;
    };

    class TC_XUI_API Visual : public Core::Dependency::DependencyObject
    {
        friend class XUI::Platform::UIRender;
        friend class Core::SceneGraph::SceneNode;
    public:
        typedef System::Collection::List< std::shared_ptr<Visual> > VisualChildList;

        /// <summary>
        /// Defines the <see cref="Bounds"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty BoundsProperty;

        /// <summary>
        /// Defines the <see cref="ClipToBounds"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ClipToBoundsProperty;

        /// <summary>
        /// Defines the <see cref="Clip"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ClipProperty;

        /// <summary>
        /// Defines the <see cref="IsVisibleProperty"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty IsVisibleProperty;

        /// <summary>
        /// Defines the <see cref="Opacity"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty OpacityProperty;

        /// <summary>
        /// Defines the <see cref="OpacityMask"/> property.
        /// </summary>
        static std::shared_ptr<Core::Dependency::DependencyProperty> OpacityMaskProperty;

        /// <summary>
        /// Defines the <see cref="RenderTransform"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty RenderTransformProperty;

        /// <summary>
        /// Defines the <see cref="RenderTransformOrigin"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty RenderTransformOriginProperty;

        /// <summary>
        /// Defines the <see cref="IVisual.VisualParent"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty VisualParentProperty;

        /// <summary>
        /// Defines the <see cref="ZIndex"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ZIndexProperty;

        /// <summary>
        /// Defines the <see cref="DesiredSize"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty DesiredSizeProperty;

        /// <summary>
        /// Defines the <see cref="Width"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty WidthProperty;

        /// <summary>
        /// Defines the <see cref="Height"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty HeightProperty;

        /// <summary>
        /// Defines the <see cref="MinWidth"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty MinWidthProperty;

        /// <summary>
        /// Defines the <see cref="MaxWidth"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty MaxWidthProperty;

        /// <summary>
        /// Defines the <see cref="MinHeight"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty MinHeightProperty;

        /// <summary>
        /// Defines the <see cref="MaxHeight"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty MaxHeightProperty;

        /// <summary>
        /// Defines the <see cref="Margin"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty MarginProperty;

        /// <summary>
        /// Defines the <see cref="HorizontalAlignment"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty HorizontalAlignmentProperty;

        /// <summary>
        /// Defines the <see cref="VerticalAlignment"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty VerticalAlignmentProperty;

        /// <summary>
        /// Defines the <see cref="UseLayoutRoundingProperty"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty UseLayoutRoundingProperty;

        /// <summary>
        /// Defines the TransformedBounds property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty TransformedBoundsProperty;

        static void StaticClassInitializer();

    protected:
        bool _measuring;
        Core::Media::Size _previousMeasure = Core::Media::Size(0, 0);
        Core::Media::Rect _previousArrange;
        Core::Media::Rect _bounds;
        Core::Media::Rect _transformedBounds;

        std::weak_ptr<Interfaces::IRenderRoot> _visualRoot;
        std::weak_ptr<Visual> m_visualParent;
        Meta(Disable) mutable VisualChildList m_visualChildrens;
        std::shared_ptr<Core::Media::Transform> m_visualTransform = nullptr;
        std::shared_ptr<Interfaces::IBrush> m_visualOpacityMask = nullptr;
        std::shared_ptr<Core::SceneGraph::SceneNode> m_renderNode;

        Core::Media::Size _desiredSize = Core::Media::Size(0, 0);
        bool IsMeasureValid = false;
        bool IsArrangeValid = false;

        /// <summary>
        /// Calls the <see cref="OnAttachedToVisualTree(VisualTreeAttachmentEventArgs)"/> method 
        /// for this control and all of its visual descendents.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void OnAttachedToVisualTreeCore(VisualTreeAttachmentEventArgs const& e);

        /// <summary>
        /// Calls the <see cref="OnDetachedFromVisualTree(VisualTreeAttachmentEventArgs)"/> method 
        /// for this control and all of its visual descendents.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void OnDetachedFromVisualTreeCore(VisualTreeAttachmentEventArgs const& e);

        virtual void OnAttachedToVisualTree(VisualTreeAttachmentEventArgs const& e);

        virtual void OnDetachedFromVisualTree(VisualTreeAttachmentEventArgs const& e);

    public:

        System::Events::EventHandler<VisualTreeAttachmentEventArgs, System::Events::DummyMutex> AttachedToVisualTree;
        System::Events::EventHandler<VisualTreeAttachmentEventArgs, System::Events::DummyMutex> DetachedFromVisualTree;

        /// <summary>
        /// Occurs when a layout pass completes for the control.
        /// </summary>
        System::Events::EventHandler<> LayoutUpdated;

        Visual();
        virtual ~Visual();

        PROPERTY_GET_ONLY(VisualChildList&, VisualChildren);
        VisualChildList& GetVisualChildren() const { return m_visualChildrens; }

        uint32 VisualChildrenCount() const { return m_visualChildrens.size(); }
        Core::Media::Transform* VisualTransform() const { return m_visualTransform.get(); }
        Interfaces::IBrush* VisualOpacityMask() const { return m_visualOpacityMask.get(); }
        Core::SceneGraph::SceneNode* RenderScene() const { return m_renderNode.get(); }
        virtual XUI::Interfaces::IDrawingContextImpl* GetDrawingContext() { return nullptr; }

		double ActualWidth() const { return _bounds.width(); }
		double ActualHeight() const { return _bounds.height(); }

        bool IsValidLayout() const { return IsMeasureValid && IsArrangeValid; }

        void ClearVisualChild();
        virtual void AddVisualChild(Visual*);
        virtual void RemoveVisualChild(Visual*);
        void VisualChildrenCollectionChanged(void * sender, System::Collection::NotifyCollectionChangedEventArgs & e);

        /// <summary>
        /// Gets the visual parent of an <see cref="IVisual"/>.
        /// </summary>
        /// <typeparam name="T">The type of the visual parent.</typeparam>
        /// <param name="visual">The visual.</param>
        /// <returns>
        /// The parent, or null if the visual is unparented or its parent is not of type <typeparamref name="T"/>.
        /// </returns>
        template<typename T>
        T* GetVisualParent()
        {
            return dynamic_cast<T*>(VisualParent);
        }

        /// <summary>
        /// Gets the size that this element computed during the measure pass of the layout process.
        /// </summary>
        PROPERTY_GET_ONLY(Core::Media::Size, DesiredSize);
        Core::Media::Size const& GetDesiredSize() const { return _desiredSize; }

        /// <summary>
        /// Gets the size that this element computed during the measure pass of the layout process.
        /// </summary>
        PROPERTY_GET_ONLY(Core::Media::Rect, TransformedBounds);
        Core::Media::Rect const& GetTransformedBounds() const { return _transformedBounds; }

        /// <summary>
        /// Gets the bounds of the control relative to its parent.
        /// </summary>
        PROPERTY_(Core::Media::Rect, Bounds);
        Core::Media::Rect const& GetBounds() const { return _bounds; }
        void SetBounds(Core::Media::Rect const& value);

        /// <summary>
        /// Gets a value indicating whether the control should be clipped to its bounds.
        /// </summary>
        PROPERTY_(bool, ClipToBounds);
        bool GetClipToBounds() const;
        void SetClipToBounds(bool value);

        /// <summary>
        /// Gets or sets the geometry clip for this visual.
        /// </summary>
        PROPERTY_(Core::Media::Geometry*, Clip);
        Core::Media::Geometry* GetClip() const;
        void SetClip(Core::Media::Geometry* value);

        /// <summary>
        /// Gets a value indicating whether this control is attached to a visual root.
        /// </summary>
        PROPERTY_GET_ONLY(bool, IsAttachedToVisualTree);
        bool GetIsAttachedToVisualTree() const;

        /// <summary>
        /// Gets a value indicating whether this control and all its parents are visible.
        /// </summary>
        PROPERTY_GET_ONLY(bool, IsEffectivelyVisible);
        bool GetIsEffectivelyVisible() const;

        /// <summary>
        /// Gets or sets a value indicating whether this control is visible.
        /// </summary>
        PROPERTY_(bool, IsVisible);
        bool GetIsVisible() const;
        void SetIsVisible(bool value);

        /// <summary>
        /// Gets or sets the opacity of the control.
        /// </summary>
        PROPERTY_(double, Opacity);
        double GetOpacity() const;
        void SetOpacity(double value);

        /// <summary>
        /// Gets or sets the opacity mask for the control.
        /// </summary>
        PROPERTY_(std::shared_ptr<XUI::Interfaces::IBrush> , OpacityMask);
        std::shared_ptr<XUI::Interfaces::IBrush>  GetOpacityMask() const;
        void SetOpacityMask(std::shared_ptr<XUI::Interfaces::IBrush>  value);

        /// <summary>
        /// Gets or sets the render transform of the control.
        /// </summary>
        PROPERTY_(std::shared_ptr<Core::Media::Transform>, RenderTransform);
        std::shared_ptr<Core::Media::Transform> GetRenderTransform() const;
        void SetRenderTransform(std::shared_ptr<Core::Media::Transform> const& value);

        /// <summary>
        /// Gets or sets the render transform origin of the control.
        /// </summary>
        PROPERTY_(Core::Media::RelativePoint, RenderTransformOrigin);
        Core::Media::RelativePoint GetRenderTransformOrigin() const;
        void SetRenderTransformOrigin(Core::Media::RelativePoint value);

        /// <summary>
        /// Gets the control's parent visual.
        /// </summary>
        PROPERTY_GET_ONLY(Visual*, VisualParent);
        Visual* GetVisualParent() const;
        XamlCPP::Core::Dependency::IDependencyObject* GetIDepVisualParent() const override final { return GetVisualParent(); }

        /// <summary>
        /// Gets the root of the visual tree, if the control is attached to a visual tree.
        /// </summary>
        PROPERTY_GET_ONLY(Interfaces::IRenderRoot*, VisualRoot);
        Interfaces::IRenderRoot* GetVisualRoot() const;

        /// <summary>
        /// Gets or sets the Z index of the node.
        /// </summary>
        PROPERTY_(int, ZIndex);
        int GetZIndex() const;
        void SetZIndex(int value);

        /// <summary>
        /// Gets or sets the width of the element.
        /// </summary>
        PROPERTY_(double, Width);
        double GetWidth() const;
        void SetWidth(double value);

        /// <summary>
        /// Gets or sets the height of the element.
        /// </summary>
        PROPERTY_(double, Height);
        double GetHeight() const;
        void SetHeight(double value);

        /// <summary>
        /// Gets or sets the minimum width of the element.
        /// </summary>
        PROPERTY_(double, MinWidth);
        double GetMinWidth() const;
        void SetMinWidth(double value);

        /// <summary>
        /// Gets or sets the maximum width of the element.
        /// </summary>
        PROPERTY_(double, MaxWidth);
        double GetMaxWidth() const;
        void SetMaxWidth(double value);

        /// <summary>
        /// Gets or sets the minimum height of the element.
        /// </summary>
        PROPERTY_(double, MinHeight);
        double GetMinHeight() const;
        void SetMinHeight(double value);

        /// <summary>
        /// Gets or sets the maximum height of the element.
        /// </summary>
        PROPERTY_(double, MaxHeight);
        double GetMaxHeight() const;
        void SetMaxHeight(double value);

        /// <summary>
        /// Gets or sets the margin around the element.
        /// </summary>
        PROPERTY_(Core::Media::Thickness, Margin);
        Core::Media::Thickness GetMargin() const;
        void SetMargin(Core::Media::Thickness const& value);

        /// <summary>
        /// Gets or sets the element's preferred horizontal alignment in its parent.
        /// </summary>
        PROPERTY_(UI::Controls::HorizontalAlignment, HorizontalAlignment);
        UI::Controls::HorizontalAlignment GetHorizontalAlignment() const;
        void SetHorizontalAlignment(UI::Controls::HorizontalAlignment value);

        /// <summary>
        /// Gets or sets the element's preferred vertical alignment in its parent.
        /// </summary>
        PROPERTY_(UI::Controls::VerticalAlignment, VerticalAlignment);
        UI::Controls::VerticalAlignment GetVerticalAlignment() const;
        void SetVerticalAlignment(UI::Controls::VerticalAlignment value);

        /// <summary>
        /// Defines the <see cref="UseLayoutRoundingProperty"/> property.
        /// </summary>
        PROPERTY_(bool, UseLayoutRounding);
        bool GetUseLayoutRounding() const;
        void SetUseLayoutRounding(bool value);

        /// <summary>
        /// Invalidates the visual and queues a repaint.
        /// </summary>
        void InvalidateVisual();

        /// <summary>
        /// Invalidates the measurement of the control and queues a new layout pass.
        /// </summary>
        void InvalidateMeasure();
        void InvalidateArrange();

        /// <summary>
        /// Renders the control to a <see cref="DrawingContext"/>.
        /// </summary>
        /// <param name="context">The context.</param>
        virtual void Render(Interfaces::IDrawingContextImpl* context);

        /// <summary>
        /// Returns a transform that transforms the visual's coordinates into the coordinates
        /// of the specified <paramref name="visual"/>.
        /// </summary>
        /// <param name="visual">The visual to translate the coordinates to.</param>
        /// <returns>
        /// A <see cref="Matrix"/> containing the transform or null if the visuals don't share a
        /// common ancestor.
        /// </returns>
        Core::Media::Matrix TransformToVisual(Visual* visual);

        /// <summary>
        /// Returns a transform that transforms the visual's coordinates into the coordinates
        /// of the specified <paramref name="visual"/>.
        /// </summary>
        /// <param name="visual">The visual to translate the coordinates to.</param>
        /// <returns>
        /// A <see cref="Matrix"/> containing the transform or null if the visuals don't share a
        /// common ancestor.
        /// </returns>
        Visual* FindCommonVisualAncestor(Visual* target);

        /// <summary>
        /// Enumerates the ancestors of an <see cref="IVisual"/> in the visual tree.
        /// </summary>
        /// <param name="visual">The visual.</param>
        /// <returns>The visual's ancestors.</returns>
        void Meta(Disable) GetVisualAncestors(std::list<Visual*>& visuals, std::function<bool(Visual*)> const& filter = nullptr)
        {
            Visual* parent = VisualParent;
            if (!parent)
                return;

            if (!filter || filter(parent))
                visuals.push_back(parent);

            parent->GetVisualAncestors(visuals, filter);
        }

        /// <summary>
        /// Enumerates an <see cref="IVisual"/> and its descendents in the visual tree.
        /// </summary>
        /// <param name="visual">The visual.</param>
        /// <returns>The visual and its ancestors.</returns>
        void Meta(Disable) GetVisualDescendents(std::list<Visual*>& visuals, std::function<bool(Visual*)> const& filter = nullptr)
        {
            Visual* visual = this;
            if (!filter || filter(visual))
                visuals.push_back(visual);

            for (auto& child : m_visualChildrens.GetNativeEnumerator())
            {
                child->GetVisualDescendents(visuals, filter);
            }
        }

        Visual* Meta(Disable) VisitVisualAncestors(std::function<bool(Visual*)> const& filter)
        {
            Visual* parent = VisualParent;
            if (!parent)
                return nullptr;

            if (filter(parent))
                return parent;

            return parent->VisitVisualAncestors(filter);
        }

        Visual* Meta(Disable) VisitVisualDescendents(std::function<bool(Visual*)> const& filter)
        {
            Visual* control = this;
            if (filter(control))
                return control;

            for (auto& child : m_visualChildrens.GetNativeEnumerator())
                if (child->VisitVisualDescendents(filter))
                    return child.get();
            return nullptr;
        }

        void Meta(Disable) CallVisualDescendents(std::function<void(Visual*)> const& func)
        {
            func(this);

            for (auto& child : m_visualChildrens.GetNativeEnumerator())
            {
                child->CallVisualDescendents(func);
            }
        }

        /// <summary>
        /// Gets the visual offset from the specified ancestor.
        /// </summary>
        /// <param name="ancestor">The ancestor visual.</param>
        /// <param name="visual">The visual.</param>
        /// <returns>The visual offset.</returns>
        Core::Media::Vector GetOffsetFrom(Visual* visual);
        Core::Media::Rect const& GetScreenArea() { return _previousArrange; }

        /// <summary>
        /// Creates the visual children of the control, if necessary
        /// </summary>
        virtual void ApplyTemplate()
        {
        }

        virtual void RemoveTemplate()
        {
        }

        virtual float GetLayoutScale() const { return 1.0; }

        /// <summary>
        /// Carries out a measure of the control.
        /// </summary>
        /// <param name="availableSize">The available size for the control.</param>
        void Measure(Core::Media::Size const& availableSize);

        /// <summary>
        /// Arranges the control and its children.
        /// </summary>
        /// <param name="rect">The control's new bounds.</param>
        void Arrange(Core::Media::Rect const& rect);

        /// <summary>
        /// Called when the <see cref="RenderTransform"/>'s <see cref="Transform.Changed"/> event
        /// is fired.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The event args.</param>
        void RenderTransformCoreChanged(void* sender, XamlCPP::Core::PropertyChangedEventArgs& e)
        {
            InvalidateVisual();
        }

        /// <summary>
        /// Calculates a control's size based on its <see cref="ILayoutable.Width"/>,
        /// <see cref="ILayoutable.Height"/>, <see cref="ILayoutable.MinWidth"/>,
        /// <see cref="ILayoutable.MaxWidth"/>, <see cref="ILayoutable.MinHeight"/> and
        /// <see cref="ILayoutable.MaxHeight"/>.
        /// </summary>
        /// <param name="control">The control.</param>
        /// <param name="constraints">The space available for the control.</param>
        /// <returns>The control's size.</returns>
        static Core::Media::Size ApplyLayoutConstraints(Visual* control, Core::Media::Size const& constraints);

        /// <summary>
        /// Calls <see cref="InvalidateMeasure"/> on the control on which a property changed.
        /// </summary>
        /// <param name="e">The event args.</param>
        static void AffectsMeasureInvalidate(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);
        static void AffectsArrangeInvalidate(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);
        static void AffectsVisualInvalidate(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);
        static void RenderTransformChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e);

        std::shared_ptr<DependencyObject> ToSharedPtr() override { return shared_from_base_static<DependencyObject>(); }

        virtual bool CanHitTest() { return true; }

    protected:

        virtual Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize);
        virtual void ArrangeCore(Core::Media::Rect const& finalRect);
        virtual Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize);
        virtual void ChildDesiredSizeChanged(Visual* control);

        /// <summary>
        /// Tests whether any of a <see cref="Rect"/>'s properties incude nagative values,
        /// a NaN or Infinity.
        /// </summary>
        /// <param name="rect">The rect.</param>
        /// <returns>True if the rect is invalid; otherwise false.</returns>
        static bool IsInvalidRect(Core::Media::Rect const& rect);

        /// <summary>
        /// Tests whether any of a <see cref="Size"/>'s properties incude nagative values,
        /// a NaN or Infinity.
        /// </summary>
        /// <param name="size">The size.</param>
        /// <returns>True if the size is invalid; otherwise false.</returns>
        static bool IsInvalidSize(Core::Media::Size const& size);

        /// <summary>
        /// The default implementation of the control's measure pass.
        /// </summary>
        /// <param name="availableSize">The size available to the control.</param>
        /// <returns>The desired size for the control.</returns>
        /// <remarks>
        /// This method calls <see cref="MeasureOverride(Size)"/> which is probably the method you
        /// want to override in order to modify a control's arrangement.
        /// </remarks>
        virtual Core::Media::Size MeasureCore(Core::Media::Size const& availableSize);

        /// <summary>
        /// Sets the visual parent of the Visual.
        /// </summary>
        /// <param name="value">The visual parent.</param>
        void SetVisualParent(Visual* value);

        /// <summary>
        /// Called when the control's visual parent changes.
        /// </summary>
        /// <param name="oldParent">The old visual parent.</param>
        /// <param name="newParent">The new visual parent.</param>
        virtual void OnVisualParentChanged(Visual* oldParent, Visual* newParent);

        static Visual* FindRenderRoot(Visual* visual);
    };
}

#endif // !__XUI_UI_CONTROLS_VISUAL_H__
