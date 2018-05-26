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

#ifndef __XUI_UI_DATATEMPLATES_CONTROLTEMPLATE_H__
#define __XUI_UI_DATATEMPLATES_CONTROLTEMPLATE_H__

#include "standard/misc.h"
#include "standard/Errors.h"
#include "UI/DataTemplates/FrameworkTemplate.h"
#include "UI/Controls/NameScope.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/Control.h"))
MetaAssemblyEnd

namespace XamlCPP::Interfaces
{
    struct IInstanceLifeCycleListener;
}

namespace XUI::UI::Controls
{
    class Control;
}

namespace XUI::UI::DataTemplates
{
    struct TC_XUI_API ControlContentTemplate : public ContentTemplate
    {
        std::shared_ptr<XamlCPP::Interfaces::IDeferredLoader> xamlTemplate;

        ControlContentTemplate(std::shared_ptr<XamlCPP::Interfaces::IDeferredLoader> const& _template)
            : xamlTemplate(_template)
        { }

        virtual ~ControlContentTemplate() {}

        // Inherited via ContentTemplate
        virtual bool SupportsRecycling() override;
        virtual System::Reflection::RefObject Build(System::Reflection::meta::Variant const & param) override;
    };

    struct
        TC_XUI_API
#ifdef __REFLECTION_PARSER__
        Meta(XamlCPP::MapKeyPropertyAttribute("TargetTypeName"))
#endif
        ControlTemplate : FrameworkTemplate
    {
        ControlTemplate() {}

        virtual ~ControlTemplate() {}

        PROPERTY_QT(const System::Reflection::Type*, TargetType)
        const System::Reflection::Type* GetTargetType() const { return m_TargetType; }
        void SetTargetType(const System::Reflection::Type* value) { m_TargetType = value; }

        PROPERTY_(System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerBase>> const&, Triggers);
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerBase>> const& GetTriggers() const
        {
            return m_triggers;
        }
        void SetTriggers(const System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerBase>> &value)
        {
            for (auto const& i : value.GetNativeEnumerator())
            {
                m_triggers.Add(i);
            }
        }

        PROPERTY_GET_ONLY(std::string, TargetTypeName);
        std::string GetTargetTypeName() const
        {
            std::string typeString = "{Type: ";
            if (m_TargetType)
                typeString += m_TargetType->GetFullName();
            else
                typeString += "unset";
            typeString += "}.ControlTemplate";
            return typeString;
        }

        // Inherited via FrameworkTemplate
        virtual std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate> CreateContentTemplate() override;

        // Inherited via FrameworkTemplate
        virtual System::Reflection::meta::RefObject Build(XamlCPP::Interfaces::IInstanceLifeCycleListener*);

        virtual void Attach(XUI::UI::Controls::Control *control, std::list<UI::Controls::Visual*> const& visuals);
        virtual void Detach(XUI::UI::Controls::Control *control);

    private:

        std::unordered_map<XUI::UI::Controls::UIElement*, std::vector<rxcpp::subscription>> _applied;

        std::vector<rxcpp::subscription>& GetSubscriptions(XUI::UI::Controls::UIElement *control);
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerBase>> m_triggers;
    };
}
#endif // !__XUI_UI_DATATEMPLATES_CONTROLTEMPLATE_H__

