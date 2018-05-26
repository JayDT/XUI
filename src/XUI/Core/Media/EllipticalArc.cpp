/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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

#include "EllipticalArc.h"
#include "Core/Media/MediaTypes.h"

const double TwoPi = 2 * M_PI_F;

/// <summary>
/// Coefficients for error estimation while using quadratic Bezier curves for approximation,
/// 0 ? b/a ? 0.25
/// </summary>
static const XUI::Core::Media::Matrix Coeffs2Low[] = {
    {
        XUI::Core::Media::Matrix(glm::mat4(3.92478, -13.5822, -0.233377, 0.0128206,
            -1.08814, 0.859987, 3.62265E-4, 2.29036E-4,
            -0.942512, 0.390456, 0.0080909, 0.00723895,
            -0.736228, 0.20998, 0.0129867, 0.0103456))
    },
    {
        XUI::Core::Media::Matrix(glm::mat4(-0.395018, 6.82464, 0.0995293, 0.0122198,
            -0.545608, 0.0774863, 0.0267327, 0.0132482,
            0.0534754, -0.0884167, 0.012595, 0.0343396,
            0.209052, -0.0599987, -0.00723897, 0.00789976))
    }
};

/// <summary>
/// Coefficients for error estimation while using quadratic Bezier curves for approximation,
/// 0.25 ? b/a ? 1
/// </summary>
static const XUI::Core::Media::Matrix Coeffs2High[] = {
    {
        XUI::Core::Media::Matrix(glm::mat4(0.0863805, -11.5595, -2.68765, 0.181224,
         0.242856, -1.81073, 1.56876, 1.68544,
         0.233337, -0.455621, 0.222856, 0.403469,
         0.0612978, -0.104879, 0.0446799, 0.00867312))
    },
    {
        XUI::Core::Media::Matrix(glm::mat4(0.028973, 6.68407, 0.171472, 0.0211706,
         0.0307674, -0.0517815, 0.0216803, -0.0749348,
         -0.0471179, 0.1288, -0.0781702, 2.0,
         -0.0309683, 0.0531557, -0.0227191, 0.0434511))
    }
};

/// <summary>
/// Safety factor to convert the "best" error approximation into a "max bound" error
/// </summary>
static const float Safety2[4] = { 0.02f, 2.83f, 0.125f, 0.01f };

/// <summary>
/// Coefficients for error estimation while using cubic Bezier curves for approximation,
/// 0.25 ? b/a ? 1
/// </summary>
static const XUI::Core::Media::Matrix Coeffs3Low[] = {
    {
        XUI::Core::Media::Matrix(glm::mat4(3.85268, -21.229, -0.330434, 0.0127842,
         -1.61486, 0.706564, 0.225945, 0.263682,
         -0.910164, 0.388383, 0.00551445, 0.00671814,
         -0.630184, 0.192402, 0.0098871, 0.0102527))
    },
    {
        XUI::Core::Media::Matrix(glm::mat4(-0.162211, 9.94329, 0.13723, 0.0124084,
         -0.253135, 0.00187735, 0.0230286, 0.01264,
         -0.0695069, -0.0437594, 0.0120636, 0.0163087,
         -0.0328856, -0.00926032, -0.00173573, 0.00527385))
    }
};

/// <summary>
/// Coefficients for error estimation while using cubic Bezier curves for approximation,
/// 0.25 ? b/a ? 1
/// </summary>
static const XUI::Core::Media::Matrix Coeffs3High[] = {
    {
        XUI::Core::Media::Matrix(glm::mat4(0.0899116, -19.2349, -4.11711, 0.183362,
         0.138148, -1.45804, 1.32044, 1.38474,
         0.230903, -0.450262, 0.219963, 0.414038,
         0.0590565, -0.101062, 0.0430592, 0.0204699))
    },
    {
        XUI::Core::Media::Matrix(glm::mat4(0.0164649, 9.89394, 0.0919496, 0.00760802,
         0.0191603, -0.0322058, 0.0134667, -0.0825018,
         0.0156192, -0.017535, 0.00326508, -0.228157,
         -0.0236752, 0.0405821, -0.0173086, 0.176187))
    }
};
/// <summary>
/// Safety factor to convert the "best" error approximation into a "max bound" error
/// </summary>
static const float Safety3[4] = { 0.0010f, 4.98f, 0.207f, 0.0067f };

XUI::Core::Media::Point XUI::Core::Media::EllipticalArc::SimpleMatrix::operator *(XUI::Core::Media::Point const& p)
{
    return Point(_a * p.x + _b * p.y, _c * p.x + _d * p.y);
}

void XUI::Core::Media::ArcTo(XUI::Interfaces::IStreamGeometryContext* streamGeometryContextImpl, XUI::Core::Media::Point const& currentPoint, XUI::Core::Media::Point const& point, XUI::Core::Media::Size const& size, double rotationAngle, bool isLargeArc, XUI::Interfaces::SweepDirection sweepDirection)
{
    XUI::Core::Media::EllipticalArc::BuildArc(streamGeometryContextImpl, currentPoint, point, size, rotationAngle*M_PI_F / 180.0f,
                            isLargeArc,
                            sweepDirection == XUI::Interfaces::SweepDirection::Clockwise);
}

