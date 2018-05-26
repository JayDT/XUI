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

#ifndef __XUI_PANEL_H__
#define __XUI_PANEL_H__

#include "UI/Controls/Control.h"
#include "UI/Controls/NameScope.h"
#include "UI/Controls/WindowIcon.h"
#include "UI/DataGrid/DataGridDefines.h"

namespace XUI::UI
{
    /// <summary>
    /// Base class for controls that can contain multiple children.
    /// </summary>
    /// <remarks>
    /// Controls can be added to a <see cref="Panel"/> by adding them to its <see cref="Children"/>
    /// collection. All children are layed out to fill the panel.
    /// </remarks>
    class TC_XUI_API Panel : public Controls::Control
    {
    public:
        /// <summary>
        /// Defines the <see cref="Background"/> property.
        /// </summary>
        static std::shared_ptr<XUI::Core::Dependency::DependencyProperty> BackgroundProperty;

    private:
        Meta(Disable) mutable LogicalChildrens _children;

    public:

        static void StaticClassInitializer();
        /// <summary>
        /// Initializes a new instance of the <see cref="Panel"/> class.
        /// </summary>
        Panel();

        /// <summary>
        /// Gets or sets the children of the <see cref="Panel"/>.
        /// </summary>
        /// <remarks>
        /// Even though this property can be set, the setter is only intended for use in object
        /// initializers. Assigning to this property does not change the underlying collection,
        /// it simply clears the existing collection and adds the contents of the assigned
        /// collection.
        /// </remarks>
        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(LogicalChildrens&, Children);
        LogicalChildrens& GetChildren() { return _children; }
        void SetChildren(LogicalChildrens const& value);

        /// <summary>
        /// Gets or Sets Panel background brush.
        /// </summary>
        PROPERTY_(std::shared_ptr<XUI::Interfaces::IBrush>, Background);
        std::shared_ptr<XUI::Interfaces::IBrush> GetBackground() const;
        void SetBackground(std::shared_ptr<XUI::Interfaces::IBrush> value);

        /// <summary>
        /// Renders the visual to a <see cref="DrawingContext"/>.
        /// </summary>
        /// <param name="context">The drawing context.</param>
        void Render(Interfaces::IDrawingContextImpl* context) override;

        /// <summary>
        /// Called when the <see cref="Children"/> collection changes.
        /// </summary>
        /// <param name="sender">The event sender.</param>
        /// <param name="e">The event args.</param>
    protected:
        virtual void ChildrenChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e);
    };
}


#endif	//#ifndef __XUI_PANEL_H__
