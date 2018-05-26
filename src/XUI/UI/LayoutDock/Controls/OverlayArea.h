#ifndef __XUI_OVERLAYAREA_H__
#define __XUI_OVERLAYAREA_H__

namespace XUI::UI
{
    class OverlayWindow;

    class OverlayArea
    {
    private:
        std::shared_ptr<OverlayWindow> _overlayWindow;
        Core::Media::Rect _screenDetectionArea;

    public:
        OverlayArea(std::shared_ptr<OverlayWindow> const& overlayWindow);

    public:
        PROPERTY_(Core::Media::Rect, ScreenDetectionArea);
        Core::Media::Rect const& GetScreenDetectionArea() const;
    protected:
        void SetScreenDetectionArea(Core::Media::Rect const& rect);
    };
}


#endif	//#ifndef __XUI_OVERLAYAREA_H__