/// <summary>
/// Builds an elliptical arc composed of the full unit circle around (0,0)
/// </summary>

XUI::Core::Media::EllipticalArc::EllipticalArc()
{
    Cx = 0;
    Cy = 0;
    A = 1;
    B = 1;
    Theta = 0;
    Eta1 = 0;
    Eta2 = TwoPi;
    _cosTheta = 1;
    _sinTheta = 0;
    IsPieSlice = false;
    _maxDegree = 3;
    _defaultFlatness = 0.5;
    ComputeFocii();
    ComputeEndPoints();
    ComputeBounds();
    ComputeDerivedFlatnessParameters();
}

/// <summary>
/// Builds an elliptical arc from its canonical geometrical elements
/// </summary>
/// <param name="center">Center of the ellipse</param>
/// <param name="a">Semi-major axis</param>
/// <param name="b">Semi-minor axis</param>
/// <param name="theta">Orientation of the major axis with respect to the x axis</param>
/// <param name="lambda1">Start angle of the arc</param>
/// <param name="lambda2">End angle of the arc</param>
/// <param name="isPieSlice">If true, the lines between the center of the ellipse
///  and the endpoints are part of the shape (it is pie slice like)</param>

XUI::Core::Media::EllipticalArc::EllipticalArc(Point const & center, double a, double b, double theta, double lambda1, double lambda2, bool isPieSlice)
    : EllipticalArc(center.x, center.y, a, b, theta, lambda1,
                    lambda2, isPieSlice)
{
}

/// <summary>
/// Builds an elliptical arc from its canonical geometrical elements
/// </summary>
/// <param name="cx">Abscissa of the center of the ellipse</param>
/// <param name="cy">Ordinate of the center of the ellipse</param>
/// <param name="a">Semi-major axis</param>
/// <param name="b">Semi-minor axis</param>
/// <param name="theta">Orientation of the major axis with respect to the x axis</param>
/// <param name="lambda1">Start angle of the arc</param>
/// <param name="lambda2">End angle of the arc</param>
/// <param name="isPieSlice">If true, the lines between the center of the ellipse
///  and the endpoints are part of the shape (it is pie slice like)</param>

XUI::Core::Media::EllipticalArc::EllipticalArc(double cx, double cy, double a, double b, double theta, double lambda1, double lambda2, bool isPieSlice)
{
    Cx = cx;
    Cy = cy;
    A = a;
    B = b;
    Theta = theta;
    IsPieSlice = isPieSlice;
    Eta1 = atan2(sin(lambda1) / b, cos(lambda1) / a);
    Eta2 = atan2(sin(lambda2) / b, cos(lambda2) / a);
    _cosTheta = cos(theta);
    _sinTheta = sin(theta);
    _maxDegree = 3;
    _defaultFlatness = 0.5; // half a pixel
    Eta2 -= TwoPi * floor((Eta2 - Eta1) / TwoPi); //make sure we have eta1 <= eta2 <= eta1 + 2 PI
                                                       // the preceding correction fails if we have exactly eta2-eta1 == 2*PI
                                                       // it reduces the interval to zero length
    if (lambda2 - lambda1 > M_PI && Eta2 - Eta1 < M_PI)
    {
        Eta2 += TwoPi;
    }
    ComputeFocii();
    ComputeEndPoints();
    ComputeBounds();
    ComputeDerivedFlatnessParameters();
}

/// <summary>
/// Build a full ellipse from its canonical geometrical elements
/// </summary>
/// <param name="center">Center of the ellipse</param>
/// <param name="a">Semi-major axis</param>
/// <param name="b">Semi-minor axis</param>
/// <param name="theta">Orientation of the major axis with respect to the x axis</param>

XUI::Core::Media::EllipticalArc::EllipticalArc(Point const & center, double a, double b, double theta)
    : EllipticalArc(center.x, center.y, a, b, theta)
{
}

/// <summary>
/// Build a full ellipse from its canonical geometrical elements
/// </summary>
/// <param name="cx">Abscissa of the center of the ellipse</param>
/// <param name="cy">Ordinate of the center of the ellipse</param>
/// <param name="a">Semi-major axis</param>
/// <param name="b">Semi-minor axis</param>
/// <param name="theta">Orientation of the major axis with respect to the x axis</param>

XUI::Core::Media::EllipticalArc::EllipticalArc(double cx, double cy, double a, double b, double theta)
{
    Cx = cx;
    Cy = cy;
    A = a;
    B = b;
    Theta = theta;
    IsPieSlice = false;
    Eta1 = 0;
    Eta2 = TwoPi;
    _cosTheta = cos(theta);
    _sinTheta = sin(theta);
    _maxDegree = 3;
    _defaultFlatness = 0.5; //half a pixel
    ComputeFocii();
    ComputeEndPoints();
    ComputeBounds();
    ComputeDerivedFlatnessParameters();
}

