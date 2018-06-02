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

#ifndef __XUI_UNDOREDOHELPER_H__
#define __XUI_UNDOREDOHELPER_H__

#include <list>

namespace XUI::UI::Detail
{
    template<typename TState>
    class UndoRedoHelper
    {
    public:
        struct IUndoRedoHost
        {
            virtual TState GetUndoRedoState() const = 0;
            virtual void SetUndoRedoState(TState const& value) = 0;
        };

    private:
        IUndoRedoHost* const _host;

        std::list<TState> _states;
        typename std::list<TState>::iterator _currentNode;

        int m_Limit = -1;

    public:
        UndoRedoHelper(IUndoRedoHost *host) : _host(host)
        {
            _currentNode = _states.begin();
        }

        virtual ~UndoRedoHelper()
        {}

        int GetLimit() const
        {
            return m_Limit;
        }
        void SetLimit(const int &value)
        {
            m_Limit = value;
        }

        void Undo()
        {
            if (_states.empty())
                return;

            if (_currentNode == _states.end())
                --_currentNode;

            if (_currentNode != _states.begin())
            {
                --_currentNode;
                _host->SetUndoRedoState(*_currentNode);
            }
        }

        bool IsLastState() const
        {
            return _currentNode == _states.end() || _currentNode == --_states.end();
        }

        bool TryGetLastState(TState *_state)
        {
            _state = nullptr;
            if (!IsLastState())
                return false;

            if (_currentNode != _states.end())
                _state = &*_currentNode;
            return true;
        }

        bool HasState() const
        {
            return _currentNode != _states.end();
        }

        void UpdateCurrentState(TState const& state)
        {
            if (_states.empty())
            {
                _states.push_back(state);
                _currentNode = --_states.end();
            }

            *_currentNode = state;
        }

        void UpdateCurrentState()
        {
            UpdateCurrentState(_host->GetUndoRedoState());
        }

        void DiscardRedo()
        {
            if (_currentNode != _states.end())
                ++_currentNode;

            while (_currentNode != _states.end())
            {
                _states.erase(_currentNode++);
            }

            if (_states.empty())
                _states.push_back(_host->GetUndoRedoState());

            _currentNode = --_states.end();
        }

        void Redo()
        {
            if (_states.empty())
                return;

            if (_currentNode != _states.end())
            {
                ++_currentNode;
                if (_currentNode != _states.end())
                    _host->SetUndoRedoState(*_currentNode);
                else
                    --_currentNode;
            }
        }

        void Snapshot(bool force = false)
        {
            auto const& current = _host->GetUndoRedoState();
            if (force || _currentNode == _states.end() || *_currentNode == current)
            {
                if (_currentNode != _states.end())
                    DiscardRedo();

                _states.push_back(current);
                _currentNode = --_states.end();
                if (_states.size() > m_Limit)
                    _states.pop_front();
            }
        }

        bool Tick()
        {
            Snapshot();
            return true;
        }
    };
}


#endif	//#ifndef __XUI_UNDOREDOHELPER_H__
