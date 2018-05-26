#include "NavigatorWindow.h"
#include "Core/GlobalInitializerScheduler.h"
#include "LayoutDock/Controls/DropArea.h"
#include "LayoutDock/DockingManager.h"
#include "LayoutDocument.h"
#include "LayoutAnchorable.h"
#include "LayoutAnchorableItem.h"
#include "LayoutDocumentItem.h"
#include "DockingManager.h"
#include "cpplinq/linq.hpp"

using namespace XamlCPP::Core::Dependency;
using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(NavigatorWindow, XUI::UI::NavigatorWindow::StaticClassInitializer)

Core::Dependency::RefDependencyProperty NavigatorWindow::DocumentsProperty;
Core::Dependency::RefDependencyProperty NavigatorWindow::AnchorablesProperty;
Core::Dependency::RefDependencyProperty NavigatorWindow::SelectedDocumentProperty;
Core::Dependency::RefDependencyProperty NavigatorWindow::SelectedAnchorableProperty;

void XUI::UI::NavigatorWindow::StaticClassInitializer()
{
	//FrameworkPropertyMetadata tempVar(NavigatorWindow::typeid);
	//DefaultStyleKeyProperty::OverrideMetadata(NavigatorWindow::typeid, &tempVar);
	//FrameworkPropertyMetadata tempVar2(false);
	//ShowActivatedProperty::OverrideMetadata(NavigatorWindow::typeid, &tempVar2);
	//FrameworkPropertyMetadata tempVar3(false);
	//ShowInTaskbarProperty::OverrideMetadata(NavigatorWindow::typeid, &tempVar3);
}

NavigatorWindow::NavigatorWindow(DockingManager *manager)
{
	_manager = manager;

	_internalSetSelectedDocument = true;

	auto anchorables = std::make_shared<System::Collection::List<std::shared_ptr<LayoutAnchorableItem>>>();
	LayoutContainer::ContainsChild(_manager->Layout.get(), [this, &anchorables](LayoutElement* x)
	{
		if (x->Is<LayoutAnchorable>())
			anchorables->Add(_manager->GetLayoutItemFromModel(x->Static_As<LayoutAnchorable>())->shared_from_base_static<LayoutAnchorableItem>());
		return false;
	});
	SetAnchorables(anchorables);

	auto documents = std::make_shared<System::Collection::List<std::shared_ptr<LayoutDocumentItem>>>();
	LayoutContainer::ContainsChild(_manager->Layout.get(), [this, &documents](LayoutElement* x)
	{
		if (x->Is<LayoutDocument>())
			documents->Add(_manager->GetLayoutItemFromModel(x->Static_As<LayoutDocument>())->shared_from_base_static<LayoutDocumentItem>());
		return false;
	});
	SetDocuments(documents);

	_internalSetSelectedDocument = false;

	if (Documents->size() > 1)
	{
		InternalSetSelectedDocument((*Documents)[1].get());
	}

	this->DataContext = this;

	this->Initialized += System::make_eventhandler(&NavigatorWindow::OnLoaded, this);
	this->DetachedFromLogicalTree += System::make_eventhandler(&NavigatorWindow::OnUnloaded, this);

	UpdateThemeResources();
}

void NavigatorWindow::UpdateThemeResources(Theme *oldTheme)
{
	//if (oldTheme != nullptr)
	//{
	//	if (dynamic_cast<DictionaryTheme*>(oldTheme) != nullptr)
	//	{
	//		if (currentThemeResourceDictionary != nullptr)
	//		{
	//			Resources::MergedDictionaries->Remove(currentThemeResourceDictionary);
	//			currentThemeResourceDictionary = nullptr;
	//		}
	//	}
	//	else
	//	{
	//		auto resourceDictionaryToRemove = Resources::MergedDictionaries::FirstOrDefault([&](void *r)
	//		{
	//			return r->Source == oldTheme->GetResourceUri();
	//		});
	//		if (resourceDictionaryToRemove != nullptr)
	//		{
	//			Resources::MergedDictionaries->Remove(resourceDictionaryToRemove);
	//		}
	//	}
	//}
	//
	//if (_manager->Theme != nullptr)
	//{
	//	if (dynamic_cast<DictionaryTheme*>(_manager->Theme) != nullptr)
	//	{
	//		currentThemeResourceDictionary = (static_cast<DictionaryTheme*>(_manager->Theme))->ThemeResourceDictionary;
	//		Resources::MergedDictionaries->Add(currentThemeResourceDictionary);
	//	}
	//	else
	//	{
	//		ResourceDictionary *tempVar = new ResourceDictionary();
	//		tempVar->Source = _manager->Theme.GetResourceUri();
	//		Resources::MergedDictionaries->Add(tempVar);
	//	}
	//}
}

