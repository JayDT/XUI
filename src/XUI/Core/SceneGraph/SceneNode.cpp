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

#include "SceneNode.h"
#include "UI/Controls/InputElement.h"
#include "Core/Media/Transform.h"
#include "Platform/UIService.h"
#include "Platform/UIRenderer.h"

#include "Core/GlobalInitializerScheduler.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

using namespace XUI;
using namespace XUI::Core;
using namespace XUI::Core::SceneGraph;

STATIC_CLASS_INITIALIZER(SceneNode, SceneNode::StaticClassInitializer)

void SceneNode::StaticClassInitializer()
{
    UI::Controls::Visual::ClipToBoundsProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        UI::Controls::Visual* control = e.Sender->Static_As<UI::Controls::Visual>();
        if (control && control->RenderScene())
            control->RenderScene()->ClipToBounds = control->ClipToBounds;
    });
    UI::Controls::Visual::OpacityProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        UI::Controls::Visual* control = e.Sender->Static_As<UI::Controls::Visual>();
        if (control && control->RenderScene())
            control->RenderScene()->Opacity = control->Opacity;
    });
    UI::Controls::Visual::IsVisibleProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const & e)
    {
        UI::Controls::Visual* control = e.Sender->Static_As<UI::Controls::Visual>();
        if (control && control->RenderScene())
        {
            control->RenderScene()->m_isVisible = control->IsVisible;
            control->RenderScene()->InvalidateRender();
        }
    });
}

SceneNode::SceneNode(UI::Controls::Visual* visual)
    : m_owner(visual)
    , HitTestVisible(true)
    , AbsoluteTransformation(Media::Matrix::Identity())
{
}

std::string SceneNode::ToString() const
{
    return m_owner->GetName();
}

void SceneNode::OnRegisterSceneNode()
{
}

void SceneNode::OnDispatchSceneNode()
{
}

void SceneNode::Detached()
{
    m_owner = nullptr;
}

void SceneNode::DelinkAttachmentTree(bool withChildrens)
{
}

void SceneNode::UpdateTransformation(Media::Matrix const& contextTransform, Core::Media::Rect const& availableSize)
{
    System::ThrowIfFailed<System::InvalidOperationException>(m_owner != nullptr);

    if (!m_owner->IsMeasureValid || !m_owner->IsArrangeValid)
    {
        auto renderService = Platform::UIService::Instance()->Get<Platform::UIRender>();
        renderService->AddDirty(m_owner);
        return;
    }
    m_renderInvalidated = false;

    m_isVisible = m_owner->IsVisible;
    Opacity = m_owner->Opacity;
    ClipToBounds = m_owner->ClipToBounds;
    Bounds = Core::Media::Rect(m_owner->Bounds.wh());
    AbsoluteTransformationBoundingBox = nullptr;

    //if (m_isVisible && Opacity > 0)
    {
        RelativeTransformation = Core::Media::Matrix::Identity();

        auto visualTransform = m_owner->RenderTransform;
        if (visualTransform != nullptr)
        {
            auto origin = m_owner->RenderTransformOrigin.ToPixels(Core::Media::Size(Bounds.width(), Bounds.height()));
            auto offset = Core::Media::Matrix::CreateTranslation(origin.x, origin.y);
            auto transform = Core::Media::Matrix::ToMat3x2(visualTransform->GetTransform());
            auto offset2 = Core::Media::Matrix::CreateTranslation(-origin.x, -origin.y);

            RelativeTransformation = Core::Media::Matrix::From(offset2 * visualTransform->GetTransform() * offset);
        }

        RelativeTransformation[3][0] += m_owner->Bounds.x0();
        RelativeTransformation[3][1] += m_owner->Bounds.y0();

        AbsoluteTransformation = Core::Media::Matrix(contextTransform * RelativeTransformation);

        m_owner->_transformedBounds = ClipRect = availableSize.intersect(Bounds * AbsoluteTransformation);
        RelativeClipRect = (ClipRect * AbsoluteTransformation.Inverse());

        if (m_owner->Bounds.width() > 0 && m_owner->Bounds.height() > 0)
        {
            HitTestVisible = m_isVisible && (!ClipToBounds || ClipRect.height() > 0 && ClipRect.width() > 0 && m_owner->CanHitTest());

            std::vector<Media::Vector> cornersOfBound;
            cornersOfBound.push_back(ClipRect.TopRight);
            cornersOfBound.push_back(ClipRect.TopLeft);
            cornersOfBound.push_back(ClipRect.BottomLeft);
            cornersOfBound.push_back(ClipRect.BottomRight);

            AbsoluteTransformationBoundingBox = std::make_shared<Media::TransformedBounds>(std::move(cornersOfBound));
        }
        else
        {
            HitTestVisible = m_isVisible && !ClipToBounds && m_owner->CanHitTest();
        }
    }

}

void SceneNode::NotifyWholeTree(std::function<void(SceneNode*)> notify)
{
}

void SceneNode::BeginDraw(SpriteBatch *spriteBatch)
{
}

void SceneNode::EndDraw(SpriteBatch *spriteBatch)
{
}

void SceneNode::Render(SpriteBatch *spriteBatch)
{
}

void SceneNode::InvalidateRender()
{
    m_owner->CallVisualDescendents([](UI::Controls::Visual* visual)
    {
        if (visual->RenderScene())
            visual->RenderScene()->m_renderInvalidated = true;
    });
}

bool SceneNode::CanFocus()
{
    return true;
}

int SceneNode::GetID() const
{
    return _ID;
}

void SceneNode::SetID(const int &value)
{
    _ID = value;
}

bool XUI::Core::SceneGraph::SceneNode::IsVisible() const
{
    return m_isVisible;
}

bool XUI::Core::SceneGraph::SceneNode::IsEnabled() const
{
    return true;
}

Media::Matrix XUI::Core::SceneGraph::SceneNode::GetAbsoluteTransform(Media::Matrix const & contextTransform)
{
    return Core::Media::Matrix(contextTransform * RelativeTransformation);
}

Media::Rect XUI::Core::SceneGraph::SceneNode::GetClipRect(Media::Matrix const & contextTransform)
{
    return RelativeClipRect * contextTransform;
}
