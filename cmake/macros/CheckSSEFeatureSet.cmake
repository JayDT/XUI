###############################################################################
# Check for the presence of SSE and figure out the flags to use for it.

#enum E_INSTRUCTIONS_AVAILABLE
#{
#    SUPPORTS_STANDARD = 0x0000001,
#    SUPPORTS_SSE = 0x0000002,
#    SUPPORTS_SSE2 = 0x0000004,
#    SUPPORTS_SSE3 = 0x0000008,
#    SUPPORTS_SSE4a = 0x0000010,
#    SUPPORTS_SSE41 = 0x0000020,
#    SUPPORTS_SSE42 = 0x0000040,
#    SUPPORTS_AVX1 = 0x0000080,
#    SUPPORTS_AVX2 = 0x0000100
#};

macro(PCL_CHECK_FOR_SSE)
    set(SSE_FLAGS)

    # Test CLANG
    #if(CMAKE_COMPILER_IS_CLANG)
    #  if(APPLE)
    #    SET(SSE_FLAGS "${SSE_FLAGS} -march=native")
    #  endif(APPLE)
    #endif(CMAKE_COMPILER_IS_CLANG)
	
    if(NOT DEFINED CPUID_CACHED)
        try_run(CPUID_FLAG CPUID_COMPILE_RESULT
                ${CMAKE_BINARY_DIR}
                ${CMAKE_SOURCE_DIR}/cmake/cpuid.cpp)
        
        if(NOT ${CPUID_COMPILE_RESULT})
          message(FATAL_ERROR "Failed to compile CPUID check binary")
	    endif()

        set(CPUID_CACHED ${CPUID_FLAG} CACHE INTERNAL "Whether CPUID has run")	
	endif()

	message(STATUS "CPUID_FLAG: ${CPUID_CACHED}")
	
    # Test GCC/G++
    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
        execute_process(COMMAND ${CMAKE_CXX_COMPILER} "-dumpversion"
                        OUTPUT_VARIABLE GCC_VERSION_STRING)
        if(GCC_VERSION_STRING VERSION_GREATER 4.2 AND NOT APPLE AND NOT CMAKE_CROSSCOMPILING)
            SET(SSE_FLAGS "${SSE_FLAGS} -march=native")
            message(STATUS "Using CPU native flags for SSE optimization: ${SSE_FLAGS}")
        endif()
    endif()

    # Unfortunately we need to check for SSE to enable "-mfpmath=sse" alongside 
    # "-march=native". The reason for this is that by default, 32bit architectures
    # tend to use the x87 FPU (which has 80 bit internal precision), thus leading
    # to different results than 64bit architectures which are using SSE2 (64 bit internal
    # precision). One solution would be to use "-ffloat-store" on 32bit (see 
    # http://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html), but that slows code down,
    # so the preferred solution is to try "-mpfmath=sse" first.
    include(CheckCXXSourceRuns)
    set(CMAKE_REQUIRED_FLAGS)

    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
		check_cxx_source_runs("
			#include <mm_malloc.h>
			int main()
			{
			void* mem = _mm_malloc (100, 16);
			return 0;
			}"
			HAVE_MM_MALLOC)
	
		check_cxx_source_runs("
			#include <stdlib.h>
			int main()
			{
			void* mem;
			return posix_memalign (&mem, 16, 100);
			}"
			HAVE_POSIX_MEMALIGN)
    endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
		

    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
        set(CMAKE_REQUIRED_FLAGS "-msse4.2")
    endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)

	math(EXPR haveFeature "${CPUID_CACHED} & 64")
    if (haveFeature)
        check_cxx_source_runs("
            #include <emmintrin.h>
            #include <nmmintrin.h>
            int main ()
            {
              long long a[2] = {  1, 2 };
              long long b[2] = { -1, 3 };
              long long c[2];
              __m128i va = _mm_loadu_si128 ((__m128i*)a);
              __m128i vb = _mm_loadu_si128 ((__m128i*)b);
              __m128i vc = _mm_cmpgt_epi64 (va, vb);
              _mm_storeu_si128 ((__m128i*)c, vc);
              if (c[0] == -1LL && c[1] == 0LL)
                return (0);
              else
                return (1);
            }"
            HAVE_SSE4_2_EXTENSIONS)
    endif (haveFeature)

    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
        set(CMAKE_REQUIRED_FLAGS "-msse4.1")
    endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)

	math(EXPR haveFeature "${CPUID_CACHED} & 32")
	if (haveFeature)
        check_cxx_source_runs("
            #include <smmintrin.h>
            int main ()
            {
              __m128 a, b;
              float vals[4] = {1, 2, 3, 4};
              const int mask = 123;
              a = _mm_loadu_ps (vals);
              b = a;
              b = _mm_dp_ps (a, a, mask);
              _mm_storeu_ps (vals,b);
              return (0);
            }"
            HAVE_SSE4_1_EXTENSIONS)
	endif (haveFeature)

    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
        set(CMAKE_REQUIRED_FLAGS "-msse3")
    endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)

	math(EXPR haveFeature "${CPUID_CACHED} & 8")
	if (haveFeature)
        check_cxx_source_runs("
            #include <pmmintrin.h>
            int main ()
            {
                __m128d a, b;
                double vals[2] = {0};
                a = _mm_loadu_pd (vals);
                b = _mm_hadd_pd (a,a);
                _mm_storeu_pd (vals, b);
                return (0);
            }"
            HAVE_SSE3_EXTENSIONS)
	endif (haveFeature)

    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
        set(CMAKE_REQUIRED_FLAGS "-msse2")
    elseif(MSVC AND NOT CMAKE_CL_64)
        set(CMAKE_REQUIRED_FLAGS "/arch:SSE2")
    endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
    
	math(EXPR haveFeature "${CPUID_CACHED} & 4")
	if (haveFeature)
        check_cxx_source_runs("
            #include <emmintrin.h>
            int main ()
            {
                __m128d a, b;
                double vals[2] = {0};
                a = _mm_loadu_pd (vals);
                b = _mm_add_pd (a,a);
                _mm_storeu_pd (vals,b);
                return (0);
            }"
            HAVE_SSE2_EXTENSIONS)
	endif (haveFeature)

    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
        set(CMAKE_REQUIRED_FLAGS "-msse")
    elseif(MSVC AND NOT CMAKE_CL_64)
        set(CMAKE_REQUIRED_FLAGS "/arch:SSE")
    endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)

	math(EXPR haveFeature "${CPUID_CACHED} & 2")
	if (haveFeature)
        check_cxx_source_runs("
            #include <xmmintrin.h>
            int main ()
            {
                __m128 a, b;
                float vals[4] = {0};
                a = _mm_loadu_ps (vals);
                b = a;
                b = _mm_add_ps (a,b);
                _mm_storeu_ps (vals,b);
                return (0);
            }"
            HAVE_SSE_EXTENSIONS)
	endif(haveFeature)
		
	math(EXPR haveFeature "${CPUID_CACHED} & 128")
	if (haveFeature)
	    PCL_CHECK_FOR_AVX()
	endif(haveFeature)
	
	math(EXPR haveFeature "${CPUID_CACHED} & 256")
	if (haveFeature)
	    PCL_CHECK_FOR_AVX2()
	endif(haveFeature)
	
    set(CMAKE_REQUIRED_FLAGS)

    if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
		if (HAVE_AVX2)
            SET(SSE_FLAGS "${SSE_FLAGS} -mavx2") # after new gcc van fpmath avx -march=corei7-avx
            message(STATUS "Found AVX extensions, using flags: ${SSE_FLAGS}")
		elseif (HAVE_AVX)
            SET(SSE_FLAGS "${SSE_FLAGS} -mavx") # after new gcc van fpmath avx -march=corei7-avx
            message(STATUS "Found AVX extensions, using flags: ${SSE_FLAGS}")
        elseif(HAVE_SSE4_2_EXTENSIONS)
            SET(SSE_FLAGS "${SSE_FLAGS} -msse4.2 -mfpmath=sse")
            message(STATUS "Found SSE4.2 extensions, using flags: ${SSE_FLAGS}")
        elseif(HAVE_SSE4_1_EXTENSIONS)
            SET(SSE_FLAGS "${SSE_FLAGS} -msse4.1 -mfpmath=sse")
            message(STATUS "Found SSE4.1 extensions, using flags: ${SSE_FLAGS}")
        elseif(HAVE_SSE3_EXTENSIONS)
            SET(SSE_FLAGS "${SSE_FLAGS} -msse3 -mfpmath=sse")
            message(STATUS "Found SSE3 extensions, using flags: ${SSE_FLAGS}")
        elseif(HAVE_SSE2_EXTENSIONS)
            SET(SSE_FLAGS "${SSE_FLAGS} -msse2 -mfpmath=sse")
            message(STATUS "Found SSE2 extensions, using flags: ${SSE_FLAGS}")
        elseif(HAVE_SSE_EXTENSIONS)
            SET(SSE_FLAGS "${SSE_FLAGS} -msse -mfpmath=sse")
            message(STATUS "Found SSE extensions, using flags: ${SSE_FLAGS}")
        else()
            # Setting -ffloat-store to alleviate 32bit vs 64bit discrepancies on non-SSE
            # platforms.
            set(SSE_FLAGS "-ffloat-store")
            message(STATUS "No SSE extensions found")
        endif()
    elseif(MSVC AND NOT CMAKE_CL_64)
		if (HAVE_AVX2)
            SET(SSE_FLAGS "${SSE_FLAGS} /arch:AVX2")
            message(STATUS "Found AVX2 extensions, using flags: ${SSE_FLAGS}")
		elseif (HAVE_AVX)
            SET(SSE_FLAGS "${SSE_FLAGS} /arch:AVX")
            message(STATUS "Found AVX extensions, using flags: ${SSE_FLAGS}")
        elseif(HAVE_SSE2_EXTENSIONS)
            SET(SSE_FLAGS "${SSE_FLAGS} /arch:SSE2")
            message(STATUS "Found SSE2 extensions, using flags: ${SSE_FLAGS}")
        elseif(HAVE_SSE_EXTENSIONS)
            SET(SSE_FLAGS "${SSE_FLAGS} /arch:SSE")
            message(STATUS "Found SSE extensions, using flags: ${SSE_FLAGS}")
        endif(HAVE_AVX2)
    endif()
	
	if (HAVE_SSE3_EXTENSIONS)
	  message(STATUS "Have SSE3")
	endif()
   
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SSE_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} ${SSE_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${SSE_FLAGS}")

endmacro(PCL_CHECK_FOR_SSE)

###############################################################################
# Check for the presence of SSE 4.1
macro(PCL_CHECK_FOR_SSE4_1)
  include(CheckCXXSourceRuns)
  set(CMAKE_REQUIRED_FLAGS)

  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
      set(CMAKE_REQUIRED_FLAGS "-msse4.1")
  endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)

  check_cxx_source_runs("
      #include <smmintrin.h>
      int main()
      {
        __m128 a, b;
        float vals[4] = {1, 2, 3, 4};
        const int mask = 123;
        a = _mm_loadu_ps(vals);
        b = a;
        b = _mm_dp_ps (a, a, mask);
        _mm_storeu_ps(vals,b);
        return 0;
      }"
      HAVE_SSE4_1_EXTENSIONS)
