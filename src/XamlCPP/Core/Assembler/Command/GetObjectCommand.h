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

#ifndef __GETOBJECTCOMMAND_H__
#define __GETOBJECTCOMMAND_H__

#include "Core/Assembler/Command.h"
#include "Interfaces/ITypeRepository.h"
#include "Interfaces/IInstanceLifeCycleListener.h"

#include <string>
#include <vector>
#include <memory>

namespace XamlCPP::Core::Assembler::Commands
{
    class GetObjectCommand : public Command
    {
    public:
        GetObjectCommand(std::shared_ptr<StateCommuter> const& stateCommuter);
        virtual ~GetObjectCommand()
        {

        }

        void Execute() override;

        std::string ToString() const override;
    };
}

#endif	//#ifndef __GETOBJECTCOMMAND_H__
