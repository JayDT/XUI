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

#ifndef __XUI_UI_DATATEMPLATES_DATATEMPLATE_H__
#define __XUI_UI_DATATEMPLATES_DATATEMPLATE_H__

#include "standard/misc.h"
#include "standard/Errors.h"
#include "UI/DataTemplates/FrameworkTemplate.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/ContentControl.h"))
MetaAssemblyEnd

namespace XUI::UI::Controls
{
    class ContentControl;
}

namespace XUI::UI::DataTemplates
{
    struct TC_XUI_API DataContentTemplate : public ContentTemplate
    {
        std::shared_ptr<XamlCPP::Interfaces::IDeferredLoader> xamlTemplate;

        DataContentTemplate(std::shared_ptr<XamlCPP::Interfaces::IDeferredLoader> const& _template)
            : xamlTemplate(_template)
        { }

        virtual ~DataContentTemplate() {}

        // Inherited via ContentTemplate
        virtual bool SupportsRecycling() override;
        virtual System::Reflection::RefObject Build(System::Reflection::meta::Variant const & param) override;
    };

    struct
#ifdef __REFLECTION_PARSER__
        Meta(XamlCPP::MapKeyPropertyAttribute("DataTypeName"))
#endif
        TC_XUI_API DataTemplate : public FrameworkTemplate
    {
        DataTemplate() {}

        virtual ~DataTemplate() {}

        PROPERTY_QT(const System::Reflection::Type*, DataType)
        const System::Reflection::Type* GetDataType() const { return m_DataType; }
        void SetDataType(const System::Reflection::Type* value) { m_DataType = value; }

        PROPERTY_QT(System::Reflection::Variant, DataTemplateKey)
        const System::Reflection::Variant GetDataTemplateKey() const { return m_DataTemplateKey; }
        void SetDataTemplateKey(const System::Reflection::Variant& value) { m_DataTemplateKey = value; }

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

        PROPERTY_GET_ONLY(std::string, DataTypeName);
        std::string GetDataTypeName() const
        {
            std::string typeString = "{Type: ";
            if (m_DataType)
                typeString += m_DataType->GetFullName();
            else
                typeString += "unset";
            typeString += "}.DataTemplate";
            return typeString;
        }

        // Inherited via FrameworkTemplate
        virtual std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate> CreateContentTemplate() override;

        // Inherited via FrameworkTemplate
        virtual System::Reflection::meta::RefObject Build(XamlCPP::Interfaces::IInstanceLifeCycleListener*);

        virtual void Attach(XUI::UI::Controls::Control *control, Core::Dependency::DependencyObject* DataTemplateRoot);
        virtual void Detach(XUI::UI::Controls::Control *control);

    private:

        std::unordered_map<XUI::UI::Controls::UIElement*, std::vector<rxcpp::subscription>> _applied;

        std::vector<rxcpp::subscription>& GetSubscriptions(XUI::UI::Controls::UIElement *control);
        System::Collection::Vector<std::shared_ptr<XamlCPP::Interfaces::ITriggerBase>> m_triggers;
    };
}
#endif // !__XUI_UI_DATATEMPLATES_DATATEMPLATE_H__

