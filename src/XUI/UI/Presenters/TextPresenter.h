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

#ifndef __XUI_TEXTPRESENTER_H__
#define __XUI_TEXTPRESENTER_H__

#include "UI/TextBlock.h"
#include "Core/DispatcherTimer.h"

namespace XUI::UI::Presenters
{
    class TC_XUI_API TextPresenter : public TextBlock
    {
    public:
        static Core::Dependency::RefDependencyProperty CaretIndexProperty;

        static Core::Dependency::RefDependencyProperty SelectionStartProperty;

        static Core::Dependency::RefDependencyProperty SelectionEndProperty;

        static Core::Dependency::RefDependencyProperty HighlightBrushProperty;

    private:
        Meta(Disable) Core::DispatcherTimer _caretTimer;
        int _caretIndex = 0;
        int _selectionStart = 0;
        int _selectionEnd = 0;
        bool _caretBlink = false;
        std::shared_ptr<Interfaces::IBrush> _highlightBrush;
        std::shared_ptr<Interfaces::IBrush> _caretBrush;

    public:

        static void StaticClassInitializer();

        virtual ~TextPresenter()
        {
        }

        TextPresenter();

        PROPERTY_(int, CaretIndex);
        int GetCaretIndex() const;
        void SetCaretIndex(const int &value);

        PROPERTY_(int, SelectionStart);
        int GetSelectionStart() const;
        void SetSelectionStart(const int &value);

        PROPERTY_(int, SelectionEnd);
        int GetSelectionEnd() const;
        void SetSelectionEnd(const int &value);

        PROPERTY_(std::shared_ptr<Interfaces::IBrush>, HighlightBrush);
        std::shared_ptr<Interfaces::IBrush> const& GetHighlightBrush() const { return _highlightBrush; }
        void SetHighlightBrush(std::shared_ptr<Interfaces::IBrush> const& value);

        int GetCaretIndex(Core::Media::Point const& point);
        void Render(Interfaces::IDrawingContextImpl *context) override;
        void ShowCaret();
        void HideCaret();
        void CaretIndexChanged(int caretIndex);

    protected:

        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

    private:
        int CoerceCaretIndex(int value);

        void CaretTimerTick(void *sender, System::Events::EventArg& e);
    };
}


#endif	//#ifndef __XUI_TEXTPRESENTER_H__
