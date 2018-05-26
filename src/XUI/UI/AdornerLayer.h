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

#ifndef __XUI_ADORNERLAYER_H__
#define __XUI_ADORNERLAYER_H__

#include "UI/Panel.h"
#include "Core/Media/TileBrush.h"
#include "Interfaces/IBitmap.h"

namespace XUI::UI
{
    struct TC_XUI_API Adorner : Controls::UIElement
    {
        rxcpp::subscription Subscription;
        std::shared_ptr<Controls::UIElement> AdornedElement;

        virtual ~Adorner()
        {
            if (Subscription.is_subscribed())
                Subscription.unsubscribe();
        }
    };

    // TODO: Need to track position of adorned elements and move the adorner if they move.
    class TC_XUI_API AdornerLayer : public Panel
    {
    public:
        static Core::Dependency::RefDependencyProperty AdornedElementsProperty;

    public:
        virtual ~AdornerLayer()
        {
        }
    
        static void StaticConstructor();


    public:
        AdornerLayer();

        System::Collection::List<std::shared_ptr<XUI::UI::Adorner>> const& GetAdornedElements(std::shared_ptr<Visual> const& element);

        void Add(std::shared_ptr<Visual> const& adorner, std::shared_ptr<XUI::UI::Adorner> const& adorned);

        static AdornerLayer* GetAdornerLayer(std::shared_ptr<Controls::Visual> const& visual);

    protected:
        Core::Media::Size ArrangeOverride(Core::Media::Size const& arrangeSize) override;

    private:
        static void AdornedElementChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);

        void ChildrenCollectionChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs const& e);

        void UpdateAdornedElement(Visual *adorner, Visual *adorned);
    };
}


#endif	//#ifndef __XUI_ADORNERLAYER_H__
