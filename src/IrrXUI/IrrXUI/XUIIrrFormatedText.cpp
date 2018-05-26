#include "XUIIrrFormatedText.h"
#include "XUIIrrDrawingContextImpl.h"
#include "XUI/Core/Media/MediaTypes.h"
#include "XUI/Platform/UIService.h"
#include "XUI/Platform/UIRenderer.h"
#include "XUIIrrWindowImpl.h"
#include "XUIIrrRenderer.h"

#include "irrlicht.h"
#include "IGUIFont.h"
#include "IGUISpriteBank.h"
#include "IVideoDriver.h"

#include "CGUITTFont.h"
#include "CGUIEnvironment.h"

#include <stdio.h>
#include <wctype.h>

using namespace XUI;
using namespace XUI::Interfaces;

constexpr float MAX_LINE_WIDTH = 10000;

extern irr::video::SColor GetIrrColorFrom(XUI::Interfaces::IBrush* bursh, IrrBackend::XUIIrrDrawingContext* context);

IrrBackend::XUIIrrFormatedText::XUIIrrFormatedText(
    std::wstring const & text, 
    XUI::Core::Media::Typeface const & typeface, 
    XUI::Core::Media::TextAlignment textAlignment, // => EGUI_ALIGNMENT
    XUI::Core::Media::TextWrapping wrapping, 
    XUI::Core::Media::Size const & constraint, 
    std::initializer_list<XUI::Interfaces::FormattedTextStyleSpan> const & spans)
    : m_text(std::move(text))
    , _size(0, 0)
    , _typeface(typeface)
    , _textAlignment(textAlignment)
    , _wrapping(wrapping)
    , _constraint(constraint)
{
    auto serviceUI = static_cast<IrrBackend::XUIIrrPlatformDriver*>(XUI::Platform::UIService::Instance()->Get<XUI::Platform::UIRender>());
    auto firstWindowHandle = serviceUI->GetFirstActiveWindow();
    GPUDriver = firstWindowHandle->GetContext()->getVideoDriver();

    use_freetype = true;
    if (use_freetype)
    {
        std::string _fontFile = "Resources/fonts/";
        _fontFile += "LiberationSans-Regular";
        _fontFile += ".ttf";

        _typeFace = firstWindowHandle->GetContext()->getGUIEnvironment()->getFont(_fontFile.c_str(), typeface.FontSize, true);

        if (_typeFace)
        {
            irr::gui::CGUITTFont* fontTT = static_cast<irr::gui::CGUITTFont*>(_typeFace);

            for (uint32 i = 0; i != fontTT->Glyph_Pages.size(); ++i)
            {
                if (fontTT->Glyph_Pages[i]->dirty)
                    fontTT->Glyph_Pages[i]->updateTexture();
            }
        }
    }
    else
    {
        std::string _fontFile = "Resources/fonts/";
        _fontFile += typeface.FontFamilyName;
        _fontFile += std::to_string(uint32(typeface.FontSize));
        _fontFile += ".xml";

        _typeFace = firstWindowHandle->GetContext()->getGUIEnvironment()->getFont(_fontFile.c_str());
    }
    if (!_typeFace)
    {
        // TODO: font type customize use for arg.typeface
        _typeFace = firstWindowHandle->GetContext()->getGUIEnvironment()->getBuiltInFont();
        use_freetype = false;
    }

    for (XUI::Interfaces::FormattedTextStyleSpan const& span : spans)
    {
        if (span.ForegroundBrush != nullptr)
        {
            SetForegroundBrush(span.ForegroundBrush, span.StartIndex, span.Length);
        }
    }

    BuildText();
}

