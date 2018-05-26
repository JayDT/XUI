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

#include "TextBox.h"
#include "Core/Input/Cursors.h"
#include "UI/Presenters/TextPresenter.h"
#include "Core/GlobalInitializerScheduler.h"
#include "XamlCPP/DataBind/BindingNotification.h"
#include "Platform/UIService.h"

#include "Interfaces/IClipboard.h"

#include <System/rtString.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(TextBox, XUI::UI::TextBox::StaticClassInitializer);

Core::Dependency::RefDependencyProperty XUI::UI::TextBox::AcceptsReturnProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::AcceptsTabProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::CanScrollHorizontallyProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::CaretIndexProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::DataValidationErrorsProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::IsReadOnlyProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::SelectionStartProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::SelectionEndProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::TextProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::TextAlignmentProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::TextWrappingProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::WatermarkProperty;
Core::Dependency::RefDependencyProperty XUI::UI::TextBox::UseFloatingWatermarkProperty;

std::vector<std::wstring> const TextBox::invalidCharacters = { L"\u007f" };

void XUI::UI::TextBox::StaticClassInitializer()
{
    AcceptsReturnProperty = (Core::Dependency::BindProperty<bool>::Register<TextBox>(nameof(AcceptsReturn)));
    AcceptsTabProperty = (Core::Dependency::BindProperty<bool>::Register<TextBox>(nameof(AcceptsTab)));
    CanScrollHorizontallyProperty = (Core::Dependency::DirectProperty<TextBox, bool>::RegisterDirect(nameof(CanScrollHorizontally),
        [](XUI::UI::TextBox* x) { return x->CanScrollHorizontally; },
        [](XUI::UI::TextBox* x, bool const& v) { x->CanScrollHorizontally = v; })
        );
    CaretIndexProperty = (Core::Dependency::DirectProperty<TextBox, int>::RegisterDirect(nameof(CaretIndex),
        [](XUI::UI::TextBox* x) { return x->CaretIndex; },
        [](XUI::UI::TextBox* x, int const& v) { x->CaretIndex = v; })
        );
    DataValidationErrorsProperty = (Core::Dependency::DirectProperty<TextBox, std::vector<std::shared_ptr<std::exception>>>::RegisterDirect(nameof(DataValidationErrors),
        [](XUI::UI::TextBox* x) { return x->DataValidationErrors; },
        [](XUI::UI::TextBox* x, std::vector<std::shared_ptr<std::exception>> const& v) { x->DataValidationErrors = v; })
        );
    IsReadOnlyProperty = (Core::Dependency::BindProperty<bool>::Register<TextBox>(nameof(IsReadOnly)));
    SelectionStartProperty = (Core::Dependency::DirectProperty<TextBox, int>::RegisterDirect(nameof(SelectionStart),
        [](XUI::UI::TextBox* x) { return x->SelectionStart; },
        [](XUI::UI::TextBox* x, int const& v) { x->SelectionStart = v; })
        );
    SelectionEndProperty = (Core::Dependency::DirectProperty<TextBox, int>::RegisterDirect(nameof(SelectionEnd),
        [](XUI::UI::TextBox* x) { return x->SelectionEnd; },
        [](XUI::UI::TextBox* x, int const& v) { x->SelectionEnd = v; })
        );
    TextProperty = (Core::Dependency::DirectProperty<TextBox, std::wstring>::RegisterDirect(nameof(Text),
        [](XUI::UI::TextBox* x) { return x->Text; },
        [](XUI::UI::TextBox* x, std::wstring const& v) { x->Text = v; })
        );

    TextAlignmentProperty = (Core::Dependency::BindProperty<Core::Media::TextAlignment>::Register<TextBox>(nameof(TextAlignment)));
    TextWrappingProperty = (Core::Dependency::BindProperty<Core::Media::TextWrapping>::Register<TextBox>(nameof(TextWrapping)));
    WatermarkProperty = (Core::Dependency::BindProperty<std::wstring>::Register<TextBox>(nameof(Watermark)));
    UseFloatingWatermarkProperty = (Core::Dependency::BindProperty<bool>::Register<TextBox>(nameof(UseFloatingWatermark)));

    FocusableProperty->OverrideDefaultValue<TextBox>(true);
}

