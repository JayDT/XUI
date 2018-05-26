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

#ifndef _XUI_MEDIA_ELLIPTICAL_H__
#define _XUI_MEDIA_ELLIPTICAL_H__

#include "Interfaces/IGeometry.h"

namespace XUI::Core::Media
{
    /// <summary>
    /// This class represents an elliptical arc on a 2D plane.
    /// 
    /// This class is adapted for use with WPF StreamGeometryContext, and needs to be created explicitly
    /// for each particular arc.
    /// 
    /// Some helpers
    /// 
    /// It can handle ellipses which are not aligned with the x and y reference axes of the plane,
    /// as well as their parts.
    /// 
    /// Another improvement is that this class can handle degenerated cases like for example very 
    /// flat ellipses(semi-minor axis much smaller than semi-major axis) and drawing of very small 
    /// parts of such ellipses at very high magnification scales.This imply monitoring the drawing 
    /// approximation error for extremely small values.Such cases occur for example while drawing 
    /// orbits of comets near the perihelion.
    /// 
    /// When the arc does not cover the complete ellipse, the lines joining the center of the 
    /// ellipse to the endpoints can optionally be included or not in the outline, hence allowing 
    /// to use it for pie-charts rendering. If these lines are not included, the curve is not 
    /// naturally closed.
    /// </summary>
    class EllipticalArc
    {
        /// <summary>
        /// Abscissa of the center of the ellipse
        /// </summary>
        double Cx;
        /// <summary>
        /// Ordinate of the center of the ellipse
        /// </summary>
        double Cy;
        /// <summary>
        /// Semi-major axis
        /// </summary>
        double A;
        /// <summary>
        /// Semi-minor axis
        /// </summary>
        double B;
        /// <summary>
        /// Orientation of the major axis with respect to the x axis
        /// </summary>
        double Theta;
        /// <summary>
        /// Pre-calculated cosine value for the major-axis-to-X orientation (Theta)
        /// </summary>
        double _cosTheta;
        /// <summary>
        /// Pre-calculated sine value for the major-axis-to-X orientation (Theta)
        /// </summary>
        double _sinTheta;
        /// <summary>
        /// Start angle of the arc
        /// </summary>
        double Eta1;
        /// <summary>
        /// End angle of the arc
        /// </summary>
        double Eta2;
        /// <summary>
        /// Abscissa of the start point
        /// </summary>
        double X1;
        /// <summary>
        /// Ordinate of the start point
        /// </summary>
        double Y1;
        /// <summary>
        /// Abscissa of the end point
        /// </summary>
        double X2;
        /// <summary>
        /// Ordinate of the end point
        /// </summary>
        double Y2;
        /// <summary>
        /// Abscissa of the first focus
        /// </summary>
        double FirstFocusX;
        /// <summary>
        /// Ordinate of the first focus
        /// </summary>
        double FirstFocusY;
        /// <summary>
        /// Abscissa of the second focus
        /// </summary>
        double SecondFocusX;
        /// <summary>
        /// Ordinate of the second focus
        /// </summary>
        double SecondFocusY;
        /// <summary>
        /// Abscissa of the leftmost point of the arc
        /// </summary>
        double _xLeft;
        /// <summary>
        /// Ordinate of the highest point of the arc
        /// </summary>
        double _yUp;
        /// <summary>
        /// Horizontal width of the arc
        /// </summary>
        double _width;
        /// <summary>
        /// Vertical height of the arc
        /// </summary>
        double _height;
        /// <summary>
        /// Indicator for center to endpoints line inclusion
        /// </summary>
        bool IsPieSlice;
        /// <summary>
        /// Maximal degree for Bezier curve approximation
        /// </summary>
        int _maxDegree;
        /// <summary>
        /// Default flatness for Bezier curve approximation
        /// </summary>
        double _defaultFlatness;

        /// <summary>
        /// Indicator for semi-major axis significance (compared to semi-minor one).
        /// Computed by dividing the (A-B) difference by the value of A.
        /// This indicator is used for an early escape in intersection test
        /// </summary>
        double F;
        /// <summary>
        /// Indicator used for an early escape in intersection test
        /// </summary>
        double E2;
        /// <summary>
        /// Indicator used for an early escape in intersection test
        /// </summary>
        double G;
        /// <summary>
        /// Indicator used for an early escape in intersection test
        /// </summary>
        double G2;