/// <summary>
/// Sets the maximal degree allowed for Bezier curve approximation.
/// </summary>
/// <param name="maxDegree">Maximal allowed degree (must be between 1 and 3)</param>
/// <exception cref="ArgumentException">Thrown if maxDegree is not between 1 and 3</exception>

void XUI::Core::Media::EllipticalArc::SetMaxDegree(int maxDegree)
{
    if (maxDegree < 1 || maxDegree > 3)
    {
        throw System::ArgumentException("maxDegree must be between 1 and 3" nameof(maxDegree));
    }
    _maxDegree = maxDegree;
}

/// <summary>
/// Sets the default flatness for Bezier curve approximation
/// </summary>
/// <param name="defaultFlatness">default flatness (must be greater than 1e-10)</param>
/// <exception cref="ArgumentException">Thrown if defaultFlatness is lower than 1e-10</exception>

void XUI::Core::Media::EllipticalArc::SetDefaultFlatness(double defaultFlatness)
{
    if (defaultFlatness < 1.0E-10)
    {
        throw System::ArgumentException("defaultFlatness must be greater than 1.0e-10" nameof(defaultFlatness));
    }
    _defaultFlatness = defaultFlatness;
}

/// <summary>
/// Computes the locations of the focii
/// </summary>

void XUI::Core::Media::EllipticalArc::ComputeFocii()
{
    double d = sqrt(A * A - B * B);
    double dx = d * _cosTheta;
    double dy = d * _sinTheta;
    FirstFocusX = Cx - dx;
    FirstFocusY = Cy - dy;
    SecondFocusX = Cx + dx;
    SecondFocusY = Cy + dy;
}

/// <summary>
/// Computes the locations of the endpoints
/// </summary>

void XUI::Core::Media::EllipticalArc::ComputeEndPoints()
{
    double aCosEta1 = A * cos(Eta1);
    double bSinEta1 = B * sin(Eta1);
    X1 = Cx + aCosEta1 * _cosTheta - bSinEta1 * _sinTheta;
    Y1 = Cy + aCosEta1 * _sinTheta + bSinEta1 * _cosTheta;
    double aCosEta2 = A * cos(Eta2);
    double bSinEta2 = B * sin(Eta2);
    X2 = Cx + aCosEta2 * _cosTheta - bSinEta2 * _sinTheta;
    Y2 = Cy + aCosEta2 * _sinTheta + bSinEta2 * _cosTheta;
}

/// <summary>
/// Computes the bounding box
/// </summary>

void XUI::Core::Media::EllipticalArc::ComputeBounds()
{
    double bOnA = B / A;
    double etaXMin;
    double etaXMax;
    double etaYMin;
    double etaYMax;
    if (abs(_sinTheta) < 0.1)
    {
        double tanTheta = _sinTheta / _cosTheta;
        if (_cosTheta < 0)
        {
            etaXMin = -atan(tanTheta * bOnA);
            etaXMax = etaXMin + M_PI;
            etaYMin = 0.5 * M_PI - atan(tanTheta / bOnA);
            etaYMax = etaYMin + M_PI;
        }
        else
        {
            etaXMax = -atan(tanTheta * bOnA);
            etaXMin = etaXMax - M_PI;
            etaYMax = 0.5 * M_PI - atan(tanTheta / bOnA);
            etaYMin = etaYMax - M_PI;
        }
    }
    else
    {
        double invTanTheta = _cosTheta / _sinTheta;
        if (_sinTheta < 0)
        {
            etaXMax = 0.5 * M_PI + atan(invTanTheta / bOnA);
            etaXMin = etaXMax - M_PI;
            etaYMin = atan(invTanTheta * bOnA);
            etaYMax = etaYMin + M_PI;
        }
        else
        {
            etaXMin = 0.5 * M_PI + atan(invTanTheta / bOnA);
            etaXMax = etaXMin + M_PI;
            etaYMax = atan(invTanTheta * bOnA);
            etaYMin = etaYMax - M_PI;
        }
    }
    etaXMin -= TwoPi * floor((etaXMin - Eta1) / TwoPi);
    etaYMin -= TwoPi * floor((etaYMin - Eta1) / TwoPi);
    etaXMax -= TwoPi * floor((etaXMax - Eta1) / TwoPi);
    etaYMax -= TwoPi * floor((etaYMax - Eta1) / TwoPi);
    _xLeft = etaXMin <= Eta2
        ? Cx + A * cos(etaXMin) * _cosTheta - B * sin(etaXMin) * _sinTheta
        : std::min(X1, X2);
    _yUp = etaYMin <= Eta2 ? Cy + A * cos(etaYMin) * _sinTheta + B * sin(etaYMin) * _cosTheta : std::min(Y1, Y2);
    _width = (etaXMax <= Eta2
        ? Cx + A * cos(etaXMax) * _cosTheta - B * sin(etaXMax) * _sinTheta
        : std::max(X1, X2)) - _xLeft;
    _height = (etaYMax <= Eta2
        ? Cy + A * cos(etaYMax) * _sinTheta + B * sin(etaYMax) * _cosTheta
        : std::max(Y1, Y2)) - _yUp;
}

