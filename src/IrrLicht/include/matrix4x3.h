#ifndef __C_matrix4x3_H__
#define __C_matrix4x3_H__

#ifdef _MSC_VER
// Disable conditional expression is constant, which occurs incorrectly on inlined functions
#   pragma warning (push)
#   pragma warning( disable : 4127 )
#endif
#include <cassert>

namespace irr
{
    namespace core
    {

        /**
        A 4x3 matrix.
        */
        class matrix4x3 {
        private:

            float elt[4][3];

        public:

            // ! No initialize (fastest contruct)
            matrix4x3()
            {
                
            }

            matrix4x3(
                float r1c1, float r1c2, float r1c3,
                float r2c1, float r2c2, float r2c3,
                float r3c1, float r3c2, float r3c3,
                float r4c1, float r4c2, float r4c3)
            {
                elt[0][0] = r1c1;  elt[0][1] = r1c2;  elt[0][2] = r1c3;
                elt[1][0] = r2c1;  elt[1][1] = r2c2;  elt[1][2] = r2c3;
                elt[2][0] = r3c1;  elt[2][1] = r3c2;  elt[2][2] = r3c3;
                elt[3][0] = r4c1;  elt[3][1] = r4c2;  elt[3][2] = r4c3;
            }

            matrix4x3(float* floatArray)
            {
                memcpy(elt, floatArray, sizeof(elt));
            }

            matrix4x3(irr::core::matrix4 const& mat)
            {
                float const* floats = mat.pointer();

                memcpy(elt[0], &floats[0], sizeof(float) * 3);
                memcpy(elt[1], &floats[4], sizeof(float) * 3);
                memcpy(elt[2], &floats[8], sizeof(float) * 3);
                memcpy(elt[3], &floats[12], sizeof(float) * 3);
            }

            inline float* operator[](int r) {
                assert(r >= 0);
                assert(r < 4);
                return (float*)&elt[r];
            }

            inline const float* operator[](int r) const {
                assert(r >= 0);
                assert(r < 4);
                return (const float*)&elt[r];
            }

            inline operator float* () {
                return (float*)&elt[0][0];
            }

            inline operator const float* () const {
                return (const float*)&elt[0][0];
            }

            inline operator irr::core::matrix4() {
                return ToMatrix4x4();
            }

            inline operator const irr::core::matrix4() const {
                return ToMatrix4x4();
            }

            //! multiply by another matrix
            inline irr::core::matrix4 matrix4x3::operator*(irr::core::matrix4 const& m2) const
            {
                irr::core::matrix4 m3;
                memset(&m3, 0, sizeof(m3));

                const float *m1 = (float const*)*this;

                m3[0] = m1[0] * m2[0] + m1[3] * m2[1] + m1[6] * m2[2] + m1[9] * m2[3];
                m3[1] = m1[1] * m2[0] + m1[4] * m2[1] + m1[7] * m2[2] + m1[10] * m2[3];
                m3[2] = m1[2] * m2[0] + m1[5] * m2[1] + m1[8] * m2[2] + m1[11] * m2[3];

                m3[4] = m1[0] * m2[4] + m1[3] * m2[5] + m1[6] * m2[6] + m1[9] *  m2[7];
                m3[5] = m1[1] * m2[4] + m1[4] * m2[5] + m1[7] * m2[6] + m1[10] * m2[7];
                m3[6] = m1[2] * m2[4] + m1[5] * m2[5] + m1[8] * m2[6] + m1[11] * m2[7];

                m3[8] = m1[0] * m2[8] + m1[3] * m2[9] + m1[6] * m2[10] + m1[9] *  m2[11];
                m3[9] = m1[1] * m2[8] + m1[4] * m2[9] + m1[7] * m2[10] + m1[10] * m2[11];
                m3[10] = m1[2] * m2[8] + m1[5] * m2[9] + m1[8] * m2[10] + m1[11] * m2[11];

                m3[12] = m1[0] * m2[12] + m1[3] * m2[13] + m1[6] * m2[14] + m1[9] *  m2[15];
                m3[13] = m1[1] * m2[12] + m1[4] * m2[13] + m1[7] * m2[14] + m1[10] * m2[15];
                m3[14] = m1[2] * m2[12] + m1[5] * m2[13] + m1[8] * m2[14] + m1[11] * m2[15];

                return m3;
            }

            inline irr::core::matrix4 ToMatrix4x4() const
            {
                irr::core::matrix4 m3;

                const float *m1 = (float const*)*this;

                m3[0] = m1[0];
                m3[1] = m1[1]; 
                m3[2] = m1[2]; 
                m3[3] = 0;
                m3[4] = m1[3]; 
                m3[5] = m1[4]; 
                m3[6] = m1[5]; 
                m3[7] = 0;
                m3[8] = m1[6];
                m3[9] = m1[7]; 
                m3[10] = m1[8];
                m3[11] = 0;
                m3[12] = m1[9];
                m3[13] = m1[10];
                m3[14] = m1[11];
                m3[15] = 0;

                return m3;
            }



            //! multiply by another matrix
            //inline matrix4x3 matrix4x3::operator*(matrix4x3 const& _m2) const
            //{
            //    matrix4x3 m3;
            //
            //    const float *m1 = (float const*)*this;
            //    const float *m2 = (float const*)_m2;
            //
            //    m3[0][0] = m1[0] * m2[0] + m1[3] * m2[1] + m1[6] * m2[2] + m1[9]  * m2[3];
            //    m3[0][1] = m1[1] * m2[0] + m1[4] * m2[1] + m1[7] * m2[2] + m1[10] * m2[3];
            //    m3[0][2] = m1[2] * m2[0] + m1[5] * m2[1] + m1[8] * m2[2] + m1[11] * m2[3];
            //
            //    m3[1][0] = m1[0] * m2[3] + m1[3] * m2[5] + m1[6] * m2[6] + m1[9] * m2[7];
            //    m3[1][1] = m1[1] * m2[3] + m1[4] * m2[5] + m1[7] * m2[6] + m1[10] * m2[7];
            //    m3[1][2] = m1[2] * m2[3] + m1[5] * m2[5] + m1[8] * m2[6] + m1[11] * m2[7];
            //
            //    m3[2][0] = m1[0] * m2[6] + m1[3] * m2[9] + m1[6] * m2[10] + m1[9] * m2[11];
            //    m3[2][1] = m1[1] * m2[6] + m1[4] * m2[9] + m1[7] * m2[10] + m1[10] * m2[11];
            //    m3[2][2] = m1[2] * m2[6] + m1[5] * m2[9] + m1[8] * m2[10] + m1[11] * m2[11];
            //
            //    m3[3][0] = m1[0] * m2[9] + m1[3] * m2[13] + m1[6] * m2[14] + m1[9] * m2[15];
            //    m3[3][1] = m1[1] * m2[9] + m1[4] * m2[13] + m1[7] * m2[14] + m1[10] * m2[15];
            //    m3[3][2] = m1[2] * m2[9] + m1[5] * m2[13] + m1[8] * m2[14] + m1[11] * m2[15];
            //
            //    return m3;
            //}

        };
    }
} // namespace

#ifdef _MSC_VER
#   pragma warning (pop)
#endif

#endif // __C_matrix4x3_H__
