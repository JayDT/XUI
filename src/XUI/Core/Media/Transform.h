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

#ifndef _XUI_TRANSFORM_H__
#define _XUI_TRANSFORM_H__

#include "Core/Media/Animation/Animatable.h"
#include "Core/Media/MediaTypes.h"
#include "standard/Platform/Define.h"
#include "standard/BasicPrimities.h"
#include "standard/events.h"

namespace XUI::Core::Media
{
    /// <summary>
    /// Describes the thickness of a frame around a rectangle.
    /// </summary>
    struct TC_XUI_API Transform : public Animation::Animatable
    {
        virtual Core::Media::Matrix const& GetTransform() const { return m_transform; }

        //  Gets the inverse of this transform, if it exists.
        Core::Media::Matrix GetInverse() const;

        // Transforms the specified bounding box and returns an axis-aligned bounding box
        Rect TransformBounds(Rect const&);

        virtual ~Transform();

        // Inherited via Animatable
        virtual void SetDirty() {}

        Transform* m_transformParent = nullptr;
    protected:

        mutable Core::Media::Matrix m_transform;
    };

    class TC_XUI_API MatrixTransform : public Transform
    {
    private:

        XUI::Core::Media::Matrix m_matrix;

        void NotifyChanged(const char* propertyName);

    public:

        MatrixTransform(XUI::Core::Media::Matrix const& matrix)
            : m_matrix(matrix)
        {
            m_transform = matrix;
        }

        PROPERTY_(XUI::Core::Media::Matrix, Matrix);
        XUI::Core::Media::Matrix const& GetMatrix() const { return m_matrix; }
        void SetMatrix(XUI::Core::Media::Matrix const& value)
        {
            m_matrix = value;
            NotifyChanged(nameof(Matrix));
        }
    };

    class TC_XUI_API ScaleTransform : public Transform
    {
    private:

        Vector _center;
        Vector _scale;

        void NotifyChanged(const char* propertyName);

    public:

        PROPERTY_(float, CenterX);
        float PROPERTY_GET(CenterX)() const { return _center.x; }
        void PROPERTY_SET(CenterX)(float value) 
        { 
            _center.x = value;
            NotifyChanged(nameof(CenterX));
        }

        PROPERTY_(float, CenterY);
        float PROPERTY_GET(CenterY)() const { return _center.y; }
        void PROPERTY_SET(CenterY)(float value) 
        { 
            _center.y = value; 
            NotifyChanged(nameof(CenterY));
        }

        PROPERTY_(float, ScaleX);
        float PROPERTY_GET(ScaleX)() const { return _scale.x; }
        void PROPERTY_SET(ScaleX)(float value) 
        { 
            _scale.x = value; 
            NotifyChanged(nameof(ScaleX));
        }

        PROPERTY_(float, ScaleY);
        float PROPERTY_GET(ScaleY)() const { return _scale.y; }
        void PROPERTY_SET(ScaleY)(float value) 
        { 
            _scale.y = value; 
            NotifyChanged(nameof(ScaleY));
        }
    };

    class TC_XUI_API RotateTransform : public Transform
    {
        private:

        Vector _center;
        double _angle;

        void NotifyChanged(const char* propertyName);

    public:

        PROPERTY_(float, CenterX);
        float PROPERTY_GET(CenterX)() const { return _center.x; }
        void PROPERTY_SET(CenterX)(float value)
        {
            _center.x = value;
            NotifyChanged(nameof(CenterX));
        }

        PROPERTY_(float, CenterY);
        float PROPERTY_GET(CenterY)() const { return _center.y; }
        void PROPERTY_SET(CenterY)(float value)
        {
            _center.y = value;
            NotifyChanged(nameof(CenterY));
        }

        PROPERTY_(double, Angle);
        double PROPERTY_GET(Angle)() const { return _angle; }
        void PROPERTY_SET(Angle)(double value)
        {
            _angle = value;
            NotifyChanged(nameof(Angle));
        }
    };

    class TC_XUI_API SkewTransform : public Transform
    {
        private:

        Vector _center;
        double _angleX;
        double _angleY;

        void NotifyChanged(const char* propertyName);

    public:

        PROPERTY_(float, CenterX);
        float PROPERTY_GET(CenterX)() const { return _center.x; }
        void PROPERTY_SET(CenterX)(float value)
        {
            _center.x = value;
            NotifyChanged(nameof(CenterX));
        }

        PROPERTY_(float, CenterY);
        float PROPERTY_GET(CenterY)() const { return _center.y; }
        void PROPERTY_SET(CenterY)(float value)
        {
            _center.y = value;
            NotifyChanged(nameof(CenterY));
        }

        PROPERTY_(double, AngleX);
        double PROPERTY_GET(AngleX)() const { return _angleX; }
        void PROPERTY_SET(AngleX)(double value)
        {
            _angleX = value;
            NotifyChanged(nameof(AngleX));
        }

        PROPERTY_(double, AngleY);
        double PROPERTY_GET(AngleY)() const { return _angleY; }
        void PROPERTY_SET(AngleY)(double value)
        {
            _angleY = value;
            NotifyChanged(nameof(AngleX));
        }
    };

    class TC_XUI_API TranslateTransform : public Transform
    {
        private:

        double _X;
        double _Y;

        void NotifyChanged(const char* propertyName);

    public:

        PROPERTY_(double, X);
        double PROPERTY_GET(X)() const { return _X; }
        void PROPERTY_SET(X)(double value)
        {
            _X = value;
            NotifyChanged(nameof(X));
        }

        PROPERTY_(double, Y);
        double PROPERTY_GET(Y)() const { return _Y; }
        void PROPERTY_SET(Y)(double value)
        {
            _Y = value;
            NotifyChanged(nameof(X));
        }
    };

    class TC_XUI_API TransformGroup : public Transform
    {
        private:

        mutable bool changed = false;
        System::Collection::Vector<std::shared_ptr<Transform>> _childrens;

        void NotifyChanged(const char* propertyName);
        void SetDirty() override;

    public:

        Meta(XamlCPP::ContentAttribute())
        PROPERTY_GET_ONLY(System::Collection::Vector<std::shared_ptr<Transform>> const&, Children);
        System::Collection::Vector<std::shared_ptr<Transform>> const& PROPERTY_GET(Children)() const { return _childrens; }

        void AddTransformChild(Transform*);
        void RemoveTransformChild(Transform*);

        Core::Media::Matrix const& GetTransform() const override final;
    };
}

#endif //!_XUI_TRANSFORM_H__