void IrrBackend::XUIIrrFormatedText::Draw(XUI::Interfaces::IDrawingContextImpl * context, XUI::Interfaces::IBrush * foreground, const irr::core::recti* clip)
{
    if (GPUDriver != static_cast<XUIIrrDrawingContext*>(context)->GetContext()->getVideoDriver())
    {
        GPUDriver = static_cast<XUIIrrDrawingContext*>(context)->GetContext()->getVideoDriver();

        if (use_freetype)
        {
            std::string _fontFile = "Resources/fonts/";
            _fontFile += "LiberationSans-Regular";
            _fontFile += ".ttf";

            _typeFace = static_cast<XUIIrrDrawingContext*>(context)->GetContext()->getGUIEnvironment()->getFont(_fontFile.c_str(), _typeface.FontSize, true);
        }
        else
        {
            std::string _fontFile = "Resources/fonts/";
            _fontFile += _typeface.FontFamilyName;
            _fontFile += std::to_string(uint32(_typeface.FontSize));
            _fontFile += ".xml";

            _typeFace = static_cast<XUIIrrDrawingContext*>(context)->GetContext()->getGUIEnvironment()->getFont(_fontFile.c_str());
        }

        if (!_typeFace)
        {
            // TODO: font type customize use for arg.typeface
            _typeFace = static_cast<XUIIrrDrawingContext*>(context)->GetContext()->getGUIEnvironment()->getBuiltInFont();
        }

        if (use_freetype)
        {
            irr::gui::CGUITTFont* fontTT = static_cast<irr::gui::CGUITTFont*>(_typeFace);

            for (uint32 i = 0; i != fontTT->Glyph_Pages.size(); ++i)
            {
                if (fontTT->Glyph_Pages[i]->dirty)
                    fontTT->Glyph_Pages[i]->updateTexture();
            }
        }
    }

    irr::video::SColor foreColor(255, 0, 0, 0);
    if (foreground)
        foreColor = GetIrrColorFrom(foreground, nullptr);

    if (use_freetype)
    {
        irr::gui::CGUITTFont* fontTT = static_cast<irr::gui::CGUITTFont*>(_typeFace);

        for (auto const& batch : Batchies)
        {
            if (batch.positions.empty() || batch.sourceRects.empty())
                continue;
        
            GPUDriver->draw2DImageBatch(fontTT->getPageTextureByIndex(batch.textureNumber), batch.positions, batch.sourceRects, clip, foreColor, true);
            //for (int i = 0; i != batch.positions.size(); ++i)
            //{
            //    irr::core::dimension2d<int32> sourceSize(batch.sourceRects[i].getSize());
            //    const irr::core::rect<int32> poss(batch.positions[i], sourceSize);
            //
            //    GPUDriver->draw2DRectangle(foreColor, poss, nullptr, false);
            //}
        }
    }
    else
    {
        irr::gui::IGUISpriteBank* spriteBank = static_cast<irr::gui::IGUIFontBitmap*>(_typeFace)->getSpriteBank();

        for (auto const& batch : Batchies)
        {
            if (batch.positions.empty() || batch.sourceRects.empty())
                continue;

            GPUDriver->draw2DImageBatch(spriteBank->getTexture(batch.textureNumber), batch.positions, batch.sourceRects, clip, foreColor, true);
        }
    }
}

