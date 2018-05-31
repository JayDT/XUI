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

#include "TextPresenter.h"
#include "Core/Media/MediaTypes.h"
#include "Core/Media/Colors.h"
#include "Core/Media/Pen.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;
using namespace XUI::UI::Presenters;

STATIC_CLASS_INITIALIZER(TextPresenter, XUI::UI::Presenters::TextPresenter::StaticClassInitializer)

Core::Dependency::RefDependencyProperty Presenters::TextPresenter::CaretIndexProperty;
Core::Dependency::RefDependencyProperty Presenters::TextPresenter::SelectionStartProperty;
Core::Dependency::RefDependencyProperty Presenters::TextPresenter::SelectionEndProperty;
Core::Dependency::RefDependencyProperty Presenters::TextPresenter::HighlightBrushProperty;

void XUI::UI::Presenters::TextPresenter::StaticClassInitializer()
{
    CaretIndexProperty = (Core::Dependency::DirectProperty<XUI::UI::Presenters::TextPresenter, int>::RegisterDirect(nameof(CaretIndex),
        [](XUI::UI::Presenters::TextPresenter* x) { return x->CaretIndex; },
        [](XUI::UI::Presenters::TextPresenter* x, int v) { x->CaretIndex = v; })
        );

    SelectionStartProperty = (Core::Dependency::DirectProperty<XUI::UI::Presenters::TextPresenter, int>::RegisterDirect(nameof(SelectionStart),
        [](XUI::UI::Presenters::TextPresenter* x) { return x->SelectionStart; },
        [](XUI::UI::Presenters::TextPresenter* x, int v) { x->SelectionStart = v; })
        );

    SelectionEndProperty = (Core::Dependency::DirectProperty<XUI::UI::Presenters::TextPresenter, int>::RegisterDirect(nameof(SelectionEnd),
        [](XUI::UI::Presenters::TextPresenter* x) { return x->SelectionEnd; },
        [](XUI::UI::Presenters::TextPresenter* x, int v) { x->SelectionEnd = v; })
        );

    HighlightBrushProperty = (Core::Dependency::DirectProperty<XUI::UI::Presenters::TextPresenter, std::shared_ptr<Interfaces::IBrush>>::RegisterDirect(nameof(HighlightBrush),
        [](XUI::UI::Presenters::TextPresenter* x) { return x->HighlightBrush; },
        [](XUI::UI::Presenters::TextPresenter* x, std::shared_ptr<Interfaces::IBrush> const& v) { x->HighlightBrush = v; })
        );
}


TextPresenter::TextPresenter()
    : _caretTimer()
{
    _caretTimer.Interval = 500;
    _caretTimer.Tick += System::make_eventhandler(&TextPresenter::CaretTimerTick, this);

    _caretBrush = std::make_shared<Core::Media::SolidColorBrush>(Core::Media::Colors::GetWhite());
    _highlightBrush = std::make_shared<Core::Media::SolidColorBrush>(Core::Media::Colors::GetDarkBlue());

    AddDirectBind(CaretIndexProperty.get(), [this](System::Reflection::Variant const& variant)
    {
        auto value = variant.GetValue<int>();
        CaretIndexChanged(value);
    });

    //auto backgroundColor = (dynamic_cast<SolidColorBrush*>((static_cast<Control*>(TemplatedParent))->GetValue(BackgroundProperty))) == nullptr ? nullptr : (dynamic_cast<SolidColorBrush*>((static_cast<Control*>(TemplatedParent))->GetValue(BackgroundProperty)))->Color;
    //auto caretBrush = Brushes::Black;
    //
    //if (backgroundColor.HasValue)
    //{
    //    unsigned char red = static_cast<unsigned char>(~(backgroundColor.Value->R));
    //    unsigned char green = static_cast<unsigned char>(~(backgroundColor.Value->G));
    //    unsigned char blue = static_cast<unsigned char>(~(backgroundColor.Value->B));
    //
    //    caretBrush = new SolidColorBrush(Color::FromRgb(red, green, blue));
    //}

    //Observable::Merge(this->GetObservable(SelectionStartProperty), this->GetObservable(SelectionEndProperty))->Subscribe([&](void *_)
    //{
    //    InvalidateFormattedText();
    //});
}

int TextPresenter::GetCaretIndex() const
{
    return _caretIndex;
}

