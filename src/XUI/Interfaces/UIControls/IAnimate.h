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

#ifndef _XUI_IANITMATE_H__
#define _XUI_IANITMATE_H__

#include <string>
#include "Core/Dependencies/DependencyObject.h"

namespace System::Reflection::Assembly
{
    class AssemblyModulXUI;
}

namespace XUI::Interfaces
{
    struct AnimEntry
    {
        std::chrono::milliseconds duration;

        /// <summary>
        /// Returns the value of the transition for the specified progress.
        /// </summary>
        /// <param name="progress">The progress of the transition, from 0 to 1.</param>
        /// <param name="start">The start value of the transition.</param>
        /// <param name="finish">The end value of the transition.</param>
        /// <returns>
        /// A value between <paramref name="start"/> and <paramref name="finish"/> as determined
        /// by <paramref name="progress"/>.
        /// </returns>
        //object Ease(double progress, object start, object finish);
    };

    typedef std::vector<AnimEntry> TransitionsList;

    /// <summary>
    /// Base class for control which can have property transitions.
    /// </summary>
    class AnimHandler : public Core::Dependency::DependencyObject
    {
        private:

        /// <summary>
        /// The property transitions for the control.
        /// </summary>
        TransitionsList _propertyTransitions;

    public:

        AnimHandler() {}

        virtual ~AnimHandler()
        {
        }

        /// <summary>
        /// Gets or sets the property transitions for the control.
        /// </summary>
        /// <value>
        /// The property transitions for the control.
        /// </value>
        TransitionsList& PropertyTransitions() { return _propertyTransitions; }

        /// <summary>
        /// Reacts to a change in a <see cref="AvaloniaProperty"/> value in order to animate the
        /// change if a <see cref="PropertyTransition"/> is set for the property..
        /// </summary>
        /// <param name="e">The event args.</param>
        void OnPropertyChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
        {
            //if (e.Priority != BindingLevel.Animation && _propertyTransitions != null)
            //{
            //    var match = _propertyTransitions.FirstOrDefault(x => x.Property == e.Property);
            //
            //    if (match != null)
            //    {
            //        Animate.Property(this, e.Property, e.OldValue, e.NewValue, match.Easing, match.Duration);
            //    }
            //}
        }

    };
}

#endif //!_XUI_IANITMATE_H__