TextBox::TextBox()
    : _undoRedoHelper(this)
{
    AddDirectBind(TextWrappingProperty.get(), [this](System::Reflection::Variant const& variant)
    {
        auto value = variant.GetValue<Core::Media::TextWrapping>();
        CanScrollHorizontally = value == Core::Media::TextWrapping::NoWrap;
    });

    //AddDirectBind(AcceptsReturnProperty.get(), [this](System::Reflection::Variant const& variant)
    //{
    //    auto value = variant.GetValue<Core::Media::ScrollBarVisibility>();
    //    CanScrollHorizontally = value == Core::Media::ScrollBarVisibility::;
    //});

    //auto horizontalScrollBarVisibility = this->GetObservable(AcceptsReturnProperty)->Select([&](void *x)
    //{
    //    x ? ScrollBarVisibility::Auto : ScrollBarVisibility::Hidden;
    //});
    //
    //Bind(ScrollViewer::HorizontalScrollBarVisibilityProperty, horizontalScrollBarVisibility, BindingPriority::Style);
    //_undoRedoHelper = new UndoRedoHelper<Avalonia::Controls::TextBox::UndoRedoState>(this);
}

TextBox::~TextBox()
{
}

UndoRedoState::UndoRedoState(const std::wstring &text, int caretPosition)
{
    Text = text;
    CaretPosition = caretPosition;
}

bool UndoRedoState::operator==(UndoRedoState const& other) const
{
    return Text == other.Text;
}

bool TextBox::GetAcceptsReturn() const
{
    return GetValue<bool>(AcceptsReturnProperty);
}

void TextBox::SetAcceptsReturn(const bool &value)
{
    SetValue(AcceptsReturnProperty, value);
}

bool TextBox::GetAcceptsTab() const
{
    return GetValue<bool>(AcceptsTabProperty);
}

void TextBox::SetAcceptsTab(const bool &value)
{
    SetValue(AcceptsTabProperty, value);
}

bool TextBox::GetCanScrollHorizontally() const
{
    return _canScrollHorizontally;
}

void TextBox::SetCanScrollHorizontally(const bool &value)
{
    SetAndRaise(CanScrollHorizontallyProperty, _canScrollHorizontally, value);
}

int TextBox::GetCaretIndex() const
{
    return _caretIndex;
}

void TextBox::SetCaretIndex(const int &value)
{
    auto _value = CoerceCaretIndex(value);
    SetAndRaise(CaretIndexProperty, _caretIndex, _value);
    if (!_skipUndoRedoHostStates)
    {
        UndoRedoState* state = nullptr;
        if (_undoRedoHelper.TryGetLastState(state) && (!state || state->Text == Text))
            _undoRedoHelper.UpdateCurrentState();
    }
}

std::vector<std::shared_ptr<std::exception>> TextBox::GetDataValidationErrors() const
{
    return _dataValidationErrors;
}

void TextBox::SetDataValidationErrors(const std::vector<std::shared_ptr<std::exception>> &value)
{
    SetAndRaise(DataValidationErrorsProperty, _dataValidationErrors, value);
}

bool TextBox::GetIsReadOnly() const
{
    return GetValue<bool>(IsReadOnlyProperty);
}

void TextBox::SetIsReadOnly(const bool &value)
{
    SetValue(IsReadOnlyProperty, value);
}

int TextBox::GetSelectionStart() const
{
    return _selectionStart;
}

void TextBox::SetSelectionStart(const int &value)
{
    auto _value = CoerceCaretIndex(value);
    SetAndRaise(SelectionStartProperty, _selectionStart, _value);
}

