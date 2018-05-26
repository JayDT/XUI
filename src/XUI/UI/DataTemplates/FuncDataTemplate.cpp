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

#include "StdAfx.h"
#include "FuncDataTemplate.h"
#include "TextBlock.h"

//std::shared_ptr<XUI::UI::DataTemplates::FuncDataTemplate> XUI::UI::DataTemplates::FuncDataTemplate::Default = std::make_shared<XUI::UI::DataTemplates::FuncDataTemplate>(
//    typeof(System::Reflection::meta::Object),
//    [](System::Reflection::meta::Variant const& data)
//    {
//        //if (data)
//        //{
//        //    auto result = std::make_shared<XUI::UI::TextBlock>();
//        //    //result.GetObservable(Control.DataContextProperty).Select(x = > x ? .ToString())
//        //    
//        //    result->Bind(
//        //        XUI::UI::TextBlock::TextProperty.get(),
//        //        result->GetObservable(XUI::UI::Controls::Control::DataContextProperty.get()));
//        //    return result;
//        //}
//        //else
//        {
//            return std::shared_ptr<XUI::UI::TextBlock>(nullptr);
//        }
//    }, true);
//
///// <summary>
///// Initializes a new instance of the <see cref="FuncDataTemplate"/> class.
///// </summary>
///// <param name="type">The type of data which the data template matches.</param>
///// <param name="build">
///// A function which when passed an object of <paramref name="type"/> returns a control.
///// </param>
///// <param name="supportsRecycling">Whether the control can be recycled.</param>
//XUI::UI::DataTemplates::FuncDataTemplate::FuncDataTemplate(
//    System::Reflection::meta::Type const& type,
//    FuncHandler const& build,
//    bool supportsRecycling)
//    : FuncDataTemplate([_type = &type](System::Reflection::meta::Variant const& o) -> bool
//        {
//            return IsInstance(o, *_type);
//        }, build, supportsRecycling)
//{
//}
//
///// <summary>
///// Initializes a new instance of the <see cref="FuncDataTemplate"/> class.
///// </summary>
///// <param name="match">
///// A function which determines whether the data template matches the specified data.
///// </param>
///// <param name="build">
///// A function which returns a control for matching data.
///// </param>
///// <param name="supportsRecycling">Whether the control can be recycled.</param>
//XUI::UI::DataTemplates::FuncDataTemplate::FuncDataTemplate(std::function<bool(System::Reflection::meta::Variant const&)> match, FuncHandler const & build, bool supportsRecycling)
//    : FuncTemplate<System::Reflection::meta::Variant const&, std::shared_ptr<UI::Controls::Control>>(build)
//{
//    System::ThrowIfFailed<System::ArgumentNullException>(match != nullptr);
//
//    _match = match;
//    _supportsRecycling = supportsRecycling;
//}
//
//XUI::UI::DataTemplates::FuncDataTemplate::~FuncDataTemplate()
//{
//}
//
//bool XUI::UI::DataTemplates::FuncDataTemplate::SupportsRecycling()
//{
//    return _supportsRecycling;
//}
//
//std::shared_ptr<XUI::UI::Controls::Control> XUI::UI::DataTemplates::FuncDataTemplate::Build(System::Reflection::meta::Variant const & param)
//{
//    return FuncTemplate<System::Reflection::meta::Variant const&, std::shared_ptr<UI::Controls::Control>>::Build(param);
//}