    public:
        /// <summary>
        /// Builds an elliptical arc composed of the full unit circle around (0,0)
        /// </summary>
        EllipticalArc();

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
        EllipticalArc(Point const& center, double a, double b, double theta, double lambda1, double lambda2,
                      bool isPieSlice);
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
        EllipticalArc(double cx, double cy, double a, double b, double theta, double lambda1, double lambda2,
            bool isPieSlice);
        /// <summary>
        /// Build a full ellipse from its canonical geometrical elements
        /// </summary>
        /// <param name="center">Center of the ellipse</param>
        /// <param name="a">Semi-major axis</param>
        /// <param name="b">Semi-minor axis</param>
        /// <param name="theta">Orientation of the major axis with respect to the x axis</param>
        EllipticalArc(Point const& center, double a, double b, double theta);

        /// <summary>
        /// Build a full ellipse from its canonical geometrical elements
        /// </summary>
        /// <param name="cx">Abscissa of the center of the ellipse</param>
        /// <param name="cy">Ordinate of the center of the ellipse</param>
        /// <param name="a">Semi-major axis</param>
        /// <param name="b">Semi-minor axis</param>
        /// <param name="theta">Orientation of the major axis with respect to the x axis</param>
        EllipticalArc(double cx, double cy, double a, double b, double theta);

        /// <summary>
        /// Sets the maximal degree allowed for Bezier curve approximation.
        /// </summary>
        /// <param name="maxDegree">Maximal allowed degree (must be between 1 and 3)</param>
        /// <exception cref="ArgumentException">Thrown if maxDegree is not between 1 and 3</exception>
        void SetMaxDegree(int maxDegree);

        /// <summary>
        /// Sets the default flatness for Bezier curve approximation
        /// </summary>
        /// <param name="defaultFlatness">default flatness (must be greater than 1e-10)</param>
        /// <exception cref="ArgumentException">Thrown if defaultFlatness is lower than 1e-10</exception>
        void SetDefaultFlatness(double defaultFlatness);

        /// <summary>
        /// Computes the locations of the focii
        /// </summary>
        void ComputeFocii();

        /// <summary>
        /// Computes the locations of the endpoints
        /// </summary>
        void ComputeEndPoints();

        /// <summary>
        /// Computes the bounding box
        /// </summary>
        void ComputeBounds();

        /// <summary>
        /// Computes the flatness parameters used in intersection tests
        /// </summary>
        void ComputeDerivedFlatnessParameters();

        /// <summary>
        /// Computes the value of a rational function.
        /// This method handles rational functions where the numerator is quadratic
        /// and the denominator is linear
        /// </summary>
        /// <param name="x">Abscissa for which the value should be computed</param>
        /// <param name="c">Coefficients array of the rational function</param>
        /// <returns></returns>
        static double RationalFunction(double x, float const* c);

        /// <summary>
        /// Estimate the approximation error for a sub-arc of the instance
        /// </summary>
        /// <param name="degree">Degree of the Bezier curve to use (1, 2 or 3)</param>
        /// <param name="etaA">Start angle of the sub-arc</param>
        /// <param name="etaB">End angle of the sub-arc</param>
        /// <returns>Upper bound of the approximation error between the Bezier curve and the real ellipse</returns>
        double EstimateError(int degree, double etaA, double etaB);

        /// <summary>
        /// Get the elliptical arc point for a given angular parameter
        /// </summary>
        /// <param name="lambda">Angular parameter for which point is desired</param> 
        /// <returns>The desired elliptical arc point location</returns>
        Point PointAt(double lambda);

        /// <summary>
        /// Tests if the specified coordinates are inside the closed shape formed by this arc.
        /// If this is not a pie, then a shape derived by adding a closing chord is considered.
        /// </summary>
        /// <param name="x">Abscissa of the test point</param>
        /// <param name="y">Ordinate of the test point</param>
        /// <returns>True if the specified coordinates are inside the closed shape of this arc</returns>
        bool Contains(double x, double y);

        /// <summary>
        /// Tests if a line segment intersects the arc
        /// </summary>
        /// <param name="xA">abscissa of the first point of the line segment</param>
        /// <param name="yA">ordinate of the first point of the line segment</param>
        /// <param name="xB">abscissa of the second point of the line segment</param>
        /// <param name="yB">ordinate of the second point of the line segment</param>
        /// <returns>true if the two line segments intersect</returns>
        bool IntersectArc(double xA, double yA, double xB, double yB);

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
        static bool Intersect(double x1, double y1, double x2, double y2, double xA, double yA, double xB,
            double yB);

