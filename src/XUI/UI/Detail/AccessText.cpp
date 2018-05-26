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

#include "AccessText.h"
#include "Core/Media/Pen.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;
using namespace XUI::UI;

STATIC_CLASS_INITIALIZER(AccessText, XUI::UI::AccessText::StaticClassInitializer);

XUI::Core::Dependency::RefDependencyProperty AccessText::ShowAccessKeyProperty;

void XUI::UI::AccessText::StaticClassInitializer()
{
    ShowAccessKeyProperty = (Core::Dependency::BindProperty<bool>::RegisterAttached<AccessText, XUI::UI::Controls::Control>(nameof(ShowAccessKey), false, true));

    ShowAccessKeyProperty->Changed.get_observable().subscribe(AffectsVisualInvalidate);
}

AccessText::AccessText()
{
    AddDirectBind(TextProperty.get(), [this](const System::Reflection::Variant& value)
    {
        std::wstring text = value.GetValue<std::wstring>();
        TextChanged(text);
    });
}

char AccessText::GetAccessKey() const
{
    return m_AccessKey;
}

void AccessText::SetAccessKey(const char &value)
{
    m_AccessKey = value;
}

bool AccessText::GetShowAccessKey() const
{
    return GetValue<bool>(ShowAccessKeyProperty);
}

void AccessText::SetShowAccessKey(const bool &value)
{
    SetValue(ShowAccessKeyProperty, value);
}

void AccessText::Render(Interfaces::IDrawingContextImpl* context)
{
    TextBlock::Render(context);

    int underscore = Text.find(L'_');

    if (underscore != std::wstring::npos && ShowAccessKey)
    {
        auto rect = _formattedText.HitTestTextPosition(underscore);
        Core::Media::Vector offset(0, -0.5);
        Core::Media::Pen pen(Foreground.get(), 1);
        context->DrawLine(pen, rect.BottomLeft + offset, rect.BottomRight + offset);
    }
}

void AccessText::SetText(std::wstring const& value)
{
    TextBlock::SetText(StripAccessKey(value));
}

void AccessText::InitializeFormattedText() const
{
    TextBlock::InitializeFormattedText();
}

Core::Media::Size AccessText::MeasureOverride(Core::Media::Size const& availableSize)
{
    auto result = TextBlock::MeasureOverride(availableSize);
    return result.WithHeight(result.Height + 1);
}

void AccessText::OnAttachedToVisualTree(Controls::VisualTreeAttachmentEventArgs const& e)
{
    TextBlock::OnAttachedToVisualTree(e);
    //_accessKeys = e.Root->Is<Interfaces::InputRoot>()->AccessKeyHandler;
    //
    //if (_accessKeys != nullptr && AccessKey != 0)
    //{
    //    _accessKeys->Register(AccessKey, this);
    //}
}

void AccessText::OnDetachedFromVisualTree(Controls::VisualTreeAttachmentEventArgs const& e)
{
    TextBlock::OnDetachedFromVisualTree(e);

    //if (_accessKeys != nullptr && AccessKey != 0)
    //{
    //    _accessKeys->Unregister(this);
    //    _accessKeys = nullptr;
    //}
}

std::wstring AccessText::StripAccessKey(const std::wstring &text)
{
    auto position = (int)text.find(L'_');

    if (position == -1)
    {
        return text;
    }
    else
    {
        return text.substr(0, position) + text.substr(position + 1);
    }
}

void AccessText::TextChanged(const std::wstring &text)
{
    auto key = static_cast<char>(0);

    if (text != L"")
    {
        int underscore = (int)text.find('_');

        if (underscore != -1 && underscore < text.length() - 1)
        {
            key = text[underscore + 1];
        }
    }

    //AccessKey = key;
    //
    //if (_accessKeys != nullptr && AccessKey != 0)
    //{
    //    _accessKeys->Register(AccessKey, this);
    //}
}