/// <summary>
/// Computes the flatness parameters used in intersection tests
/// </summary>

void XUI::Core::Media::EllipticalArc::ComputeDerivedFlatnessParameters()
{
    F = (A - B) / A;
    E2 = F * (2.0 - F);
    G = 1.0 - F;
    G2 = G * G;
}

/// <summary>
/// Computes the value of a rational function.
/// This method handles rational functions where the numerator is quadratic
/// and the denominator is linear
/// </summary>
/// <param name="x">Abscissa for which the value should be computed</param>
/// <param name="c">Coefficients array of the rational function</param>
/// <returns></returns>

double XUI::Core::Media::EllipticalArc::RationalFunction(double x, float const* c)
{
    return (x * (x * c[0] + c[1]) + c[2]) / (x + c[3]);
}

/// <summary>
/// Estimate the approximation error for a sub-arc of the instance
/// </summary>
/// <param name="degree">Degree of the Bezier curve to use (1, 2 or 3)</param>
/// <param name="etaA">Start angle of the sub-arc</param>
/// <param name="etaB">End angle of the sub-arc</param>
/// <returns>Upper bound of the approximation error between the Bezier curve and the real ellipse</returns>

double XUI::Core::Media::EllipticalArc::EstimateError(int degree, double etaA, double etaB)
{
    if (degree < 1 || degree > _maxDegree)
        throw System::ArgumentException("degree should be between {1} and {_maxDegree}" nameof(degree));
    double eta = 0.5 * (etaA + etaB);
    if (degree < 2)
    {
        //start point
        double aCosEtaA = A * cos(etaA);
        double bSinEtaA = B * sin(etaA);
        double xA = Cx + aCosEtaA * _cosTheta - bSinEtaA * _sinTheta;
        double yA = Cy + aCosEtaA * _sinTheta + bSinEtaA * _cosTheta;

        //end point
        double aCosEtaB = A * cos(etaB);
        double bSinEtaB = B * sin(etaB);
        double xB = Cx + aCosEtaB * _cosTheta - bSinEtaB * _sinTheta;
        double yB = Cy + aCosEtaB * _sinTheta + bSinEtaB * _cosTheta;

        //maximal error point
        double aCosEta = A * cos(eta);
        double bSinEta = B * sin(eta);
        double x = Cx + aCosEta * _cosTheta - bSinEta * _sinTheta;
        double y = Cy + aCosEta * _sinTheta + bSinEta * _cosTheta;

        double dx = xB - xA;
        double dy = yB - yA;

        return abs(x * dy - y * dx + xB * yA - xA * yB) / sqrt(dx * dx + dy * dy);
    }
    else
    {
        double x = B / A;
        double dEta = etaB - etaA;
        double cos2 = cos(2 * eta);
        double cos4 = cos(4 * eta);
        double cos6 = cos(6 * eta);

        // select the right coeficients set according to degree and b/a
        XUI::Core::Media::Matrix const* coeffs;
        float const* safety;
        if (degree == 2)
        {
            coeffs = x < 0.25 ? Coeffs2Low : Coeffs2High;
            safety = Safety2;
        }
        else
        {
            coeffs = x < 0.25 ? Coeffs3Low : Coeffs3High;
            safety = Safety3;
        }
        double c0 = RationalFunction(x, &coeffs[0][0].x) + cos2 * RationalFunction(x, &coeffs[0][1].x) +
            cos4 * RationalFunction(x, &coeffs[0][2].x) + cos6 * RationalFunction(x,
                &coeffs[0][3].x);
        double c1 = RationalFunction(x, &coeffs[1][0].x) + cos2 * RationalFunction(x, &coeffs[1][1].x) +
            cos4 * RationalFunction(x, &coeffs[1][2].x) + cos6 * RationalFunction(x,
                &coeffs[1][3].x);
        return RationalFunction(x, safety) * A * std::exp(c0 + c1 * dEta);
    }
}

/// <summary>
/// Get the elliptical arc point for a given angular parameter
/// </summary>
/// <param name="lambda">Angular parameter for which point is desired</param> 
/// <returns>The desired elliptical arc point location</returns>

XUI::Core::Media::Point XUI::Core::Media::EllipticalArc::PointAt(double lambda)
{
    double eta = atan2(sin(lambda) / B, cos(lambda) / A);
    double aCosEta = A * cos(eta);
    double bSinEta = B * sin(eta);
    return Point(Cx + aCosEta * _cosTheta - bSinEta * _sinTheta, Cy + aCosEta * _sinTheta + bSinEta * _cosTheta);
}

/// <summary>
/// Tests if the specified coordinates are inside the closed shape formed by this arc.
/// If this is not a pie, then a shape derived by adding a closing chord is considered.
/// </summary>
/// <param name="x">Abscissa of the test point</param>
/// <param name="y">Ordinate of the test point</param>
/// <returns>True if the specified coordinates are inside the closed shape of this arc</returns>

