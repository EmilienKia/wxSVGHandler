cmake_minimum_required(VERSION 3.20)
project(wxSvgHandler VERSION 0.1 LANGUAGES CXX)

set(wxWidgets_CONFIGURATION gtku)
find_package(wxWidgets REQUIRED COMPONENTS base core)
include(${wxWidgets_USE_FILE})

find_package(nanosvg REQUIRED)

add_library(wx_svg_handler
    src/imagsvg.cpp
)
target_compile_definitions(wx_svg_handler PUBLIC WITH_NANOSVG)

target_compile_features(wx_svg_handler PRIVATE cxx_std_17)
target_include_directories(wx_svg_handler PRIVATE ${nanosvg_INCLUDE_DIR})
target_link_libraries(wx_svg_handler PRIVATE ${wxWidgets_LIBRARIES})


add_executable(svg_viewer examples/Viewer.cpp)
target_link_libraries(svg_viewer PRIVATE wx_svg_handler ${wxWidgets_LIBRARIES})

enable_testing()
add_executable(tests tests/Tests.cpp)
target_link_libraries(tests PRIVATE wx_svg_handler)
add_test(NAME svg_tests COMMAND tests)