int TextBox::GetSelectionEnd() const
{
    return _selectionEnd;
}

void TextBox::SetSelectionEnd(const int &value)
{
    auto _value = CoerceCaretIndex(value);
    SetAndRaise(SelectionEndProperty, _selectionEnd, _value);
}

std::wstring const& TextBox::GetText() const
{
    return _text;
}

void TextBox::SetText(const std::wstring &value)
{
    if (!_ignoreTextChanges)
    {
        CaretIndex = CoerceCaretIndex(CaretIndex, value.size());
        SetAndRaise(TextProperty, _text, value);
    }
}

Core::Media::TextAlignment TextBox::GetTextAlignment() const
{
    return GetValue<Core::Media::TextAlignment>(TextAlignmentProperty);
}

void TextBox::SetTextAlignment(Core::Media::TextAlignment value)
{
    SetValue(TextAlignmentProperty, value);
}

std::wstring TextBox::GetWatermark() const
{
    return GetValue<std::wstring>(WatermarkProperty);
}

void TextBox::SetWatermark(const std::wstring &value)
{
    SetValue(WatermarkProperty, value);
}

bool TextBox::GetUseFloatingWatermark() const
{
    return GetValue<bool>(UseFloatingWatermarkProperty);
}

void TextBox::SetUseFloatingWatermark(const bool &value)
{
    SetValue(UseFloatingWatermarkProperty, value);
}

Core::Media::TextWrapping TextBox::GetTextWrapping() const
{
    return GetValue<Core::Media::TextWrapping>(TextWrappingProperty);
}

void TextBox::SetTextWrapping(Core::Media::TextWrapping value)
{
    SetValue(TextWrappingProperty, value);
}

void TextBox::OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e)
{
    auto object = Template->FindName("PART_TextPresenter", this);
    if (!object)
        throw System::KeyNotFoundException("Not found presenter");

    auto pPresenter = object->Static_As<Presenters::TextPresenter>();
    if (!pPresenter)
        throw System::KeyNotFoundException("Not found presenter");

    _presenter = pPresenter->shared_from_base_static<Presenters::TextPresenter>();
    _presenter->Cursor = std::make_shared<Core::Input::Cursor>(Core::Input::StandardCursorType::Ibeam);

    //if (_presenter->Parent != this)
    //{
    //    if (_presenter->Parent != nullptr)
    //        _presenter->Parent->RemoveLogicalChild(_presenter.get());
    //    AddLogicalChild(_presenter.get());
    //}

    if (IsFocused)
    {
        _presenter->ShowCaret();
    }
}

void TextBox::OnGotFocus(Interfaces::GotFocusEventArgs& e)
{
    Controls::Control::OnGotFocus(e);

    // when navigating to a textbox via the tab key, select all text if
    //   1) this textbox is *not* a multiline textbox
    //   2) this textbox has any text to select
    if (e.NavigationMethod == Interfaces::NavigationMethod::Tab && !AcceptsReturn && !Text.empty())
    {
        SelectionStart = 0;
        SelectionEnd = Text.length();
    }
    else if (_presenter)
    {
        _presenter->ShowCaret();
    }
}

void TextBox::OnLostFocus(Core::Observer::RoutedEventArgs& e)
{
    Controls::Control::OnLostFocus(e);
    SelectionStart = 0;
    SelectionEnd = 0;
    if (_presenter)
        _presenter->HideCaret();
}

void TextBox::OnTextInput(Interfaces::TextInputEventArgs& e)
{
    HandleTextInput(e.Text);
}