bool XUI::Core::Media::EllipticalArc::Contains(double x, double y)
{
    // position relative to the focii
    double dx1 = x - FirstFocusX;
    double dy1 = y - FirstFocusY;
    double dx2 = x - SecondFocusX;
    double dy2 = y - SecondFocusY;
    if (dx1 * dx1 + dy1 * dy1 + dx2 * dx2 + dy2 * dy2 > 4 * A * A)
    {
        // the point is outside of the ellipse
        return false;
    }
    if (IsPieSlice)
    {
        // check the location of the test point with respect to the
        // angular sector counted from the centre of the ellipse
        double dxC = x - Cx;
        double dyC = y - Cy;
        double u = dxC * _cosTheta + dyC * _sinTheta;
        double v = dyC * _cosTheta - dxC * _sinTheta;
        double eta = atan2(v / B, u / A);
        eta -= TwoPi * floor((eta - Eta1) / TwoPi);
        return eta <= Eta2;
    }
    // check the location of the test point with respect to the
    // chord joining the start and end points
    double dx = X2 - X1;
    double dy = Y2 - Y1;
    return x * dy - y * dx + X2 * Y1 - X1 * Y2 >= 0;
}

/// <summary>
/// Tests if a line segment intersects the arc
/// </summary>
/// <param name="xA">abscissa of the first point of the line segment</param>
/// <param name="yA">ordinate of the first point of the line segment</param>
/// <param name="xB">abscissa of the second point of the line segment</param>
/// <param name="yB">ordinate of the second point of the line segment</param>
/// <returns>true if the two line segments intersect</returns>

bool XUI::Core::Media::EllipticalArc::IntersectArc(double xA, double yA, double xB, double yB)
{
    double dx = xA - xB;
    double dy = yA - yB;
    double l = sqrt(dx * dx + dy * dy);
    if (l < 1.0E-10 * A)
    {
        // too small line segment, we consider it doesn't intersect anything
        return false;
    }
    double cz = (dx * _cosTheta + dy * _sinTheta) / l;
    double sz = (dy * _cosTheta - dx * _sinTheta) / l;

    // express position of the first point in canonical frame
    dx = xA - Cx;
    dy = yA - Cy;
    double u = dx * _cosTheta + dy * _sinTheta;
    double v = dy * _cosTheta - dx * _sinTheta;
    double u2 = u * u;
    double v2 = v * v;
    double g2U2Ma2 = G2 * (u2 - A * A);
    //double g2U2Ma2Mv2 = g2U2Ma2 - v2;
    double g2U2Ma2Pv2 = g2U2Ma2 + v2;

    // compute intersections with the ellipse along the line
    // as the roots of a 2nd degree polynom : c0 k^2 - 2 c1 k + c2 = 0
    double c0 = 1.0 - E2 * cz * cz;
    double c1 = G2 * u * cz + v * sz;
    double c2 = g2U2Ma2Pv2;
    double c12 = c1 * c1;
    double c0C2 = c0 * c2;
    if (c12 < c0C2)
    {
        // the line does not intersect the ellipse at all
        return false;
    }
    double k = c1 >= 0 ? (c1 + sqrt(c12 - c0C2)) / c0 : c2 / (c1 - sqrt(c12 - c0C2));
    if (k >= 0 && k <= l)
    {
        double uIntersect = u - k * cz;
        double vIntersect = v - k * sz;
        double eta = atan2(vIntersect / B, uIntersect / A);
        eta -= TwoPi * floor((eta - Eta1) / TwoPi);
        if (eta <= Eta2)
        {
            return true;
        }
    }
    k = c2 / (k * c0);
    if (k >= 0 && k <= l)
    {
        double uIntersect = u - k * cz;
        double vIntersect = v - k * sz;
        double eta = atan2(vIntersect / B, uIntersect / A);
        eta -= TwoPi * floor((eta - Eta1) / TwoPi);
        if (eta <= Eta2)
        {
            return true;
        }
    }
    return false;
}

/// <summary>
/// Tests if two line segments intersect
/// </summary>
/// <param name="x1">Abscissa of the first point of the first line segment</param>
/// <param name="y1">Ordinate of the first point of the first line segment</param>
/// <param name="x2">Abscissa of the second point of the first line segment</param>
/// <param name="y2">Ordinate of the second point of the first line segment</param>
/// <param name="xA">Abscissa of the first point of the second line segment</param>
/// <param name="yA">Ordinate of the first point of the second line segment</param>
/// <param name="xB">Abscissa of the second point of the second line segment</param>
/// <param name="yB">Ordinate of the second point of the second line segment</param>
/// <returns>true if the two line segments intersect</returns>

