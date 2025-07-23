#include <wx/mstream.h>
#include <wx/sstream.h>

#include "SvgRenderer.h"
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"
#include "imagsvg.h"

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
