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
#include <memory>
#include <iostream>

#include <wx/image.h>
#include <wx/zstream.h>
#include <wx/mstream.h>
#include <wx/sstream.h>
#include <wx/msgdlg.h>

#include "imagsvg.h"

wxSvgImageHandler::wxSvgImageHandler()
{
    m_name = "SVG";
    m_extension = "svg"; // "svg;svgz"
    m_type = wxBITMAP_TYPE_ANY;
    m_mime = "image/svg+xml";
}

bool wxSvgImageHandler::LoadFile(wxImage *image, wxInputStream &stream, bool verbose, int index) {
    if (IsGzipCompressed(stream)) {
        wxZlibInputStream zstream(stream);
        return DoLoadFile(image, zstream, verbose, index);
    } else {
        return DoLoadFile(image, stream, verbose, index);
    }
}

bool wxSvgImageHandler::DoCanRead(wxInputStream &stream) {
    wxFileOffset pos = stream.TellI();
    if (IsGzipCompressed(stream)) {
        wxZlibInputStream zstream(stream);
        char buffer[512] = {};
        zstream.Read(buffer, sizeof(buffer));
        wxString str = wxString::FromUTF8(buffer, 512);
        stream.SeekI(pos);
        return str.Lower().Contains("<svg");
    } else {
        char buffer[512] = {};
        stream.Read(buffer, sizeof(buffer));
        wxString str = wxString::FromUTF8(buffer, 512);
        stream.SeekI(pos);
        return str.Lower().Contains("<svg");
    }
}

bool wxSvgImageHandler::SaveFile(wxImage* image, wxOutputStream& stream, bool verbose) {
    return false;
}

bool wxSvgImageHandler::IsGzipCompressed(wxInputStream& stream) {
    wxFileOffset pos = stream.TellI();
    char sig[2] = {};
    stream.Read(sig, 2);
    stream.SeekI(pos);
    return (static_cast<unsigned char>(sig[0]) == 0x1F && static_cast<unsigned char>(sig[1]) == 0x8B);
}

#ifdef WITH_NANOSVG

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

bool wxSvgImageHandler::DoLoadFile(wxImage* image, wxInputStream& stream, bool verbose, int index)
{
    wxString unit = image->HasOption(wxIMAGE_OPTION_SVG_UNIT) ? image->GetOption(wxIMAGE_OPTION_SVG_UNIT) : "px";
    int dpi = image->HasOption(wxIMAGE_OPTION_SVG_DPI) ? image->GetOptionInt(wxIMAGE_OPTION_SVG_DPI) : 96;

    wxStringOutputStream memOut;
    stream.Read(memOut/*.GetOutputStreamBuffer()->GetBufferStart(), stream.GetSize()*/);
    std::string svgData = memOut.GetString().utf8_string();

    struct NSVGimage* imageData = nsvgParse(const_cast<char*>(svgData.c_str()), unit.utf8_str(), dpi);
    if (!image) return false;

    int targetWidth = image->HasOption(wxIMAGE_OPTION_SVG_WIDTH) ? image->GetOptionInt(wxIMAGE_OPTION_SVG_WIDTH) : imageData->width;
    int targetHeight = image->HasOption(wxIMAGE_OPTION_SVG_HEIGHT) ? image->GetOptionInt(wxIMAGE_OPTION_SVG_HEIGHT) : imageData->height;

    float scaleX = targetWidth / imageData->width;
    float scaleY = targetHeight / imageData->height;

    NSVGrasterizer* rast = nsvgCreateRasterizer();
    uint8_t* imgData = new uint8_t[targetWidth * targetHeight * 4];

    nsvgRasterize(rast, imageData, 0, 0, std::min(scaleX, scaleY), imgData, targetWidth, targetHeight, targetWidth * 4);

    image->Create(targetWidth, targetHeight);
    image->SetAlpha();

    uint8_t* imgColor = image->GetData();
    uint8_t* imgAlpha = image->GetAlpha();

    for (int i = 0; i < targetWidth * targetHeight; ++i) {
        imgColor[i * 3 + 0] = imgData[i * 4 + 0];
        imgColor[i * 3 + 1] = imgData[i * 4 + 1];
        imgColor[i * 3 + 2] = imgData[i * 4 + 2];
        imgAlpha[i] = imgData[i * 4 + 3];
    }

    delete[] imgData;
    nsvgDeleteRasterizer(rast);
    nsvgDelete(imageData);

    return true;
}

#endif // #ifdef WITH_NANOSVG
