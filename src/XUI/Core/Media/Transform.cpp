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

#include "Transform.h"

#include <math.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/mat4x4.hpp"
#include "glm/gtx/transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace XUI::Core::Media;

XUI::Core::Media::Transform::~Transform()
{
}

Matrix XUI::Core::Media::Transform::GetInverse() const
{
    return GetTransform().Inverse();
}

Rect XUI::Core::Media::Transform::TransformBounds(Rect const &)
{
    return Rect();
}

void XUI::Core::Media::ScaleTransform::NotifyChanged(const char * propertyName)
{
    Matrix::CreateScale(_scale.x, _scale.y, 1.0f, m_transform);

    NotifyPropertyChanged::RaisePropertyChanged(propertyName);

    if (m_transformParent)
        m_transformParent->SetDirty();
}

void XUI::Core::Media::RotateTransform::NotifyChanged(const char * propertyName)
{
    Matrix::CreateFromYawPitchRoll(0.f, 0.f, glm::radians(_angle), m_transform);

    NotifyPropertyChanged::RaisePropertyChanged(propertyName);

    if (m_transformParent)
        m_transformParent->SetDirty();
}

void XUI::Core::Media::SkewTransform::NotifyChanged(const char * propertyName)
{
    Matrix::CreateFromYawPitchRoll(glm::radians(AngleX), glm::radians(AngleY), 0.f, m_transform);

    NotifyPropertyChanged::RaisePropertyChanged(propertyName);

    if (m_transformParent)
        m_transformParent->SetDirty();
}

void XUI::Core::Media::TranslateTransform::NotifyChanged(const char * propertyName)
{
    Matrix::CreateTranslation(X, Y, 0.0f, m_transform);

    NotifyPropertyChanged::RaisePropertyChanged(propertyName);

    if (m_transformParent)
        m_transformParent->SetDirty();
}

void XUI::Core::Media::TransformGroup::NotifyChanged(const char * propertyName)
{
    NotifyPropertyChanged::RaisePropertyChanged(propertyName);

    if (m_transformParent)
        m_transformParent->SetDirty();
}

void XUI::Core::Media::TransformGroup::SetDirty()
{
    changed = true;

    if (m_transformParent)
        m_transformParent->SetDirty();
}

void XUI::Core::Media::TransformGroup::AddTransformChild(Transform * transform)
{
    System::ThrowIfFailed<System::ArgumentException>(transform->m_transformParent == nullptr);

    _childrens.Add(transform->shared_from_base_static<Transform>());
    transform->m_transformParent = this;
}

void XUI::Core::Media::TransformGroup::RemoveTransformChild(Transform * transform)
{
    System::ThrowIfFailed<System::ArgumentException>(transform->m_transformParent == this);

    for(auto itr = _childrens.begin(); itr != _childrens.end(); ++itr)
        if ((*itr).get() == transform)
        {
            transform->m_transformParent = nullptr;
            _childrens.Remove(*itr);
            break;
        }
}

XUI::Core::Media::Matrix const & XUI::Core::Media::TransformGroup::GetTransform() const
{
    if (changed)
    {
        changed = false;

        glm::mat3x2 result(1);

        for(auto const& transform : Children.GetNativeEnumerator())
            result = result * XUI::Core::Media::Matrix::ToMat3x2(transform->GetTransform());

        m_transform = XUI::Core::Media::Matrix::From(result);
    }

    return m_transform; 
}

void XUI::Core::Media::MatrixTransform::NotifyChanged(const char * propertyName)
{
    m_transform = m_matrix;

    NotifyPropertyChanged::RaisePropertyChanged(propertyName);

    if (m_transformParent)
        m_transformParent->SetDirty();
}
