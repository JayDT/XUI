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

#ifndef __XUI_GRIDSPLITTER_H__
#define __XUI_GRIDSPLITTER_H__

#include "UI/Thumb.h"
#include "UI/Grid.h"
#include "UI/Detail/DefinitionBase.h"

namespace XUI::UI
{
    /// <summary>
    ///     Represents the control that redistributes space between columns or rows of a Grid control.
    /// </summary>
    class TC_XUI_API GridSplitter : public Thumb
    {
    private:
        std::vector<std::shared_ptr<DefinitionBase>> _definitions;

        std::shared_ptr<Grid> _grid;

        Orientation _orientation;
        std::shared_ptr<DefinitionBase> _nextDefinition;
        std::shared_ptr<DefinitionBase> _prevDefinition;

        void GetDeltaConstraints(double &min, double &max);

    protected:
        void OnDragDelta(Interfaces::VectorEventArgs& e) override;

    private:
        double GetActualLength(DefinitionBase *definition);

        double GetMinLength(DefinitionBase *definition);

        double GetMaxLength(DefinitionBase *definition);

        bool IsStar(DefinitionBase *definition);

        void SetLengthInStars(DefinitionBase *definition, double value);

    protected:
        void OnAttachedToVisualTree(Controls::VisualTreeAttachmentEventArgs const& e) override;

    private:
        Orientation DetectOrientation();
    };
}


#endif	//#ifndef __XUI_GRIDSPLITTER_H__
