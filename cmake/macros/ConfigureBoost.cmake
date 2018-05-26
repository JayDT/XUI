#CONFIG
SET(BOOST_PACKAGE_NAME "boost_1_67_0")
SET(BOOST_TARBALL "${BOOST_PACKAGE_NAME}.tar.gz")
SET(BOOST_DOWNLOAD_URL
  "http://sourceforge.net/projects/boost/files/boost/1.67.0/${BOOST_TARBALL}"
  )
SET(OLD_PACKAGE_NAMES "boost_1_55_0 boost_1_56_0 boost_1_57_0 boost_1_58_0 boost_1_59_0")
SET(OLD_WITH_BOOST ${WITH_BOOST} CACHE INTERNAL
  "Previous version of WITH_BOOST" FORCE)

if (UNIX)
    SET(Boost_USE_STATIC_LIBS        OFF)
ELSE(WIN32)
    SET(Boost_USE_STATIC_LIBS        ON)
ENDIF(UNIX)

SET(Boost_USE_MULTITHREADED      ON)
SET(Boost_USE_STATIC_RUNTIME     OFF)

IF (DEBUG GREATER 1)
	SET(Boost_USE_DEBUG_RUNTIME		 ON)
ELSE()
	SET(Boost_USE_DEBUG_RUNTIME		 OFF)
ENDIF()

if( WIN32 )
    SET(MSVC_SHORT_VER "<unk>")
    if (${MSVC_VERSION} GREATER 2000)
    	message(FATAL_ERROR "Unsupported Visual studio ${MSVC_VERSION}")
    elseif (${MSVC_VERSION} GREATER 1900)
    	SET(MSVC_SHORT_VER "14.1")
    elseif (${MSVC_VERSION} EQUAL 1900)
    	SET(MSVC_SHORT_VER "14.0")
    elseif (${MSVC_VERSION} EQUAL 1800)
    	SET(MSVC_SHORT_VER "12.0")
    elseif (${MSVC_VERSION} EQUAL 1700)
    	SET(MSVC_SHORT_VER "11.0")
    elseif (${MSVC_VERSION} EQUAL 1600)
    	SET(MSVC_SHORT_VER "10.0")
    elseif (${MSVC_VERSION} EQUAL 1500)
    	SET(MSVC_SHORT_VER "9.0")
    elseif (${MSVC_VERSION} EQUAL 1400)
    	SET(MSVC_SHORT_VER "8.0")
    else()
    	message(FATAL_ERROR "Unsupported Visual studio")
    endif()
ENDIF()
  
#CODE
# There is a similar option in unittest/gunit.
# But the boost tarball is much bigger, so we have a separate option.
SET(DOWNLOAD_BOOST_TIMEOUT 600 CACHE STRING
  "Timeout in seconds when downloading boost.")

macro(get_WIN32_WINNT version)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REPLACE "." "" ver ${ver})
        string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})

        set(${version} "0x${ver}")
    endif()
endmacro()

MACRO(RESET_BOOST_VARIABLES)
  UNSET(BOOST_INCLUDE_DIR)
  UNSET(BOOST_INCLUDE_DIR CACHE)
  UNSET(LOCAL_BOOST_DIR)
  UNSET(LOCAL_BOOST_DIR CACHE)
  UNSET(LOCAL_BOOST_ZIP)
  UNSET(LOCAL_BOOST_ZIP CACHE)
ENDMACRO()

MACRO(ECHO_BOOST_VARIABLES)
  MESSAGE(STATUS "BOOST_INCLUDE_DIR ${BOOST_INCLUDE_DIR}")
  MESSAGE(STATUS "LOCAL_BOOST_DIR ${LOCAL_BOOST_DIR}")
  MESSAGE(STATUS "LOCAL_BOOST_ZIP ${LOCAL_BOOST_ZIP}")
ENDMACRO()

