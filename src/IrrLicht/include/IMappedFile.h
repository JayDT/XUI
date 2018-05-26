// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_MAPPED_FILE_H_INCLUDED__
#define __I_MAPPED_FILE_H_INCLUDED__

#include "IReferenceCounted.h"
#include "coreutil.h"

namespace irr
{
namespace io
{
	
	//! Interface providing access to a file mapped to virtual memory
	class IMappedFile : public virtual IReferenceCounted
	{
	public:
		//! Get pointer to data
		/** \return Pointer to file's data mapped to virtual memory */
		virtual void* getDataPointer() = 0;

		//! Get const pointer to data
		/** \return Const pointer to file's data mapped to virtual memory */
		virtual const void* getDataPointer() const =0;

		//! Get size of file.
		/** \return Size of the file in bytes. */
		virtual long getSize() const = 0;

		//! Flush all modification from memory to disk
		/** \return True is flush succedded, false otherwise */
		virtual bool flush() =0;

		//! Get name of file.
		/** \return File name as zero terminated character string. */
		virtual const path& getFileName() const = 0;
	};

	//! Internal function, please do not use.
	IMappedFile* createMemoryMappedFile(const io::path& fileName, bool readOnly);

}
}

#endif