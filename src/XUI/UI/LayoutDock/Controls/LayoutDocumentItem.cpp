#include "LayoutDocumentItem.h"
#include "LayoutDocument.h"
#include "LayoutRoot.h"
#include "DockingManager.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(LayoutDocumentItem, XUI::UI::LayoutDocumentItem::StaticClassInitializer)

XUI::Core::Dependency::RefDependencyProperty LayoutDocumentItem::DescriptionProperty;

LayoutDocumentItem::LayoutDocumentItem()
{

}

void LayoutDocumentItem::Attach(LayoutContent *model)
{
    _document = model->shared_from_base_static<LayoutDocument>();
    LayoutItem::Attach(model);
}

void LayoutDocumentItem::Close()
{
    if (_document->Root && _document->Root->GetManager())
    {
        auto dockingManager = _document->Root->GetManager();
        dockingManager->_ExecuteCloseCommand(_document.get());
    }
}

void LayoutDocumentItem::OnDescriptionChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    (static_cast<LayoutDocumentItem*>(d))->OnDescriptionChanged(e);
}

void LayoutDocumentItem::OnDescriptionChanged(Core::Dependency::DependencyPropertyChangedEventArgs & e)
{
    _document->Description = e.NewValue.GetValue<std::string>();
}

void LayoutDocumentItem::Detach()
{
    _document = nullptr;
    LayoutItem::Detach();
}
