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

#ifndef __XUI_UI_CONTROLS_CONTROL_H__
#define __XUI_UI_CONTROLS_CONTROL_H__

#include "UI/Controls/UIElement.h"
#include "Styling/Style.h"
#include "XamlCPP/Interfaces/IStyle.h"
#include "XamlCPP/Interfaces/IStyleHost.h"
#include "XamlCPP/Interfaces/ISupportInitialize.h"
#include "XamlCPP/Interfaces/INamedScope.h"
#include "XamlCPP/Interfaces/IResourceMap.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/ContextMenu.h"))
MetaAssemblyEnd

namespace XUI
{
    namespace Core
    {
        namespace Styling
        {
        }

        namespace Media
        {
            struct Transform;
        }
    }

    namespace UI
    {
        class ContextMenu;
        class TabControl;
    }

    namespace UI::DataTemplates
    {
        struct ControlTemplate;
        struct DataTemplate;
    }
}

namespace XUI::UI::Controls
{
    class Control;
    class ItemsControl;
    class Visual;

    struct RequestBringIntoViewEventArgs : Core::Observer::RoutedEventArgs
    {
        Visual* TargetObject;
        Core::Media::Rect TargetRect;
    };

    /// <summary>
    /// Holds the details of the <see cref="TemplatedControl.TemplateApplied"/> event.
    /// </summary>
    struct
        Meta(AbstractType)
        TemplateAppliedEventArgs : Core::Observer::RoutedEventArgs
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="TemplateAppliedEventArgs"/> class.
        /// </summary>
        /// <param name="nameScope">The applied template's name scope.</param>
        TemplateAppliedEventArgs(Control* scope);

