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

#ifndef __XUI_STYLE_H__
#define __XUI_STYLE_H__

#include "XamlCPP/Interfaces/INamedScope.h"
#include "XamlCPP/Interfaces/ISetter.h"
#include "XamlCPP/Interfaces/IStyle.h"
#include "XamlCPP/Interfaces/IStyleable.h"
#include "XamlCPP/Interfaces/IResourceMap.h"
#include "XamlCPP/Interfaces/ITriggerBase.h"
#include "Styling/Setter.h"

#include <unordered_map>

#undef FindResource

namespace XUI::Styling
{
    /// <summary>
    /// Defines a style.
    /// </summary>
    class
        TC_XUI_API
#ifdef __REFLECTION_PARSER__
        Meta(XamlCPP::MapKeyPropertyAttribute("TargetTypeName"))
#endif
        Style : public XamlCPP::Interfaces::IStyle
    {
    private:
        std::unordered_map<XamlCPP::Interfaces::IStyleable*, std::vector<rxcpp::subscription>> _applied;

        const System::Reflection::Type* _targetType = nullptr;
        XamlCPP::Interfaces::ResourceMap _resources;
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ISetter>> _setters;
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerBase>> _triggers;

    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="Style"/> class.
        /// </summary>
        Style()
        {
        }

        Style(const System::Reflection::Type* targetType)
        {
        }

        Style(const System::Reflection::Type* targetType, Style* basedOn)
        {
        }

        virtual ~Style()
        {

        }

        PROPERTY_AUTO_QT(Style*, BasedOn)
        PROPERTY_(const System::Reflection::Type*, TargetType);
        const System::Reflection::Type* const& GetTargetType() const
        {
            return _targetType;
        }
        void SetTargetType(const System::Reflection::Type* value)
        {
            _targetType = value;
        }

        PROPERTY_GET_ONLY(std::string, TargetTypeName);
        std::string GetTargetTypeName() const
        {
            std::string typeString = "{Type: ";
            if (_targetType)
                typeString += _targetType->GetFullName();
            else
                typeString += "unset";
            typeString += "}";
            return typeString;
        }

        /// <summary>
        /// Gets or sets a dictionary of style resources.
        /// </summary>
        PROPERTY_(XamlCPP::Interfaces::ResourceMap, Resources);
        XamlCPP::Interfaces::ResourceMap const& GetResources() const
        {
            return _resources;
        }
        void SetResources(const XamlCPP::Interfaces::ResourceMap &value)
        {
            for (auto i : value.GetNativeEnumerator())
            {
                _resources.Add(i.first, i.second);
            }
        }

        PROPERTY_(System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerBase>> const&, Triggers);
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerBase>> const& GetTriggers() const
        {
            return _triggers;
        }
        void SetTriggers(const System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerBase>> &value)
        {
            for (auto const& i : value.GetNativeEnumerator())
            {
                _triggers.Add(i);
            }
        }

        /// <summary>
        /// Gets or sets style's setters.
        /// </summary>
        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ISetter>> const&, Setters);
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ISetter>> const& GetSetters()
        {
            return _setters;
        }

        void SetSetters(const System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ISetter>> &value)
        {
            for (auto const& v : value.GetNativeEnumerator())
                _setters.Add(v);
        }

        /// <summary>
        /// Attaches the style to a control if the style's selector matches.
        /// </summary>
        /// <param name="control">The control to attach to.</param>
        /// <param name="container">
        /// The control that contains this style. May be null.
        /// </param>
        virtual void Attach(XamlCPP::Interfaces::IStyleable *control, XamlCPP::Interfaces::IStyleHost *container) override;
        virtual void Detach(XamlCPP::Interfaces::IStyleable *control);

        /// <summary>
        /// Tries to find a named resource within the style.
        /// </summary>
        /// <param name="name">The resource name.</param>
        /// <returns>
        /// The resource if found, otherwise <see cref="AvaloniaProperty.UnsetValue"/>.
        /// </returns>
        virtual System::Reflection::Variant const& FindResource(const std::string &name) const override;

        /// <summary>
        /// Returns a string representation of the style.
        /// </summary>
        /// <returns>A string representation of the style.</returns>
        std::string ToString() const override
        {
            return "Style";
        }

    private:
        std::vector<rxcpp::subscription>& GetSubscriptions(XamlCPP::Interfaces::IStyleable *control);
        
        /// <summary>
        /// Called when a control's <see cref="IStyleable.StyleDetach"/> is signalled to remove
        /// all applied styles.
        /// </summary>
        /// <param name="control">The control.</param>
        void ControlDetach(XamlCPP::Interfaces::IStyleable *control);
    };
}


#endif	//#ifndef __XUI_STYLE_H__
