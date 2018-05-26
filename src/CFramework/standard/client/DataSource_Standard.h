#pragma once
#ifndef __C_DATASOURCE_CASC_H__
#define __C_DATASOURCE_CASC_H__

#include <stdio.h>
#include <standard/client/IDataSourceClient.h>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>

namespace System
{
    namespace IO
    {
        struct StandardFile : IFileReader
        {
            FILE* _file_resource;

            StandardFile() : _file_resource(nullptr)
            {

            }

            virtual ~StandardFile()
            {
                Close();
            }

            virtual void Close() override
            {
                if (_file_resource)
                    fclose(_file_resource);
                _file_resource = nullptr;
            }

            virtual uint64 Size() override
            {
                int current_pos = ftell(_file_resource);
                fseek(_file_resource, 0, SEEK_END);
                int end_position = ftell(_file_resource);
                fseek(_file_resource, current_pos, SEEK_SET);
                return end_position;
            }

            virtual uint64 Position()
            {
                return ftell(_file_resource);
            }

            virtual unsigned long Seek(size_t offset, bool relative) override
            {
                return fseek(_file_resource, offset, relative ? SEEK_CUR : SEEK_SET);
            }

            virtual DWORD Read(byte* lpBuffer, size_t size) override
            {
                DWORD readBytes = 0;
                return fread(lpBuffer, size, 1, _file_resource);
            }

            virtual byte* Read(size_t size) override
            {
                byte* dataBlock = new byte[size];
                if (Read(dataBlock, size) == size)
                    return dataBlock;
                delete[] dataBlock;
                return nullptr;
            }
        };

        struct MemoryStreamReader : IFileReader
        {
            typedef boost::iostreams::basic_array_source<byte> Device;
            boost::iostreams::stream<Device> _stream;
            size_t length;

            MemoryStreamReader(byte* buffer, size_t length) : _stream(buffer, length), length(length)
            {

            }

            virtual ~MemoryStreamReader()
            {
                Close();
            }

            virtual void Close() override
            {
                _stream.clear();
            }

            virtual uint64 Size() override
            {
                return length;
            }

            virtual uint64 Position()
            {
                return _stream.tellg();
            }

            virtual unsigned long Seek(size_t offset, bool relative) override
            {
                _stream.seekg(offset, int(relative ? std::ios_base::cur : std::ios_base::beg));
                return Position();
            }

            virtual DWORD Read(byte* lpBuffer, size_t size) override
            {
                DWORD readBytes = 0;
                _stream.read(lpBuffer, size);
                return size;
            }

            virtual byte* Read(size_t size) override
            {
                byte* dataBlock = new byte[size];
                if (Read(dataBlock, size) == size)
                    return dataBlock;
                delete[] dataBlock;
                return nullptr;
            }
        };

        class StandardDataSource : public IDataSource
        {
        public:
            virtual bool HasLocale(int locale) const { return false; }
            size_t GetLocaleCount() const override final { return 0; }
            const char* GetLocaleName(int locale) const override final { return nullptr; }

            IO::IFileReader* OpenFile(const char* filename, int locale = -1) override
            {
                FILE* f = fopen(filename, "rb");
                if (!f)
                    return nullptr;

                StandardFile* _file = new StandardFile;
                _file->_file_resource = f;
                _file->FileName = filename;
                return _file;
            }

            void ListFiles(int locale, const char* pattern, IDataSource::TFileList& files) override
            {

            }
        };
    }
}

#endif