bool XUI::Core::Media::EllipticalArc::Intersect(double x1, double y1, double x2, double y2, double xA, double yA, double xB, double yB)
{
    // elements of the equation of the (1, 2) line segment
    double dx12 = x2 - x1;
    double dy12 = y2 - y1;
    double k12 = x2 * y1 - x1 * y2;
    // elements of the equation of the (A, B) line segment
    double dxAb = xB - xA;
    double dyAb = yB - yA;
    double kAb = xB * yA - xA * yB;
    // compute relative positions of endpoints versus line segments
    double pAvs12 = xA * dy12 - yA * dx12 + k12;
    double pBvs12 = xB * dy12 - yB * dx12 + k12;
    double p1VsAb = x1 * dyAb - y1 * dxAb + kAb;
    double p2VsAb = x2 * dyAb - y2 * dxAb + kAb;

    return pAvs12 * pBvs12 <= 0 && p1VsAb * p2VsAb <= 0;
}

/// <summary>
/// Tests if a line segment intersects the outline
/// </summary>
/// <param name="xA">Abscissa of the first point of the line segment</param>
/// <param name="yA">Ordinate of the first point of the line segment</param>
/// <param name="xB">Abscissa of the second point of the line segment</param>
/// <param name="yB">Ordinate of the second point of the line segment</param>
/// <returns>true if the two line segments intersect</returns>

bool XUI::Core::Media::EllipticalArc::IntersectOutline(double xA, double yA, double xB, double yB)
{
    if (IntersectArc(xA, yA, xB, yB))
    {
        return true;
    }
    if (IsPieSlice)
    {
        return Intersect(Cx, Cy, X1, Y1, xA, yA, xB, yB) || Intersect(Cx, Cy, X2, Y2, xA, yA, xB, yB);
    }
    return Intersect(X1, Y1, X2, Y2, xA, yA, xB, yB);
}

/// <summary>
/// Tests if the interior of a closed path derived from this arc entirely contains the specified rectangular area.
/// The closed path is derived with respect to the IsPieSlice value.
/// </summary>
/// <param name="x">Abscissa of the upper-left corner of the test rectangle</param>
/// <param name="y">Ordinate of the upper-left corner of the test rectangle</param>
/// <param name="w">Width of the test rectangle</param>
/// <param name="h">Height of the test rectangle</param>
/// <returns>true if the interior of a closed path derived from this arc entirely contains the specified rectangular area; false otherwise</returns>

bool XUI::Core::Media::EllipticalArc::Contains(double x, double y, double w, double h)
{
    double xPlusW = x + w;
    double yPlusH = y + h;
    return Contains(x, y) && Contains(xPlusW, y) && Contains(x, yPlusH) && Contains(xPlusW, yPlusH) &&
        !IntersectOutline(x, y, xPlusW, y) && !IntersectOutline(xPlusW,
            y, xPlusW, yPlusH) && !IntersectOutline(xPlusW, yPlusH, x, yPlusH) &&
        !IntersectOutline(x, yPlusH, x, y);
}

/// <summary>
/// Tests if a specified Point2D is inside the boundary of a closed path derived from this arc.
/// The closed path is derived with respect to the IsPieSlice value.
/// </summary>
/// <param name="p">Test point</param>
/// <returns>true if the specified point is inside a closed path derived from this arc</returns>

bool XUI::Core::Media::EllipticalArc::Contains(Point p)
{
    return Contains(p.x, p.y);
}

/// <summary>
/// Tests if the interior of a closed path derived from this arc entirely contains the specified Rectangle2D.
/// The closed path is derived with respect to the IsPieSlice value.
/// </summary>
/// <param name="r">Test rectangle</param>
/// <returns>True if the interior of a closed path derived from this arc entirely contains the specified Rectangle2D; false otherwise</returns>

bool XUI::Core::Media::EllipticalArc::Contains(Rect r)
{
    return Contains(r.x0(), r.y0(), r.width(), r.height());
}

/// <summary>
/// Returns an integer Rectangle that completely encloses the closed path derived from this arc.
/// The closed path is derived with respect to the IsPieSlice value.
/// </summary> 

XUI::Core::Media::Rect XUI::Core::Media::EllipticalArc::GetBounds()
{
    Rect rect(Vector(_width, _height));
    return rect + Vector(_xLeft, _yUp);
    //return Rect(_xLeft, _yUp, _width, _height);
}

/// <summary>
/// Builds the arc outline using given StreamGeometryContext and default (max) Bezier curve degree and acceptable error of half a pixel (0.5)
/// </summary>
/// <param name="path">A StreamGeometryContext to output the path commands to</param>

void XUI::Core::Media::EllipticalArc::BuildArc(Interfaces::IStreamGeometryContext* path)
{
    BuildArc(path, _maxDegree, _defaultFlatness, true);
}

/// <summary>
/// Builds the arc outline using given StreamGeometryContext
/// </summary>
/// <param name="path">A StreamGeometryContext to output the path commands to</param>
/// <param name="degree">degree of the Bezier curve to use</param>
/// <param name="threshold">acceptable error</param>
/// <param name="openNewFigure">if true, a new figure will be started in the specified StreamGeometryContext</param>

