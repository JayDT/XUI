/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
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
#include "DeferredXamlLoader.h"
#include "Instruction.h"
#include "ObjectAssembler.h"
#include "StateCommuter.h"
#include "ReflEx/ValueContext.h"
#include "Assembler/TopDownValueContext.h"
#include "Settings.h"
#include "XamlCPP.h"

using namespace XamlCPP;

System::Reflection::RefObject XamlCPP::Core::DeferredXamlLoader::Load(Interfaces::IInstanceLifeCycleListener* listener, const Interfaces::IRuntimeTypeSource * typeSource, Core::Dependency::IDependencyObject* resourceOwner)
{
    //static const System::Reflection::Type* ControlType = &System::Reflection::ReflectionModulManager::Instance().GetType("XUI::UI::Controls::Control");

    Core::Assembler::Settings settings;
    settings.IsTemplateLoader = true;
    settings.setInstanceLifeCycleListener(listener ? listener->shared_from_this() : nullptr);

    auto topdownContext = std::make_shared<Assembler::TopDownValueContext>();

    if (m_resourceNode)
    {
        auto obj = m_resourceNode->ToIDependencyObject2();
        topdownContext->Add(obj, Xaml::GetRuntimeTypeSource()->GetByType(obj->GetType()));
    }

    auto objectAssembler = std::make_unique<Assembler::ObjectAssembler>(Xaml::GetRuntimeTypeSource().get(), std::make_shared<ReflEx::ValueContext>(Xaml::GetRuntimeTypeSource(), topdownContext), &settings);
    auto valueContext = objectAssembler->getStateCommuter()->getValueContext();
    valueContext->ClrClass = m_clrClass;
    valueContext->ClrSubClass = m_clrSubClass;

    //std::string debugstr = objectAssembler->ToString(m_nodeList);

    for (int i = 0; i != m_nodeList.size(); ++i)
    {
        objectAssembler->Process(m_nodeList[i].get());
    }

    return objectAssembler->getResult();
}

void XamlCPP::Core::DeferredXamlLoader::PushInstruction(Instruction * i)
{
    m_nodeList.push_back(i->shared_from_this());
}
