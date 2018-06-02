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

#ifndef __XUI_PROGRESSBAR_H__
#define __XUI_PROGRESSBAR_H__

#include "UI/Detail/RangeBase.h"
#include "UI/Border.h"
#include "UI/Panel.h"

#include <chrono>

namespace XUI::UI
{
    /// <summary>
    /// A control used to indicate the progress of an operation.
    /// </summary>
    class ProgressBar : public RangeBase
    {
    public:
        class IndeterminateAnimation
        {
        private:
            ProgressBar* _progressBar;
            rxcpp::subscription _indeterminateBindSubscription;
            std::chrono::time_point<std::chrono::system_clock> _startTime;
            bool _disposed = false;

        public:
            PROPERTY_GET_ONLY(bool, Disposed);
            bool GetDisposed() const;

            IndeterminateAnimation(ProgressBar *progressBar);
            ~IndeterminateAnimation();

            void InitializeComponent();

            void StartAnimation(ProgressBar *progressBar);
            void Stop();

        private:
            Core::Media::Rect GetAnimationRect(std::chrono::time_point<std::chrono::system_clock> const& time);

            void AnimationTick(Core::Media::Rect const& rect);
        };

        static XUI::Core::Dependency::RefDependencyProperty IsIndeterminateProperty;

        static XUI::Core::Dependency::RefDependencyProperty OrientationProperty;

        /// <inheritdoc/>
        void BeginInit() override;

    private:
        std::shared_ptr<Border> _indicator;
        IndeterminateAnimation _indeterminateAnimation;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="ProgressBar"/> class.
        /// </summary>
        static void StaticClassInitializer();

        ProgressBar();
        virtual ~ProgressBar() {}

        PROPERTY_(bool , IsIndeterminate);
        bool GetIsIndeterminate() const;
        void SetIsIndeterminate(const bool &value);

        PROPERTY_(UI::Orientation , Orientation);
        UI::Orientation GetOrientation() const;
        void SetOrientation(UI::Orientation value);

        /// <inheritdoc/>
    protected:
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

        /// <inheritdoc/>
        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

    private:
        void UpdateIndicator(Core::Media::Size const& bounds);

        void UpdateOrientation(UI::Orientation orientation);

        void UpdateIsIndeterminate(bool isIndeterminate);

        void ValueChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);
    };
}


#endif	//#ifndef __XUI_PROGRESSBAR_H__
