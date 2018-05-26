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

#ifndef __XAMLCPP_COMMAND_H__
#define __XAMLCPP_COMMAND_H__

namespace XamlCPP::Core::Assembler
{
    class StateCommuter;

    class Command
    {
    protected:
        std::shared_ptr<StateCommuter> m_StateCommuter;

    protected:
        Command(std::shared_ptr<StateCommuter> const& stateCommuter)
        {
            m_StateCommuter = stateCommuter;
        }

        StateCommuter *getStateCommuter() const
        {
            return m_StateCommuter.get();
        }

        virtual std::string ToString() const
        {
            return "{Command}";
        }

    public:
        virtual void Execute() = 0;
    };

}


#endif	//#ifndef __XAMLCPP_COMMAND_H__