int IrrBackend::XUIIrrFormatedText::LineBreak(std::wstring const & textInput, int textIndex, int stop, float maxWidth, int & trailingCount)
{
    int lengthBreak;
    if (maxWidth == -1)
    {
        lengthBreak = stop - textIndex;
    }
    else
    {
        std::wstring subText = textInput.substr(textIndex, stop - textIndex);
        lengthBreak = _typeFace->BreakText(subText.c_str(), maxWidth);
        if (lengthBreak == 0)
            lengthBreak = stop - textIndex;
    }

    //Check for white space or line breakers before the lengthBreak
    int startIndex = textIndex;
    int index = textIndex;
    int word_start = textIndex;
    bool prevBreak = true;

    trailingCount = 0;

    while (index < stop)
    {
        int prevText = index;
        char currChar = textInput[index++];
        bool currBreak = iswspace(currChar);

        if (!currBreak && prevBreak)
        {
            word_start = prevText;
        }

        prevBreak = currBreak;

        if (index > startIndex + lengthBreak)
        {
            if (currBreak)
            {
                // eat the rest of the whitespace
                while (index < stop && iswspace(textInput[index]))
                {
                    index++;
                }

                trailingCount = index - prevText;
            }
            else
            {
                // backup until a whitespace (or 1 char)
                if (word_start == startIndex)
                {
                    if (prevText > startIndex)
                    {
                        index = prevText;
                    }
                }
                else
                {
                    index = word_start;
                }
            }
            break;
        }

        if ('\n' == currChar)
        {
            int ret = index - startIndex;
            int lineBreakSize = 1;
            if (index < stop)
            {
                currChar = textInput[index++];
                if ('\r' == currChar)
                {
                    ret = index - startIndex;
                    ++lineBreakSize;
                }
            }

            trailingCount = lineBreakSize;

            return ret;
        }

        if ('\r' == currChar)
        {
            int ret = index - startIndex;
            int lineBreakSize = 1;
            if (index < stop)
            {
                currChar = textInput[index++];
                if ('\n' == currChar)
                {
                    ret = index - startIndex;
                    ++lineBreakSize;
                }
            }

            trailingCount = lineBreakSize;

            return ret;
        }
    }

    return index - startIndex;
}

void IrrBackend::XUIIrrFormatedText::BuildText()
{
    float curY = 0;
    int curOff = 0;
    size_t textLength = m_text.length();

    float widthConstraint = float(_constraint.Width);

    std::wstring subString;
    for (int c = 0; curOff < textLength; ++c)
    {
        float constraint = -1;
        int trailingnumber = 0;

        if (_wrapping == XUI::Core::Media::TextWrapping::Wrap)
        {
            constraint = widthConstraint <= 0 || std::isinf(_constraint.Width) ? MAX_LINE_WIDTH : widthConstraint;
            if (constraint > MAX_LINE_WIDTH)
                constraint = MAX_LINE_WIDTH;
        }

        _lines.emplace_back();
        XUI::Interfaces::FormattedTextLine& line = _lines.back();
        line.TextLength = LineBreak(m_text, curOff, textLength, constraint, trailingnumber);
        subString = m_text.substr(curOff, line.TextLength - trailingnumber);
        irr::core::dimension2du size_px = _typeFace->getDimension(subString.c_str());
        line.Start = curOff;
        line.Length = line.TextLength - trailingnumber;
        line.Width = size_px.Width;
        line.Height = size_px.Height;
        line.Top = curY;

        curY += size_px.Height;
        curOff += line.TextLength;
        //curY += padding;
    }

    if (use_freetype)
        TTBuildRects();
    else
        BuildRects();
}

float IrrBackend::XUIIrrFormatedText::TransformX(float originX, float lineWidth, XUI::Core::Media::TextAlignment align)
{
    float x = 0;

    if (align == XUI::Core::Media::TextAlignment::Left)
    {
        x = originX;
    }
    else
    {
        double width = _constraint.Width > 0 && _constraint.Width < std::numeric_limits<double>::max() ?
            Constraint.Width :
            lineWidth;

        switch (align)
        {
            case XUI::Core::Media::TextAlignment::Center: x = originX + (float)(width - lineWidth) / 2; break;
            case XUI::Core::Media::TextAlignment::Right: x = originX + (float)(width - lineWidth); break;
        }
    }

    return x;
}

