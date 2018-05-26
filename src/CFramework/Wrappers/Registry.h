#ifndef _REGISTRY_H_
#define _REGISTRY_H_

#ifdef WIN32
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101
#endif

#include <minwindef.h>
#include <string>

namespace System
{
    namespace Wrappers
    {
        class CRegistry
        {
        public: // Constructor/Destructor
            CRegistry(HKEY hRoot, PCWSTR pszSubKey, REGSAM samDesired = KEY_ALL_ACCESS, bool blCreateIfNotExist = true);
            CRegistry(HKEY hRoot, PCSTR pszSubKey, REGSAM samDesired = KEY_ALL_ACCESS, bool blCreateIfNotExist = true);
            CRegistry(const CRegistry& that);
            virtual ~CRegistry();
        public: // Modify Data Operator
            CRegistry& __fastcall operator= (const CRegistry& that);
        public: // Write Value Method
            DWORD WriteBinaryValue(LPCVOID lpValue, DWORD dwLength, PCWSTR pszValueName = NULL) const;
            DWORD WriteBinaryValue(LPCVOID lpValue, DWORD dwLength, PCSTR pszValueName = NULL) const;
            DWORD WriteDoubleWordValue(DWORD dwValue, PCWSTR pszValueName = NULL) const;
            DWORD WriteDoubleWordValue(DWORD dwValue, PCSTR pszValueName = NULL) const;
            DWORD WriteStringValue(PCWSTR pszValue, PCWSTR pszValueName = NULL) const;
            DWORD WriteStringValue(PCSTR pszValue, PCSTR pszValueName = NULL) const;
        public: // Read Value Method
            DWORD ReadBinaryValue(LPVOID lpValue, DWORD dwSize, PCWSTR pszValueName = NULL, PDWORD pType = NULL) const;
            DWORD ReadBinaryValue(LPVOID lpValue, DWORD dwSize, PCSTR pszValueName = NULL, PDWORD pType = NULL) const;
            DWORD ReadDoubleWordValue(PDWORD pValue, PCWSTR pszValueName = NULL, PDWORD pType = NULL) const;
            DWORD ReadDoubleWordValue(PDWORD pValue, PCSTR pszValueName = NULL, PDWORD pType = NULL) const;
            DWORD ReadStringValue(std::wstring& strValue, PCWSTR pszValueName = NULL, PDWORD pType = NULL) const;
            DWORD ReadStringValue(std::string& strValue, PCSTR pszValueName = NULL, PDWORD pType = NULL) const;
        protected: // Registry Key Handle
            HKEY	m_hKey;
            HKEY	m_hUser;
        };

        DWORD GetRegistryUIntValue(CRegistry* RegHandle, const char* rId, DWORD defaultValue /*= 0*/);

        std::string GetRegistryStringValue(CRegistry* RegHandle, const char* rId, const char* defaultValue /*= ""*/);
    };

};
#endif /* WIN32 */
#endif /* _REGISTRY_H_ */