void TextBox::HandleTextInput(const std::wstring &input)
{
    if (!IsReadOnly)
    {
        auto _input = RemoveInvalidCharacters(input);
        std::wstring const& text = GetText();
        int caretIndex = CaretIndex;
        if (!input.empty())
        {
            _undoRedoHelper.DiscardRedo();
            _undoRedoHelper.Snapshot();
            DeleteSelection();
            caretIndex = CaretIndex;
            SetTextInternal(text.substr(0, caretIndex) + input + text.substr(caretIndex));
            CaretIndex += input.length();
            SelectionEnd = CaretIndex;
            SelectionStart = SelectionEnd;
        }
    }
}

std::wstring TextBox::RemoveInvalidCharacters(const std::wstring &text)
{
    std::wstring replaced = text;
    for (size_t i = 0; i < invalidCharacters.size(); ++i)
    {
        replaced = boost::replace_all_copy(replaced, invalidCharacters[i], "");
    }

    return replaced;
}

void XUI::UI::TextBox::Select(int start, int length)
{
    if (_text.empty())
        return;

    SelectionStart = std::min(start, (int)_text.size());
    SelectionEnd = std::min(length, (int)_text.size());
}

void TextBox::Copy()
{
    Interfaces::IClipboard* clipboard = Platform::UIService::Instance()->Get<Interfaces::IClipboard>();
    if (clipboard)
        clipboard->SetText(System::String::FromWString(GetSelection()));
}

void TextBox::Paste()
{
    Interfaces::IClipboard* clipboard = Platform::UIService::Instance()->Get<Interfaces::IClipboard>();
    if (clipboard)
    {
        std::wstring text = System::String::ToWString(clipboard->GetText());
        if (!text.empty())
        {
            _undoRedoHelper.Snapshot();
            HandleTextInput(text);
        }
    }
}

void TextBox::OnKeyDown(Interfaces::KeyEventArgs& e)
{
    std::wstring const& text = GetText();
    int caretIndex = CaretIndex;
    bool movement = false;
    bool handled = true;
    auto modifiers = e.Modifiers;

    switch (e.Key)
    {
        case Interfaces::Key::A:
            if (modifiers == Interfaces::InputModifiers::Control)
            {
                SelectAll();
            }

            break;
        case Interfaces::Key::C:
            if (modifiers == Interfaces::InputModifiers::Control)
            {
                Copy();
            }
            break;

        case Interfaces::Key::X:
            if (modifiers == Interfaces::InputModifiers::Control)
            {
                Copy();
                DeleteSelection();
            }
            break;

        case Interfaces::Key::V:
            if (modifiers == Interfaces::InputModifiers::Control)
            {
                Paste();
            }

            break;

        case Interfaces::Key::Z:
            if (modifiers == Interfaces::InputModifiers::Control)
            {
                _undoRedoHelper.Undo();
            }

            break;
        case Interfaces::Key::Y:
            if (modifiers == Interfaces::InputModifiers::Control)
            {
                _undoRedoHelper.Redo();
            }

            break;
        case Interfaces::Key::Left:
            MoveHorizontal(-1, modifiers);
            movement = true;
            break;

        case Interfaces::Key::Right:
            MoveHorizontal(1, modifiers);
            movement = true;
            break;

        case Interfaces::Key::Up:
            MoveVertical(-1, modifiers);
            movement = true;
            break;

        case Interfaces::Key::Down:
            MoveVertical(1, modifiers);
            movement = true;
            break;

        case Interfaces::Key::Home:
            MoveHome(modifiers);
            movement = true;
            break;

        case Interfaces::Key::End:
            MoveEnd(modifiers);
            movement = true;
            break;

        case Interfaces::Key::Back:
            if (modifiers == Interfaces::InputModifiers::Control && SelectionStart == SelectionEnd)
            {
                SetSelectionForControlBackspace(modifiers);
            }

            if (!DeleteSelection() && CaretIndex > 0)
            {
                auto removedCharacters = 1;
                // handle deleting /r/n
                // you don't ever want to leave a dangling /r around. So, if deleting /n, check to see if 
                // a /r should also be deleted.
                if (CaretIndex > 1 && text[CaretIndex - 1] == '\n' && text[CaretIndex - 2] == '\r')
                {
                    removedCharacters = 2;
                }

                SetTextInternal(text.substr(0, caretIndex - removedCharacters) + text.substr(caretIndex));
                CaretIndex -= removedCharacters;
                SelectionEnd = CaretIndex;
                SelectionStart = SelectionEnd;
            }

            break;

        case Interfaces::Key::Delete:
            if (modifiers == Interfaces::InputModifiers::Control && SelectionStart == SelectionEnd)
            {
                SetSelectionForControlDelete(modifiers);
            }

            if (!DeleteSelection() && caretIndex < text.length())
            {
                auto removedCharacters = 1;
                // handle deleting /r/n
                // you don't ever want to leave a dangling /r around. So, if deleting /n, check to see if 
                // a /r should also be deleted.
                if (CaretIndex < text.length() - 1 && text[caretIndex + 1] == '\n' && text[caretIndex] == '\r')
                {
                    removedCharacters = 2;
                }

                SetTextInternal(text.substr(0, caretIndex) + text.substr(caretIndex + removedCharacters));
            }

            break;

        case Interfaces::Key::Enter:
            if (AcceptsReturn)
            {
                HandleTextInput(L"\r\n");
            }
            else
            {
                Control::OnKeyDown(e);
                handled = false;
            }

            break;

        case Interfaces::Key::Tab:
            if (AcceptsTab)
            {
                HandleTextInput(L"\t");
            }
            else
            {
                Control::OnKeyDown(e);
                handled = false;
            }

            break;

        default:
            handled = false;
            break;
    }

    if (movement && ((modifiers & Interfaces::InputModifiers::Shift) != 0))
    {
        SelectionEnd = CaretIndex;
    }
    else if (movement)
    {
        SelectionEnd = CaretIndex;
        SelectionStart = SelectionEnd;
    }

    if (handled)
        e.Handled = true;
}

