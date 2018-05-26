#                                               -*- cmake -*-
#
#  FindLibdl.cmake: Try to find Libdl
#
#  (C) Copyright 2005-2011 EDF-EADS-Phimeca
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  @author: $LastChangedBy: dutka $
#  @date:   $LastChangedDate: 2010-02-04 16:44:49 +0100 (Thu, 04 Feb 2010) $
#  Id:      $Id: Makefile.am 1473 2010-02-04 15:44:49Z dutka $
#
#
# - Try to find Libdl
# Once done this will define
#
#  LIBDL_FOUND - System has Libdl
#  LIBDL_INCLUDE_DIR - The Libdl include directory
#  LIBDL_LIBRARIES - The libraries needed to use Libdl
#  LIBDL_DEFINITIONS - Compiler switches required for using Libdl

FIND_PATH(LIBDL_INCLUDE_DIR dlfcn.h
   HINTS
        /usr/include
        /usr/local/include
   PATH_SUFFIXES libdl
   )

FIND_LIBRARY(LIBDL_LIBRARIES 
    NAMES 
    dl libdl
   HINTS
        /usr/lib
        /usr/local/lib
   )

if ( LIBDL_LIBRARIES )
  if ( LIBDL_INCLUDE_DIR )
    set( LIBDL_FOUND 1 )
  else ( LIBDL_INCLUDE_DIR )
    message(FATAL_ERROR "Could not find LIBDL headers! Please install LIBDL libraries and headers")
  endif ( LIBDL_INCLUDE_DIR )
endif ( LIBDL_LIBRARIES )

message( ${ASTATUS} "Found LibDL library        : ${LIBDL_LIBRARIES}")
message( ${ASTATUS} "Found LibDL headers        : ${LIBDL_INCLUDE_DIR}")

MARK_AS_ADVANCED(LIBDL_INCLUDE_DIR LIBDL_LIBRARIES)