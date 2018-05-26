#ifndef __XUI_SELECTINGITEMSCONTROL_H__
#define __XUI_SELECTINGITEMSCONTROL_H__

#include "UI/Controls/ItemsControl.h"

namespace XUI::UI
{
    class TabControl;

    /// <summary>
    /// Defines the selection mode for a control which can select multiple items.
    /// </summary>
    enum class SelectionMode
    {
        /// <summary>
        /// One item can be selected.
        /// </summary>
        Single = 0x00,

        /// <summary>
        /// Multiple items can be selected.
        /// </summary>
        Multiple = 0x01,

        /// <summary>
        /// Item selection can be toggled by tapping/spacebar.
        /// </summary>
        Toggle = 0x02,

        /// <summary>
        /// An item will always be selected as long as there are items to select.
        /// </summary>
        AlwaysSelected = 0x04,

        Extended       = 0x08,
    };

    /// <summary>
    /// An <see cref="ItemsControl"/> that maintains a selection.
    /// </summary>
    /// <remarks>
    /// <para>
    /// <see cref="SelectingItemsControl"/> provides a base class for <see cref="ItemsControl"/>s
    /// that maintain a selection (single or multiple). By default only its 
    /// <see cref="SelectedIndex"/> and <see cref="SelectedItem"/> properties are visible; the
    /// current multiple selection <see cref="SelectedItems"/> together with the 
    /// <see cref="SelectionMode"/> properties are protected, however a derived  class can expose 
    /// these if it wishes to support multiple selection.
    /// </para>
    /// <para>
    /// <see cref="SelectingItemsControl"/> maintains a selection respecting the current 
    /// <see cref="SelectionMode"/> but it does not react to user input; this must be handled in a
    /// derived class. It does, however, respond to <see cref="IsSelectedChangedEvent"/> events
    /// from items and updates the selection accordingly.
    /// </para>
    /// </remarks>
    class TC_XUI_API SelectingItemsControl : public Controls::ItemsControl
    {
        friend class TabControl;

    public:

        /// <summary>
        /// Defines the <see cref="AutoScrollToSelectedItem"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty AutoScrollToSelectedItemProperty;

        /// <summary>
        /// Defines the <see cref="SelectedIndex"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty SelectedIndexProperty;

        /// <summary>
        /// Defines the <see cref="SelectedItem"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty SelectedItemProperty;

    protected:
        /// <summary>
        /// Defines the <see cref="SelectedItems"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty SelectedItemsProperty;

        /// <summary>
        /// Defines the <see cref="SelectionMode"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty SelectionModeProperty;

    public:
        /// <summary>
        /// Event that should be raised by items that implement <see cref="ISelectable"/> to
        /// notify the parent <see cref="SelectingItemsControl"/> that their selection state
        /// has changed.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> IsSelectedChangedEvent;

        /// <summary>
        /// Defines the <see cref="SelectionChanged"/> event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> SelectionChangedEvent;

    protected:

        int _selectedIndex = -1;
        System::Reflection::Variant _selectedItem;
        System::Collection::DynamicList _selectedItems;
        bool _ignoreContainerSelectionChanged = false;
        bool _syncingSelectedItems = false;
        int _updateCount = 0;
        int _updateSelectedIndex = 0;
        System::Collection::DynamicList _updateSelectedItems;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="SelectingItemsControl"/> class.
        /// </summary>
        static void StaticClassInitializer();

        SelectingItemsControl();

        /// <summary>
        /// Occurs when the control's selection changes.
        /// </summary>
        Core::Observer::SpecPropertyRoutedEventHandler<&IsSelectedChangedEvent> IsSelectedChanged;
        Core::Observer::SpecPropertyRoutedEventHandler<&SelectionChangedEvent> SelectionChanged;

    public:
        /// <summary>
        /// Gets or sets a value indicating whether to automatically scroll to newly selected items.
        /// </summary>
        PROPERTY_(bool , AutoScrollToSelectedItem);
        bool GetAutoScrollToSelectedItem();
        void SetAutoScrollToSelectedItem(bool value);

        /// <summary>
        /// Gets or sets the index of the selected item.
        /// </summary>
        PROPERTY_(int , SelectedIndex);
        int GetSelectedIndex() const;

        void SetSelectedIndex(int value);

        /// <summary>
        /// Gets or sets the selected item.
        /// </summary>
        PROPERTY_(System::Reflection::Variant, SelectedItem);
        System::Reflection::Variant GetSelectedItem();

        void SetSelectedItem(System::Reflection::Variant const& value);

        void UnSelectAll();

        /// <summary>
        /// Gets the selected items.
        /// </summary>
        PROPERTY_(System::Collection::DynamicList&, SelectedItems);
        System::Collection::DynamicList& GetSelectedItems();
        System::Collection::DynamicList const& GetSelectedItems() const;

    protected:

        void SetSelectedItems(System::Collection::DynamicList const& value);

        /// <summary>
        /// Gets or sets the selection mode.
        /// </summary>
        PROPERTY_(UI::SelectionMode , SelectionMode);
        UI::SelectionMode GetSelectionMode();
        void SetSelectionMode(UI::SelectionMode value);

        /// <summary>
        /// Gets a value indicating whether <see cref="SelectionMode.AlwaysSelected"/> is set.
        /// </summary>
        PROPERTY_GET_ONLY(bool , AlwaysSelected);
        bool GetAlwaysSelected();

    public:
        /// <inheritdoc/>
        void BeginInit() override;

        /// <inheritdoc/>
        void EndInit() override;

        /// <summary>
        /// Scrolls the specified item into view.
        /// </summary>
        /// <param name="item">The item.</param>
        void ScrollIntoView(System::Reflection::Variant const& item);