void TextBox::OnPointerPressed(Interfaces::PointerPressedEventArgs& e)
{
    if (e.Source == _presenter.get())
    {
        auto point = e.GetPosition(_presenter.get());
        int index = _presenter->GetCaretIndex(point);
        CaretIndex = index;
        auto text = Text;

        if (!text.empty())
        {
            switch (e.ClickCount)
            {
                case 1:
                    SelectionEnd = index;
                    SelectionStart = SelectionEnd;
                    break;
                case 2:
                    if (!System::String::IsStartOfWord(text, index))
                    {
                        SelectionStart = System::String::PreviousWord(text, index);
                    }

                    SelectionEnd = System::String::NextWord(text, index);
                    break;
                case 3:
                    SelectionStart = 0;
                    SelectionEnd = text.length();
                    break;
            }
        }

        e.Device->Capture(_presenter.get());
        e.Handled = true;
    }
}

void TextBox::OnPointerMoved(Interfaces::PointerEventArgs& e)
{
    if (_presenter != nullptr && e.Device->Captured == _presenter.get())
    {
        auto point = e.GetPosition(_presenter.get());
        SelectionEnd = _presenter->GetCaretIndex(point);
        CaretIndex = SelectionEnd;
    }
}

void TextBox::OnPointerReleased(Interfaces::PointerReleasedEventArgs& e)
{
    if (_presenter != nullptr && e.Device->Captured == _presenter.get())
    {
        e.Device->Capture(nullptr);
    }
}

void TextBox::UpdateDataValidation(XamlCPP::Core::Dependency::IPropertyObject* _property,
    XamlCPP::DataBind::BindingNotification* status)
{
    if (_property == TextProperty.get())
    {
        //auto& classes = PseudoClasses;
        DataValidationErrors = UnpackException(status->Error);
        //if (DataValidationErrors.size() > 0)
        //    classes.insert(":error");
        //else
        //    classes.erase(":error");
    }
}