endmacro(PCL_CHECK_FOR_SSE4_1)

###############################################################################
# Check for the presence of SSE 3
macro(PCL_CHECK_FOR_SSE3)
  include(CheckCXXSourceRuns)
  set(CMAKE_REQUIRED_FLAGS)

  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)
      set(CMAKE_REQUIRED_FLAGS "-msse3")
  endif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG)

  check_cxx_source_runs("
      #include <pmmintrin.h>
      int main ()
      {
          __m128d a, b;
          double vals[2] = {0};
          a = _mm_loadu_pd (vals);
          b = _mm_hadd_pd (a,a);
          _mm_storeu_pd (vals, b);
          return (0);
      }"
      HAVE_SSE3_EXTENSIONS)
endmacro(PCL_CHECK_FOR_SSE3)

macro(PCL_CHECK_FOR_AVX)
  #include(CheckCXXSourceRuns)
  #set(CMAKE_REQUIRED_FLAGS)
	
  #
  # Check compiler for AVX intrinsics
  #
  check_cxx_source_runs("
  #include <immintrin.h>
  int main()
  {
   __m256i a = _mm256_setzero_si256();
   return 0;
  }"
  HAVE_AVX)
endmacro(PCL_CHECK_FOR_AVX)

macro(PCL_CHECK_FOR_AVX2)
  #include(CheckCXXSourceRuns)
  #set(CMAKE_REQUIRED_FLAGS)
	
  #
  # Check compiler for AVX2 intrinsics
  #
  check_cxx_source_runs("
  #include <immintrin.h>
  int main()
  {
    try
	{
       int bytes = 0x01;
       int shift = 1;
       int kMul = 0x1e35a7bd;
       __m256i a, b, c, resShift, shift_256i;
       a = _mm256_set1_epi32(bytes);
       b = _mm256_set1_epi32(kMul);
       shift_256i = _mm256_set1_epi32(shift);
       c = _mm256_mul_epi32(a, b);
       resShift = _mm256_srlv_epi64(c, shift_256i);
       __m128i lower = _mm256_extracti128_si256(resShift, 0);
	} catch(...) {}
    return 0;
  }"
  HAVE_AVX2)
  
  message(STATUS "Test AVX2 ${HAVE_AVX2}")
endmacro(PCL_CHECK_FOR_AVX2)

PCL_CHECK_FOR_SSE()
#PCL_CHECK_FOR_SSE3()
#PCL_CHECK_FOR_SSE4_1()