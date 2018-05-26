/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
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

#pragma once

#include <memory>
#include <unordered_map>

template<typename TokenEnumType, typename InputType>
class Tokenizer;

template<typename TokenEnumType, typename InputType>
class TokenizerState
{
public:
    typedef TokenizerState* Handle;

    explicit TokenizerState(TokenEnumType acceptingType);

    void SetAcceptingType(TokenEnumType acceptingType);

    void AddEdge(Handle to, char character);

    template<typename... CharacterList>
    void AddEdge(Handle to, char character, CharacterList &&...list);

    template<typename... CharacterList>
    void SetLooping(CharacterList &&...list);

    void SetDefaultEdge(Handle defaultEdge);

private:
    friend class Tokenizer<TokenEnumType, InputType>;

    TokenEnumType m_acceptingType;

    std::unordered_map<char, Handle> m_edges;
    Handle m_defaultEdge;
};

#include "Impl/TokenizerState.hpp"