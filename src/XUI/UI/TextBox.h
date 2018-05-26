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

#ifndef __XUI_TEXTBOX_H__
#define __XUI_TEXTBOX_H__

#include "UI/Controls/Control.h"
#include "UI/Detail/UndoRedoHelper.h"
#include "Interfaces/IFormatedText.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Presenters/TextPresenter.h"))
MetaAssemblyEnd

namespace XUI::UI
{
    namespace Presenters
    {
        class TextPresenter;
    }

    class TC_XUI_API UndoRedoState
    {
    public:
        std::wstring Text;
        int CaretPosition = 0;

        UndoRedoState(const std::wstring &text, int caretPosition);

        bool operator==(UndoRedoState const& other) const;
    };

    class TC_XUI_API TextBox : public Controls::Control, public Detail::UndoRedoHelper<XUI::UI::UndoRedoState>::IUndoRedoHost
    {
    public:
        static Core::Dependency::RefDependencyProperty AcceptsReturnProperty;

        static Core::Dependency::RefDependencyProperty AcceptsTabProperty;

        static Core::Dependency::RefDependencyProperty CanScrollHorizontallyProperty;

        static Core::Dependency::RefDependencyProperty CaretIndexProperty;

        static Core::Dependency::RefDependencyProperty DataValidationErrorsProperty;

        static Core::Dependency::RefDependencyProperty IsReadOnlyProperty;

        static Core::Dependency::RefDependencyProperty SelectionStartProperty;

        static Core::Dependency::RefDependencyProperty SelectionEndProperty;

        static Core::Dependency::RefDependencyProperty TextProperty;

        static Core::Dependency::RefDependencyProperty TextAlignmentProperty;

        static Core::Dependency::RefDependencyProperty TextWrappingProperty;

        static Core::Dependency::RefDependencyProperty WatermarkProperty;

        static Core::Dependency::RefDependencyProperty UseFloatingWatermarkProperty;

    private:
        std::wstring _text;
        int _caretIndex = 0;
        int _selectionStart = 0;
        int _selectionEnd = 0;
        bool _skipUndoRedoHostStates = false;
        bool _canScrollHorizontally = false;
        bool _ignoreTextChanges = false;
        std::shared_ptr<Presenters::TextPresenter> _presenter = nullptr;
        Detail::UndoRedoHelper<XUI::UI::UndoRedoState> _undoRedoHelper;
        std::vector<std::shared_ptr<std::exception>> _dataValidationErrors;
        static std::vector<std::wstring> const invalidCharacters;

    public:
        TextBox();
        virtual ~TextBox();

        static void StaticClassInitializer();

        PROPERTY_(bool, AcceptsReturn);
        bool GetAcceptsReturn() const;
        void SetAcceptsReturn(const bool &value);

        PROPERTY_(bool, AcceptsTab);
        bool GetAcceptsTab() const;
        void SetAcceptsTab(const bool &value);

        PROPERTY_(bool, CanScrollHorizontally);
        bool GetCanScrollHorizontally() const;
    private:
        void SetCanScrollHorizontally(const bool &value);

    public:
        PROPERTY_(int, CaretIndex);
        int GetCaretIndex() const;
        void SetCaretIndex(const int &value);

        PROPERTY_(std::vector<std::shared_ptr<std::exception>>, DataValidationErrors);
        std::vector<std::shared_ptr<std::exception>> GetDataValidationErrors() const;
    private:
        void SetDataValidationErrors(const std::vector<std::shared_ptr<std::exception>> &value);

    public:
        PROPERTY_(bool, IsReadOnly);
        bool GetIsReadOnly() const;
        void SetIsReadOnly(const bool &value);

        PROPERTY_(int, SelectionStart);
        int GetSelectionStart() const;

        void SetSelectionStart(const int &value);

        PROPERTY_(int, SelectionEnd);
        int GetSelectionEnd() const;

        void SetSelectionEnd(const int &value);

        PROPERTY_(std::wstring, Text);
        std::wstring const& GetText() const;
        void SetText(const std::wstring &value);

        PROPERTY_(Core::Media::TextAlignment, TextAlignment);
        Core::Media::TextAlignment GetTextAlignment() const;
        void SetTextAlignment(Core::Media::TextAlignment value);

        PROPERTY_(std::wstring, Watermark);
        std::wstring GetWatermark() const;
        void SetWatermark(const std::wstring &value);

        PROPERTY_(bool, UseFloatingWatermark);
        bool GetUseFloatingWatermark() const;
        void SetUseFloatingWatermark(const bool &value);

        PROPERTY_(Core::Media::TextWrapping, TextWrapping);
        Core::Media::TextWrapping GetTextWrapping() const;
        void SetTextWrapping(Core::Media::TextWrapping value);

    protected:
        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

        void OnGotFocus(Interfaces::GotFocusEventArgs& e) override;

        void OnLostFocus(Core::Observer::RoutedEventArgs& e) override;

        void OnTextInput(Interfaces::TextInputEventArgs& e) override;

    private:
        void HandleTextInput(const std::wstring &input);

    public:
        std::wstring RemoveInvalidCharacters(const std::wstring &text);

        void Select(int start, int length);

    private:
        void Copy();

        void Paste();

    protected:
        void OnKeyDown(Interfaces::KeyEventArgs& e) override;

        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void OnPointerMoved(Interfaces::PointerEventArgs& e) override;

        void OnPointerReleased(Interfaces::PointerReleasedEventArgs& e) override;

        void UpdateDataValidation(XamlCPP::Core::Dependency::IPropertyObject* _property,
            XamlCPP::DataBind::BindingNotification* status) override;

    private:
        static std::vector<std::shared_ptr<std::exception>> UnpackException(const std::exception* exception);

        int CoerceCaretIndex(int value);

        int CoerceCaretIndex(int value, int length);

        int DeleteCharacter(int index);

        void MoveHorizontal(int direction, Interfaces::InputModifiers modifiers);

        void MoveVertical(int count, Interfaces::InputModifiers modifiers);

        void MoveHome(Interfaces::InputModifiers modifiers);

        void MoveEnd(Interfaces::InputModifiers modifiers);

    public:

        void SelectAll();

        bool DeleteSelection();

        std::wstring GetSelection();

        int GetLine(int caretIndex, const std::vector<Interfaces::FormattedTextLine> &lines);
        int GetCharacterIndexFromLineIndex(int lineIndex);
        int GetCharacterIndexFromPoint(Core::Media::Point const& point, bool snapToText);


        void SetTextInternal(const std::wstring &value);

        void SetSelectionForControlBackspace(Interfaces::InputModifiers modifiers);

        void SetSelectionForControlDelete(Interfaces::InputModifiers modifiers);

    public:
        XUI::UI::UndoRedoState GetUndoRedoState() const override;
        void SetUndoRedoState(const XUI::UI::UndoRedoState &value) override;
    };
}


#endif	//#ifndef __XUI_TEXTBOX_H__