    protected:
        /// <summary>
        /// Tries to get the container that was the source of an event.
        /// </summary>
        /// <param name="eventSource">The control that raised the event.</param>
        /// <returns>The container or null if the event did not originate in a container.</returns>
        Controls::Control *GetContainerFromEventSource(Controls::InputElement *eventSource);

        /// <inheritdoc/>
        void ItemsChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e) override;

        /// <inheritdoc/>
        void ItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e) override;

        /// <inheritdoc/>
        void OnContainersMaterialized(Interfaces::ItemContainerEventArgs& e) override;

        /// <inheritdoc/>
        void OnContainersDematerialized(Interfaces::ItemContainerEventArgs& e) override;

        void OnContainersRecycled(Interfaces::ItemContainerEventArgs& e) override;

        /// <inheritdoc/>
        void OnDataContextChanging() override;

        /// <inheritdoc/>
        void OnDataContextChanged() override;

        /// <summary>
        /// Updates the selection for an item based on user interaction.
        /// </summary>
        /// <param name="index">The index of the item.</param>
        /// <param name="select">Whether the item should be selected or unselected.</param>
        /// <param name="rangeModifier">Whether the range modifier is enabled (i.e. shift key).</param>
        /// <param name="toggleModifier">Whether the toggle modifier is enabled (i.e. ctrl key).</param>
        void UpdateSelection(int index, bool select = true, bool rangeModifier = false, bool toggleModifier = false);

        /// <summary>
        /// Updates the selection for a container based on user interaction.
        /// </summary>
        /// <param name="container">The container.</param>
        /// <param name="select">Whether the container should be selected or unselected.</param>
        /// <param name="rangeModifier">Whether the range modifier is enabled (i.e. shift key).</param>
        /// <param name="toggleModifier">Whether the toggle modifier is enabled (i.e. ctrl key).</param>
        void UpdateSelection(Controls::Control *container, bool select = true, bool rangeModifier = false, bool toggleModifier = false);

        /// <summary>
        /// Updates the selection based on an event that may have originated in a container that 
        /// belongs to the control.
        /// </summary>
        /// <param name="eventSource">The control that raised the event.</param>
        /// <param name="select">Whether the container should be selected or unselected.</param>
        /// <param name="rangeModifier">Whether the range modifier is enabled (i.e. shift key).</param>
        /// <param name="toggleModifier">Whether the toggle modifier is enabled (i.e. ctrl key).</param>
        /// <returns>
        /// True if the event originated from a container that belongs to the control; otherwise
        /// false.
        /// </returns>
        bool UpdateSelectionFromEventSource(Controls::InputElement *eventSource, bool select = true, bool rangeModifier = false, bool toggleModifier = false);

        /// <summary>
        /// Called when the currently selected item is lost and the selection must be changed
        /// depending on the <see cref="SelectionMode"/> property.
        /// </summary>
        virtual void LostSelection();

    private:
        /// <summary>
        /// Gets a range of items from an IEnumerable.
        /// </summary>
        /// <param name="items">The items.</param>
        /// <param name="first">The index of the first item.</param>
        /// <param name="last">The index of the last item.</param>
        /// <returns>The items.</returns>
        static std::vector<System::Reflection::Variant> GetRange(System::Collection::Generic::IContainer* items, int first, int last);

        /// <summary>
        /// Makes a list of objects equal another.
        /// </summary>
        /// <param name="items">The items collection.</param>
        /// <param name="desired">The desired items.</param>
        static void SynchronizeItems(System::Collection::Generic::IContainer* items, std::vector<System::Reflection::Variant> &desired);

        /// <summary>
        /// Called when a container raises the <see cref="IsSelectedChangedEvent"/>.
        /// </summary>
        /// <param name="e">The event.</param>
        void ContainerSelectionChanged(Core::Observer::RoutedEventArgs& e);

        /// <summary>
        /// Sets a container's 'selected' class or <see cref="ISelectable.IsSelected"/>.
        /// </summary>
        /// <param name="container">The container.</param>
        /// <param name="selected">Whether the control is selected</param>
        /// <returns>The previous selection state.</returns>
        bool MarkContainerSelected(Controls::Control *container, bool selected);

        /// <summary>
        /// Sets an item container's 'selected' class or <see cref="ISelectable.IsSelected"/>.
        /// </summary>
        /// <param name="index">The index of the item.</param>
        /// <param name="selected">Whether the item should be selected or deselected.</param>
        void MarkItemSelected(int index, bool selected);

        /// <summary>
        /// Sets an item container's 'selected' class or <see cref="ISelectable.IsSelected"/>.
        /// </summary>
        /// <param name="item">The item.</param>
        /// <param name="selected">Whether the item should be selected or deselected.</param>
        void MarkItemSelected(System::Reflection::Variant const& item, bool selected);

        /// <summary>
        /// Called when the <see cref="SelectedItems"/> CollectionChanged event is raised.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
        void SelectedItemsCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e);

        /// <summary>
        /// Called when items are added to the <see cref="SelectedItems"/> collection.
        /// </summary>
        /// <param name="items">The added items.</param>
        void SelectedItemsAdded(std::vector<System::Reflection::Variant> const& items);

        /// <summary>
        /// Subscribes to the <see cref="SelectedItems"/> CollectionChanged event, if any.
        /// </summary>
        void SubscribeToSelectedItems();

        /// <summary>
        /// Unsubscribes from the <see cref="SelectedItems"/> CollectionChanged event, if any.
        /// </summary>
        void UnsubscribeFromSelectedItems();

        void UpdateFinished();
    };
}


#endif	//#ifndef __XUI_SELECTINGITEMSCONTROL_H__
