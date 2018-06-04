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

#ifndef __XUI_THUMB_H__
#define __XUI_THUMB_H__

#include "UI/Controls/Control.h"
#include "Interfaces/InputEventArgs.h"

namespace XUI::UI
{
    class TC_XUI_API Thumb : public Controls::Control
    {
    public:
        static std::shared_ptr<Core::Observer::RoutedEvent> DragStartedEvent;

        static std::shared_ptr<Core::Observer::RoutedEvent> DragDeltaEvent;

        static std::shared_ptr<Core::Observer::RoutedEvent> DragCompletedEvent;

        static void StaticClassInitializer();

        Thumb();
        virtual ~Thumb();

        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&DragStartedEvent>       DragStarted;
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&DragDeltaEvent>         DragDelta;
        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&DragCompletedEvent>     DragCompleted;

    private:
        Core::Media::Point _lastPoint = Core::Media::Point(0, 0);
        bool _dragStarted = false;

    protected:
        virtual void OnDragStarted(Interfaces::VectorEventArgs& e);

        virtual void OnDragDelta(Interfaces::VectorEventArgs& e);

        virtual void OnDragCompleted(Interfaces::VectorEventArgs& e);

        void OnPointerMoved(Interfaces::PointerEventArgs& e) override;

        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void OnPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;
    };
}


#endif	//#ifndef __XUI_THUMB_H__
