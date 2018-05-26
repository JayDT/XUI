#include "LayoutDocumentControl.h"
#include "LayoutAnchorGroupControl.h"
#include "LayoutDocumentPane.h"
#include "LayoutAnchorControl.h"
#include "LayoutContent.h"
#include "LayoutContainer.h"
#include "LayoutRoot.h"
#include "DockingManager.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutDocumentControl, XUI::UI::LayoutDocumentControl::StaticClassInitializer);

XUI::Core::Dependency::RefDependencyProperty LayoutDocumentControl::ModelProperty;
XUI::Core::Dependency::RefDependencyProperty LayoutDocumentControl::LayoutItemProperty;

void XUI::UI::LayoutDocumentControl::StaticClassInitializer()
{
    ModelProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::LayoutContent>>::Register<LayoutDocumentControl>(nameof(Model)));
    LayoutItemProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::LayoutItem>>::Register<LayoutDocumentControl>(nameof(LayoutItem)));

    ModelProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        LayoutDocumentControl* control = e.Sender->Static_As<LayoutDocumentControl>();
        if (control)
        {
            UI::LayoutContent* content = static_cast<UI::LayoutContent*>(e.NewValue.ToObject());
            control->Content = content->Content;
        }
        static_cast<UI::Controls::Control*>(e.Sender)->OnTemplateChanged(e);
    });

    //FrameworkPropertyMetadata tempVar(LayoutDocumentControl::typeid);
    //DefaultStyleKeyProperty::OverrideMetadata(LayoutDocumentControl::typeid, &tempVar);
    //FrameworkPropertyMetadata tempVar2(true);
    //FocusableProperty::OverrideMetadata(LayoutDocumentControl::typeid, &tempVar2);
}

XUI::UI::LayoutDocumentControl::LayoutDocumentControl()
{
}

void XUI::UI::LayoutDocumentControl::OnModelChanged(Core::Dependency::DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    (static_cast<LayoutDocumentControl*>(d))->OnModelChanged(e);
}

void XUI::UI::LayoutDocumentControl::OnModelChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    if (e.OldValue != nullptr)
    {
        e.OldValue.ToObject()->Static_As<LayoutContent>()->PropertyChanged -= System::make_eventhandler(&LayoutDocumentControl::Model_PropertyChanged, this);
    }
    
    if (Model != nullptr)
    {
        Model->PropertyChanged += System::make_eventhandler(&LayoutDocumentControl::Model_PropertyChanged, this);
        SetLayoutItem(Model->Root->GetManager()->GetLayoutItemFromModel(Model.get())->shared_from_base_static<UI::LayoutItem>());
    }
    else
    {
        SetLayoutItem(nullptr);
    }
}

void LayoutDocumentControl::Model_PropertyChanged(void* sender, XamlCPP::Core::PropertyChangedEventArgs& e)
{
    if (e.PropertyName == "IsEnabled")
    {
        if (Model != nullptr)
        {
            IsEnabled = Model->IsEnabled;
            if (!IsEnabled && Model->IsActive)
            {
                if (Model->Parent != nullptr && Model->Parent->Is<LayoutDocumentPane>())
                {
                    Model->Parent->Static_As<LayoutDocumentPane>()->SetNextSelectedIndex();
                }
            }
        }
    }
}

//void LayoutDocumentControl::OnPreviewGotKeyboardFocus(KeyboardFocusChangedEventArgs *e)
//{
//    this->SetIsActive();
//    Control::OnPreviewGotKeyboardFocus(e);
//}

void LayoutDocumentControl::OnPointerPressed(Interfaces::PointerPressedEventArgs& e)
{
    this->SetIsActive();
    Control::OnPointerPressed(e);
}

void LayoutDocumentControl::SetIsActive()
{
    if (this->Model != nullptr)
    {
        this->Model->IsActive = true;
    }
}