std::vector<std::shared_ptr<std::exception>> TextBox::UnpackException(const std::exception* exception)
{
    //if (exception != nullptr)
    //{
    //    auto aggregate = dynamic_cast<const System::AggregateException*>(exception);
    //    return std::vector<std::shared_ptr<std::exception>>();
    //}

    return std::vector<std::shared_ptr<std::exception>>();
}

int TextBox::CoerceCaretIndex(int value)
{
    return CoerceCaretIndex(value, GetText().size());
}

int TextBox::CoerceCaretIndex(int value, int length)
{
    auto text = Text;

    if (value < 0)
    {
        return 0;
    }
    else if (value > length)
    {
        return length;
    }
    else if (value > 0 && text[value - 1] == '\r' && text[value] == '\n')
    {
        return value + 1;
    }
    else
    {
        return value;
    }
}

int TextBox::DeleteCharacter(int index)
{
    auto start = index + 1;
    auto text = Text;
    auto c = text[index];
    auto result = 1;

    if (c == '\n' && index > 0 && text[index - 1] == '\r')
    {
        --index;
        ++result;
    }
    else if (c == '\r' && index < text.length() - 1 && text[index + 1] == '\n')
    {
        ++start;
        ++result;
    }

    Text = text.substr(0, index) + text.substr(start);

    return result;
}

void TextBox::MoveHorizontal(int direction, Interfaces::InputModifiers modifiers)
{
    auto const& text = _text;
    auto caretIndex = CaretIndex;

    if ((modifiers & Interfaces::InputModifiers::Control) == 0)
    {
        auto index = caretIndex + direction;

        if (index < 0 || index > text.length())
        {
            return;
        }
        else if (index == text.length())
        {
            CaretIndex = index;
            return;
        }

        auto c = text[index];

        if (direction > 0)
        {
            CaretIndex += (c == '\r' && index < text.length() - 1 && text[index + 1] == '\n') ? 2 : 1;
        }
        else
        {
            CaretIndex -= (c == '\n' && index > 0 && text[index - 1] == '\r') ? 2 : 1;
        }
    }
    else
    {
        if (direction > 0)
        {
            CaretIndex += System::String::NextWord(text, caretIndex) - caretIndex;
        }
        else
        {
            CaretIndex += System::String::PreviousWord(text, caretIndex) - caretIndex;
        }
    }
}

void TextBox::MoveVertical(int count, Interfaces::InputModifiers modifiers)
{
    auto& formattedText = _presenter->FormattedText;
    auto& lines = formattedText.GetLines();
    auto caretIndex = CaretIndex;
    auto lineIndex = std::min(GetLine(caretIndex, lines) + count, (int)lines.size() - 1);

    if (lineIndex >= 0 && lineIndex < lines.size())
    {
        auto const& line = lines[lineIndex];
        auto rect = formattedText.HitTestTextPosition(caretIndex);
        auto y = count < 0 ? rect.y0() : rect.Bottom;
        auto point = Core::Media::Point(rect.x0(), y + (count * (line.Height / 2)));
        auto hit = formattedText.HitTestPoint(point);
        CaretIndex = hit.TextPosition + (hit.IsTrailing ? 1 : 0);
    }
}

void TextBox::MoveHome(Interfaces::InputModifiers modifiers)
{
    auto const& text = _text;
    auto caretIndex = CaretIndex;

    if ((modifiers & Interfaces::InputModifiers::Control) != 0)
    {
        caretIndex = 0;
    }
    else
    {
        auto& lines = _presenter->FormattedText.GetLines();
        auto pos = 0;

        for (auto const& line : lines)
        {
            if (pos + line.Length > caretIndex || pos + line.Length == text.length())
            {
                break;
            }

            pos += line.Length;
        }

        caretIndex = pos;
    }

    CaretIndex = caretIndex;
}

