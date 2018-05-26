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

#ifndef __XAMLCPP_TOPDOWNVALUECONTEXT_H__
#define __XAMLCPP_TOPDOWNVALUECONTEXT_H__

#include <vector>
#include "Interfaces/IParser.h"
#include "Core/Assembler/StoredInstance.h"

namespace XamlCPP::Core::Assembler
{
    class StoredInstance;
}

namespace XamlCPP::Core::Assembler
{
    class TopDownValueContext : public Interfaces::ITopDownValueContext
    {
    private:
        std::set<StoredInstance> privateStore;

        std::set<StoredInstance> const& getStore() const;
    public:
        // Inherited via ITopDownValueContext
        void Add(System::Reflection::Object *instance, Proto::XamlType *xamlType) override;

        System::Reflection::Object *GetLastInstance(Proto::XamlType *xamlType) override;

        virtual std::set<XamlCPP::Core::Assembler::StoredInstance> const & GetStoredInstances() const override;
    };
}


#endif	//#ifndef __XAMLCPP_TOPDOWNVALUECONTEXT_H__