        /// <summary>
        /// Tests if a line segment intersects the outline
        /// </summary>
        /// <param name="xA">Abscissa of the first point of the line segment</param>
        /// <param name="yA">Ordinate of the first point of the line segment</param>
        /// <param name="xB">Abscissa of the second point of the line segment</param>
        /// <param name="yB">Ordinate of the second point of the line segment</param>
        /// <returns>true if the two line segments intersect</returns>
        bool IntersectOutline(double xA, double yA, double xB, double yB);

        /// <summary>
        /// Tests if the interior of a closed path derived from this arc entirely contains the specified rectangular area.
        /// The closed path is derived with respect to the IsPieSlice value.
        /// </summary>
        /// <param name="x">Abscissa of the upper-left corner of the test rectangle</param>
        /// <param name="y">Ordinate of the upper-left corner of the test rectangle</param>
        /// <param name="w">Width of the test rectangle</param>
        /// <param name="h">Height of the test rectangle</param>
        /// <returns>true if the interior of a closed path derived from this arc entirely contains the specified rectangular area; false otherwise</returns>
        bool Contains(double x, double y, double w, double h);

        /// <summary>
        /// Tests if a specified Point2D is inside the boundary of a closed path derived from this arc.
        /// The closed path is derived with respect to the IsPieSlice value.
        /// </summary>
        /// <param name="p">Test point</param>
        /// <returns>true if the specified point is inside a closed path derived from this arc</returns>
        bool Contains(Point p);

        /// <summary>
        /// Tests if the interior of a closed path derived from this arc entirely contains the specified Rectangle2D.
        /// The closed path is derived with respect to the IsPieSlice value.
        /// </summary>
        /// <param name="r">Test rectangle</param>
        /// <returns>True if the interior of a closed path derived from this arc entirely contains the specified Rectangle2D; false otherwise</returns>
        bool Contains(Rect r);

        /// <summary>
        /// Returns an integer Rectangle that completely encloses the closed path derived from this arc.
        /// The closed path is derived with respect to the IsPieSlice value.
        /// </summary> 
        Rect GetBounds();

        /// <summary>
        /// Builds the arc outline using given StreamGeometryContext and default (max) Bezier curve degree and acceptable error of half a pixel (0.5)
        /// </summary>
        /// <param name="path">A StreamGeometryContext to output the path commands to</param>
        void BuildArc(Interfaces::IStreamGeometryContext* path);

        /// <summary>
        /// Builds the arc outline using given StreamGeometryContext
        /// </summary>
        /// <param name="path">A StreamGeometryContext to output the path commands to</param>
        /// <param name="degree">degree of the Bezier curve to use</param>
        /// <param name="threshold">acceptable error</param>
        /// <param name="openNewFigure">if true, a new figure will be started in the specified StreamGeometryContext</param>
        void BuildArc(Interfaces::IStreamGeometryContext* path, int degree, double threshold, bool openNewFigure);

        /// <summary>
        /// Calculates the angle between two vectors
        /// </summary>
        /// <param name="v1">Vector V1</param>
        /// <param name="v2">Vector V2</param>
        /// <returns>The signed angle between v2 and v1</returns>
        static double GetAngle(Vector v1, Vector v2);

        /// <summary>
        /// Simple matrix used for rotate transforms. 
        /// At some point I did not trust the WPF Matrix struct, and wrote my own simple one -_-
        /// This is supposed to be replaced with proper WPF Matrices everywhere
        /// </summary>
        struct SimpleMatrix
        {
            double _a, _b, _c, _d;

            SimpleMatrix(double a, double b, double c, double d);

            Point operator *(Point const& p);
        };

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
        static void BuildArc(Interfaces::IStreamGeometryContext* path, Point p1, Point p2, Size size, double theta, bool isLargeArc, bool clockwise);

        /// <summary>
        /// Tests if the interior of the closed path derived from this arc intersects the interior of a specified rectangular area.
        /// The closed path is derived with respect to the IsPieSlice value.
        /// </summary>
        bool Intersects(double x, double y, double w, double h);

        /// <summary>
        /// Tests if the interior of the closed path derived from this arc intersects the interior of a specified rectangular area.
        /// The closed path is derived with respect to the IsPieSlice value.
        /// </summary>
        bool Intersects(Rect r);
    };

    void ArcTo(XUI::Interfaces::IStreamGeometryContext* streamGeometryContextImpl, XUI::Core::Media::Point const& currentPoint, XUI::Core::Media::Point const& point, XUI::Core::Media::Size const& size, double rotationAngle, bool isLargeArc, XUI::Interfaces::SweepDirection sweepDirection);
}

#endif //!