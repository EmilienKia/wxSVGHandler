#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <wx/image.h>
#include "../src/imagsvg.h"

TEST_CASE("Load SVG file") {
    wxImage::AddHandler(new wxSvgImageHandler());
    wxImage img;
}