void XUI::Core::Media::EllipticalArc::BuildArc(Interfaces::IStreamGeometryContext* path, int degree, double threshold, bool openNewFigure)
{
    if (degree < 1 || degree > _maxDegree)
        throw System::ArgumentException("degree should be between {1} and {_maxDegree}" nameof(degree));

    // find the number of Bezier curves needed
    bool found = false;
    int n = 1;
    double dEta;
    double etaB;
    while (!found && n < 1024)
    {
        dEta = (Eta2 - Eta1) / n;
        if (dEta <= 0.5 * M_PI)
        {
            etaB = Eta1;
            found = true;
            for (int i = 0; found && i < n; ++i)
            {
                double etaA = etaB;
                etaB += dEta;
                found = EstimateError(degree, etaA, etaB) <= threshold;
            }
        }
        n = n << 1;
    }
    dEta = (Eta2 - Eta1) / n;
    etaB = Eta1;
    double cosEtaB = cos(etaB);
    double sinEtaB = sin(etaB);
    double aCosEtaB = A * cosEtaB;
    double bSinEtaB = B * sinEtaB;
    double aSinEtaB = A * sinEtaB;
    double bCosEtaB = B * cosEtaB;
    double xB = Cx + aCosEtaB * _cosTheta - bSinEtaB * _sinTheta;
    double yB = Cy + aCosEtaB * _sinTheta + bSinEtaB * _cosTheta;
    double xBDot = -aSinEtaB * _cosTheta - bCosEtaB * _sinTheta;
    double yBDot = -aSinEtaB * _sinTheta + bCosEtaB * _cosTheta;

    /*
    This controls the drawing in case of pies
    if (openNewFigure)
    {
    if (IsPieSlice)
    {
    path->BeginFigure(Point(Cx, Cy), false, false);
    path->LineTo(Point(xB, yB), true, true);
    }
    else
    {
    path->BeginFigure(Point(xB, yB), false, false);
    }
    }
    else
    {
    //path->LineTo(Point(xB, yB), true, true);
    }
    */

    //otherwise we're supposed to be already at the (xB,yB)

    double t = tan(0.5 * dEta);
    double alpha = sin(dEta) * (sqrt(4 + 3 * t * t) - 1) / 3;
    for (int i = 0; i < n; ++i)
    {
        //double etaA = etaB;
        double xA = xB;
        double yA = yB;
        double xADot = xBDot;
        double yADot = yBDot;
        etaB += dEta;
        cosEtaB = cos(etaB);
        sinEtaB = sin(etaB);
        aCosEtaB = A * cosEtaB;
        bSinEtaB = B * sinEtaB;
        aSinEtaB = A * sinEtaB;
        bCosEtaB = B * cosEtaB;
        xB = Cx + aCosEtaB * _cosTheta - bSinEtaB * _sinTheta;
        yB = Cy + aCosEtaB * _sinTheta + bSinEtaB * _cosTheta;
        xBDot = -aSinEtaB * _cosTheta - bCosEtaB * _sinTheta;
        yBDot = -aSinEtaB * _sinTheta + bCosEtaB * _cosTheta;
        if (degree == 1)
        {
            path->LineTo(Point(xB, yB));
        }
        else if (degree == 2)
        {
            double k = (yBDot * (xB - xA) - xBDot * (yB - yA)) / (xADot * yBDot - yADot * xBDot);
            path->QuadraticBezierTo(Point(xA + k * xADot, yA + k * yADot), Point(xB, yB));
        }
        else
        {
            path->CubicBezierTo(
                Point(xA + alpha * xADot, yA + alpha * yADot),
                Point(xB - alpha * xBDot, yB - alpha * yBDot),
                Point(xB, yB)
            );
        }
    }
    if (IsPieSlice)
    {
        path->LineTo(Point(Cx, Cy));
    }
}

/// <summary>
/// Calculates the angle between two vectors
/// </summary>
/// <param name="v1">Vector V1</param>
/// <param name="v2">Vector V2</param>
/// <returns>The signed angle between v2 and v1</returns>

double XUI::Core::Media::EllipticalArc::GetAngle(Vector v1, Vector v2)
{
    auto scalar = glm::dot((glm::vec2&)v1, (glm::vec2&)v2);
    return atan2(v1.x * v2.y - v2.x * v1.y, scalar);
}

/// <summary>
/// ArcTo Helper for StreamGeometryContext
/// </summary>
/// <param name="path">Target path</param>
/// <param name="p1">Start point</param>
/// <param name="p2">End point</param>
/// <param name="size">Ellipse radii</param>
/// <param name="theta">Ellipse theta (angle measured from the abscissa)</param>
/// <param name="isLargeArc">Large Arc Indicator</param>
/// <param name="clockwise">Clockwise direction flag</param>

