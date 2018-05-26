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

#ifndef __XUI_POPUP_H__
#define __XUI_POPUP_H__

#include "UI/Controls/Control.h"
#include "UI/TopLevel.h"

namespace XUI::UI
{
    class PopupRoot;

    /// <summary>
    /// Defines the placement for a <see cref="Popup"/> control.
    /// </summary>
    enum class PlacementMode
    {
        /// <summary>
        /// The popup is placed at the pointer position.
        /// </summary>
        Pointer,

        /// <summary>
        /// The popup is placed at the bottom left of its target.
        /// </summary>
        Bottom,

        /// <summary>
        /// The popup is placed at the top right of its target.
        /// </summary>
        Right
    };

    /// <summary>
    /// Displays a popup window.
    /// </summary>
    class TC_XUI_API Popup : public Controls::Control//, public IVisualTreeHost
    {
    public:
        /// <summary>
        /// Defines the <see cref="Child"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ChildProperty;

        /// <summary>
        /// Defines the <see cref="IsOpen"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsOpenProperty;

        /// <summary>
        /// Defines the <see cref="PlacementMode"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty PlacementModeProperty;

        /// <summary>
        /// Defines the <see cref="HorizontalOffset"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty HorizontalOffsetProperty;

        /// <summary>
        /// Defines the <see cref="VerticalOffset"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty VerticalOffsetProperty;

        /// <summary>
        /// Defines the <see cref="PlacementTarget"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty PlacementTargetProperty;

        /// <summary>
        /// Defines the <see cref="StaysOpen"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty StaysOpenProperty;

        static XUI::Core::Dependency::RefDependencyProperty IsEmbedProperty;

    private:
        bool _isOpen = false;
        bool _ignoreIsOpenChanged = false;
        std::shared_ptr<XUI::UI::PopupRoot> _popupRoot;
        std::shared_ptr<TopLevel> _topLevel;
        rxcpp::subscription _nonClientListener;
        rxcpp::subscription _nonPointerPressSubcribe;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="Popup"/> class.
        /// </summary>
        static void StaticClassInitializer();

    public:
        /// <summary>
        /// Raised when the popup closes.
        /// </summary>
        System::Events::EventHandler<> Closed;

        /// <summary>
        /// Raised when the popup opens.
        /// </summary>
        System::Events::EventHandler<> Opened;

        /// <summary>
        /// Raised when the popup root has been created, but before it has been shown.
        /// </summary>
        System::Events::EventHandler<> PopupRootCreated;

        /// <summary>
        /// Gets or sets the control to display in the popup.
        /// </summary>
        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(std::shared_ptr<Controls::Control>, Child);
        std::shared_ptr<Controls::Control> GetChild() const;
        void SetChild(std::shared_ptr<Controls::Control> const& value);

        /// <summary>
        /// Gets or sets a dependency resolver for the <see cref="PopupRoot"/>.
        /// </summary>
        /// <remarks>
        /// This property allows a client to customize the behaviour of the popup by injecting
        /// a specialized dependency resolver into the <see cref="PopupRoot"/>'s constructor.
        /// </remarks>
        //IXUIDependencyResolver *DependencyResolver;

        /// <summary>
        /// Gets or sets a value indicating whether the popup is currently open.
        /// </summary>
        PROPERTY_(bool , IsOpen);
        bool GetIsOpen() const;
        void SetIsOpen(const bool &value);

        /// <summary>
        /// Gets or sets the placement mode of the popup in relation to the <see cref="PlacementTarget"/>.
        /// </summary>
        PROPERTY_(UI::PlacementMode , PlacementMode);
        UI::PlacementMode GetPlacementMode() const;
        void SetPlacementMode(UI::PlacementMode value);

        /// <summary>
        /// Gets or sets the Horizontal offset of the popup in relation to the <see cref="PlacementTarget"/>
        /// </summary>
        PROPERTY_(double , HorizontalOffset);
        double GetHorizontalOffset() const;
        void SetHorizontalOffset(const double &value);

        /// <summary>
        /// Gets or sets the Vertical offset of the popup in relation to the <see cref="PlacementTarget"/>
        /// </summary>
        PROPERTY_(double , VerticalOffset);
        double GetVerticalOffset() const;
        void SetVerticalOffset(const double &value);

        /// <summary>
        /// Gets or sets the control that is used to determine the popup's position.
        /// </summary>
        PROPERTY_(std::shared_ptr<Controls::Control> , PlacementTarget);
        std::shared_ptr<Controls::Control> GetPlacementTarget() const;
        void SetPlacementTarget(std::shared_ptr<Controls::Control> const& value);

        /// <summary>
        /// Gets the root of the popup window.
        /// </summary>
        PROPERTY_GET_ONLY(UI::PopupRoot * , PopupRoot);
        UI::PopupRoot *GetPopupRoot() const;

        /// <summary>
        /// Gets or sets a value indicating whether the popup should stay open when the popup is
        /// pressed or loses focus.
        /// </summary>
        PROPERTY_(bool , StaysOpen);
        bool GetStaysOpen() const;
        void SetStaysOpen(const bool value);
        void SetInternalStaysOpen(const bool value);

        PROPERTY_(bool, IsEmbed);
        bool GetIsEmbed() const;
        void SetIsEmbed(const bool value);

        /// <summary>
        /// Gets the root of the popup window.
        /// </summary>
        PROPERTY_GET_ONLY(Controls::Visual * , Root);
        Controls::Visual *GetRoot() const;

        /// <summary>
        /// Opens the popup.
        /// </summary>
        void Open();

        /// <summary>
        /// Closes the popup.
        /// </summary>
        void Close();

        /// <summary>
        /// Measures the control.
        /// </summary>
        /// <param name="availableSize">The available size for the control.</param>
        /// <returns>A size of 0,0 as Popup itself takes up no space.</returns>
    protected:
        Core::Media::Size MeasureCore(Core::Media::Size const& availableSize) override;

        /// <inheritdoc/>
        void OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e) override;

        /// <inheritdoc/>
        void OnDetachedFromLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e) override;

        void SetTemplatedParent(std::shared_ptr<Core::Dependency::DependencyObject> const& value) override;
    private:
        /// <summary>
        /// Called when the <see cref="IsOpen"/> property changes.
        /// </summary>
        /// <param name="e">The event args.</param>
        void IsOpenChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Called when the <see cref="Child"/> property changes.
        /// </summary>
        /// <param name="e">The event args.</param>
        void ChildChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        /// <summary>
        /// Gets the position for the popup based on the placement properties.
        /// </summary>
        /// <returns>The popup's position in screen coordinates.</returns>
    protected:
        virtual Core::Media::Point GetPosition();

    public:
        static Core::Media::Point GetPosition(Control *target, UI::PlacementMode placement, UI::PopupRoot *popupRoot, double horizontalOffset, double verticalOffset);

    private:
        void ListenForNonClientClick(Interfaces::RawInputEventArgs& e);

        void PointerPressedOutside(void* sender, Interfaces::PointerPressedEventArgs& e);

        void WindowDeactivated(void* sender, System::Events::EventArg& e);
    };
}


#endif	//#ifndef __XUI_POPUP_H__
