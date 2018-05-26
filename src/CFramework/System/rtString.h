/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
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

#ifndef __SYSTEM_STRING_H__
#define __SYSTEM_STRING_H__

#include <stdarg.h>
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include <codecvt>
#include <locale>

#include <cppformat/format.h>
#include <standard/Platform/Define.h>
#include <Runtime/Object.h>
#include <Runtime/Type.h>
#include <Runtime/VariantBase.h>

#ifndef ATTR_NORETURN
#  define ATTR_NORETURN
#  define ATTR_PRINTF(F,V)
#endif

namespace System
{
    /// Default TC string format function.
    template<typename Format, typename... Args>
    inline std::string StringFormat(Format&& fmt, Args&&... args)
    {
        return fmt::sprintf(std::forward<Format>(fmt), std::forward<Args>(args)...);
    }

    /// Returns true if the given char pointer is null.
    inline bool IsFormatEmptyOrNull(const char* fmt)
    {
        return fmt == nullptr;
    }

    /// Returns true if the given std::string is empty.
    inline bool IsFormatEmptyOrNull(std::string const& fmt)
    {
        return fmt.empty();
    }

    enum class CharClass
    {
        UnCategorized,
        Whitespace,
        AlphaNumeric,
    };

    TC_CFRAMEWORK_API std::string ByteArrayToHexStr(uint8 const* bytes, uint32 arrayLen, bool reverse /* = false */);
    TC_CFRAMEWORK_API void HexStrToByteArray(std::string const& str, uint8* out, bool reverse /*= false*/);

    TC_CFRAMEWORK_API bool Utf8toWStr(const std::string& utf8str, std::wstring& wstr);
    // in wsize==max size of buffer, out wsize==real string size
    TC_CFRAMEWORK_API bool Utf8toWStr(char const *utf8str, size_t csize, wchar_t* wstr, size_t& wsize);
    inline bool Utf8toWStr(const std::string& utf8str, wchar_t* wstr, size_t& wsize)
    {
        return Utf8toWStr(utf8str.c_str(), utf8str.size(), wstr, wsize);
    }

    TC_CFRAMEWORK_API bool WStrToUtf8(std::wstring wstr, std::string& utf8str);
    // size==real string size
    TC_CFRAMEWORK_API bool WStrToUtf8(wchar_t* wstr, size_t size, std::string& utf8str);

    TC_CFRAMEWORK_API size_t utf8length(std::string& utf8str);                    // set string to "" if invalid utf8 sequence
    TC_CFRAMEWORK_API void utf8truncate(std::string& utf8str, size_t len);

    inline bool isBasicLatinCharacter(wchar_t wchar)
    {
        if (wchar >= L'a' && wchar <= L'z')                      // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
            return true;
        if (wchar >= L'A' && wchar <= L'Z')                      // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
            return true;
        return false;
    }

    inline bool isExtendedLatinCharacter(wchar_t wchar)
    {
        if (isBasicLatinCharacter(wchar))
            return true;
        if (wchar >= 0x00C0 && wchar <= 0x00D6)                  // LATIN CAPITAL LETTER A WITH GRAVE - LATIN CAPITAL LETTER O WITH DIAERESIS
            return true;
        if (wchar >= 0x00D8 && wchar <= 0x00DF)                  // LATIN CAPITAL LETTER O WITH STROKE - LATIN CAPITAL LETTER THORN
            return true;
        if (wchar == 0x00DF)                                     // LATIN SMALL LETTER SHARP S
            return true;
        if (wchar >= 0x00E0 && wchar <= 0x00F6)                  // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
            return true;
        if (wchar >= 0x00F8 && wchar <= 0x00FE)                  // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
            return true;
        if (wchar >= 0x0100 && wchar <= 0x012F)                  // LATIN CAPITAL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK
            return true;
        if (wchar == 0x1E9E)                                     // LATIN CAPITAL LETTER SHARP S
            return true;
        return false;
    }

