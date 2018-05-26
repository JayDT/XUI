#ifndef CFRAMEWORK_SPLINE_H
#define CFRAMEWORK_SPLINE_H

#include <glm/vec2.hpp>

namespace System::Spline
{
    class TC_CFRAMEWORK_API Spline2D
    {
    public:

        enum EvaluationMode
        {
            ModeLinear,
            ModeCatmullrom,
            ModeBezier3,
            UninitializedMode,
            ModesEnd
        };

        std::vector<glm::vec2> points;

        uint8 m_mode;
        size_t index_lo;
        size_t index_hi;

        void Init();

        void EvaluateLinear(size_t i, float, glm::vec2&);
        void EvaluateCatmullRom(size_t i, float, glm::vec2&);
        void EvaluateBezier3(size_t i, float, glm::vec2&);

        void EvaluateDerivativeLinear(size_t i, float, glm::vec2&);
        void EvaluateDerivativeCatmullRom(size_t i, float, glm::vec2&);
        void EvaluateDerivativeBezier3(size_t i, float, glm::vec2&);

    };
}

#endif // CFRAMEWORK_SPLINE_H
