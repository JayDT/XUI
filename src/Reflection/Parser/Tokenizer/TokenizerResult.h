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

template<typename TokenType>
class TokenizerResult
{
public:
    typedef std::vector<TokenType> TokenList;

    TokenizerResult(const TokenList &tokens);

    const TokenList &GetTokens(void) const;

    template<typename... TokenTypeList>
    size_t FindNext(size_t start, TokenTypeList &&...types);

    template<typename... TokenTypeList>
    size_t FindPrevious(size_t start, TokenTypeList &&...types);

    template<typename... TokenTypeList>
    void RemoveAll(TokenTypeList &&...types);

    typename TokenType::InputValueType ConsumeRange(size_t start, size_t end);

    template<typename... TokenTypeList>
    typename TokenType::InputValueType ConsumeAllPrevious(size_t start, TokenTypeList &&...types);

    template<typename... TokenTypeList>
    typename TokenType::InputValueType ConsumeAllNext(size_t start, TokenTypeList &&...types);
private:
    TokenList m_tokens;
};

#include "Impl/TokenizerResult.hpp"