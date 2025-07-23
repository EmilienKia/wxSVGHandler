#include <wx/image.h>
#include <wx/zstream.h>
#include <wx/mstream.h>
#include <memory>
#include <iostream>
#include <wx/msgdlg.h>

#include "imagsvg.h"
#include "SvgRenderer.h"

wxSvgImageHandler::wxSvgImageHandler()
{
    m_name = "SVG";
    m_extension = "svg"; // "svg;svgz"
    m_type = wxBITMAP_TYPE_ANY;
    m_mime = "image/svg+xml";
}

bool wxSvgImageHandler::LoadFile(wxImage *image, wxInputStream &stream, bool verbose, int index) {
    wxString unit = image->HasOption(wxIMAGE_OPTION_SVG_UNIT) ? image->GetOption(wxIMAGE_OPTION_SVG_UNIT) : "px";
    int dpi = image->HasOption(wxIMAGE_OPTION_SVG_DPI) ? image->GetOptionInt(wxIMAGE_OPTION_SVG_DPI) : 96;
    int width = image->HasOption(wxIMAGE_OPTION_SVG_WIDTH) ? image->GetOptionInt(wxIMAGE_OPTION_SVG_WIDTH) : 0;
    int height = image->HasOption(wxIMAGE_OPTION_SVG_HEIGHT) ? image->GetOptionInt(wxIMAGE_OPTION_SVG_HEIGHT) : 0;

    if (IsGzipCompressed(stream)) {
        wxZlibInputStream zstream(stream);
        return DoLoadFile(image, zstream, verbose, index);
/*
        *image = SvgRenderer::Get().RenderSvg(zstream, width, height, dpi);
        return image->IsOk();
*/
    } else {
        return DoLoadFile(image, stream, verbose, index);
/*
        *image = SvgRenderer::Get().RenderSvg(stream, width, height, dpi);
        return image->IsOk();
*/
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
