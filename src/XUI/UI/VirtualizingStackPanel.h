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

#ifndef __XUI_VIRTUALIZINGSTACKPANEL_H__
#define __XUI_VIRTUALIZINGSTACKPANEL_H__

#include "UI/StackPanel.h"
#include "UI/Presenters/ItemVirtualizer.h"

namespace XUI::UI
{
    class TC_XUI_API VirtualizingStackPanel : public StackPanel
    {
    private:
        std::shared_ptr<Presenters::ItemVirtualizer> _controller;
        Core::Media::Size _availableSpace;
        double _takenSpace = 0;
        double _averageItemSize = 0;
        double _pixelOffset = 0;
        double _crossAxisOffset = 0;
        int _canBeRemoved = 0;
        int _averageCount = 0;
        bool _forceRemeasure = false;

    public:

        static Core::Dependency::RefDependencyProperty IsVirtualizingProperty;

        static void StaticClassInitializer();

        virtual ~VirtualizingStackPanel();

        PROPERTY_GET_ONLY(bool , IsFull);
        bool GetIsFull();

        PROPERTY_(std::shared_ptr<Presenters::ItemVirtualizer> , Controller);
        std::shared_ptr<Presenters::ItemVirtualizer> GetController();
        void SetController(std::shared_ptr<Presenters::ItemVirtualizer> const& value);

        PROPERTY_GET_ONLY(int , OverflowCount);
        int GetOverflowCount();

        PROPERTY_GET_ONLY(UI::Orientation  , ScrollDirection);
        UI::Orientation GetScrollDirection();

        PROPERTY_GET_ONLY(double , AverageItemSize);
        double GetAverageItemSize();
        
        PROPERTY_GET_ONLY(double , PixelOverflow);
        double GetPixelOverflow();
        
        PROPERTY_(double , PixelOffset);
        double GetPixelOffset();
        void SetPixelOffset(double value);
        
        PROPERTY_(double , CrossAxisOffset);
        double GetCrossAxisOffset();
        void SetCrossAxisOffset(double value);

        static bool GetIsVirtualizing(Controls::Control* control)
        {
            return control->GetValue<bool>(IsVirtualizingProperty.get());
        }

        static void SetIsVirtualizing(Controls::Control* control, bool value)
        {
            return control->SetValue(IsVirtualizingProperty.get(), value);
        }

        void ForceInvalidateMeasure();

        void ArrangeChild(Controls::Control *child, Core::Media::Rect const& rect, Core::Media::Size const& panelSize, UI::Orientation orientation) override;

    protected:
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

        void ChildrenChanged(void *sender, System::Collection::NotifyCollectionChangedEventArgs& e) override;

        virtual Controls::InputElement *GetControlInDirection(Presenters::NavigationDirection direction, Controls::Control *from) override;

    private:
        void UpdateAdd(Controls::Control *child);

        void UpdateRemove(Controls::Control *child);

        void AddToAverageItemSize(double value);

        void RemoveFromAverageItemSize(double value);
    };
}


#endif	//#ifndef __XUI_VIRTUALIZINGSTACKPANEL_H__
