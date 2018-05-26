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

template<typename TokenEnumType, typename InputType>
TokenizerState<TokenEnumType, InputType>::TokenizerState(TokenEnumType acceptingType)
    : m_acceptingType( acceptingType )
    , m_defaultEdge( nullptr ) { }

template<typename TokenEnumType, typename InputType>
void TokenizerState<TokenEnumType, InputType>::SetAcceptingType(TokenEnumType acceptingType)
{
    m_acceptingType = acceptingType;
}

template<typename TokenEnumType, typename InputType>
void TokenizerState<TokenEnumType, InputType>::AddEdge(Handle to, char character)
{
    m_edges[ character ] = to;
}

template<typename TokenEnumType, typename InputType>
template<typename ... CharacterList>
void TokenizerState<TokenEnumType, InputType>::AddEdge(Handle to, char character, CharacterList &&...list)
{
    AddEdge( to, character );
    AddEdge( to, list... );
}

template<typename TokenEnumType, typename InputType>
template<typename ... CharacterList>
void TokenizerState<TokenEnumType, InputType>::SetLooping(CharacterList &&...list)
{
    AddEdge( Handle( this ), list... );
}

template<typename TokenEnumType, typename InputType>
void TokenizerState<TokenEnumType, InputType>::SetDefaultEdge(Handle defaultEdge)
{
    m_defaultEdge = defaultEdge;
}