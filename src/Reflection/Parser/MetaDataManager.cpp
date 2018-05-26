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

#include "Precompiled.h"

#include "MetaDataManager.h"
#include "MetaDataConfig.h"
#include "MetaUtils.h"
#include "ReflectionParser.h"

#include "Tokenizer/TokenType.h"
#include "Tokenizer/Tokenizer.h"

#include <boost/regex.hpp>

#define TOKENS_WHITESPACE   ' ', '\r', '\n', '\t'
#define TOKENS_NUMBER       '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
#define TOKENS_ALPHANUMERIC 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', \
                            'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', \
                            's', 't', 'u', 'v', 'w', 'x', 'y', 'z',      \
                            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', \
                            'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', \
                            'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'       \

#define LITERAL_ESCAPE_CHAR '\\'
#define LITERAL_SINGLE_QUOTE '\''
#define LITERAL_DOUBLE_QUOTE '"'

#define TOKEN(name, value) { value, ConstructorTokenType::name },

namespace
{
    typedef Token<ConstructorTokenType, std::string> ConstructorToken;

    const Tokenizer<ConstructorTokenType> &getConstructorTokenizer(void);
}

//MetaDataManager::MetaDataManager(const Cursor &cursor)
//{
//}

void MetaDataManager::Parse(const Cursor & cursor)
{
    for (auto &child : cursor.GetChildren())
    {
        m_specifications.insert(std::make_pair(child.GetKind(), child));

        switch (child.GetKind())
        {
            case CXCursorKind::CXCursor_Constructor:
            case CXCursorKind::CXCursor_Destructor:
            case CXCursorKind::CXCursor_FieldDecl:
            case CXCursorKind::CXCursor_CXXMethod:
            case CXCursorKind::CXCursor_ClassDecl:
            case CXCursorKind::CXCursor_StructDecl:
            case CXCursorKind::CXCursor_VarDecl:
            case CXCursorKind::CXCursor_FunctionDecl:
            case CXCursorKind::CXCursor_FunctionTemplate:
            case CXCursorKind::CXCursor_EnumDecl:
            case CXCursorKind::CXCursor_ClassTemplate:
            case CXCursorKind::CXCursor_ClassTemplatePartialSpecialization:
            case CXCursorKind::CXCursor_EnumConstantDecl:
            case CXCursorKind::CXCursor_UsingDeclaration:
            case CXCursorKind::CXCursor_TypeAliasDecl:
            case CXCursorKind::CXCursor_UsingDirective:
            case CXCursorKind::CXCursor_TypeAliasTemplateDecl:
            case CXCursorKind::CXCursor_UnexposedDecl:
            case CXCursorKind::CXCursor_TypedefDecl:
            case CXCursorKind::CXCursor_Namespace:
            case CXCursorKind::CXCursor_ConversionFunction:
            case CXCursorKind::CXCursor_NamespaceAlias:
                continue;
            default: break;
        }

        std::string ns;
        for (auto &prop : extractProperties(child, ns))
        {
            m_attributes.emplace(child.GetKind(), prop);
            if (!ns.empty())
                m_namespacedProperties[ns].emplace(prop.first, prop.second);

            if (child.GetKind() == CXCursorKind::CXCursor_AnnotateAttr)
                m_properties[prop.first] = prop.second;
        }
    }
}

std::string MetaDataManager::GetProperty(const std::string &key) const
{
    auto search = m_properties.find( key );

    // use an empty string by default
    return search == m_properties.end( ) ? "" : search->second;
}

bool MetaDataManager::GetFlag(const std::string &key) const
{
    return m_properties.find( key ) == m_properties.end( ) ? false : true;
}

std::string MetaDataManager::GetNativeString(const std::string &key) const
{
    auto search = m_properties.find( key );

    // wasn't set
    if (search == m_properties.end( ))
        return "";

    static const boost::regex quotedString(
        // opening quote
        "(?:\\s*\")"
        // actual string contents
        "([^\"]*)"
        // closing quote
        "\"",
        boost::regex::icase
    );

    auto &value = search->second;
    
    auto flags = boost::match_default | boost::format_all;

    boost::match_results<std::string::const_iterator> match;

    if (boost::regex_search( 
            value.cbegin( ), 
            value.cend( ), 
            match, 
            quotedString, 
            flags 
        )
    )
    {
        return match[ 1 ].str( );
    }

    // couldn't find one
    return "";
}

MetaDataManager::RangeAttributeMap MetaDataManager::GetAttribute(CXCursorKind key) const
{
    return m_attributes.equal_range(key);
}

MetaDataManager::RangeSpecMap MetaDataManager::GetSpecCursor(CXCursorKind key) const
{
    return m_specifications.equal_range(key);
}

void MetaDataManager::AddProperty(std::string const & type, std::string const & arguments)
{
    m_properties[type] = arguments;
}

