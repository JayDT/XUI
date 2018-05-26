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

#include "Token.h"
#include "TokenizerState.h"
#include "TokenizerResult.h"

template<typename TokenEnumType, typename InputType = std::string>
class Tokenizer
{
public:
    typedef Token<TokenEnumType, InputType> TokenType;
    typedef TokenizerResult<TokenType> ResultType;
    typedef TokenizerState<TokenEnumType, InputType> StateType;
    typedef typename InputType::const_iterator InputIterator;
    typedef std::unordered_map<InputType, TokenEnumType> SymbolTable;

    Tokenizer(void);
    ~Tokenizer(void);

    const typename StateType::Handle GetRootState(void) const;

    typename StateType::Handle CreateState(TokenEnumType acceptingType = TokenEnumType::Invalid);

    void LoadSymbols(const SymbolTable &table);

    ResultType Tokenize(const InputType &input) const;
    
private:
    void readToken(InputIterator start, InputIterator end, TokenType &outputToken) const;

    std::vector<typename StateType::Handle> m_createdStates;

    typename StateType::Handle m_rootState;
};

#include "Impl/Tokenizer.hpp"