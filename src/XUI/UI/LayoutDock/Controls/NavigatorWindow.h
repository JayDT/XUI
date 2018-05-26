#ifndef __XUI_NAVIGATORWINDOW_H__
#define __XUI_NAVIGATORWINDOW_H__

#include "UI/Window.h"
#include "UI/LayoutDock/Controls/LayoutDocumentItem.h"
#include "UI/LayoutDock/Controls/LayoutAnchorableItem.h"
#include "DataGridColumnCollection.h"

namespace XUI::UI
{
  class NavigatorWindow : public Window
  {
private:
    //ResourceDictionary *currentThemeResourceDictionary; // = null
    DockingManager *_manager = nullptr;
	bool _internalSetSelectedDocument = false;

public:

	static Core::Dependency::RefDependencyProperty DocumentsProperty;
	static Core::Dependency::RefDependencyProperty AnchorablesProperty;
	static Core::Dependency::RefDependencyProperty SelectedDocumentProperty;
	static Core::Dependency::RefDependencyProperty SelectedAnchorableProperty;

	static void StaticClassInitializer();

    NavigatorWindow(DockingManager *manager);
	virtual ~NavigatorWindow() {}

    void UpdateThemeResources(Theme *oldTheme = nullptr);
	void SelectNextDocument();

	DEPENDENCY_PROPERTY_(std::shared_ptr<System::Collection::List<std::shared_ptr<LayoutDocumentItem>>>, Documents)
	DEPENDENCY_PROPERTY_(std::shared_ptr<System::Collection::List<std::shared_ptr<LayoutAnchorableItem>>>, Anchorables)
	DEPENDENCY_PROPERTY_(std::shared_ptr<LayoutDocumentItem>, SelectedDocument)
	DEPENDENCY_PROPERTY_(std::shared_ptr<LayoutAnchorableItem>, SelectedAnchorable)

  protected:
    void OnLoaded(void *sender, System::Events::EventArg& e);

    void OnUnloaded(void *sender, Controls::LogicalTreeAttachmentEventArgs& e);

    /// <summary>
    /// Handles changes to the SelectedDocument property.
    /// </summary>
    static void OnSelectedDocumentChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

    /// <summary>
    /// Provides derived classes an opportunity to handle changes to the SelectedDocument property.
    /// </summary>
    virtual void OnSelectedDocumentChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

  	void InternalSetSelectedDocument(LayoutDocumentItem *documentToSelect);

    /// <summary>
    /// Handles changes to the SelectedAnchorable property.
    /// </summary>
    static void OnSelectedAnchorableChanged(DependencyObject *d, Core::Dependency::DependencyPropertyChangedEventArgs& e);

    /// <summary>
    /// Provides derived classes an opportunity to handle changes to the SelectedAnchorable property.
    /// </summary>
    virtual void OnSelectedAnchorableChanged(Core::Dependency::DependencyPropertyChangedEventArgs& e);

	void OnKeyDown(Interfaces::KeyEventArgs& e) override;

    void OnPreviewKeyDown(Interfaces::KeyEventArgs& e) override;

    void OnPreviewKeyUp(Interfaces::KeyEventArgs& e) override;


  };
}


#endif	//#ifndef __XUI_NAVIGATORWINDOW_H__