void MetaDataManager::CompileTemplateData(Mustache::Data<std::string> &data, const ReflectionParser *context, std::string const& ns) const
{
    Mustache::Data<std::string> propertyData { Mustache::Data<std::string>::Type::List };

    std::vector<std::string> lines;

    // list of keywords to ignore in the initializer list
    static const std::vector<std::string> reservedKeywords
    {
        native_attribute::Enable,
        native_attribute::Disable,
        native_attribute::Register,
        native_attribute::WhiteListMethods,
        native_attribute::DisableNonDynamicCtor,
        native_attribute::DynamicCtorWrap,
        native_attribute::EnablePtrType,
        native_attribute::EnableConstPtrType,
        native_attribute::ReflIncludeType,
        native_attribute::AbstractType,
        native_attribute::EnableArrayType,
        native_attribute::DisplayName,
        native_attribute::PropertyType,
        native_attribute::PropertyGetter,
        native_attribute::PropertySetter,
        native_attribute::ExplicitGetter,
        native_attribute::ExplicitSetter
    };

    int i = 0;

    if (ns.empty())
    {
        auto propertyCount = m_properties.size() - 1;

        for (auto &prop : m_properties)
        {
            Mustache::Data<std::string> item{ Mustache::Data<std::string>::Type::Object };

            // skip reserved keywords
            if (std::find(
                reservedKeywords.begin(),
                reservedKeywords.end(),
                prop.first
            ) != reservedKeywords.end()
                )
            {
                --propertyCount;

                continue;
            }

            item["type"] = prop.first;
            item["arguments"] = prop.second;

            item["isLast"] = utils::TemplateBool(i == propertyCount);

            propertyData << item;

            ++i;
        }
    }
    else
    {
        auto nsAttrs = GetNsAttributes(ns);
        if (nsAttrs)
        {
            auto propertyCount = nsAttrs->size() - 1;

            for (auto &prop : *nsAttrs)
            {
                Mustache::Data<std::string> item{ Mustache::Data<std::string>::Type::Object };

                // skip reserved keywords
                if (std::find(
                    reservedKeywords.begin(),
                    reservedKeywords.end(),
                    prop.first
                ) != reservedKeywords.end()
                    )
                {
                    --propertyCount;

                    continue;
                }

                item["type"] = prop.first;
                item["arguments"] = prop.second;

                item["isLast"] = utils::TemplateBool(i == propertyCount);

                propertyData << item;

                ++i;
            }
        }
    }

    data[ "metaProperty" ] = propertyData;
    data[ "metaDataInitializerList" ] = 
        context->LoadTemplatePartial( kPartialMetaInitializerList );
}

std::vector<MetaDataManager::Property> MetaDataManager::extractProperties(
    const Cursor &cursor, std::string& attrNs
) const
{
    static std::vector<Property> Empty;

    std::string propertyList = cursor.GetDisplayName( );
    if (propertyList.empty())
        return Empty;

    std::vector<Property> properties;

    auto &tokenizer = getConstructorTokenizer( );

    auto result = tokenizer.Tokenize( propertyList );

    result.RemoveAll( ConstructorTokenType::Whitespace );

    auto &tokens = result.GetTokens( );
    int tokenCount = static_cast<int>( tokens.size( ) );

    int tokenStart = 0;
    if (tokens.size() > 1 && (tokens[0].type == ConstructorTokenType::Identifier && tokens[1].type == ConstructorTokenType::Colon))
    {
        attrNs = tokens[0].value;
        tokenStart = 2;
    }

    // case where there is only one identifier, which means there's one property
    // with a default constructor
    if (tokenCount == (tokenStart + 1) && tokens[ tokenStart ].type == ConstructorTokenType::Identifier)
    {
        properties.emplace_back( tokens[ tokenStart ].value, "" );
    }

    auto lastType = ConstructorTokenType::Invalid;

    int firstOpenParenToken = 0;
    int openParens = 0;

    for (int i = tokenStart; i < tokenCount; ++i)
    {
        auto &token = tokens[ i ];

        switch (token.type)
        {
        case ConstructorTokenType::OpenParentheses:
        {
            if (openParens == 0)
                firstOpenParenToken = i;

            ++openParens;

            break;
        }
        case ConstructorTokenType::CloseParentheses:
        {
            --openParens;

            // we've read a constructor
            if (openParens == 0)
            {
                properties.emplace_back(
                    result.ConsumeAllPrevious( std::max( 0, firstOpenParenToken - 1 ),
                        ConstructorTokenType::Identifier, 
                        ConstructorTokenType::ScopeResolution,
                        ConstructorTokenType::LessThan,
                        ConstructorTokenType::GreaterThan
                    ),
                    result.ConsumeRange( firstOpenParenToken + 1, std::max( 0, i - 1 ) )
                );

                ++i;
            }

            break;
        }
        case ConstructorTokenType::Comma:
        {
            // constructor with no parameters
            if (openParens == 0 && lastType == ConstructorTokenType::Identifier)
            {
                properties.emplace_back(
                    result.ConsumeAllPrevious( i - 1, 
                        ConstructorTokenType::Identifier, 
                        ConstructorTokenType::ScopeResolution,
                        ConstructorTokenType::LessThan,
                        ConstructorTokenType::GreaterThan
                    ),
                    ""
                );
            }

            break;
        }
        default:
            break;
        }

        lastType = token.type;
    }

    // case where a default constructor is the last in the list
    if (tokenCount >= 2 && 
        tokens[ tokenCount - 1 ].type == ConstructorTokenType::Identifier &&
        tokens[ tokenCount - 2 ].type == ConstructorTokenType::Comma)
    {
        properties.emplace_back( tokens.back( ).value, "" );
    }

    return properties;
}

