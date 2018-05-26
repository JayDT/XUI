#pragma once
#ifndef _WEBCLIENT_H__
#define _WEBCLIENT_H__

#include "libcurl/curl.h"

namespace System
{
    namespace Net
    {
        class TC_CFRAMEWORK_API WebClient
        {
        private:
            std::string m_url;
            std::vector<char> m_data;
            CURL* m_curl_handle;
            CURLcode m_result;

        public:
            WebClient();
            ~WebClient();

            double GetInfo(CURLINFO _info);
            void WriteDebugInfo();
            void Query(std::string _url);
            bool DownloadFile(std::string _url, std::string _targetFile);
            long GetResponseCode();

        public:
            std::string GetString() { return std::string(&m_data[0]); }
            std::vector<char> GetData() { return m_data; }
        };
    }
}
#endif //_WEBCLIENT_H__