MACRO(LOOKUP_OLD_PACKAGE_NAMES)
  FOREACH(pkg ${OLD_PACKAGE_NAMES})
    FIND_FILE(OLD_BOOST_DIR
              NAMES "${pkg}"
              PATHS ${WITH_BOOST}
              NO_DEFAULT_PATH
             )
    IF(OLD_BOOST_DIR)
      MESSAGE(STATUS "")
      MESSAGE(STATUS "Found old boost installation at ${OLD_BOOST_DIR}")
      MESSAGE(STATUS "You must upgrade to ${BOOST_PACKAGE_NAME}")
      MESSAGE(STATUS "")
    ENDIF()
    UNSET(OLD_BOOST_DIR)
    UNSET(OLD_BOOST_DIR CACHE)
  ENDFOREACH()
ENDMACRO()

MACRO(COULD_NOT_FIND_BOOST)
  LOOKUP_OLD_PACKAGE_NAMES()
  ECHO_BOOST_VARIABLES()
  RESET_BOOST_VARIABLES()
  MESSAGE(STATUS "Could not find (the correct version of) boost.")
  MESSAGE(STATUS "World Server currently requires ${BOOST_PACKAGE_NAME}\n")
  MESSAGE(FATAL_ERROR
    "You can download it with -DDOWNLOAD_BOOST=1 -DWITH_BOOST=<directory>\n"
    "This CMake script will look for boost in <directory>. "
    "If it is not there, it will download and unpack it "
    "(in that directory) for you.\n"
    "If you are inside a firewall, you may need to use an http proxy:\n"
    "export http_proxy=http://example.com:80\n"
    )
ENDMACRO()

MACRO(DOWNLOAD_BOOST)

IF(NOT WITH_BOOST) #Set Default Location (Comfort Case)
	SET(WITH_BOOST "C:/local")
ENDIF()

# If we could not find it, then maybe download it.
IF(NOT EXISTS "${WITH_BOOST}/${BOOST_PACKAGE_NAME}")	
  MESSAGE(STATUS "Boost source not found. Here: ${WITH_BOOST}/${BOOST_PACKAGE_NAME}")
	
  IF (NOT EXISTS "${WITH_BOOST}/${BOOST_TARBALL}")
	IF(NOT DOWNLOAD_BOOST)
		MESSAGE(STATUS "WITH_BOOST=${WITH_BOOST}")
		COULD_NOT_FIND_BOOST()
	ENDIF()
	
	MESSAGE(STATUS "Boost package not found. ${WITH_BOOST}/${BOOST_TARBALL}")
	
	SET(ERR 0)
	
	# Download the tarball
	MESSAGE(STATUS "Downloading ${BOOST_TARBALL} to ${WITH_BOOST}")
	FILE(DOWNLOAD ${BOOST_DOWNLOAD_URL}
		${WITH_BOOST}/${BOOST_TARBALL}
		TIMEOUT ${DOWNLOAD_BOOST_TIMEOUT}
		STATUS ERR
		SHOW_PROGRESS
	)
	IF(ERR EQUAL 0)
		SET(LOCAL_BOOST_ZIP "${WITH_BOOST}/${BOOST_TARBALL}")
		SET(LOCAL_BOOST_ZIP "${WITH_BOOST}/${BOOST_TARBALL}" CACHE INTERNAL "")
	ELSE()
		MESSAGE(STATUS "Download failed, error: ${ERR}")
		# A failed DOWNLOAD leaves an empty file, remove it
		FILE(REMOVE ${WITH_BOOST}/${BOOST_TARBALL})
		# STATUS returns a list of length 2
		LIST(GET ERR 0 NUMERIC_RETURN)
		IF(NUMERIC_RETURN EQUAL 28)
		MESSAGE(FATAL_ERROR
			"You can try downloading ${BOOST_DOWNLOAD_URL} manually"
			" using curl/wget or a similar tool,"
			" or increase the value of DOWNLOAD_BOOST_TIMEOUT"
			" (which is now ${DOWNLOAD_BOOST_TIMEOUT} seconds)"
		)
		ENDIF()
		MESSAGE(FATAL_ERROR
		"You can try downloading ${BOOST_DOWNLOAD_URL} manually"
		" using curl/wget or a similar tool"
		)
	ENDIF()
  ENDIF()
  
  GET_FILENAME_COMPONENT(LOCAL_BOOST_DIR ${LOCAL_BOOST_ZIP} PATH)
  IF(NOT EXISTS "${LOCAL_BOOST_DIR}/${BOOST_PACKAGE_NAME}")
    MESSAGE(STATUS "cd ${LOCAL_BOOST_DIR}; tar xfz ${LOCAL_BOOST_ZIP}")
    EXECUTE_PROCESS(
      COMMAND ${CMAKE_COMMAND} -E tar xfz "${LOCAL_BOOST_ZIP}"
      WORKING_DIRECTORY "${LOCAL_BOOST_DIR}"
      RESULT_VARIABLE tar_result
      )
    IF (tar_result MATCHES 0)
      SET(BOOST_FOUND 1 CACHE INTERNAL "")
    ELSE()
      MESSAGE(STATUS "WITH_BOOST ${WITH_BOOST}.")
      MESSAGE(STATUS "Failed to extract files.\n"
        "   Please try downloading and extracting yourself.\n"
        "   The url is: ${BOOST_DOWNLOAD_URL}")
      MESSAGE(FATAL_ERROR "Giving up.")
    ENDIF()
  ENDIF()