    inline bool isCyrillicCharacter(wchar_t wchar)
    {
        if (wchar >= 0x0410 && wchar <= 0x044F)                  // CYRILLIC CAPITAL LETTER A - CYRILLIC SMALL LETTER YA
            return true;
        if (wchar == 0x0401 || wchar == 0x0451)                  // CYRILLIC CAPITAL LETTER IO, CYRILLIC SMALL LETTER IO
            return true;
        return false;
    }

    inline bool isEastAsianCharacter(wchar_t wchar)
    {
        if (wchar >= 0x1100 && wchar <= 0x11F9)                  // Hangul Jamo
            return true;
        if (wchar >= 0x3041 && wchar <= 0x30FF)                  // Hiragana + Katakana
            return true;
        if (wchar >= 0x3131 && wchar <= 0x318E)                  // Hangul Compatibility Jamo
            return true;
        if (wchar >= 0x31F0 && wchar <= 0x31FF)                  // Katakana Phonetic Ext.
            return true;
        if (wchar >= 0x3400 && wchar <= 0x4DB5)                  // CJK Ideographs Ext. A
            return true;
        if (wchar >= 0x4E00 && wchar <= 0x9FC3)                  // Unified CJK Ideographs
            return true;
        if (wchar >= 0xAC00 && wchar <= 0xD7A3)                  // Hangul Syllables
            return true;
        if (wchar >= 0xFF01 && wchar <= 0xFFEE)                  // Halfwidth forms
            return true;
        return false;
    }

    inline bool isWhiteSpace(char c)
    {
        return ::isspace(int(c)) != 0;
    }

    inline bool isNumeric(wchar_t wchar)
    {
        return (wchar >= L'0' && wchar <= L'9');
    }

    inline bool isNumeric(char c)
    {
        return (c >= '0' && c <= '9');
    }

    inline bool isNumericOrSpace(wchar_t wchar)
    {
        return isNumeric(wchar) || wchar == L' ';
    }

    inline bool isNumeric(std::string const& str)
    {
        for (std::string::const_iterator itr = str.begin(); itr != str.end(); ++itr)
            if (!isNumeric(*itr))
                return false;

        return true;
    }

    inline bool isNumeric(std::wstring const& str)
    {
        for (std::wstring::const_iterator itr = str.begin(); itr != str.end(); ++itr)
            if (!isNumeric(*itr))
                return false;

        return true;
    }

