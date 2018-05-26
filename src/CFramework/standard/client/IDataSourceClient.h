#pragma once
#ifndef __CLIENT_DATASOURCE_INTERFACE_H__
#define __CLIENT_DATASOURCE_INTERFACE_H__

#include <standard/Platform/Common.h>
#include <standard/filesystem.h>
#include <standard/threading.h>

namespace System
{
    namespace Memory
    {
        struct MemoryBlock
        {
            MemoryBlock() : ptr(NULL), size(0) {}
            MemoryBlock(uint8 *p, uint32 s) : ptr(p), size(s) {}
            ~MemoryBlock() { free(); }

            void alloc(uint32 s) { size = s; ptr = new uint8[s]; }
            void free(void)
            {
                if (ptr)
                    delete[] ptr;
            }

            uint8* operator[](int seek)
            {
                if (seek >= size)
                    throw std::runtime_error("invalid operation");
                return &ptr[seek];
            }

            uint8 *ptr;
            uint32 size;
        };
    }

    namespace IO
    {
        struct IFileReader
        {
            std::string FileName;

            virtual ~IFileReader() {}
            virtual unsigned long Seek(size_t offset, bool relative) = 0;
            virtual DWORD Read(byte* lpBuffer, size_t size) = 0;
            virtual byte* Read(size_t size) = 0;
            virtual uint64 Size() = 0;
            virtual void Close() = 0;
            virtual uint64 Position() = 0;

            int Read(Memory::MemoryBlock& block)
            {
                return Read(block.ptr, block.size);
            }

            DWORD Read(Memory::MemoryBlock* mem, size_t size)
            {
                return mem->size = Read(mem->ptr, size);
            }

            DWORD ReadToEnd(Memory::MemoryBlock* mem)
            {
                return mem->size = Read(mem->ptr, Size());
            }

            template<typename T>
            IFileReader& operator >> (T* const& buffer)
            {
                Read((byte*)buffer, sizeof(T));
                return *this;
            }

            template<typename T, unsigned int Size>
            IFileReader& operator >> (T(& buffer)[Size])
            {
                Read((byte*)buffer, sizeof(T) * Size);
                return *this;
            }

            //GetArrLength

            template<typename T>
            IFileReader& operator >> (T& buffer)
            {
                Read((byte*)&buffer, sizeof(T));
                return *this;
            }
        };

        struct IDataSource
        {
            typedef std::list<std::string> TFileList;

            virtual ~IDataSource() {}
            virtual bool HasLocale(int locale) const = 0;
            virtual size_t GetLocaleCount() const = 0;
            virtual const char* GetLocaleName(int locale) const = 0;
            virtual uint32 GetFileID(const char* filename) { return 0; }
            virtual IO::IFileReader* OpenFile(const char* filename, int locale = -1) = 0;
            virtual void ListFiles(int locale, const char* pattern, IDataSource::TFileList& files) = 0;
            //virtual IO::IFileReader* CreateMemoryStream(const char* filename, int length) override
        };
    }
}

#endif