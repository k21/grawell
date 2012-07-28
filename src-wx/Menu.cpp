#include <wx/wx.h>

class MainFrame : public wxFrame {
public:
	MainFrame():
			wxFrame(0, wxID_ANY, _("title"), wxDefaultPosition, wxSize(200,200)) {
		wxPanel *panel = new wxPanel(this, wxID_ANY);

		wxBoxSizer *boxParts = new wxBoxSizer(wxVERTICAL);
		{
			wxBoxSizer *boxHostJoin = new wxBoxSizer(wxHORIZONTAL);
			{
				checkBoxHost = new wxCheckBox(panel, wxID_ANY, _("Host"));
				checkBoxJoin = new wxCheckBox(panel, wxID_ANY, _("Join"));
				checkBoxJoin->SetValue(true);

				boxHostJoin->Add(checkBoxHost);
				boxHostJoin->Add(checkBoxJoin);
			}
			boxParts->Add(boxHostJoin);

			wxFlexGridSizer *gridTextConfig = new wxFlexGridSizer(2, 2, 5, 10);
			{
				wxStaticText *labelName = new wxStaticText(
						panel, wxID_ANY, _("Player name"));
				wxStaticText *labelAddress = new wxStaticText(
						panel, wxID_ANY, _("Server address"));

				wxTextCtrl *textName = new wxTextCtrl(panel, wxID_ANY);
				wxTextCtrl *textAddress = new wxTextCtrl(panel, wxID_ANY);

				gridTextConfig->Add(labelName);
				gridTextConfig->Add(textName, 1, wxEXPAND);
				gridTextConfig->Add(labelAddress);
				gridTextConfig->Add(textAddress, 1, wxEXPAND);

				gridTextConfig->AddGrowableCol(1, 1);
			}
			boxParts->Add(gridTextConfig, 0, wxEXPAND | wxALL, 10);

			buttonSettings = new wxButton(panel, wxID_ANY, _("Settings"));
			boxParts->Add(buttonSettings, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

			buttonStart = new wxButton(panel, wxID_ANY, _("Start game"));
			boxParts->Add(buttonStart, 1, wxEXPAND | wxALL, 10);
		}
		panel->SetSizer(boxParts);

		Connect(checkBoxHost->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
				wxCommandEventHandler(MainFrame::OnSelectHost));
		Connect(checkBoxJoin->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
				wxCommandEventHandler(MainFrame::OnSelectJoin));
		Connect(buttonSettings->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
				wxCommandEventHandler(MainFrame::OnButtonSettingsClick));
		Connect(buttonStart->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
				wxCommandEventHandler(MainFrame::OnButtonStartClick));
	}

	void OnSelectHost(wxCommandEvent &event) {
		checkBoxHost->SetValue(true);
		checkBoxJoin->SetValue(false);
	}

	void OnSelectJoin(wxCommandEvent &event) {
		checkBoxHost->SetValue(false);
		checkBoxJoin->SetValue(true);
	}

	void OnButtonSettingsClick(wxCommandEvent &event) {
		
	}

	void OnButtonStartClick(wxCommandEvent &event) {
		
	}

	wxCheckBox *checkBoxHost;
	wxCheckBox *checkBoxJoin;
	wxButton *buttonSettings;
	wxButton *buttonStart;

};

class LauncherApp : public wxApp {
	bool OnInit() {
		MainFrame *frame = new MainFrame();
		frame->Show(true);
		return true;
	}
};

IMPLEMENT_APP(LauncherApp);
