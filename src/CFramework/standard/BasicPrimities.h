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

//
// Simple and fast atof (ascii to float) function.
//
// - Executes about 5x faster than standard MSCRT library atof().
// - An attractive alternative if the number of calls is in the millions.
// - Assumes input is a proper integer, fraction, or scientific format.
// - Matches library atof() to 15 digits (except at extreme exponents).
// - Follows atof() precedent of essentially no error checking.
//
// 09-May-2009 Tom Van Baak (tvb) www.LeapSecond.com
//

#ifndef __BasicPrimites__
#define __BasicPrimites__

#include "Reflection/Runtime/MetaProperty.h"
#include "standard/Preprocessor/pp_platform.h"

#define nameof STRINGIFY

#ifdef __REFLECTION_PARSER__

#define __PROPERTY_DECL(getter, setter, type, n) Meta(PropertyType, Getter(#getter), Setter(#setter)) type n
#define __PROPERTY_R_DECL(getter, type, n)  Meta(PropertyType, Getter(#getter)) type n
#define __PROPERTY_W_DECL(setter, type, n)  Meta(PropertyType, Setter(#setter)) type n

#else

#define __PROPERTY_DECL(getter, setter, type, n) __declspec( property (get = getter, put = setter)) type n
#define __PROPERTY_R_DECL(getter, type, n) __declspec( property (get = getter)) type n
#define __PROPERTY_W_DECL(setter, type, n) __declspec( property (put = setter)) type n

#endif __REFLECTION_PARSER__

#pragma region Depricated Property Marcros

#define __PROPERTY_GET_PREFIX Get
#define __PROPERTY_SET_PREFIX Set

#define M_PGETTER(n) Get##n
#define M_PSETTER(n) Set##n

#define PGETTER(n, t) t Get##n() 
#define PSETTER(n, t) void Set##n(const t& value) 

#define PGETTER_EX(n, t) t Get##n() 
#define PSETTER_EX(n, t) void Set##n(const std::remove_reference<std::remove_const<t>::type>::type& value) 

#define PURE_VIRTUAL_PROPERTY(t, n) __PROPERTY_DECL(Get##n, Set##n, t, n); \
        virtual PGETTER(n, t) = 0;\
        virtual PSETTER(n, t) = 0;

#define PURE_VIRTUAL_W_PROPERTY(t, n) __PROPERTY_W_DECL(Set##n, t, n); \
        virtual PSETTER(n, t) = 0;

#define PURE_VIRTUAL_R_PROPERTY(t, n) __PROPERTY_R_DECL(Get##n, t, n); \
        virtual PGETTER(n, t) = 0;

#define PURE_VIRTUAL_AUTO_PROPERTY(t, n) __PROPERTY_DECL(Get##n, Set##n, decltype(r), n); \
        virtual PGETTER(n, decltype(r)) = 0; \
        virtual PSETTER(n, decltype(r)) = 0;

#define PURE_VIRTUAL_AUTO_W_PROPERTY(t, n) __PROPERTY_W_DECL(Set##n, decltype(r), n); \
        virtual PSETTER(n, decltype(r)) = 0;

#define PURE_VIRTUAL_AUTO_R_PROPERTY(t, n) __PROPERTY_R_DECL(Get##n, decltype(r), n); \
        virtual PGETTER(n, decltype(r)) = 0;

#define AUTO_PROPERTY(r, n, m) __PROPERTY_DECL(Get##n, Set##n, decltype(r), n); \
        m PGETTER(n, decltype(r)) { return r; } \
        m PSETTER(n, decltype(r)) { r = value; }

#define AUTO_R_PROPERTY(r, n, m) __PROPERTY_R_DECL(Get##n, decltype(r), n); \
        m PGETTER(n, decltype(r)) { return r; }

#define AUTO_W_PROPERTY(r, n, m) __PROPERTY_W_DECL(Set##n, decltype(r), n); \
        m PSETTER(n, decltype(r)) { r = value; }

#define PROPERTY_G(r, n) __PROPERTY_R_DECL(Get##n, decltype(r), n); \
        PGETTER(n, decltype(r)) { return r; }

#define PROPERTY_S(r, n) __PROPERTY_W_DECL(Set##n, decltype(r), n); \
        PSETTER(n, decltype(r)) { r = value; }