namespace
{
    const Tokenizer<ConstructorTokenType> &getConstructorTokenizer(void)
    {
        static bool initialized = false;
        static Tokenizer<ConstructorTokenType> tokenizer;

        if (initialized)
            return tokenizer;

        auto root = tokenizer.GetRootState( );

        // Whitespace
        {
            auto whitespace = tokenizer.CreateState( ConstructorTokenType::Whitespace );

            whitespace->SetLooping( TOKENS_WHITESPACE );

            root->AddEdge( whitespace, TOKENS_WHITESPACE );
        }

        // Identifier
        {
            auto firstCharacter = tokenizer.CreateState( ConstructorTokenType::Identifier );
            auto anyCharacters = tokenizer.CreateState( ConstructorTokenType::Identifier );

            root->AddEdge( firstCharacter, TOKENS_ALPHANUMERIC, '_' );

            anyCharacters->SetLooping( TOKENS_ALPHANUMERIC, TOKENS_NUMBER, '_' );

            firstCharacter->AddEdge( anyCharacters, TOKENS_ALPHANUMERIC, TOKENS_NUMBER, '_' );
        }

        // Integer Literal
        auto integerLiteral = tokenizer.CreateState( ConstructorTokenType::IntegerLiteral );
        {
            integerLiteral->SetLooping( TOKENS_NUMBER );

            root->AddEdge( integerLiteral, TOKENS_NUMBER );
        }

        // Float Literal
        {
            auto period = tokenizer.CreateState( );
            auto floatNoExponent = tokenizer.CreateState( ConstructorTokenType::FloatLiteral );
            auto exponent = tokenizer.CreateState( );
            auto plusOrMinus = tokenizer.CreateState( );
            auto validOptionalExponent = tokenizer.CreateState( ConstructorTokenType::FloatLiteral );
            auto fCharacter = tokenizer.CreateState( ConstructorTokenType::FloatLiteral );

            integerLiteral->AddEdge( period, '.' );
            period->AddEdge( floatNoExponent, TOKENS_NUMBER );

            floatNoExponent->AddEdge( exponent, 'e', 'E' );
            floatNoExponent->SetLooping( TOKENS_NUMBER );

            exponent->AddEdge( validOptionalExponent, TOKENS_NUMBER );
            exponent->AddEdge( plusOrMinus, '+', '-' );

            plusOrMinus->AddEdge( validOptionalExponent, TOKENS_NUMBER );

            validOptionalExponent->AddEdge( fCharacter, 'f', 'F' );
            validOptionalExponent->SetLooping( TOKENS_NUMBER );

            floatNoExponent->AddEdge( fCharacter, 'f', 'F' );
        }

        // String Literal
        {
            auto firstDoubleQuote = tokenizer.CreateState( );
            auto escapeSlash = tokenizer.CreateState(  );
            auto escapeChars = tokenizer.CreateState( );
            auto validStringLiteral = tokenizer.CreateState( ConstructorTokenType::StringLiteral );
            auto anyCharacter = tokenizer.CreateState( );

            root->AddEdge( firstDoubleQuote, LITERAL_DOUBLE_QUOTE );

            firstDoubleQuote->AddEdge( escapeSlash, LITERAL_ESCAPE_CHAR );
            firstDoubleQuote->AddEdge( validStringLiteral, LITERAL_DOUBLE_QUOTE );
            firstDoubleQuote->SetDefaultEdge( anyCharacter );

            escapeSlash->AddEdge( escapeChars, LITERAL_DOUBLE_QUOTE );

            // accept any escape token
            escapeSlash->SetDefaultEdge( firstDoubleQuote );

            escapeChars->SetDefaultEdge( firstDoubleQuote );
            escapeChars->AddEdge( validStringLiteral, LITERAL_DOUBLE_QUOTE );

            anyCharacter->SetDefaultEdge( anyCharacter );
            anyCharacter->AddEdge( escapeSlash, LITERAL_ESCAPE_CHAR );
            anyCharacter->AddEdge( validStringLiteral, LITERAL_DOUBLE_QUOTE );
        }

        // Symbols
        {
            decltype(tokenizer)::SymbolTable symbols
            {
                #include "Tokenizer/ConstructorTokenSymbols.inl"
            };

            tokenizer.LoadSymbols( symbols );
        }

        initialized = true;

        return tokenizer;
    }
}