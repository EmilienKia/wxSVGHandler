/*
 * wxSVGHandler - A wxWidgets image handler for SVG files.
 * Copyright (C) 2025 Emilien KIA
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#ifndef WXSVGHANDLER_IMAGSVG_H
#define WXSVGHANDLER_IMAGSVG_H

#include <wx/image.h>

#define wxIMAGE_OPTION_SVG_UNIT   wxString("unit")
#define wxIMAGE_OPTION_SVG_DPI   wxString("dpi")
#define wxIMAGE_OPTION_SVG_WIDTH   wxString("width")
#define wxIMAGE_OPTION_SVG_HEIGHT  wxString("height")


class wxSvgImageHandler : public wxImageHandler {
public:
    wxSvgImageHandler();

    bool LoadFile(wxImage* image, wxInputStream& stream, bool verbose, int index) override;

    bool SaveFile(wxImage* image, wxOutputStream& stream, bool verbose) override;

    bool DoCanRead(wxInputStream& stream) override;

private:

    bool DoLoadFile(wxImage* image, wxInputStream& stream, bool verbose, int index);

    bool IsGzipCompressed(wxInputStream& stream);
};



#endif //WXSVGHANDLER_IMAGSVG_H
