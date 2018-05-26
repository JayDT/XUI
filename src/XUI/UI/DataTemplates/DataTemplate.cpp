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
#include "DataTemplate.h"
#include "ContentControl.h"

#include "XamlCPP/XamlCPP.h"
#include "XamlCPP/Core/Assembler/Settings.h"

std::shared_ptr<XUI::UI::DataTemplates::ContentTemplate> XUI::UI::DataTemplates::DataTemplate::CreateContentTemplate()
{
    return std::make_shared<XUI::UI::DataTemplates::DataContentTemplate>(XamlTemplate);
}

System::Reflection::meta::RefObject XUI::UI::DataTemplates::DataTemplate::Build(XamlCPP::Interfaces::IInstanceLifeCycleListener*)
{
    return ContentTemplate->Build(nullptr);
}

void XUI::UI::DataTemplates::DataTemplate::Attach(XUI::UI::Controls::Control * _control, Core::Dependency::DependencyObject* DataTemplateRoot)
{
    if (_control && DataTemplateRoot)
    {
        auto templateControl = DataTemplateRoot->Dynamic_As<UI::Controls::Control>();
        if (templateControl)
        {
            auto& namedScope = _namedScopes[_control];
            if (!namedScope)
                namedScope = std::make_shared<UI::Controls::NameScope>();
            templateControl->_nameScope = namedScope;

            std::list<UI::Controls::Visual*> visuals;
            templateControl->GetVisualDescendents(visuals);
            for (auto iv = visuals.rbegin(); iv != visuals.rend(); ++iv)
            {
                UI::Controls::Control* control = (*iv)->Dynamic_As<UI::Controls::Control>();
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

void XUI::UI::DataTemplates::DataTemplate::Detach(XUI::UI::Controls::Control * control)
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

std::vector<rxcpp::subscription>& XUI::UI::DataTemplates::DataTemplate::GetSubscriptions(XUI::UI::Controls::UIElement * control)
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

bool XUI::UI::DataTemplates::DataContentTemplate::SupportsRecycling()
{
    return false;
}

System::Reflection::RefObject XUI::UI::DataTemplates::DataContentTemplate::Build(System::Reflection::meta::Variant const & param)
{
    return xamlTemplate->Load(nullptr);
}
