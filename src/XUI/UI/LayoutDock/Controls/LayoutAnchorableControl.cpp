#include "LayoutAnchorableControl.h"
#include "LayoutAnchorablePane.h"
#include "LayoutContent.h"
#include "LayoutAnchorable.h"
#include "LayoutItem.h"
#include "DockingManager.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutAnchorableControl, XUI::UI::LayoutAnchorableControl::StaticClassInitializer);

XUI::Core::Dependency::RefDependencyProperty LayoutAnchorableControl::LayoutItemProperty;
XUI::Core::Dependency::RefDependencyProperty LayoutAnchorableControl::ModelProperty;

void XUI::UI::LayoutAnchorableControl::StaticClassInitializer()
{
    ModelProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::LayoutContent>>::Register<LayoutAnchorableControl>(nameof(Model)));
    LayoutItemProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::LayoutItem>>::Register<LayoutAnchorableControl>(nameof(LayoutItem)));

    ModelProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        LayoutAnchorableControl* control = e.Sender->Static_As<LayoutAnchorableControl>();
        if (control)
        {
            UI::LayoutContent* content = static_cast<UI::LayoutContent*>(e.NewValue.ToObject());
            control->Content = content->Content;
        }
        static_cast<UI::Controls::Control*>(e.Sender)->OnTemplateChanged(e);
    });

    //FrameworkPropertyMetadata tempVar(LayoutAnchorableControl::typeid);
    //DefaultStyleKeyProperty::OverrideMetadata(LayoutAnchorableControl::typeid, &tempVar);
    //FrameworkPropertyMetadata tempVar2(false);
    //FocusableProperty::OverrideMetadata(LayoutAnchorableControl::typeid, &tempVar2);
}

LayoutAnchorableControl::LayoutAnchorableControl()
{
    //SetBinding(FlowDirectionProperty, new Binding("Model.Root.Manager.FlowDirection") { Source = this });
}

void LayoutAnchorableControl::Initialize()
{

}

void LayoutAnchorableControl::OnModelChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    (static_cast<LayoutAnchorableControl*>(d))->OnModelChanged(e);
}

void LayoutAnchorableControl::OnModelChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    if (e.OldValue.Valid())
    {
        e.OldValue.ToObject()->Static_As<UI::LayoutContent>()->PropertyChanged -= System::make_eventhandler(&LayoutAnchorableControl::Model_PropertyChanged, this);
    }
    
    if (Model != nullptr)
    {
        Model->PropertyChanged += System::make_eventhandler(&LayoutAnchorableControl::Model_PropertyChanged, this);;
        SetLayoutItem(Model->Root->GetManager()->GetLayoutItemFromModel(Model.get())->shared_from_base_static<UI::LayoutItem>());
    }
    else
    {
        SetLayoutItem(nullptr);
    }
}

void LayoutAnchorableControl::Model_PropertyChanged(void* sender, XamlCPP::Core::PropertyChangedEventArgs& e)
{
    if (e.PropertyName == "IsEnabled")
    {
        if (Model != nullptr)
        {
            IsEnabled = Model->IsEnabled;
            if (!IsEnabled && Model->IsActive)
            {
                if (Model->Parent != nullptr && Model->Parent->Is<LayoutAnchorablePane>())
                {
                    Model->Parent->Static_As<LayoutAnchorablePane>()->SetNextSelectedIndex();
                }
            }
        }
    }
}


void LayoutAnchorableControl::OnGotFocus(Interfaces::GotFocusEventArgs& e)
{
    if (Model != nullptr)
    {
        Model->IsActive = true;
    }

    Control::OnGotFocus(e);
}

void LayoutAnchorableControl::OnPointerPressed(Interfaces::PointerPressedEventArgs& e)
{
    if (Model != nullptr)
    {
        Model->IsActive = true;
    }

    Control::OnPointerPressed(e);
}