    inline bool isBasicLatinString(std::wstring wstr, bool numericOrSpace)
    {
        for (size_t i = 0; i < wstr.size(); ++i)
            if (!isBasicLatinCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
                return false;
        return true;
    }

    inline bool isExtendedLatinString(std::wstring wstr, bool numericOrSpace)
    {
        for (size_t i = 0; i < wstr.size(); ++i)
            if (!isExtendedLatinCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
                return false;
        return true;
    }

    inline bool isCyrillicString(std::wstring wstr, bool numericOrSpace)
    {
        for (size_t i = 0; i < wstr.size(); ++i)
            if (!isCyrillicCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
                return false;
        return true;
    }

    inline bool isEastAsianString(std::wstring wstr, bool numericOrSpace)
    {
        for (size_t i = 0; i < wstr.size(); ++i)
            if (!isEastAsianCharacter(wstr[i]) && (!numericOrSpace || !isNumericOrSpace(wstr[i])))
                return false;
        return true;
    }

    inline wchar_t wcharToUpper(wchar_t wchar)
    {
        if (wchar >= L'a' && wchar <= L'z')                      // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
            return wchar_t(uint16(wchar) - 0x0020);
        if (wchar == 0x00DF)                                     // LATIN SMALL LETTER SHARP S
            return wchar_t(0x1E9E);
        if (wchar >= 0x00E0 && wchar <= 0x00F6)                  // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
            return wchar_t(uint16(wchar) - 0x0020);
        if (wchar >= 0x00F8 && wchar <= 0x00FE)                  // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
            return wchar_t(uint16(wchar) - 0x0020);
        if (wchar >= 0x0101 && wchar <= 0x012F)                  // LATIN SMALL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK (only %2=1)
        {
            if (wchar % 2 == 1)
                return wchar_t(uint16(wchar) - 0x0001);
        }
        if (wchar >= 0x0430 && wchar <= 0x044F)                  // CYRILLIC SMALL LETTER A - CYRILLIC SMALL LETTER YA
            return wchar_t(uint16(wchar) - 0x0020);
        if (wchar == 0x0451)                                     // CYRILLIC SMALL LETTER IO
            return wchar_t(0x0401);

        return wchar;
    }

    inline wchar_t wcharToUpperOnlyLatin(wchar_t wchar)
    {
        return isBasicLatinCharacter(wchar) ? wcharToUpper(wchar) : wchar;
    }

    inline wchar_t wcharToLower(wchar_t wchar)
    {
        if (wchar >= L'A' && wchar <= L'Z')                      // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
            return wchar_t(uint16(wchar) + 0x0020);
        if (wchar >= 0x00C0 && wchar <= 0x00D6)                  // LATIN CAPITAL LETTER A WITH GRAVE - LATIN CAPITAL LETTER O WITH DIAERESIS
            return wchar_t(uint16(wchar) + 0x0020);
        if (wchar >= 0x00D8 && wchar <= 0x00DE)                  // LATIN CAPITAL LETTER O WITH STROKE - LATIN CAPITAL LETTER THORN
            return wchar_t(uint16(wchar) + 0x0020);
        if (wchar >= 0x0100 && wchar <= 0x012E)                  // LATIN CAPITAL LETTER A WITH MACRON - LATIN CAPITAL LETTER I WITH OGONEK (only %2=0)
        {
            if (wchar % 2 == 0)
                return wchar_t(uint16(wchar) + 0x0001);
        }
        if (wchar == 0x1E9E)                                     // LATIN CAPITAL LETTER SHARP S
            return wchar_t(0x00DF);
        if (wchar == 0x0401)                                     // CYRILLIC CAPITAL LETTER IO
            return wchar_t(0x0451);
        if (wchar >= 0x0410 && wchar <= 0x042F)                  // CYRILLIC CAPITAL LETTER A - CYRILLIC CAPITAL LETTER YA
            return wchar_t(uint16(wchar) + 0x0020);

        return wchar;
    }

    inline void wstrToUpper(std::wstring& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), wcharToUpper);
    }

    inline void wstrToLower(std::wstring& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), wcharToLower);
    }

    class Char : public Reflection::Object
    {
    public:
        static inline bool IsLetter(char ch)
        {
            return ((ch <= 'z' && ch >= 'a') || (ch <= 'Z' && ch >= 'A'));
        }

        static inline bool IsLetter(wchar_t ch)
        {
            return ((ch <= L'z' && ch >= L'a') || (ch <= L'Z' && ch >= L'A'));
        }

        static CharClass GetClass(wchar_t c, std::locale const& loc = std::locale())
        {
            if (std::isspace(c, loc))
            {
                return CharClass::Whitespace;
            }
            else if (iswalpha(c) || std::isdigit(c, loc))
            {
                return CharClass::AlphaNumeric;
            }
            else
            {
                return CharClass::UnCategorized;
            }
        }

        static bool IsCharInSeqvence(wchar_t t, wchar_t c)
        {
            return t == c;
        }

        template<typename... ARGS>
        static bool IsCharInSeqvence(wchar_t t, wchar_t c, ARGS... args)
        {
            return IsCharInSeqvence(t, c) || IsCharInSeqvence(t, args...);
        }

        static bool IsEol(wchar_t c)
        {
            return c == L'\n';
        }
    };

    class String : public std::string, public Reflection::Object
    {
        //TODO: size_t HashCode; // std::hash<std::string>()(identifier.c_str());

    public:
        typedef std::vector<std::string> Tokens;
        typedef std::vector<std::wstring> WTokens;

        explicit String() : std::string() {}
        String(const char* _right) : std::string(_right) {}
        String(std::string const& _right) : std::string(_right) {}
        String(std::wstring wStr) : std::string(wStr.begin(), wStr.end()) {}
        String(const wchar_t* wStr) : std::string(wcslen(wStr), ' ')
        {
            //size_t convertedChars = 0;
            //size_t  sizeInBytes = ((size() + 1) * 2);
            //
            //auto err = wcstombs_s(&convertedChars,
            //    &(*this)[0], size() + 1,
            //    wStr, sizeInBytes);
            std::wcstombs(&(*this)[0], wStr, size());
        }
        virtual ~String() {}

        String& operator=(std::string const& _right)
        {
            *((std::string*)this) = _right;
            return *this;
        }

        String& operator=(std::string& _right)
        {
            *((std::string*)this) = _right;
            return *this;
        }

        String& operator=(const wchar_t* wStr)
        {
            resize(wcslen(wStr), ' ');

            //size_t convertedChars = 0;
            //size_t  sizeInBytes = ((size() + 1) * 2);
            //
            //auto err = wcstombs_s(&convertedChars,
            //    &(*this)[0], sizeInBytes,
            //    wStr, sizeInBytes);

            std::wcstombs(&(*this)[0], wStr, size());
            return *this;
        }

        String& operator=(std::wstring const& wStr)
        {
            *((std::string*)this) = std::string(wStr.begin(), wStr.end());
            return *this;
        }

        String& operator=(const char* _right)
        {
            *((std::string*)this) = _right;
            return *this;
        }

        String ToLower()
        {
            String output = *this;
            std::transform(begin(), end(), output.begin(), tolower);
            return output;
        }

        String ToUpper()
        {
            String output = *this;
            std::transform(begin(), end(), output.begin(), toupper);
            return output;
        }

        String& ToLowerFirst()
        {
            if (!empty())
                (*this)[0] = tolower((*this)[0]);
            return (*this);
        }

        String& ToUpperFirst()
        {
            if (!empty())
                (*this)[0] = toupper((*this)[0]);
            return (*this);
        }

        std::wstring ToWString() const
        {
            //#ifdef WIN32
            //            size_t length = size();
            //            std::wstring out(length, L'#');
            //            size_t convertedChars = 0;
            //            mbstowcs_s(&convertedChars, &out[0], length * sizeof(wchar_t), c_str(), length);
            //            return out;
            //#endif
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.from_bytes(*this);
        }

        std::string ToString() const override
        {
            return *this;
        }

        static std::wstring ToWString(std::string const& input)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.from_bytes(input);
        }

        static std::string FromWString(std::wstring const& str)
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            return converter.to_bytes(str);
        }

        int Count(const char find)
        {
            String source = *this;
            size_t cnt = 0, pos = 0;

            while ((pos = source.find(find, pos)) != std::string::npos)
            {
                ++pos;
                ++cnt;
            }
            return cnt;
        }

        int Count(const char* find)
        {
            String source = *this;
            size_t cnt = 0, pos = 0;

            while ((pos = source.find(find, pos)) != std::string::npos)
            {
                ++cnt;
            }
            return cnt;
        }

        bool Contains(const char* str)
        {
            return find(str) != std::string::npos;
        }

        String& RemoveRef(int startIndex, int count = -1)
        {
            if (count == -1)
                count = size() - startIndex;

            if (count < 1)
                return *this;

            erase(startIndex, count);
            return *this;
        }


        String Remove(int startIndex, int count = -1)
        {
            if (count == -1)
                count = size() - startIndex;

            if (count < 1)
                return *this;

            String _ret = *this;
            _ret.erase(startIndex, count);
            return _ret;
        }

        String& ReplaceRef(std::string const& find, std::string const& replace)
        {
            String& source = *this;

            //ASSERT(find != NULL);
            //ASSERT(replace != NULL);
            size_t findLen = find.size();
            //size_t replaceLen = strlen(replace);
            size_t pos = 0;

            //search for the next occurrence of find within source
            while ((pos = source.find(find, pos)) != std::string::npos)
            {
                //replace the found string with the replacement
                source.replace(pos, findLen, replace);

                //the next line keeps you from searching your replace string,
                //so your could replace "hello" with "hello world"
                //and not have it blow chunks.
            }
            return source;
        }

        String& ReplaceRef(const char find, const char replace, size_t pos = 0)
        {
            String& source = *this;

            //ASSERT(find != NULL);
            //ASSERT(replace != NULL);
            //size_t findLen = 1;
            //size_t replaceLen = strlen(replace);
            //size_t pos = 0;

            //search for the next occurrence of find within source
            while ((pos = source.find(find, pos)) != std::string::npos)
            {
                //replace the found string with the replacement
                source.replace(pos, 1, &replace, 1);

                //the next line keeps you from searching your replace string,
                //so your could replace "hello" with "hello world"
                //and not have it blow chunks.
            }
            return source;
        }

        String Replace(std::string const& find, std::string const& replace, size_t pos = 0)
        {
            String source = *this;

            //ASSERT(find != NULL);
            //ASSERT(replace != NULL);
            size_t findLen = find.size();
            //size_t replaceLen = strlen(replace);
            //size_t pos = 0;

            //search for the next occurrence of find within source
            while ((pos = source.find(find, pos)) != std::string::npos)
            {
                //replace the found string with the replacement
                source.replace(pos, findLen, replace);

                //the next line keeps you from searching your replace string,
                //so your could replace "hello" with "hello world"
                //and not have it blow chunks.
            }
            return source;
        }

        String Replace(const char find, const char replace)
        {
            String source = *this;

            //ASSERT(find != NULL);
            //ASSERT(replace != NULL);
            //size_t findLen = 1;
            //size_t replaceLen = strlen(replace);
            size_t pos = 0;

            //search for the next occurrence of find within source
            while ((pos = source.find(find, pos)) != std::string::npos)
            {
                //replace the found string with the replacement
                source.replace(pos, 1, &replace, 1);

                //the next line keeps you from searching your replace string,
                //so your could replace "hello" with "hello world"
                //and not have it blow chunks.
            }
            return source;
        }

        void Tokenize(Tokens& tokens, char delimiters = ' ')
        {
            // Skip delimiters at beginning.
            std::string::size_type lastPos = find_first_not_of(delimiters, 0);
            // Find first "non-delimiter".
            std::string::size_type pos = find_first_of(delimiters, lastPos);

            while (std::string::npos != pos || std::string::npos != lastPos)
            {
                // Found a token, add it to the vector.
                tokens.push_back(substr(lastPos, pos - lastPos));
                // Skip delimiters.  Note the "not_of"
                lastPos = find_first_not_of(delimiters, pos);
                // Find next "non-delimiter"
                pos = find_first_of(delimiters, lastPos);
            }
        }

        void Tokenize(WTokens& tokens, char delimiters = ' ')
        {
            // Skip delimiters at beginning.
            std::string::size_type lastPos = find_first_not_of(delimiters, 0);
            // Find first "non-delimiter".
            std::string::size_type pos = find_first_of(delimiters, lastPos);

            while (std::string::npos != pos || std::string::npos != lastPos)
            {
                // Found a token, add it to the vector.
                tokens.push_back(System::String(substr(lastPos, pos - lastPos)).ToWString());
                // Skip delimiters.  Note the "not_of"
                lastPos = find_first_not_of(delimiters, pos);
                // Find next "non-delimiter"
                pos = find_first_of(delimiters, lastPos);
            }
        }

        std::vector<std::string> Split(char delim)
        {
            std::vector<std::string> elems;

            std::stringstream ss(*this);
            std::string item;
            while (std::getline(ss, item, delim))
            {
                elems.push_back(item);
            }
            return elems;
        }

        System::String BeforeFirst(char delim)
        {
            std::stringstream ss(*this);
            System::String item;
            while (std::getline(ss, item, delim))
            {
                if (item != "")
                    return item;
            }
            return *this;
        }

        System::String BeforeLast(char delim)
        {
            size_t pos = std::string::find_last_of(delim);
            return substr(0, pos);
        }

        System::String GetLastSubString(char delim)
        {
            size_t pos = std::string::find_last_of(delim);
            return substr(pos);
        }

        String Trim(char trimChar = ' ', bool full = false)
        {
            std::string str = *this;

            if (full)
            {
                for (std::string::iterator itr = str.begin(); itr != str.end();)
                {
                    if ((*itr) == trimChar)
                    {
                        str.erase(itr);
                        itr = str.begin();
                    }
                    else
                        ++itr;
                }
            }
            else
            {
                std::string::iterator itr = str.begin();
                while ((*itr) == trimChar)
                {
                    str.erase(itr);
                    itr = str.begin();
                }

                std::string::reverse_iterator itr1 = str.rbegin();
                while ((*itr1) == trimChar)
                {
                    str.erase(--itr1.base());
                    itr1 = str.rbegin();
                }
            }
            return str;
        }

        static String format(FORMAT_STRING(const char* fmt), ...) ATTR_PRINTF(1, 2)
        {
            va_list ap;
            std::string result;
            char temp[4096];
            va_start(ap, fmt);
            vsnprintf(&temp[0], 4096, fmt, ap);
            va_end(ap);
            return String(&temp[0]);
        }

        static int LineBegin(std::wstring const& text, int pos)
        {
            while (pos > 0 && !Char::IsEol(text[pos - 1]))
            {
                pos--;
            }

            return pos;
        }

        static int LineEnd(std::wstring const& text, int cursor, bool include = false)
        {
            while (cursor < int(text.size()) && !Char::IsEol(text[cursor]))
            {
                cursor++;
            }

            if (include && cursor < int(text.size()))
            {
                if (text[cursor] == L'\r' && text[cursor + 1] == L'\n')
                {
                    cursor += 2;
                }
                else
                {
                    cursor++;
                }
            }

            return cursor;
        }

        static bool IsStartOfWord(std::wstring const& text, int index, std::locale const& loc = std::locale())
        {
            if (index >= int(text.size()))
                return false;

            if (index > 0 && !std::isspace(text[index - 1], loc))
                return false;

            if (iswalpha(text[index]) || std::isdigit(text[index], loc) || std::ispunct(text[index], loc))
                return true;

            if (Char::IsCharInSeqvence(text[index], L'+', L'-', L'/', L'*', L'=', L'|', L'&', L'~', L'^'))
                return true;

            return false;
        }

        static int PreviousWord(std::wstring const& text, int cursor, std::locale const& loc = std::locale())
        {
            int begin;
            int i;
            int cr;
            int lf;

            lf = LineBegin(text, cursor) - 1;

            if (lf > 0 && text[lf] == L'\n' && text[lf - 1] == L'\r')
            {
                cr = lf - 1;
            }
            else
            {
                cr = lf;
            }

            // if the cursor is at the beginning of the line, return the end of the prev line
            if (cursor - 1 == lf)
                return (cr > 0) ? cr : 0;

            CharClass cc = Char::GetClass(text[cursor - 1]);
            begin = lf + 1;
            i = cursor;

            // skip over the word, punctuation, or run of whitespace
            while (i > begin && Char::GetClass(text[i - 1]) == cc)
            {
                i--;
            }

            // if the cursor was at whitespace, skip back a word too
            if (cc == CharClass::Whitespace && i > begin)
            {
                cc = Char::GetClass(text[i - 1]);
                while (i > begin && Char::GetClass(text[i - 1]) == cc)
                {
                    i--;
                }
            }

            return i;
        }

        static int NextWord(std::wstring const& text, int cursor, std::locale const& loc = std::locale())
        {
            int i, lf, cr;

            cr = LineEnd(text, cursor);

            if (cr < int(text.size()) && text[cr] == '\r' && text[cr + 1] == '\n')
            {
                lf = cr + 1;
            }
            else
            {
                lf = cr;
            }

            // if the cursor is at the end of the line, return the starting offset of the next line
            if (cursor == cr || cursor == lf)
            {
                if (lf < int(text.size()))
                {
                    return lf + 1;
                }

                return cursor;
            }

            CharClass cc = Char::GetClass(text[cursor]);
            i = cursor;

            // skip over the word, punctuation, or run of whitespace
            while (i < cr && Char::GetClass(text[i]) == cc)
            {
                i++;
            }

            // skip any whitespace after the word/punct
            while (i < cr && std::isspace(text[i], loc))
            {
                i++;
            }

            return i;
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant.ToPointer())
                return false;

            if (variant.GetType() == typeid(System::String))
                return *this == *(System::String*)(variant.ToPointer());
            return *this == variant.ToString();
        }
    };
};

#endif