void TextBox::MoveEnd(Interfaces::InputModifiers modifiers)
{
    auto const& text = _text;
    auto caretIndex = CaretIndex;

    if ((modifiers & Interfaces::InputModifiers::Control) != 0)
    {
        caretIndex = text.length();
    }
    else
    {
        auto& lines = _presenter->FormattedText.GetLines();
        auto pos = 0;

        for (auto const& line : lines)
        {
            pos += line.Length;

            if (pos > caretIndex)
            {
                if (pos < text.length())
                {
                    --pos;
                    if (pos > 0 && text[pos - 1] == L'\r' && text[pos] == L'\n')
                    {
                        --pos;
                    }
                }

                break;
            }
        }

        caretIndex = pos;
    }

    CaretIndex = caretIndex;
}

void TextBox::SelectAll()
{
    SelectionStart = 0;
    SelectionEnd = _text.size();
}

bool TextBox::DeleteSelection()
{
    if (!IsReadOnly)
    {
        auto selectionStart = SelectionStart;
        auto selectionEnd = SelectionEnd;

        if (selectionStart != selectionEnd)
        {
            auto start = std::min(selectionStart, selectionEnd);
            auto end = std::max(selectionStart, selectionEnd);
            auto const& text = _text;
            SetTextInternal(text.substr(0, start) + text.substr(end));
            CaretIndex = start;
            SelectionEnd = CaretIndex;
            SelectionStart = SelectionEnd;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return true;
    }
}

std::wstring TextBox::GetSelection()
{
    auto& text = Text;
    if (text.empty())
    {
        return L"";
    }
    auto selectionStart = SelectionStart;
    auto selectionEnd = SelectionEnd;
    auto start = std::min(selectionStart, selectionEnd);
    auto end = std::max(selectionStart, selectionEnd);
    if (start == end || Text.size() < end)
    {
        return L"";
    }
    return text.substr(start, end - start);
}

int TextBox::GetLine(int caretIndex, const std::vector<Interfaces::FormattedTextLine> &lines)
{
    int pos = 0;
    int i;

    for (i = 0; i < lines.size(); ++i)
    {
        auto line = lines[i];
        pos += line.Length;

        if (pos > caretIndex)
        {
            break;
        }
    }

    return i;
}

int XUI::UI::TextBox::GetCharacterIndexFromLineIndex(int lineIndex)
{
    if (_presenter)
    {
        auto& lines = _presenter->FormattedText.GetLines();

        if (lineIndex >= 0 && lineIndex < lines.size())
        {
            auto const& line = lines[lineIndex];
            return line.Start;
        }
    }
    return 0;
}

int XUI::UI::TextBox::GetCharacterIndexFromPoint(Core::Media::Point const & point, bool snapToText)
{
    if (_presenter)
        return _presenter->GetCaretIndex(point);
    return -1;
}

void TextBox::SetTextInternal(const std::wstring &value)
{
    try
    {
        _ignoreTextChanges = true;
        SetAndRaise(TextProperty, _text, value);
    }
    catch(...)
    {

    }

    _ignoreTextChanges = false;
}

void TextBox::SetSelectionForControlBackspace(Interfaces::InputModifiers modifiers)
{
    SelectionStart = CaretIndex;
    MoveHorizontal(-1, modifiers);
    SelectionEnd = CaretIndex;
}

void TextBox::SetSelectionForControlDelete(Interfaces::InputModifiers modifiers)
{
    SelectionStart = CaretIndex;
    MoveHorizontal(1, modifiers);
    SelectionEnd = CaretIndex;
}


XUI::UI::UndoRedoState TextBox::GetUndoRedoState() const
{
    return UndoRedoState(Text, CaretIndex);
}

void TextBox::SetUndoRedoState(const XUI::UI::UndoRedoState &value)
{
    _skipUndoRedoHostStates = true;
    Text = value.Text;
    CaretIndex = value.CaretPosition;
    SelectionEnd = CaretIndex;
    SelectionStart = SelectionEnd;
    _skipUndoRedoHostStates = false;
}
