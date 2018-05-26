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
#include "ControlTemplate.h"
#include "UI/Controls/Control.h"

#include "XamlCPP/XamlCPP.h"
#include "XamlCPP/Interfaces/IInstanceLifeCycleListener.h"
#include "XamlCPP/Core/Assembler/Settings.h"

bool XUI::UI::DataTemplates::ControlContentTemplate::SupportsRecycling()
{
    return false;
}

System::Reflection::RefObject XUI::UI::DataTemplates::ControlContentTemplate::Build(System::Reflection::meta::Variant const & param)
{
    return xamlTemplate->Load(param.GetValue<XamlCPP::Interfaces::IInstanceLifeCycleListener*>());
}

std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate> XUI::UI::DataTemplates::ControlTemplate::CreateContentTemplate()
{
    return std::make_shared<XUI::UI::DataTemplates::ControlContentTemplate>(XamlTemplate);
}

System::Reflection::meta::RefObject XUI::UI::DataTemplates::ControlTemplate::Build(XamlCPP::Interfaces::IInstanceLifeCycleListener* lifecycle)
{
    System::Reflection::RefObject object = ContentTemplate->Build(lifecycle);
    if (object)
    {
        auto templateControl = object->Static_As<UI::Controls::Control>();
        if (templateControl)
            templateControl->_selfTemplateOwner = shared_from_base_static<DataTemplates::FrameworkTemplate>();
    }
    return object;
}

void XUI::UI::DataTemplates::ControlTemplate::Attach(XUI::UI::Controls::Control * _control, std::list<UI::Controls::Visual*> const& visuals)
{
    if (_control && _control->m_controlTemplateRoot)
    {
        auto templateControl = _control->m_controlTemplateRoot->Static_As<UI::Controls::Control>();
        if (templateControl)
        {
            auto& namedScope = _namedScopes[_control];
            if (!namedScope)
                namedScope = std::make_shared<UI::Controls::NameScope>();
            templateControl->_nameScope = namedScope;

            for (auto iv = visuals.rbegin(); iv != visuals.rend(); ++iv)
            {
                UI::Controls::Control* control = (*iv)->Static_As<UI::Controls::Control>();
                if (control && control->_initCount)
                    control->EndInit();
            }

            templateControl->RegisterWithNameScope();
        }

        auto& subs = GetSubscriptions(_control);
        for (auto triggers : Triggers.GetContainer())
        {
            auto sub = triggers->Apply(_control);
            subs.push_back(sub);
        }
    }
}

void XUI::UI::DataTemplates::ControlTemplate::Detach(XUI::UI::Controls::Control * control)
{
    if (control)
    {
        auto iSubscriptions = _applied.find(control);

        if (iSubscriptions != _applied.end())
        {
            for (auto& subscription : iSubscriptions->second)
            {
                if (subscription.is_subscribed())
                    subscription.unsubscribe();
            }

            _applied.erase(iSubscriptions);
        }

        _namedScopes.erase(control);
    }
}

std::vector<rxcpp::subscription>& XUI::UI::DataTemplates::ControlTemplate::GetSubscriptions(XUI::UI::Controls::UIElement * control)
{
    auto _applied_iterator = _applied.find(control);
    if (_applied_iterator == _applied.end())
    {
        auto& subscriptions = _applied[control];
        //subscriptions.push_back(control->GetControlTemplateDetach()->get_observable().subscribe([weakStyle = weak_from_base_static<XUI::UI::DataTemplates::ControlTemplate>()](XUI::UI::Controls::UIElement * control)
        //{
        //    if (!weakStyle.expired())
        //        weakStyle.lock()->Detach(control);
        //}));
        _applied_iterator = _applied.emplace(control, subscriptions).first;
    }

    return  _applied_iterator->second;
}