#define PROPERTY_GS(r, n) __PROPERTY_DECL(Get##n, Set##n, decltype(r), n); \
        PGETTER(n, decltype(r)) { return r; }\
        PSETTER(n, decltype(r)) { r = value; }

#define PROPERTY_T_G(t, r, n) __PROPERTY_R_DECL(Get##n, t, n); \
        PGETTER_EX(n, t) { return r; }

#define PROPERTY_T_S(t, r, n) __PROPERTY_W_DECL(Set##n, t, n); \
        PSETTER_EX(n, t) { r = value; }

#define PROPERTY_T_GS(t, r, n) __PROPERTY_DECL(Get##n, Set##n, t, n); \
        PGETTER_EX(n, t) { return r; }\
        PSETTER_EX(n, t) { r = value; }

#define PROPERTY_(t, n) __PROPERTY_DECL(Get##n, Set##n, t, n)
#define PROPERTY_SET_ONLY(t, n) __PROPERTY_W_DECL(Set##n, t, n)
#define PROPERTY_GET_ONLY(t, n) __PROPERTY_R_DECL(Get##n, t, n)
#define PROPERTY_GET(n) Get##n
#define PROPERTY_SET(n) Set##n 

#pragma endregion



#pragma region New Property Macros

#define PROPERTY_QT(t, n) using M_CONC_(__pt_,n) = t; \
                    private: M_CONC_(__pt_,n) M_CONC_(m_,n) = M_CONC_(__pt_,n)(); \
                    public: __PROPERTY_DECL(Get##n , Set##n , t, n);

#define PROPERTY_T(t, n) using M_CONC_(__pt_,n) = t; \
                    __PROPERTY_DECL(Get##n , Set##n , t, n);

#define PROPERTY_TS(t, n) typedef t M_CONC_(__pt_,n); \
                    __PROPERTY_W_DECL(Set##n, t, n);

#define PROPERTY_TG(t, n) typedef t M_CONC_(__pt_,n); \
                    __PROPERTY_R_DECL(Get##n, t, n);

#define PROPERTY_T_GET(n, m) IDENTITY(M_CONC_(__pt_,n) m Get##n())
#define PROPERTY_T_SET(n) void Set##n (M_CONC_(__pt_,n) const& value)
#define PROPERTY_T_END(n)

#define PROPERTY_AUTO_QT(t, n) PROPERTY_QT(t, n); \
        PROPERTY_T_GET(n, const&) const { return M_CONC_(m_,n); } \
        PROPERTY_T_SET(n) { M_CONC_(m_,n) = value; }

#pragma endregion


namespace System
{
    template<typename T>
    T& Default() 
    {
        static T default = T();
        return default; 
    };

    namespace Basic
    {
        inline double atof (const char *p)
        {

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')

            int frac;
            double sign, value, scale;

            // Skip leading white space, if any.

            while (white_space(*p) ) {
                p += 1;
            }

            // Get sign, if any.

            sign = 1.0;
            if (*p == '-') {
                sign = -1.0;
                p += 1;

            } else if (*p == '+') {
                p += 1;
            }

            // Get digits before decimal point or exponent, if any.

            for (value = 0.0; valid_digit(*p); p += 1) {
                value = value * 10.0 + (*p - '0');
            }

            // Get digits after decimal point, if any.

            if (*p == '.') {
                double pow10 = 10.0;
                p += 1;
                while (valid_digit(*p)) {
                    value += (*p - '0') / pow10;
                    pow10 *= 10.0;
                    p += 1;
                }
            }

            // Handle exponent, if any.

            frac = 0;
            scale = 1.0;
            if ((*p == 'e') || (*p == 'E')) {
                unsigned int expon;

                // Get sign of exponent, if any.

                p += 1;
                if (*p == '-') {
                    frac = 1;
                    p += 1;

                } else if (*p == '+') {
                    p += 1;
                }

                // Get digits of exponent, if any.

                for (expon = 0; valid_digit(*p); p += 1) {
                    expon = expon * 10 + (*p - '0');
                }
                if (expon > 308) expon = 308;

                // Calculate scaling factor.

                while (expon >= 50) { scale *= 1E50; expon -= 50; }
                while (expon >=  8) { scale *= 1E8;  expon -=  8; }
                while (expon >   0) { scale *= 10.0; expon -=  1; }
            }

            // Return signed and scaled floating point result.

            return sign * (frac ? (value / scale) : (value * scale));

#undef white_space
#undef valid_digit

        }
    };
};
#endif