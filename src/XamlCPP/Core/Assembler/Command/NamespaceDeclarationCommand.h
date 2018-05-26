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

#ifndef __NAMESPACEDECLARATIONCOMMAND_H__
#define __NAMESPACEDECLARATIONCOMMAND_H__

#include "Core/Assembler/Command.h"
#include "Core/Assembler/StoredInstance.h"
#include "Interfaces/ITypeRepository.h"
#include "Interfaces/IInstanceLifeCycleListener.h"

#include <standard/collection.h>
#include <string>
#include <vector>
#include <memory>

namespace XamlCPP::Core
{
    namespace ReflEx
    {
        class NamespaceDeclaration;
    }
}

namespace XamlCPP::Core::Assembler::Commands
{
    class NamespaceDeclarationCommand : public Command
    {
    private:
        ReflEx::NamespaceDeclaration *const namespaceDeclaration;

    public:
        virtual ~NamespaceDeclarationCommand()
        {
        }

        NamespaceDeclarationCommand(ReflEx::NamespaceDeclaration *namespaceDeclaration, std::shared_ptr<StateCommuter> const& stateCommuter);

        void Execute() override;

        std::string ToString() const override;
    };
}


#endif	//#ifndef __NAMESPACEDECLARATIONCOMMAND_H__