ELSE()
  MESSAGE(STATUS "Boost source found. Here: ${WITH_BOOST}/${BOOST_PACKAGE_NAME}")
ENDIF()

# CHECK VALID CONTEND
# DOWNLOAD AND EXTRACT STAGE DONE (Start Compile)

# TOOL-SET
  MESSAGE(STATUS "Boost Compile With MSVC-${MSVC_SHORT_VER}")

  SET(BOOST_MODIFIERS "architecture=x86")
  IF (PLATFORM EQUAL 64)
    SET(ARCHITECT " address-model=64")
    SET(BOOST_MODIFIERS "${BOOST_MODIFIERS} address-model=64 ")
  ELSE(PLATFORM EQUAL 64)
    SET(ARCHITECT "")
  ENDIF(PLATFORM EQUAL 64)

  MESSAGE (STATUS "Arch Model=${ARCHITECT}")

  #SET(BOOST_MODIFIERS "${BOOST_MODIFIERS}link=static,shared")

  IF (Boost_USE_STATIC_RUNTIME)
    SET(BOOST_MODIFIERS "${BOOST_MODIFIERS}runtime-link=static")
  ELSE()
      SET(BOOST_MODIFIERS "${BOOST_MODIFIERS}runtime-link=shared")
  ENDIF()

  #IF (Boost_USE_STATIC_LIBS)
  #  SET(BOOST_MODIFIERS "${BOOST_MODIFIERS} link=static,shared")
  #ELSE()
  #  SET(BOOST_MODIFIERS "${BOOST_MODIFIERS} link=shared")
  #ENDIF()

  IF (Boost_USE_MULTITHREADED)
    SET(BOOST_MODIFIERS "${BOOST_MODIFIERS} threading=multi")
  ELSE()
    SET(BOOST_MODIFIERS "${BOOST_MODIFIERS} threading=single")
  ENDIF()

IF (NOT BOOST_CONFIGURED EQUAL 1)  
  EXECUTE_PROCESS(
    COMMAND bootstrap.bat --libdir=${WITH_BOOST}/${BOOST_PACKAGE_NAME}/lib${PLATFORM}-msvc-${MSVC_SHORT_VER} --link=static,shared --threading=multi --runtime-link=shared
    WORKING_DIRECTORY "${WITH_BOOST}/${BOOST_PACKAGE_NAME}/"
    RESULT_VARIABLE bootstrap_result
    )
  
  IF (NOT bootstrap_result MATCHES 0)
	message(FATAL_ERROR "Bootstrap Command ${WITH_BOOST}/${BOOST_PACKAGE_NAME}/bootstrap.bat Result ${bootstrap_result}")
  ENDIF()

  SET(BOOST_CONFIGURED 1 CACHE INTERNAL "")  
