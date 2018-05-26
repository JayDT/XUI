#include "StdAfx.h"
#include "LayoutDocumentFloatingWindow.h"

XUI::UI::LayoutDocumentFloatingWindow::LayoutDocumentFloatingWindow()
{
}

XUI::UI::LayoutDocumentFloatingWindow::~LayoutDocumentFloatingWindow()
{
}

void XUI::UI::LayoutDocumentFloatingWindow::SetRootDocument(std::shared_ptr<LayoutDocument> const & value)
{
    if (_rootDocument != value)
    {
        _rootDocument = value;
        if (_rootDocument)
            _rootDocument->Parent = this;
        NotifyPropertyChanged::RaisePropertyChanged("RootDocument");

        if (RootDocumentChanged)
        {
            static System::Events::EventArg e;
            RootDocumentChanged(this, e);
        }
    }
}

bool XUI::UI::LayoutDocumentFloatingWindow::IsValid()
{
    return _rootDocument != nullptr;
}

void XUI::UI::LayoutDocumentFloatingWindow::AddChild(LayoutElement * child)
{
    if (!child->Is<LayoutDocument>() || _rootDocument != nullptr)
        throw System::ArgumentException();

    RootDocument = child->shared_from_base_static<LayoutDocument>();
    LayoutFloatingWindow::AddChild(child);
}

void XUI::UI::LayoutDocumentFloatingWindow::RemoveChild(LayoutElement * child)
{
    ASSERT(child == _rootDocument.get() && child);
    RootDocument = nullptr;
    LayoutFloatingWindow::RemoveChild(child);
}

void XUI::UI::LayoutDocumentFloatingWindow::ReplaceChild(LayoutElement * oldElement, LayoutElement * newElement)
{
    if (!newElement->Is<LayoutDocument>() || (oldElement != nullptr && _rootDocument.get() != oldElement))
        throw System::ArgumentException();

    RootDocument = newElement->shared_from_base_static<LayoutDocument>();
    LayoutFloatingWindow::ReplaceChild(oldElement, newElement);
}
