#include "StdAfx.h"
#include "LayoutDocument.h"
#include "LayoutRoot.h"
#include "LayoutDocumentPane.h"
#include "DockingManager.h"

XUI::UI::LayoutDocument::LayoutDocument()
{
}

XUI::UI::LayoutDocument::~LayoutDocument()
{
}

void XUI::UI::LayoutDocument::SetDescription(std::string const & value)
{
    if (_description != value)
    {
        _description = value;
        NotifyPropertyChanged::RaisePropertyChanged("Description");
    }
}

void XUI::UI::LayoutDocument::Close()
{
    if (GetRoot() && GetRoot()->GetManager())
    {
        GetRoot()->GetManager()->_ExecuteCloseCommand(this);
    }
    else
    {
        CloseDocument();
    }
}

bool XUI::UI::LayoutDocument::CloseDocument()
{
    if (TestCanClose())
    {
        CloseInternal();
        return true;
    }

    return false;
}

void XUI::UI::LayoutDocument::InternalDock()
{
    auto root = GetRoot();
    LayoutDocumentPane* documentPane = nullptr;
    if (root->LastFocusedDocument &&
        root->LastFocusedDocument != this)
    {
        documentPane = root->LastFocusedDocument->GetParent()->Static_As<LayoutDocumentPane>();
    }

    if (documentPane)
    {
        LayoutContainer::ContainsChild(root, [&documentPane](LayoutElement* x) { return documentPane = x->Static_As<LayoutDocumentPane>(); });
    }


    bool added = false;
    if (GetRoot()->GetManager()->LayoutUpdateStrategy)
    {
        added = GetRoot()->GetManager()->LayoutUpdateStrategy->BeforeInsertDocument(root, this, documentPane);
    }

    if (!added)
    {
        if (documentPane == nullptr)
            throw System::InvalidOperationException("Layout must contains at least one LayoutDocumentPane in order to host documents");

        documentPane->AddChild(this);
        added = true;
    }

    if (GetRoot()->GetManager()->LayoutUpdateStrategy)
    {
        GetRoot()->GetManager()->LayoutUpdateStrategy->AfterInsertDocument(root, this);
    }


    LayoutContent::InternalDock();
}