ENDIF()

    SET(BOOST_COMMAND "${WITH_BOOST}/${BOOST_PACKAGE_NAME}/b2.exe link=static,shared threading=multi runtime-link=shared -j8 --without-python --without-mpi")
    SET(ARCHITECT " ${ARCHITECT} ")

  IF (PLATFORM EQUAL 64)
    message(STATUS "BJAM Command ${WITH_BOOST}/${BOOST_PACKAGE_NAME}/b2.exe link=static,shared architecture=x86 address-model=64 threading=multi runtime-link=shared toolset=msvc-${MSVC_SHORT_VER} --libdir=${WITH_BOOST}/${BOOST_PACKAGE_NAME}/lib${PLATFORM}-msvc-${MSVC_SHORT_VER} --includedir=${WITH_BOOST}/${BOOST_PACKAGE_NAME}/include -j8 --build-type=complete --without-python --without-mpi install")
  EXECUTE_PROCESS(COMMAND 
    ${WITH_BOOST}/${BOOST_PACKAGE_NAME}/b2.exe link=static,shared architecture=x86 address-model=64 threading=multi runtime-link=shared toolset=msvc-${MSVC_SHORT_VER} --libdir=${WITH_BOOST}/${BOOST_PACKAGE_NAME}/lib${PLATFORM}-msvc-${MSVC_SHORT_VER} --includedir=${WITH_BOOST}/${BOOST_PACKAGE_NAME}/include -j8 --build-type=complete --without-python --without-mpi install
    WORKING_DIRECTORY "${WITH_BOOST}/${BOOST_PACKAGE_NAME}/" RESULT_VARIABLE bjam_result) 	
  ELSE(PLATFORM EQUAL 64)
    message(STATUS "BJAM Command ${WITH_BOOST}/${BOOST_PACKAGE_NAME}/b2.exe link=static,shared architecture=x86 address-model=32 threading=multi runtime-link=shared toolset=msvc-${MSVC_SHORT_VER} --libdir=${WITH_BOOST}/${BOOST_PACKAGE_NAME}/lib${PLATFORM}-msvc-${MSVC_SHORT_VER} --includedir=${WITH_BOOST}/${BOOST_PACKAGE_NAME}/include -j8 --build-type=complete --without-python --without-mpi install")
  EXECUTE_PROCESS(COMMAND
    ${WITH_BOOST}/${BOOST_PACKAGE_NAME}/b2.exe link=static,shared architecture=x86 threading=multi runtime-link=shared toolset=msvc-${MSVC_SHORT_VER} --libdir=${WITH_BOOST}/${BOOST_PACKAGE_NAME}/lib${PLATFORM}-msvc-${MSVC_SHORT_VER} --includedir=${WITH_BOOST}/${BOOST_PACKAGE_NAME}/include -j8 --build-type=complete --without-python --without-mpi install
  WORKING_DIRECTORY "${WITH_BOOST}/${BOOST_PACKAGE_NAME}/" RESULT_VARIABLE bjam_result) 	
  ENDIF(PLATFORM EQUAL 64)
  
  #TODO: CHECK CORE AMOUNT

  IF (NOT bjam_result MATCHES 0)
	message(FATAL_ERROR "Result ${bjam_result}")
  ENDIF()

  SET(SelectedBoost "${WITH_BOOST}/${BOOST_PACKAGE_NAME}")
  
ENDMACRO()