void IrrBackend::XUIIrrFormatedText::BuildRects()
{
    irr::gui::IGUISpriteBank* spriteBank = static_cast<irr::gui::IGUIFontBitmap*>(_typeFace)->getSpriteBank();

    auto const& Rectangles = spriteBank->getRectangles();

    Batchies.clear();
    Batchies.resize(spriteBank->getTextureCount());

    float width = 0;
    float height = 0;

    for (int li = 0; li < _lines.size(); ++li)
    {
        auto const& line = _lines[li];
        float prevRight = TransformX(0, line.Width, _textAlignment);
        double nextTop = line.Top + line.Height;
        float linewidth = 0;
        float lineheight = 0;

        if (li + 1 < _lines.size())
        {
            nextTop = _lines[li + 1].Top;
        }


        for (int i = line.Start; i < line.Start + line.TextLength; i++)
        {
            uint32 rectId;
            uint32 textureId;
            uint32 spriteId = static_cast<irr::gui::IGUIFontBitmap*>(_typeFace)->getSpriteNoFromChar(&m_text[i]);
            GetAnimationFrame(spriteId, textureId, rectId);
            
            const irr::core::recti& r = Rectangles[rectId];
            const irr::video::ITexture* texture = spriteBank->getTexture(textureId);

            auto& batch = Batchies[textureId];
            batch.positions.push_back(irr::core::position2di(prevRight, line.Top));
            batch.sourceRects.push_back(r);
            batch.textureNumber = textureId;

            auto bound = _typeFace->MeasureText(&m_text[i], 1);
            if (m_text[i] == L'\n' || m_text[i] == L'\r')
                bound.Height *= 0.5f;

            _rects.push_back(irr::core::recti(
                prevRight,
                line.Top,
                prevRight + bound.Width,
                nextTop));

            _rects.back().repair();

            prevRight += bound.Width;

            linewidth += bound.Width;
            lineheight = std::max(float(bound.Height), lineheight);
        }

        width = std::max(width, linewidth);
        height += lineheight;
    }

    _size.Width = width;
    _size.Height = height;
}

void IrrBackend::XUIIrrFormatedText::TTBuildRects()
{
    irr::gui::CGUITTFont* fontTT = static_cast<irr::gui::CGUITTFont*>(_typeFace);

    InitializeTTArrange();

    Batchies.clear();
    Batchies.resize(fontTT->Glyph_Pages.size());

    float width = 0;
    float height = 0;

    for (int li = 0; li < _lines.size(); ++li)
    {
        auto const& line = _lines[li];
        float prevRight = TransformX(0, line.Width, _textAlignment);
        double nextTop = line.Top + line.Height;
        float linewidth = 0;
        float lineheight = 0;

        if (li + 1 < _lines.size())
        {
            nextTop = _lines[li + 1].Top;
        }

        for (int i = line.Start; i < line.Start + line.TextLength; i++)
        {
            uint32 glyphId = fontTT->getGlyphIndexByChar(m_text[i]);
            const irr::gui::SGUITTGlyph& glyph = fontTT->Glyphs[std::max(0, int(glyphId) - 1)];

            const irr::core::recti& r = glyph.source_rect;
            const irr::video::ITexture* texture = fontTT->getPageTextureByIndex(glyph.glyph_page);

            float offY = (fontTT->font_metrics.ascender / 64) - glyph.offset.Y - 1; //(fontTT->font_metrics.ascender / 256)
            auto& batch = Batchies[glyph.glyph_page];
            batch.positions.push_back(irr::core::position2di(prevRight, line.Top + offY));
            batch.sourceRects.push_back(r);
            batch.textureNumber = glyph.glyph_page;

            auto bound = _typeFace->MeasureText(&m_text[i], 1);

            _rects.push_back(irr::core::recti(
                prevRight,
                line.Top,
                prevRight + bound.Width,
                nextTop));

            _rects.back().repair();

            prevRight += bound.Width;

            linewidth += bound.Width;
            lineheight = std::max(float(bound.Height), lineheight);
        }

        width = std::max(width, linewidth);
        height += lineheight;
    }

    _size.Width = width;
    _size.Height = height;
}

