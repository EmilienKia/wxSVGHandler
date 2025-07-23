#include <wx/wx.h>
#include <wx/image.h>
#include <wx/filename.h>
#include <wx/filedlg.h>
#include <wx/dnd.h>
#include <wx/sizer.h>
#include <wx/dcbuffer.h>
#include "../src/imagsvg.h"

class ImagePanel : public wxPanel {
public:
    ImagePanel(wxWindow* parent) : wxPanel(parent, wxID_ANY) {
        wxWindow::SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &ImagePanel::OnPaint, this);
        Bind(wxEVT_SIZE, &ImagePanel::OnSize, this);
    }

    void SetImage(const wxImage& image) {
        m_image = image;
        Refresh();
    }

private:
    wxImage m_image;

    void OnPaint(wxPaintEvent& event) {
        wxAutoBufferedPaintDC dc(this);

        dc.SetBrush(wxBrush(GetBackgroundColour()));
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle(GetClientRect());

        if (m_image.IsOk()) {
            dc.SetBrush(wxBrush(*wxWHITE, wxBRUSHSTYLE_CROSS_HATCH ));
            dc.DrawRectangle(wxPoint(0, 0), m_image.GetSize());
            dc.DrawBitmap(m_image, 0, 0, true);
        }

    }

    void OnSize(wxSizeEvent& event) {
        if (m_image.IsOk()) {
            Refresh();
        }
        event.Skip();
    }

};

class FileDropTarget : public wxFileDropTarget {
public:
    FileDropTarget(wxWindow* parent) : m_parent(parent) {}

    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override {
        if (filenames.GetCount() > 0) {
            wxString filename = filenames[0];
            if (filename.Lower().EndsWith(".svg")) {
                wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxID_OPEN);
                event.SetString(filename);
                m_parent->GetEventHandler()->ProcessEvent(event);
                return true;
            }
        }
        return false;
    }

private:
    wxWindow* m_parent;
};

class MainFrame : public wxFrame {
public:
    MainFrame() : wxFrame(nullptr, wxID_ANY, "SVG Viewer", wxDefaultPosition, wxSize(800, 600)) {
        CreateStatusBar();
        CreateMenuBar();
        CreateControls();
        SetupDragAndDrop();

        Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnOpen, this, wxID_OPEN);
        Bind(wxEVT_COMMAND_MENU_SELECTED, &MainFrame::OnExit, this, wxID_EXIT);
    }

private:
    ImagePanel* m_imagePanel;

    void CreateMenuBar() {
        wxMenuBar* menuBar = new wxMenuBar();

        wxMenu* fileMenu = new wxMenu();
        fileMenu->Append(wxID_OPEN, "&Open...\tCtrl+O", "Open a SVG file");
        fileMenu->AppendSeparator();
        fileMenu->Append(wxID_EXIT, "&Quit\tCtrl+Q", "Quit  the application");

        menuBar->Append(fileMenu, "&Files");
        SetMenuBar(menuBar);
    }

    void CreateControls() {
        m_imagePanel = new ImagePanel(this);

        wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(m_imagePanel, 1, wxEXPAND | wxALL, 5);
        SetSizer(sizer);
    }

    void SetupDragAndDrop() {
        SetDropTarget(new FileDropTarget(this));
    }

    void OnOpen(wxCommandEvent& event) {
        wxString filename = event.GetString();

        if (filename.IsEmpty()) {
            wxFileDialog dialog(this, "Select a SVG file", "", "",
                                "SVG files (*.svg;*.svg.gz)|*.svg;*.svg.gz|All files (*.*)|*.*",
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST);

            if (dialog.ShowModal() != wxID_OK) return;
            filename = dialog.GetPath();
        }

        LoadSvgFile(filename);
    }

    void OnExit(wxCommandEvent& event) {
        Close();
    }

    void LoadSvgFile(const wxString& filename) {
        wxImage image;

        if (image.LoadFile(filename)) {
            m_imagePanel->SetImage(image);
            SetTitle(wxString::Format("SVG Viewer - %s", wxFileName(filename).GetName()));
            SetStatusText(wxString::Format("%d x %d", image.GetWidth(), image.GetHeight()));
        } else {
            SetTitle("SVG Viewer");
            SetStatusText("Failed to load SVG file");
            wxMessageBox("Cannot load the SVG file", "Error",
                         wxOK | wxICON_ERROR, this);
        }
    }
};

class ViewerApp : public wxApp {
public:
    bool OnInit() override {
        wxInitAllImageHandlers();
        wxImage::AddHandler(new wxSvgImageHandler());
        MainFrame* frame = new MainFrame();
        frame->Show(true);

        return true;
    }
};

wxIMPLEMENT_APP(ViewerApp);