if(WIN32)
  set(BOOST_DEBUG ON)

  if(DEFINED ENV{BOOST_ROOT})
    set(BOOST_ROOT $ENV{BOOST_ROOT})
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib${PLATFORM}-msvc-${MSVC_SHORT_VER})
  elseif (BOOST_ROOT)
    message("Use Boost Root from input:" ${BOOST_ROOT})
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib${PLATFORM}-msvc-${MSVC_SHORT_VER})
  else()
	message("-- Finding installation.")  
	message("-- Find location: c:, c:/local")
	
	SUBDIRLIST(RootDir c:)
	SUBDIRLIST(LocalDir c:/local)
	LIST(APPEND RootDir ${LocalDir})
		
	SET(BoostDirList "")
	FOREACH(child ${RootDir})
		IF (${child} MATCHES ".*/boost.*")		
			LIST(APPEND BoostDirList ${child})
		ENDIF()
	ENDFOREACH()


	message("-- Find Lib Location: /lib${PLATFORM}-msvc-${MSVC_SHORT_VER}")
	
	SET(SelectedBoost "")
	FOREACH(dirList ${BoostDirList})
		SUBDIRLIST(BoostLibSubDir ${dirList})
		FOREACH(child ${BoostLibSubDir})
		    message (${ASTATUS} "SubDir:  ${child} finding /lib${PLATFORM}-msvc-${MSVC_SHORT_VER}-MT")
			IF (${child} MATCHES ".*/lib${PLATFORM}-msvc-${MSVC_SHORT_VER}")		
				LIST(APPEND SelectedBoost ${dirList})
				BREAK()
			ENDIF()
		ENDFOREACH()
	ENDFOREACH()

	list(LENGTH SelectedBoost SelectedBoostSize)	

	if (SelectedBoostSize GREATER 1)
		SET(CurrBoost "")
		SET(HighestVer 0)
		FOREACH(Selected ${SelectedBoost})
			string(REGEX REPLACE ".*boost_([0-9]*)_([0-9]*)_([0-9]*)"
			"\\1\\2\\3"
			BoostVersion "${Selected}")	
			if (${HighestVer} LESS ${BoostVersion})
				SET(CurrBoost ${Selected})
				SET(HighestVer ${BoostVersion})
			endif()
		ENDFOREACH()
		SET(SelectedBoost ${CurrBoost})
	elseif (SelectedBoostSize EQUAL 0)
		DOWNLOAD_BOOST() # Last Chance we try download
		#message(FATAL_ERROR "No BOOST_ROOT environment variable could be found! Please make sure it is set and the points to your Boost installation.")
	endif()

    set(BOOST_ROOT "${SelectedBoost}")
    set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib${PLATFORM}-msvc-${MSVC_SHORT_VER})
  
  message ("Found: Root -> ${BOOST_ROOT} Lib -> ${BOOST_LIBRARYDIR}")
  endif()

  get_WIN32_WINNT(ver)
  add_definitions(-D_WIN32_WINNT=${ver})
endif()

find_package(Boost 1.55 REQUIRED atomic system filesystem thread program_options iostreams regex)
add_definitions(-DBOOST_DATE_TIME_NO_LIB)
add_definitions(-DBOOST_REGEX_NO_LIB)
add_definitions(-DBOOST_CHRONO_NO_LIB)

# Find if Boost was compiled in C++03 mode because it requires -DBOOST_NO_CXX11_SCOPED_ENUMS

include (CheckCXXSourceCompiles)

set(CMAKE_REQUIRED_INCLUDES ${Boost_INCLUDE_DIR})
set(CMAKE_REQUIRED_LIBRARIES ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_IOSTREAMS_LIBRARY})
#set(CMAKE_REQUIRED_FLAGS "-std=c++11")
unset(boost_filesystem_copy_links_without_NO_SCOPED_ENUM CACHE)
check_cxx_source_compiles("
  #include <boost/filesystem/path.hpp>
  #include <boost/filesystem/operations.hpp>
  int main() { boost::filesystem::copy_file(boost::filesystem::path(), boost::filesystem::path()); }"
boost_filesystem_copy_links_without_NO_SCOPED_ENUM)
unset(CMAKE_REQUIRED_INCLUDES CACHE)
unset(CMAKE_REQUIRED_LIBRARIES CACHE)
unset(CMAKE_REQUIRED_FLAGS CACHE)

if (NOT boost_filesystem_copy_links_without_NO_SCOPED_ENUM)
  if (Boost_VERSION LESS 105100) # 1.51
    add_definitions(-DBOOST_NO_SCOPED_ENUMS)
  else()
    add_definitions(-DBOOST_NO_CXX11_SCOPED_ENUMS)
  endif()
endif()

#if(Boost_FOUND)
#  include_directories(${Boost_INCLUDE_DIRS})
#endif()
