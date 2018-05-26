#include "spline.h"
//#include <sstream>

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace System::Spline;

static const glm::mat4 s_catmullRomCoeffs(
    -0.5f, 1.5f,-1.5f, 0.5f,
    1.f, -2.5f, 2.f, -0.5f,
    -0.5f, 0.f,  0.5f, 0.f,
    0.f,  1.f,  0.f,  0.f);

static const glm::mat4 s_Bezier3Coeffs(
    -1.f,  3.f, -3.f, 1.f,
    3.f, -6.f,  3.f, 0.f,
    -3.f,  3.f,  0.f, 0.f,
    1.f,  0.f,  0.f, 0.f);

inline void C_Evaluate(const glm::vec2 *vertice, float t, const glm::mat4& matr, glm::vec2 &result)
{
    glm::vec4 tvec(t*t*t, t*t, t, 1.f);
    glm::vec4 weights(tvec * matr);

    result = vertice[0] * weights[0] + vertice[1] * weights[1]
        + vertice[2] * weights[2] + vertice[3] * weights[3];
}

inline void C_Evaluate_Derivative(const glm::vec2 *vertice, float t, const glm::mat4& matr, glm::vec2 &result)
{
    glm::vec4 tvec(3.f*t*t, 2.f*t, 1.f, 0.f);
    glm::vec4 weights(tvec * matr);

    result = vertice[0] * weights[0] + vertice[1] * weights[1]
        + vertice[2] * weights[2];
}

inline glm::vec2 C_Evaluate_Hermite(const float r, const glm::vec2 &v1, const glm::vec2 &v2, const glm::vec2 &in, const glm::vec2 &out)
{
    float h1 = 2.0f*r*r*r - 3.0f*r*r + 1.0f;
    float h2 = -2.0f*r*r*r + 3.0f*r*r;
    float h3 = r * r*r - 2.0f*r*r + r;
    float h4 = r * r*r - r * r;

    return v1 * h1 + v2 * h2 + in * h3 + out * h4;
}

void System::Spline::Spline2D::EvaluateLinear(size_t i, float t, glm::vec2 & result)
{
    result = glm::lerp(points[i], points[i + 1], t);
}

void System::Spline::Spline2D::EvaluateCatmullRom(size_t i, float t, glm::vec2 & result)
{
    C_Evaluate(&points[i - 1], t, s_catmullRomCoeffs, result);
}

void System::Spline::Spline2D::EvaluateBezier3(size_t i, float t, glm::vec2 & result)
{
    C_Evaluate(&points[i], t, s_Bezier3Coeffs, result);
}

void System::Spline::Spline2D::EvaluateDerivativeLinear(size_t i, float, glm::vec2 & result)
{
    result = points[i + 1] - points[i];
}

void System::Spline::Spline2D::EvaluateDerivativeCatmullRom(size_t i, float t, glm::vec2 & result)
{
    C_Evaluate_Derivative(&points[i - 1], t, s_catmullRomCoeffs, result);
}

void System::Spline::Spline2D::EvaluateDerivativeBezier3(size_t i, float t, glm::vec2 & result)
{
    C_Evaluate_Derivative(&points[i], t, s_Bezier3Coeffs, result);
}

void System::Spline::Spline2D::Init()
{
    const int count = points.size();

    if (m_mode == Spline2D::ModeBezier3)
    {
        index_lo = 0;
        index_hi = count - 3;
    }
    else
    {
        const int real_size = count + 2;

        int lo_index = 1;
        int high_index = lo_index + count - 1;

        points.insert(points.begin(), glm::lerp(points[0], points[1], -1.0f));
        points.push_back(points[count]);

        index_lo = lo_index;
        index_hi = high_index;
    }
}