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

#ifndef __XUI_SCENENODE_H__
#define __XUI_SCENENODE_H__

#include "Core/Media/MediaTypes.h"
#include "standard/BasicPrimities.h"
#include "standard/events.h"

#include <rxcpp/rx.hpp>

MetaAssemblyBegin
Meta(modul: ReflInclude("UI/Controls/InputElement.h"))
MetaAssemblyEnd

namespace System::Reflection::Assembly
{
    class AssemblyModulXUI;
}

namespace XUI::UI::Controls
{
    class Visual;
}

namespace XUI::Core::SceneGraph
{
    // Not Implement Yet
    class SpriteBatch {};

    // Simple render cache object
    class SceneNode : public System::Reflection::Object
    {
        friend class UI::Controls::Visual;
    private:
        UI::Controls::Visual* m_owner = nullptr;
        int _ID = -1;
        bool m_renderInvalidated = true;
        bool m_isVisible = true;

    public:

        //! Absolute transformation of the node.
        bool ClipToBounds = false;
        bool HitTestVisible = true;
        std::shared_ptr<Media::TransformedBounds> AbsoluteTransformationBoundingBox;
        Media::Matrix RelativeTransformation;
        Media::Matrix AbsoluteTransformation;
        Media::Rect ClipRect;
        Media::Rect RelativeClipRect;
        Core::Media::Rect Bounds;
        double Opacity = 1.0;

        virtual ~SceneNode()
        {
        }

        SceneNode(UI::Controls::Visual*);

        static void StaticClassInitializer();

        std::string ToString() const override;

        virtual void OnRegisterSceneNode();
        virtual void OnDispatchSceneNode();

        //! Removes this scene node from the scene
        /** If no other grab exists for this node, it will be deleted.
        */
        void Detached();

        void DelinkAttachmentTree(bool withChildrens = false);

        virtual void UpdateTransformation(Media::Matrix const& contextTransform, Core::Media::Rect const& availableSize);

        void NotifyWholeTree(std::function<void(SceneNode*)> notify);

        virtual void BeginDraw(SpriteBatch *spriteBatch);
        virtual void EndDraw(SpriteBatch *spriteBatch);
        virtual void Render(SpriteBatch *spriteBatch);

        virtual void InvalidateRender();
        virtual bool CanFocus();

        //! ID of the node.
        int GetID() const;
        void SetID(const int &value);

        bool IsVisible() const;
        bool IsEnabled() const;

        bool InvalidatedVisual() const { return m_renderInvalidated; }
        UI::Controls::Visual* Visual() const { return m_owner; }

        Media::Matrix GetAbsoluteTransform(Media::Matrix const& contextTransform);
        Media::Rect GetClipRect(Media::Matrix const& contextTransform);

    };
}


#endif	//#ifndef __XUI_SCENENODE_H__
