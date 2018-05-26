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

#ifndef __XUI_UI_DATATEMPLATES_FUNCDATATEMPLATE_H__
#define __XUI_UI_DATATEMPLATES_FUNCDATATEMPLATE_H__

#include "UI/DataTemplates/DataTemplate.h"

namespace XUI::UI::DataTemplates
{
    /// <summary>
    /// Builds a control for a piece of data.
    /// </summary>
    //class
    //    Meta(ReflInclude("UI/Controls/Control.h"))
    //    FuncDataTemplate : public FuncTemplate<System::Reflection::meta::Variant const&, std::shared_ptr<UI::Controls::Control>>, public DataTemplate
    //{
    //    friend class System::Reflection::Assembly::AssemblyModulXUI;
    //
    //    /// <summary>
    //    /// The implementation of the <see cref="Match"/> method.
    //    /// </summary>
    //    Meta(Disable) std::function<bool(System::Reflection::meta::Variant const&)> _match;
    //    bool _supportsRecycling = false;
    //
    //public:
    //    /// <summary>
    //    /// The default data template used in the case where no matching data template is found.
    //    /// </summary>
    //    static std::shared_ptr<FuncDataTemplate> Default;
    //
    //    /// <summary>
    //    /// Initializes a new instance of the <see cref="FuncDataTemplate"/> class.
    //    /// </summary>
    //    /// <param name="type">The type of data which the data template matches.</param>
    //    /// <param name="build">
    //    /// A function which when passed an object of <paramref name="type"/> returns a control.
    //    /// </param>
    //    /// <param name="supportsRecycling">Whether the control can be recycled.</param>
    //    FuncDataTemplate(
    //        System::Reflection::meta::Type const& type,
    //        FuncHandler const& build,
    //        bool supportsRecycling = false);
    //
    //    /// <summary>
    //    /// Initializes a new instance of the <see cref="FuncDataTemplate"/> class.
    //    /// </summary>
    //    /// <param name="match">
    //    /// A function which determines whether the data template matches the specified data.
    //    /// </param>
    //    /// <param name="build">
    //    /// A function which returns a control for matching data.
    //    /// </param>
    //    /// <param name="supportsRecycling">Whether the control can be recycled.</param>
    //    FuncDataTemplate(
    //        std::function<bool(System::Reflection::meta::Variant const&)> match,
    //        FuncHandler const& build,
    //        bool supportsRecycling = false);
    //
    //    virtual ~FuncDataTemplate();
    //
    //    /// <summary>
    //    /// Checks to see if this data template matches the specified data.
    //    /// </summary>
    //    /// <param name="data">The data.</param>
    //    /// <returns>
    //    /// True if the data template can build a control for the data, otherwise false.
    //    /// </returns>
    //    bool Match(System::Reflection::meta::Variant const& data) override
    //    {
    //        return _match(data);
    //    }
    //
    //    /// <summary>
    //    /// Determines of an object is of the specified type.
    //    /// </summary>
    //    /// <param name="o">The object.</param>
    //    /// <param name="t">The type.</param>
    //    /// <returns>
    //    /// True if <paramref name="o"/> is of type <paramref name="t"/>, otherwise false.
    //    /// </returns>
    //    static bool IsInstance(System::Reflection::meta::Variant const& o, System::Reflection::meta::Type const& t)
    //    {
    //        return o.GetBase() && t.IsAssignableFrom(*o.GetType());
    //    }
    //
    //    // Inherited via DataTemplate
    //    virtual bool SupportsRecycling() override;
    //    virtual std::shared_ptr<UI::Controls::Control> Build(System::Reflection::meta::Variant const & param) override;
    //};
}

#endif // !__XUI_UI_DATATEMPLATES_FUNCDATATEMPLATE_H__
