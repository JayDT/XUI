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

#include "TopDownValueContext.h"
#include "StoredInstance.h"

using namespace XamlCPP::Core;
using namespace XamlCPP::Core::Assembler;

std::set<StoredInstance> const& TopDownValueContext::getStore() const
{
    return privateStore;
}

void TopDownValueContext::Add(System::Reflection::Object *instance, Proto::XamlType *xamlType)
{
    //if (privateStore.capacity() < 100)
    //    privateStore.reserve(100);

    //for (auto const& si : privateStore.GetNativeEnumerator())
    //{
    //    if (si.GetInstance() == instance)
    //        return;
    //}

    if (privateStore.find(StoredInstance(instance, xamlType)) != privateStore.end())
        return;

    privateStore.emplace(instance, xamlType, privateStore.size());
}

System::Reflection::Object *TopDownValueContext::GetLastInstance(Proto::XamlType *xamlType)
{
    System::Reflection::Object * result = nullptr;
    int orderID = -1;
    for (auto const& i : privateStore)
        if (i.GetXamlType() == xamlType && orderID < i.GetOrderID())
        {
            orderID = i.GetOrderID();
            result = i.GetInstance();
        }

    return nullptr;
}

std::set<XamlCPP::Core::Assembler::StoredInstance> const & XamlCPP::Core::Assembler::TopDownValueContext::GetStoredInstances() const
{
    return getStore();
}
