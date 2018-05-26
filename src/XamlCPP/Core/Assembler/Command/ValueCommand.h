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

#ifndef __VALUECOMMAND_H__
#define __VALUECOMMAND_H__

#include "Core/Assembler/Command.h"
#include "Core/Assembler/StoredInstance.h"
#include "Interfaces/ITypeRepository.h"
#include "Interfaces/IValueContext.h"

#include <standard/collection.h>
#include <string>
#include <vector>
#include <memory>

namespace XamlCPP::Core
{
    namespace ReflEx
    {
        class MemberBase;
        class MutableMember;
        class NamespaceDeclaration;
    }
}

namespace XamlCPP::Core::Assembler::Commands
{
    class ValueCommand : public Command
    {
    private:
        Interfaces::IValueContext *const valueContext;
        const std::string value;

    public:
        virtual ~ValueCommand()
        {
        }

        ValueCommand(std::shared_ptr<StateCommuter> const& stateCommuter, Interfaces::IValueContext *valueContext, const std::string &value);

        void Execute() override;

        std::string ToString() const override;
    };
}


#endif	//#ifndef __VALUECOMMAND_H__