void XUI::Core::Media::EllipticalArc::BuildArc(Interfaces::IStreamGeometryContext* path, Point p1, Point p2, Size size, double theta, bool isLargeArc, bool clockwise)
{

    // auto orthogonalizer = new RotateTransform(-theta);
    auto orth = SimpleMatrix(cos(theta), sin(theta), -sin(theta), cos(theta));
    auto rest = SimpleMatrix(cos(theta), -sin(theta), sin(theta), cos(theta));

    // auto restorer = orthogonalizer.Inverse;
    // if(restorer == null) throw new InvalidOperationException("Can't get a restorer!");

    Point p1S = orth * (Point((p1.x - p2.x) / 2, (p1.y - p2.y) / 2));

    double rx = size.Width;
    double ry = size.Height;
    double rx2 = rx * rx;
    double ry2 = ry * ry;
    double y1S2 = p1S.y * p1S.y;
    double x1S2 = p1S.x * p1S.x;

    double numerator = rx2*ry2 - rx2*y1S2 - ry2*x1S2;
    double denominator = rx2*y1S2 + ry2*x1S2;

    if (abs(denominator) < 1e-8)
    {
        path->LineTo(p2);
        return;
    }
    if ((numerator / denominator) < 0)
    {
        double lambda = x1S2 / rx2 + y1S2 / ry2;
        double lambdaSqrt = sqrt(lambda);
        if (lambda > 1)
        {
            rx *= lambdaSqrt;
            ry *= lambdaSqrt;
            rx2 = rx*rx;
            ry2 = ry*ry;
            numerator = rx2 * ry2 - rx2 * y1S2 - ry2 * x1S2;
            if (numerator < 0)
                numerator = 0;

            denominator = rx2 * y1S2 + ry2 * x1S2;
        }

    }

    double multiplier = sqrt(numerator / denominator);
    Point mulVec = Point(rx * p1S.y / ry, -ry * p1S.x / rx);

    int sign = (clockwise != isLargeArc) ? 1 : -1;

    Point cs = Point(mulVec.x * multiplier * sign, mulVec.y * multiplier * sign);

    Vector translation = Vector((p1.x + p2.x) / 2, (p1.y + p2.y) / 2);

    Point c = rest * (cs)+translation;

    // See "http://www.w3.org/TR/SVG/implnote.html#ArcConversionEndpointToCenter" to understand
    // how the ellipse center is calculated 


    // from here, W3C recommendations from the above link make less sense than Darth Vader pouring
    // some sea water in a water filter while standing in the water confused 

    // Therefore, we are on our own with our task of finding out lambda1 and lambda2
    // matching our points p1 and p2.

    // Fortunately it is not so difficult now, when we already know the ellipse centre.

    // We eliminate the offset, making our ellipse zero-centered, then we eliminate the theta,
    // making its Y and X axes the same as global axes. Then we can easily get our angles using
    // good old school formula for angles between vectors.

    // We should remember that this class expects true angles, and not the t-values for ellipse equation.
    // To understand how t-values are obtained, one should see Etas calculation in the constructor code.

    auto p1NoOffset = orth * (p1 - c);
    auto p2NoOffset = orth * (p2 - c);

    // if the arc is drawn clockwise, we swap start and end points
    auto revisedP1 = clockwise ? p1NoOffset : p2NoOffset;
    auto revisedP2 = clockwise ? p2NoOffset : p1NoOffset;


    auto thetaStart = GetAngle(Vector(1, 0), revisedP1);
    auto thetaEnd = GetAngle(Vector(1, 0), revisedP2);


    // Uncomment this to draw a pie
    // path->LineTo(c, true, true);
    // path->LineTo(clockwise ? p1 : p2, true,true);

    path->LineTo(clockwise ? p1 : p2);
    auto arc = EllipticalArc(c.x, c.y, rx, ry, theta, thetaStart, thetaEnd, false);
    arc.BuildArc(path, arc._maxDegree, arc._defaultFlatness, false);

    //uncomment this to draw a pie
    //path->LineTo(c, true, true);
}

/// <summary>
/// Tests if the interior of the closed path derived from this arc intersects the interior of a specified rectangular area.
/// The closed path is derived with respect to the IsPieSlice value.
/// </summary>

bool XUI::Core::Media::EllipticalArc::Intersects(double x, double y, double w, double h)
{
    double xPlusW = x + w;
    double yPlusH = y + h;
    return Contains(x, y) || Contains(xPlusW, y) || Contains(x, yPlusH) || Contains(xPlusW, yPlusH) ||
        IntersectOutline(x, y, xPlusW, y) || IntersectOutline(xPlusW,
            y, xPlusW, yPlusH) || IntersectOutline(xPlusW, yPlusH, x, yPlusH) ||
        IntersectOutline(x, yPlusH, x, y);
}

/// <summary>
/// Tests if the interior of the closed path derived from this arc intersects the interior of a specified rectangular area.
/// The closed path is derived with respect to the IsPieSlice value.
/// </summary>

bool XUI::Core::Media::EllipticalArc::Intersects(Rect r)
{
    return Intersects(r.x0(), r.y0(), r.width(), r.height());
}

XUI::Core::Media::EllipticalArc::SimpleMatrix::SimpleMatrix(double a, double b, double c, double d)
{
    _a = a;
    _b = b;
    _c = c;
    _d = d;
}
