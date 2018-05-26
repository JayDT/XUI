#include "stdafx.h"
#include "CURL.h"
#include "Log.h"

using namespace System::Net;

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;

    mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
   
    return realsize;
}

WebClient::WebClient()
{
    /* init the curl session */
    m_curl_handle = curl_easy_init();
}

WebClient::~WebClient()
{
    /* cleanup curl stuff */
    curl_easy_cleanup(m_curl_handle);
}

double WebClient::GetInfo(CURLINFO _info)
{
    if (m_result != CURLE_OK)
        return 0.0;

    double val = 0.0;
    CURLcode res = curl_easy_getinfo(m_curl_handle, _info, &val);
    if ((CURLE_OK == res) && (val > 0))
        return val;

    return 0.0;
}

void WebClient::WriteDebugInfo()
{
    double val;
    /* check for bytes downloaded */
    val = GetInfo(CURLINFO_SIZE_DOWNLOAD);
    if (val > 0)
        printf("Data downloaded: %0.0f bytes.\n", val);

    /* check for total download time */
    val = GetInfo(CURLINFO_TOTAL_TIME);
    if (val > 0)
        printf("Total download time: %0.3f sec.\n", val);

    /* check for average download speed */
    val = GetInfo(CURLINFO_SPEED_DOWNLOAD);
    if (val > 0)
        printf("Average download speed: %0.3f kbyte/sec.\n", val / 1024);

    if (true) {
        /* check for name resolution time */
        val = GetInfo(CURLINFO_NAMELOOKUP_TIME);
        if (val > 0)
            printf("Name lookup time: %0.3f sec.\n", val);

        /* check for connect time */
        val = GetInfo(CURLINFO_CONNECT_TIME);
        if (val > 0)
            printf("Connect time: %0.3f sec.\n", val);
    }
}

void WebClient::Query(std::string _url)
{
    m_url = _url;

    struct MemoryStruct chunk;
    chunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    /* specify URL to get */
    curl_easy_setopt(m_curl_handle, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(m_curl_handle, CURLOPT_NOSIGNAL, 1);

    //curl_easy_setopt(m_curl_handle, CURLOPT_HTTPHEADER, "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7");
    /* send all data to this function  */
    curl_easy_setopt(m_curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(m_curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(m_curl_handle, CURLOPT_ACCEPT_ENCODING, "gzip, deflate");

    /* some servers don't like requests that are made without a user-agent
    field, so we provide one */
    curl_easy_setopt(m_curl_handle, CURLOPT_USERAGENT,
        "Mozilla/5.0 (Windows NT 10.0; WOW64; rv:39.0) Gecko/20100101 Firefox/39.0");

    curl_easy_setopt(m_curl_handle, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(m_curl_handle, CURLOPT_SSL_VERIFYPEER, false);

    /* get it! */
    m_result = curl_easy_perform(m_curl_handle);

    if (CURLE_OK == m_result)
    {
        //WriteDebugInfo();
    }
    else {
        fprintf(stderr, "Error while fetching '%s' : %s\n",
            m_url.c_str(), curl_easy_strerror(m_result));
    }

    m_data.reserve(chunk.size);

    m_data = std::vector<char>(chunk.memory, chunk.memory + chunk.size + 1);
    if (m_data.size() != chunk.size + 1)
    {
        System::Console()->WriteErrorLine("Vector meret: %d Eredeti meret: %d", m_data.size(), chunk.size);
    }
    free(chunk.memory);
    //return m_data;
}

bool WebClient::DownloadFile(std::string _url, std::string _targetFile)
{
    Query(_url);
    if (CURLE_OK != m_result)
    {
        System::Console()->WriteErrorLine("Error download file %s from url: %s", _targetFile.c_str(),  _url.c_str());
        return false;
    }
    FILE* file = fopen(_targetFile.c_str(), "wb");
    if (file)
    {
        // A \0 lezaro nem kell bele, ezert kell kivonni 1-et
        fwrite(&m_data[0], m_data.size() - 1, 1, file);
        fclose(file);
    }
    return true;
}

long WebClient::GetResponseCode()
{
    long respcode; //response code of the http transaction
    curl_easy_getinfo(m_curl_handle, CURLINFO_RESPONSE_CODE, &respcode);// grabbing it from curl
    return respcode;
}