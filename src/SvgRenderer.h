#pragma once

#include <wx/image.h>
#include <wx/stream.h>
#include <memory>

class SvgRenderer {
public:
    virtual ~SvgRenderer() = default;
    virtual wxImage RenderSvg(wxInputStream& stream, int width = 0, int height = 0, int dpi = 96) = 0;

    static SvgRenderer& Get();
};