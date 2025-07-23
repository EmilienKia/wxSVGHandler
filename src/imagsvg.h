//
// Created by emilien on 23/07/25.
//

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
