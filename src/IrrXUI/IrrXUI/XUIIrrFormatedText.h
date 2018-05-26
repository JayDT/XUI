#ifndef __WCLIENT_XUI_PLATFORM_FORMATED_TEXT_H__
#define __WCLIENT_XUI_PLATFORM_FORMATED_TEXT_H__

#include "Reflection/Runtime/CXXReflection.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include "XUI/Core/Media/MediaTypes.h"
#include "XUI/Core/Media/FontStyle.h"
#include "XUI/Core/Media/Geometries/PathGeometry.h"
#include "XUI/Interfaces/IFormatedText.h"
#include "XUI/Interfaces/IBrush.h"
#include "XUI/Interfaces/IDrawingContext.h"

#include "rect.h"
#include "irrArray.h"

//TODO:
// - sprite animation

namespace irr
{
    class IrrlichtDevice;

    namespace gui
    {
        class IGUIFont;
    }

    namespace video
    {
        class IImage;
        class ITexture;
        class IVideoDriver;
    }
}

namespace IrrBackend
{
    struct FBrushRange
    {
        FBrushRange(XUI::Interfaces::IBrush* brush, int startIndex, int length)
        {
            StartIndex = startIndex;
            Length = length;
            Brush = brush;
        }

        int Length;
        int StartIndex;
        XUI::Interfaces::IBrush* Brush;

        int EndIndex() const { return StartIndex + Length - 1; }
        bool Intersects(int index, int len) const
        {
            return (index + len) > StartIndex &&
                (StartIndex + Length) > index;
        }

        //std::string ToString() const
        //{
        //    return "{StartIndex}-{EndIndex}";
        //}
    };

    struct FDrawBatch
    {
        irr::core::array<irr::core::position2di> positions;
        irr::core::array<irr::core::recti> sourceRects;
        uint32 textureNumber;
    };

    class XUIIrrFormatedText : public XUI::Interfaces::IFormattedText
    {
        std::wstring m_text;
        XUI::Core::Media::Typeface _typeface;
        XUI::Core::Media::TextAlignment _textAlignment;
        XUI::Core::Media::TextWrapping _wrapping;
        XUI::Core::Media::Size _constraint;
        float _lineHeight = 0;
        float _lineOffset = 0;
        XUI::Core::Media::Size _size;
        std::vector<XUI::Interfaces::FormattedTextLine> _lines;
        std::vector<FBrushRange> _foregroundBrushes;
        std::vector<irr::core::recti> _rects;

        irr::gui::IGUIFont* _typeFace;

        // For Sprite Animation
        bool loop_animation = true;
        bool use_freetype = false;
        uint32 starttime = 0;
        uint32 currenttime = 0;

        // Batch Cache
        std::vector<FDrawBatch> Batchies;
        irr::video::IVideoDriver* GPUDriver = nullptr;

        void SetForegroundBrush(XUI::Interfaces::IBrush* brush, int startIndex, int length);
        XUI::Interfaces::IBrush* GetNextForegroundBrush(XUI::Interfaces::FormattedTextLine* line, int index, int& length);
        int LineBreak(std::wstring const& textInput, int textIndex, int stop, float maxWidth, int& trailingCount);
        void BuildText();
        float TransformX(float originX, float lineWidth, XUI::Core::Media::TextAlignment align);
        void BuildRects();
        void TTBuildRects();
        void InitializeTTArrange();

        uint32 GetAnimationFrame(uint32 index, uint32& textureId, uint32& rectId) const;

    public:

        XUIIrrFormatedText(std::wstring const & text, XUI::Core::Media::Typeface const & typeface, XUI::Core::Media::TextAlignment textAlignment, XUI::Core::Media::TextWrapping wrapping, XUI::Core::Media::Size const & constraint, std::initializer_list<XUI::Interfaces::FormattedTextStyleSpan> const& spans);

        void Draw(XUI::Interfaces::IDrawingContextImpl * context, XUI::Interfaces::IBrush * foreground, const irr::core::recti* clip = nullptr);

        // Inherited via IFormattedText
        virtual std::vector<XUI::Interfaces::FormattedTextLine> const& GetLines() override;
        virtual XUI::Interfaces::TextHitTestResult HitTestPoint(XUI::Core::Media::Point const & point) override;
        virtual XUI::Core::Media::Rect HitTestTextPosition(int index) override;
        virtual std::vector<XUI::Core::Media::Rect> HitTestTextRange(int index, int length) override;
        virtual XUI::Core::Media::Size GetConstraint();
        virtual XUI::Core::Media::Size GetSize();
        virtual const std::wstring & GetText();
    };
}

#endif //!__WCLIENT_XUI_PLATFORM_FORMATED_TEXT_H__