void IrrBackend::XUIIrrFormatedText::InitializeTTArrange()
{
    //irr::gui::CGUITTFont* fontTT = static_cast<irr::gui::CGUITTFont*>(_typeFace);
    //
    //// Clear the glyph pages of their render information.
    //for (uint i = 0; i < fontTT->Glyph_Pages.size(); ++i)
    //{
    //    fontTT->Glyph_Pages[i]->render_positions.clear();
    //    fontTT->Glyph_Pages[i]->render_source_rects.clear();
    //}
    //
    //// Set up some variables.
    //irr::core::dimension2d<int32> textDimension;
    //irr::core::position2d<int32> offset;
    //
    //// Set up our render map.
    ////std::map<uint, irr::gui::CGUITTGlyphPage*> Render_Map;
    //
    //// Start parsing characters.
    //uint n;
    //wchar_t previousChar = 0;
    //const wchar_t* iter = m_text.c_str();
    //std::vector<irr::video::SColor> applied_colors;
    //while (*iter != L'\0')
    //{
    //    wchar_t currentChar = *iter;
    //    n = fontTT->getGlyphIndexByChar(currentChar);
    //    //bool visible = (Invisible.findFirst(currentChar) == -1);
    //    bool lineBreak = false;
    //    if (currentChar == L'\r') // Mac or Windows breaks
    //    {
    //        lineBreak = true;
    //        if (*(iter + 1) == (wchar_t)L'\n') 	// Windows line breaks.
    //            currentChar = *(++iter);
    //    }
    //    else if (currentChar == (wchar_t)L'\n') // Unix breaks
    //    {
    //        lineBreak = true;
    //    }
    //
    //    if (lineBreak)
    //    {
    //        previousChar = 0;
    //        offset.Y += fontTT->font_metrics.height / 64;
    //        offset.X = 0;
    //        ++iter;
    //        continue;
    //    }
    //
    //    if (n > 0)
    //    {
    //        // Calculate the glyph offset.
    //        int32 offx = fontTT->Glyphs[n - 1].offset.X;
    //        int32 offy = (fontTT->font_metrics.ascender / 64) - fontTT->Glyphs[n - 1].offset.Y;
    //
    //        // Apply kerning.
    //        irr::core::vector2di k = fontTT->getKerning(currentChar, previousChar);
    //        offset.X += k.X;
    //        offset.Y += k.Y;
    //
    //        // Determine rendering information.
    //        irr::gui::SGUITTGlyph& glyph = fontTT->Glyphs[n - 1];
    //        irr::gui::CGUITTGlyphPage* const page = fontTT->Glyph_Pages[glyph.glyph_page];
    //        page->render_positions.push_back(irr::core::position2di(offset.X + offx, offset.Y + offy));
    //        page->render_source_rects.push_back(glyph.source_rect);
    //        //Render_Map.emplace(glyph.glyph_page, page);
    //        //int32 current_color = iter.getPos();
    //        //if (current_color < colors.size())
    //        //    applied_colors.push_back(colors[current_color]);
    //    }
    //    offset.X += fontTT->getWidthFromCharacter(currentChar);
    //
    //    previousChar = currentChar;
    //    ++iter;
    //}
    //
    //for (uint32 i = 0; i != fontTT->Glyph_Pages.size(); ++i)
    //{
    //    if (fontTT->Glyph_Pages[i]->dirty)
    //        fontTT->Glyph_Pages[i]->updateTexture();
    //}
}

uint32 IrrBackend::XUIIrrFormatedText::GetAnimationFrame(uint32 index, uint32 & textureId, uint32 & rectId) const
{
    irr::gui::IGUISpriteBank* spriteBank = static_cast<irr::gui::IGUIFontBitmap*>(_typeFace)->getSpriteBank();

    auto const& Sprites = spriteBank->getSprites();

    // work out frame number
    uint32 frame = 0;
    if (Sprites[index].frameTime)
    {
        uint32 f = ((currenttime - starttime) / Sprites[index].frameTime);
        if (loop_animation)
            frame = f % Sprites[index].Frames.size();
        else
            frame = (f >= Sprites[index].Frames.size()) ? Sprites[index].Frames.size() - 1 : f;
    }

    textureId = Sprites[index].Frames[frame].textureNumber;
    rectId = Sprites[index].Frames[frame].rectNumber;

    return frame;
}

void IrrBackend::XUIIrrFormatedText::SetForegroundBrush(XUI::Interfaces::IBrush* brush, int startIndex, int length)
{
    if (brush != nullptr)
    {
        _foregroundBrushes.push_back(FBrushRange(brush, startIndex, length));
    }
}