void TextPresenter::SetCaretIndex(const int &value)
{
    int _value = CoerceCaretIndex(value);
    SetAndRaise(CaretIndexProperty, _caretIndex, _value);
}

int TextPresenter::GetSelectionStart() const
{
    return _selectionStart;
}

void TextPresenter::SetSelectionStart(const int &value)
{
    int _value = CoerceCaretIndex(value);
    SetAndRaise(SelectionStartProperty, _selectionStart, _value);
}

int TextPresenter::GetSelectionEnd() const
{
    return _selectionEnd;
}

void TextPresenter::SetSelectionEnd(const int &value)
{
    int _value = CoerceCaretIndex(value);
    SetAndRaise(SelectionEndProperty, _selectionEnd, _value);
}

int TextPresenter::GetCaretIndex(Core::Media::Point const& point)
{
    auto hit = FormattedText.HitTestPoint(point);
    return hit.TextPosition + (hit.IsTrailing ? 1 : 0);
}

void TextPresenter::SetHighlightBrush(std::shared_ptr<Interfaces::IBrush> const& value)
{
    SetAndRaise(HighlightBrushProperty, _highlightBrush, value);
}

void TextPresenter::Render(Interfaces::IDrawingContextImpl *context)
{
    auto selectionStart = SelectionStart;
    auto selectionEnd = SelectionEnd;

    if (selectionStart != selectionEnd && _highlightBrush != nullptr)
    {
        auto start = std::min(selectionStart, selectionEnd);
        auto length = std::max(selectionStart, selectionEnd) - start;

        // issue #600: set constaint before any FormattedText manipulation
        //             see base.Render(...) implementation
        FormattedText.Constraint = Bounds.GetSize();

        auto rects = FormattedText.HitTestTextRange(start, length);

        for (auto rect : rects)
            context->FillRectangle(_highlightBrush.get(), rect);
    }

    TextBlock::Render(context);

    if (selectionStart == selectionEnd)
    {
        if (_caretBlink)
        {
            auto charPos = FormattedText.HitTestTextPosition(CaretIndex);
            auto x = std::floor(charPos.x0()) + 0.5;
            auto y = std::floor(std::max(1.0f, charPos.y0())) + 0.5;
            auto b = std::ceil(charPos.Bottom) - 0.5;
    
            Core::Media::Pen pen(_caretBrush.get(), 1);
            context->DrawLine(pen, Core::Media::Point(x, b), Core::Media::Point(x, y));
        }
    }
}

void TextPresenter::ShowCaret()
{
    _caretBlink = true;
    _caretTimer.Start();
    InvalidateVisual();
}

void TextPresenter::HideCaret()
{
    _caretBlink = false;
    _caretTimer.Stop();
    InvalidateVisual();
}

void TextPresenter::CaretIndexChanged(int caretIndex)
{
    if (this->GetVisualParent() != nullptr)
    {
        if (_caretTimer.IsEnabled)
        {
            _caretBlink = true;
            _caretTimer.Stop();
            _caretTimer.Start();
            InvalidateVisual();
        }

        if (IsMeasureValid)
        {
            auto rect = FormattedText.HitTestTextPosition(caretIndex);
            BringIntoView(rect);
        }
        else
        {
            // The measure is currently invalid so there's no point trying to bring the 
            // current char into view until a measure has been carried out as the scroll
            // viewer extents may not be up-to-date.
            XUI::Core::UIDispatcher::OnRender += [this, caretIndex]()
            {
                auto rect = FormattedText.HitTestTextPosition(caretIndex);
                this->BringIntoView(rect);
            };
        }
    }
}

Core::Media::Size TextPresenter::MeasureOverride(Core::Media::Size const& availableSize)
{
    auto const& text = Text;

    if (!text.empty())
    {
        return TextBlock::MeasureOverride(availableSize);
    }
    else
    {
        // Min Caret Bounds
        return Core::Media::Size(5, FontSize);
    }
}

int TextPresenter::CoerceCaretIndex(int value)
{
    auto text = Text;
    auto length = text.empty() ? size_t(0) : text.size();
    return std::max(size_t(0), std::min(length, size_t(value)));
}

void TextPresenter::CaretTimerTick(void *sender, System::Events::EventArg& e)
{
    _caretBlink = !_caretBlink;
    InvalidateVisual();
}