void NavigatorWindow::OnLoaded(void *sender, System::Events::EventArg& e)
{
	//this->Loaded -= new RoutedEventHandler(OnLoaded);
	//
	//this->Focus();
	//
	////this.SetParentToMainWindowOf(_manager);
	//WindowStartupLocation = WindowStartupLocation::CenterOwner;
}

void NavigatorWindow::OnUnloaded(void *sender, Controls::LogicalTreeAttachmentEventArgs& e)
{
	//this->Unloaded -= new RoutedEventHandler(OnUnloaded);

	//_hwndSrc.RemoveHook(_hwndSrcHook);
	//_hwndSrc.Dispose();
	//_hwndSrc = null;
}

void NavigatorWindow::OnSelectedDocumentChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
	(static_cast<NavigatorWindow*>(d))->OnSelectedDocumentChanged(e);
}

void NavigatorWindow::OnSelectedDocumentChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
	if (_internalSetSelectedDocument)
	{
		return;
	}

	if (SelectedDocument != nullptr && SelectedDocument->ActivateCommand->CanExecute(nullptr, this))
	{
		SelectedDocument->ActivateCommand->Execute(nullptr, this);
		Hide();
	}

}

void NavigatorWindow::InternalSetSelectedDocument(LayoutDocumentItem *documentToSelect)
{
	_internalSetSelectedDocument = true;
	SelectedDocument = documentToSelect->shared_from_base_static<LayoutDocumentItem>();
	_internalSetSelectedDocument = false;
}

void NavigatorWindow::OnSelectedAnchorableChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
	(static_cast<NavigatorWindow*>(d))->OnSelectedAnchorableChanged(e);
}

void NavigatorWindow::OnSelectedAnchorableChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
	auto selectedAnchorable = e.NewValue.ToObject()->Static_As<LayoutAnchorableItem>();
	if (SelectedAnchorable != nullptr && SelectedAnchorable->ActivateCommand->CanExecute(nullptr, this))
	{
		SelectedAnchorable->ActivateCommand->Execute(nullptr, this);
		Close();
	}
}

void NavigatorWindow::SelectNextDocument()
{
	if (SelectedDocument != nullptr)
	{
		int docIndex = Documents->IndexOf(SelectedDocument);
		docIndex++;
		if (docIndex == Documents->size())
		{
			docIndex = 0;
		}
		InternalSetSelectedDocument((*Documents)[docIndex].get());
	}

}

void NavigatorWindow::OnKeyDown(Interfaces::KeyEventArgs& e)
{
	Window::OnKeyDown(e);
}

void NavigatorWindow::OnPreviewKeyDown(Interfaces::KeyEventArgs& e)
{
	if (e.Key == Interfaces::Key::Tab)
	{
		SelectNextDocument();
		e.Handled = true;
	}


	Window::OnPreviewKeyDown(e);
}

void NavigatorWindow::OnPreviewKeyUp(Interfaces::KeyEventArgs& e)
{
	if (e.Key != Interfaces::Key::Tab)
	{
		if (SelectedAnchorable != nullptr && SelectedAnchorable->ActivateCommand->CanExecute(nullptr, this))
		{
			SelectedAnchorable->ActivateCommand->Execute(nullptr, this);
		}

		if (SelectedAnchorable == nullptr && SelectedDocument != nullptr && SelectedDocument->ActivateCommand->CanExecute(nullptr, this))
		{
			SelectedDocument->ActivateCommand->Execute(nullptr, this);
		}
		Close();
		e.Handled = true;
	}


	Window::OnPreviewKeyUp(e);
}
