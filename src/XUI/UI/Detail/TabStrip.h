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

#ifndef __XUI_TABSTRIP_H__
#define __XUI_TABSTRIP_H__

#include "UI\Detail\SelectingItemsControl.h"

namespace XUI::UI
{
    class TC_XUI_API TabStrip : public SelectingItemsControl
    {
    protected:
        static std::shared_ptr<XamlCPP::Interfaces::ITemplate> DefaultPanel;

        static std::function<System::Reflection::Variant(System::Reflection::Variant const&)> MemberSelector;

    public:
        static void StaticClassInitializer();

    protected:
        virtual std::shared_ptr<Generators::IItemContainerGenerator> CreateItemContainerGenerator() override;

        /// <inheritdoc/>
        void OnGotFocus(Interfaces::GotFocusEventArgs& e) override;

        /// <inheritdoc/>
        void OnPointerPressed(Interfaces::PointerPressedEventArgs& e) override;

        void ItemsChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e) override;

    private:
        static System::Reflection::Variant SelectHeader(System::Reflection::Variant o);
    };
}


#endif	//#ifndef __XUI_TABSTRIP_H__