XUI::Interfaces::IBrush* IrrBackend::XUIIrrFormatedText::GetNextForegroundBrush(XUI::Interfaces::FormattedTextLine* line, int index, int& length)
{
    XUI::Interfaces::IBrush* result = nullptr;
    if (!_foregroundBrushes.empty())
    {
        for (auto const& forebrush : _foregroundBrushes)
        {
            if (forebrush.Intersects(index, length))
            {
                result = forebrush.Brush;
                length = forebrush.Length;
                break;
            }

            if (forebrush.StartIndex <= (index + length))
                break;
        }
    }

    return result;
}

std::vector<FormattedTextLine> const& IrrBackend::XUIIrrFormatedText::GetLines()
{
    return _lines;
}

TextHitTestResult IrrBackend::XUIIrrFormatedText::HitTestPoint(Core::Media::Point const & point)
{
    irr::core::position2di posi(point.x, point.y);

    float sumHeight = 0.f;
    for (int li = 0; li < _lines.size(); ++li)
    {
        auto const& line = _lines[li];

        if (line.Top <= point.y && (line.Top + line.Height) > point.y)
        {
            for (int i = 0; i != line.TextLength; ++i)
            {
                auto const& rect = _rects[line.Start + i];
                if (rect.isPointInside(posi))
                {
                    TextHitTestResult result;
                    result.IsInside = !(line.TextLength > line.Length);
                    result.TextPosition = line.Start + i;
                    result.IsTrailing = (point.x - rect.UpperLeftCorner.X) > rect.getWidth() / 2;
                    return result;
                }
            }

            int offset = 0;

            if (point.x >= (_rects[line.Start].UpperLeftCorner.X + line.Width) / 2 && line.Length > 0)
                offset = line.TextLength > line.Length ? line.Length : (line.Length - 1);

            TextHitTestResult result;
            result.IsInside = false;
            result.TextPosition = line.Start + offset;
            result.IsTrailing = Text.length() == (line.Start + offset + 1);
            return result;
        }

        sumHeight += line.Height;
    }

    bool end = point.x > _size.Width || point.y > sumHeight;

    TextHitTestResult result;
    result.IsInside = false;
    result.TextPosition = end;
    result.IsTrailing = end ? Text.length() - 1: 0;
    return result;
}

Core::Media::Rect IrrBackend::XUIIrrFormatedText::HitTestTextPosition(int index)
{
    if (_rects.empty())
        return Core::Media::Rect(Core::Media::Vector(1, 1));

    if (index < 0 || index >= _rects.size())
    {
        auto const& rect = _rects.back();
        Core::Media::Rect ret(Core::Media::Vector(rect.getWidth(), rect.getHeight()));
        ret = ret + Core::Media::Vector(rect.LowerRightCorner.X, rect.UpperLeftCorner.Y);
        return ret;
    }

    auto const& rect = _rects[index];
    Core::Media::Rect ret(Core::Media::Vector(rect.getWidth(), rect.getHeight()));
    ret = ret + Core::Media::Vector(rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y);
    return ret;
}

std::vector<Core::Media::Rect> IrrBackend::XUIIrrFormatedText::HitTestTextRange(int index, int length)
{
    std::vector<Core::Media::Rect> result;

    int lastIndex = index + length - 1;

    for (int i = index; i <= lastIndex; ++i)
    {
        auto const& rect = _rects[i];
        Core::Media::Rect ret(Core::Media::Vector(rect.getWidth(), rect.getHeight()));
        result.push_back(ret + Core::Media::Vector(rect.UpperLeftCorner.X, rect.UpperLeftCorner.Y));
    }

    return result;
}

XUI::Core::Media::Size IrrBackend::XUIIrrFormatedText::GetConstraint()
{
    return _constraint;
}

XUI::Core::Media::Size IrrBackend::XUIIrrFormatedText::GetSize()
{
    return _size;
}

const std::wstring & IrrBackend::XUIIrrFormatedText::GetText()
{
    return m_text;
}