        /// <summary>
        /// Gets the name scope of the applied template.
        /// </summary>
        Control* Scope;
    };

    struct CollectorLifeCycleListener : XamlCPP::Interfaces::IInstanceLifeCycleListener
    {
        std::list<std::shared_ptr<Controls::Control>>* m_collection = nullptr;
        CollectorLifeCycleListener(std::list<std::shared_ptr<Controls::Control>>* collection)
            : m_collection(collection)
        {}

        void OnBegin(System::Reflection::Object *instance) override
        {
        }
        void OnAfterProperties(System::Reflection::Object *instance) override
        {

        }
        void OnAssociatedToParent(System::Reflection::Object *instance) override
        {

        }
        void OnEnd(System::Reflection::Object *instance) override;
    };

    class TC_XUI_API Control
        : public UIElement, public XamlCPP::Interfaces::IStyleHost, public XamlCPP::Interfaces::IStyleable, public XamlCPP::Interfaces::ISupportInitialize, public XamlCPP::Interfaces::IResourceNode
    {
        friend class XUI::UI::Controls::ItemsControl;
        friend class XUI::UI::Controls::Visual;
        friend class UI::DataTemplates::ControlTemplate;
        friend class UI::DataTemplates::DataTemplate;
        friend class UI::TabControl;

    public:
        typedef System::Collection::List<std::shared_ptr<Control>> LogicalChildrens;

        /// <summary>
        /// Defines the <see cref="DataContext"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty DataContextProperty;

        /// <summary>
        /// Defines the <see cref="Name"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty NameProperty;

        /// <summary>
        /// Defines the <see cref="Parent"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ParentProperty;

        /// <summary>
        /// Defines the <see cref="Tag"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty TagProperty;

        /// <summary>
        /// Defines the <see cref="ContextMenu"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ContextMenuProperty;

        /// <summary>
        /// Defines the <see cref="Template"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty TemplateProperty;

        /// <summary>
        /// Defines the <see cref="ToolTip"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ToolTipProperty;

        static Core::Dependency::RefDependencyProperty StyleProperty;
        static Core::Dependency::RefDependencyProperty FocusVisualStyleProperty;

        /// <summary>
        /// Event raised when an element wishes to be scrolled into view.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> RequestBringIntoViewEvent;

        static void StaticClassInitializer();

        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&RequestBringIntoViewEvent> RequestBringIntoView;

    protected:
        /// <summary>
        /// Adds a pseudo-class to be set when a property equals a certain value.
        /// </summary>
        /// <typeparam name="T">The type of the property.</typeparam>
        /// <param name="property">The property.</param>
        /// <param name="selector">Returns a boolean value based on the property value.</param>
        /// <param name="className">The pseudo-class.</param>
        //template<typename TPropertyValue>
        //static void PseudoClass(
        //    Core::Dependency::DependencyProperty* _property,
        //    std::function<bool(TPropertyValue)> selector,
        //    std::string className)
        //{
        //    System::ThrowIfFailed<System::ArgumentNullException>(_property != nullptr);
        //    System::ThrowIfFailed<System::ArgumentNullException>(selector != nullptr);
        //    System::ThrowIfFailed<System::ArgumentNullException>(!className.empty());
        //    System::ThrowIfFailed<System::ArgumentNullException>(_property != nullptr);
        //
        //    _property->Changed.get_observable().merge(_property->Initialized.get_observable()).subscribe([className, selector](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
        //    {
        //        Control* control = dynamic_cast<Control*>(e.Sender);
        //        if (!control)
        //            return;
        //
        //        if (selector(e.NewValue.GetValue<TPropertyValue>()))
        //        {
        //            control->_classes.insert(className);
        //        }
        //        else
        //        {
        //            control->_classes.erase(className);
        //        }
        //    });
        //}

        void RegisterWithNameScope();
        void InitializedIfPrepared();

    protected:
        int _initCount = 0;
		int _tabIndex = -1;
        std::shared_ptr<Core::Dependency::DependencyObject> m_controlTemplateRoot;
        std::weak_ptr<DataTemplates::FrameworkTemplate> _selfTemplateOwner;
        std::weak_ptr<Control> _parent;
        std::set<std::string> _classes;
        //Control* _focusAdorner = nullptr;
        Meta(Disable) mutable LogicalChildrens _logicalChildren;
        mutable std::weak_ptr<XamlCPP::Interfaces::INameScope> _nameScope;
        Meta(Disable) mutable XamlCPP::Interfaces::StylesList _styles;
        bool _isAttachedToLogicalTree = false;
        bool _isMovingInLogicalTemplate = false;
        bool _styled = false;
        bool _defaultControlTemplateInitialized = false;
        bool _isInitialized = false;
        rxcpp::subjects::subject<IStyleable*>* _styleDetach = nullptr;
        std::shared_ptr<XamlCPP::Interfaces::IStyle> _style;
        std::shared_ptr<XamlCPP::Interfaces::IStyle> _focusVisualStyle;
        XamlCPP::Interfaces::ResourceMap m_Resources;
        XamlCPP::Interfaces::ResourceMap m_DataTemplates;
        rxcpp::subscription _toolTipSubsciption;

    public:

        /// <summary>
        /// Raised when the control's template is applied.
        /// </summary>
        Meta(XamlCPP::EventAttribute)
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        EventHandler<TemplateAppliedEventArgs>     TemplateApplied;

        /// <summary>
        /// Initializes a new instance of the <see cref="Control"/> class.
        /// </summary>
        Control();
        virtual ~Control();

        /// <summary>
        /// Raised when the control is attached to a rooted logical tree.
        /// </summary>
        System::Events::EventHandler<LogicalTreeAttachmentEventArgs, System::Events::DummyMutex> AttachedToLogicalTree;

        /// <summary>
        /// Raised when the control is detached from a rooted logical tree.
        /// </summary>
        System::Events::EventHandler<LogicalTreeAttachmentEventArgs, System::Events::DummyMutex> DetachedFromLogicalTree;

        System::Events::EventHandler<System::Events::EventArg, System::Events::DummyMutex> ChangedLogicalTree;

        /// <summary>
        /// Occurs when the <see cref="DataContext"/> property changes.
        /// </summary>
        /// <remarks>
        /// This event will be raised when the <see cref="DataContext"/> property has changed and
        /// all subscribers to that change have been notified.
        /// </remarks>
        System::Events::EventHandler<System::Events::EventArg, System::Events::DummyMutex> DataContextChanged;

        PROPERTY_GET_ONLY(XamlCPP::Interfaces::INameScope*, NamedScope);
        XamlCPP::Interfaces::INameScope* GetNamedScope() const;

        std::shared_ptr<XamlCPP::Interfaces::INameScope> GetSharedNamedScope() const override;

        /// <summary>
        /// Occurs when the control has finished initialization.
        /// </summary>
        /// <remarks>
        /// The Initialized event indicates that all property values on the control have been set.
        /// When loading the control from markup, it occurs when 
        /// <see cref="ISupportInitialize.EndInit"/> is called *and* the control
        /// is attached to a rooted logical tree. When the control is created by code and
        /// <see cref="ISupportInitialize"/> is not used, it is called when the control is attached
        /// to the visual tree.
        /// </remarks>
        System::Events::EventHandler<System::Events::EventArg, System::Events::DummyMutex> Initialized;

        System::Events::EventHandler<System::Events::EventArg, System::Events::DummyMutex>* OnInitialized() override final { return &Initialized; };

        PROPERTY_GET_ONLY(LogicalChildrens&, LogicalChildren);
        LogicalChildrens& GetLogicalChildren() { return _logicalChildren; }

        /// <summary>
        /// Gets or sets the control's classes.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Classes can be used to apply user-defined styling to controls, or to allow controls
        /// that share a common purpose to be easily selected.
        /// </para>
        /// <para>
        /// Even though this property can be set, the setter is only intended for use in object
        /// initializers. Assigning to this property does not change the underlying collection,
        /// it simply clears the existing collection and addds the contents of the assigned
        /// collection.
        /// </para>
        /// </remarks>
        //PROPERTY_GET_ONLY(std::set<std::string> const&, Classes);
        //std::set<std::string> const& GetClasses() const { return _classes; }
        //void SetClasses(std::string const& value);

        //PROPERTY_GET_ONLY(std::set<std::string>&, PseudoClasses);
        //std::set<std::string>& GetPseudoClasses() { return _classes; }

        /// <summary>
        /// Gets or sets the control's data context.
        /// </summary>
        /// <remarks>
        /// The data context is an inherited property that specifies the default object that will
        /// be used for data binding.
        /// </remarks>
        PROPERTY_(System::Reflection::Variant, DataContext);
        System::Reflection::Variant GetDataContext() const override final;
        void SetDataContext(System::Reflection::Variant const& value);

        /// <summary>
        /// Gets a value that indicates whether the element has finished initialization.
        /// </summary>
        /// <remarks>
        /// For more information about when IsInitialized is set, see the <see cref="Initialized"/>
        /// event.
        /// </remarks>
        PROPERTY_GS(_isInitialized, IsInitialized);

        bool IsInited() const override final { return _isInitialized; }

        /// <summary>
        /// Gets or sets the styles for the control.
        /// </summary>
        /// <remarks>
        /// Styles for the entire application are added to the Application.Styles collection, but
        /// each control may in addition define its own styles which are applied to the control
        /// itself and its children.
        /// </remarks>
        PROPERTY_GET_ONLY(XamlCPP::Interfaces::StylesList&, Styles);
        XamlCPP::Interfaces::StylesList& GetStyles() override { return _styles; }

        /// <summary>
        /// Gets the control's logical parent.
        /// </summary>
        PROPERTY_GET_ONLY(Control*, Parent);
        Control* GetParent() const { return _parent.lock().get(); }

        /// <summary>
        /// Gets or sets a context menu to the control.
        /// </summary>
        PROPERTY_(std::shared_ptr<XUI::UI::ContextMenu>, ContextMenu);
        std::shared_ptr<XUI::UI::ContextMenu> GetContextMenu() const;
        void SetContextMenu(std::shared_ptr<XUI::UI::ContextMenu> const& value);

        /// <summary>
        /// Gets or sets a context menu to the control.
        /// </summary>
        PROPERTY_(System::Reflection::RefObject, ToolTip);
        System::Reflection::RefObject GetToolTip() const;
        void SetToolTip(System::Reflection::RefObject const& value);

        /// <summary>
        /// Gets or sets a user-defined object attached to the control.
        /// </summary>
        PROPERTY_(System::Reflection::meta::Variant, Tag);
        System::Reflection::meta::Variant GetTag() const;
        void SetTag(System::Reflection::meta::Variant const& value);

        PROPERTY_(XamlCPP::Interfaces::ResourceMap, Resources);
        XamlCPP::Interfaces::ResourceMap const& GetResources() const
        {
            return m_Resources;
        }

        void SetResources(const XamlCPP::Interfaces::ResourceMap &value)
        {
            for (auto i : value.GetNativeEnumerator())
            {
                m_Resources.Add(i.first, i.second);
            }
        }

        PROPERTY_(XamlCPP::Interfaces::ResourceMap const&, DataTemplates);
        XamlCPP::Interfaces::ResourceMap const& GetDataTemplates() const
        {
            return m_DataTemplates;
        }

        void SetDataTemplates(const XamlCPP::Interfaces::ResourceMap &value)
        {
            for (auto i : value.GetNativeEnumerator())
            {
                m_DataTemplates.Add(i.first, i.second);
            }
        }

        XamlCPP::Interfaces::IResourceNode* GetResourceParent() const
        {
            Control* control = GetInheritanceParent()->As<Control>();
            if (control)
                return static_cast<XamlCPP::Interfaces::IResourceNode*>(control);
            return nullptr;
        }

        virtual System::Reflection::Variant const& FindInternalResource(const std::string &name) const override;
        virtual System::Reflection::Variant const& FindResource(const std::string &name) const override;

        PROPERTY_(std::shared_ptr<XamlCPP::Interfaces::IStyle>, Style);
        std::shared_ptr<XamlCPP::Interfaces::IStyle> const& GetStyle() const;
        void SetStyle(std::shared_ptr<XamlCPP::Interfaces::IStyle> const& value);

        PROPERTY_(std::shared_ptr<XamlCPP::Interfaces::IStyle>, FocusVisualStyle);
        std::shared_ptr<XamlCPP::Interfaces::IStyle> const& GetFocusVisualStyle() const;
        void SetFocusVisualStyle(std::shared_ptr<XamlCPP::Interfaces::IStyle> const & value);

        /// <summary>
        /// Gets a value indicating whether the element is attached to a rooted logical tree.
        /// </summary>
        PROPERTY_G(_isAttachedToLogicalTree, IsAttachedToLogicalTree);

        PROPERTY_GET_ONLY(rxcpp::subjects::subject<IStyleable*> *, StyleDetach);
        rxcpp::subjects::subject<IStyleable*> * GetStyleDetach()
        {
            if (!_styleDetach)
                _styleDetach = new rxcpp::subjects::subject<IStyleable*>;
            return _styleDetach;
        }

        /// <summary>
        /// Gets or sets the template that defines the control's appearance.
        /// </summary>
        PROPERTY_(std::shared_ptr<UI::DataTemplates::ControlTemplate>, Template);
        std::shared_ptr<UI::DataTemplates::ControlTemplate> GetTemplate() const;
        void SetTemplate(std::shared_ptr<UI::DataTemplates::ControlTemplate> const& value);

        /// <summary>
        /// Enumerates the ancestors of an <see cref="IVisual"/> in the logical tree.
        /// </summary>
        /// <param name="visual">The visual.</param>
        /// <returns>The logical's ancestors.</returns>
        void Meta(Disable) GetLogicalAncestors(std::list<Control*>& logicals, std::function<bool(Control*)> const& filter = nullptr)
        {
            Control* parent = Parent;
            if (!filter || filter(parent))
                logicals.push_back(parent);

            if (parent)
                parent->GetLogicalAncestors(logicals, filter);
        }

        void Meta(Disable) GetLogicalDescendents(std::list<Control*>& logicals, std::function<bool(Control*)> const& filter = nullptr)
        {
            Control* control = this;
            if (!filter || filter(control))
                logicals.push_back(control);

            for (auto& child : _logicalChildren.GetNativeEnumerator())
            {
                child->GetLogicalDescendents(logicals, filter);
            }
        }

        Control* Meta(Disable) VisitLogicalAncestors(std::function<bool(Control*)> const& filter)
        {
            Control* parent = Parent;
            if (!parent)
                return nullptr;

            if (filter(parent))
                return parent;

            return parent->VisitLogicalAncestors(filter);
        }

        Control* Meta(Disable) VisitLogicalDescendents(std::function<bool(Control*)> const& filter)
        {
            Control* control = this;
            if (filter(control))
                return control;

            for (auto& child : _logicalChildren.GetNativeEnumerator())
                if (child->VisitLogicalDescendents(filter))
                    return child.get();
            return nullptr;
        }

        template<class TParent>
        TParent* FindLogicalAncestor()
        {
            Control* parent = Parent;
            if (!parent)
                return nullptr;

            if (parent->Is<TParent>())
                return static_cast<TParent*>(parent);

            return parent->FindLogicalAncestor<TParent>();
        }

		PROPERTY_(int, TabIndex);
		int GetTabIndex() const { return _tabIndex; }
		void SetTabIndex(int const& value) { _tabIndex = value; }

        /// <inheritdoc/>
        void ApplyTemplate() override;
        void RemoveTemplate() override;
        virtual void ApplyDataTemplate() {}
        virtual void RemoveDataTemplate() {}

        IStyleHost *GetStylingParent() override;

        /// <inheritdoc/>
        virtual void BeginInit() override;
        
        /// <inheritdoc/>
        virtual void EndInit() override;

        /// <summary>
        /// Sets the control's logical parent.
        /// </summary>
        /// <param name="parent">The parent.</param>
        void SetParent(Control* parent);

        /// <summary>
        /// Called when the control is added to a rooted logical tree.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void OnAttachedToLogicalTree(LogicalTreeAttachmentEventArgs& e);
        
        /// <summary>
        /// Called when the control is removed from a rooted logical tree.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void OnDetachedFromLogicalTree(LogicalTreeAttachmentEventArgs& e);
        
        /// <inheritdoc/>
        void OnAttachedToVisualTreeCore(VisualTreeAttachmentEventArgs const& e) override;
        
        /// <inheritdoc/>
        void OnDetachedFromVisualTreeCore(VisualTreeAttachmentEventArgs const& e) override;
        
        /// <summary>
        /// Called before the <see cref="DataContext"/> property changes.
        /// </summary>
        virtual void OnDataContextChanging();
        
        /// <summary>
        /// Called after the <see cref="DataContext"/> property changes.
        /// </summary>
        virtual void OnDataContextChanged();
        
        /// <inheritdoc/>
        void OnGotFocus(Interfaces::GotFocusEventArgs& e) override;
        
        /// <inheritdoc/>
        void OnLostFocus(Core::Observer::RoutedEventArgs& e) override;

        /// <summary>
        /// Called when the <see cref="DataContext"/> property begins and ends being notified.
        /// </summary>
        /// <param name="o">The object on which the DataContext is changing.</param>
        /// <param name="notifying">Whether the notifcation is beginning or ending.</param>
        static void DataContextNotifying(XamlCPP::Core::Dependency::IDependencyObject* o, System::Reflection::meta::Variant const& notifying);
        
        static Control* FindStyleRoot(Control* e);
        
        void ApplyStyling();

        void OnAttachedToLogicalTreeCore(LogicalTreeAttachmentEventArgs& e);

        void OnDetachedFromLogicalTreeCore(LogicalTreeAttachmentEventArgs& e);

        void LogicalChildrenCollectionChanged(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e);

		void ResourceCollectionChanged(void* sender, System::Collection::NotifyCollectionChangedEventArgs& e);

        void SetLogicalParent(Control* children);
        
        void ClearLogicalParent(Control* children);

        void AddLogicalChild(Control * child);

        void RemoveLogicalChild(Control * child);

        void BringIntoView(Core::Media::Rect const& rect);

        /// <summary>
        /// Called when the control's template is applied.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void OnTemplateApplied(TemplateAppliedEventArgs const& e);

        /// <summary>
        /// Called when the <see cref="Template"/> property changes.
        /// </summary>
        /// <param name="e">The event args.</param>
        virtual void OnTemplateChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);

        /// <summary>
        /// Find a data template that matches a piece of data.
        /// </summary>
        virtual std::shared_ptr<XUI::UI::DataTemplates::DataTemplate> FindInternalDataTemplate(std::string const& data);
        virtual std::shared_ptr<XUI::UI::DataTemplates::DataTemplate> FindDataTemplate(System::Reflection::meta::Variant const& data);
        virtual std::shared_ptr<XUI::UI::DataTemplates::ControlTemplate> FindControlTemplate(System::Reflection::meta::Variant const& data);

        XamlCPP::Core::Dependency::IDependencyObject* ToIDependencyObject() override final { return static_cast<XamlCPP::Core::Dependency::IDependencyObject*>(this); }
        XamlCPP::Core::Dependency::IDependencyObject* ToIDependencyObject2() override final { return static_cast<XamlCPP::Core::Dependency::IDependencyObject*>(this); }
    };
}

#endif // !__XUI_UI_CONTROLS_CONTROL_H__