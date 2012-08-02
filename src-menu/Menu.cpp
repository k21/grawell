#include <wx/wx.h>

#include "../src/Driver.h"
#include "../src/GameScreen.h"
#include "../src/Server.h"

static bool startGame = false;
static bool host = false;
static wxString IPAddress;
static wxString playerName;

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

				textName = new wxTextCtrl(panel, wxID_ANY);
				textAddress = new wxTextCtrl(panel, wxID_ANY);

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

	void OnSelectHost(wxCommandEvent &) {
		checkBoxHost->SetValue(true);
		checkBoxJoin->SetValue(false);
		host = true;
	}

	void OnSelectJoin(wxCommandEvent &) {
		checkBoxHost->SetValue(false);
		checkBoxJoin->SetValue(true);
		host = false;
	}

	void OnButtonSettingsClick(wxCommandEvent &) {
		
	}

	void OnButtonStartClick(wxCommandEvent &) {
		IPAddress = textAddress->GetValue();
		playerName = textName->GetValue();
		startGame = true;
		Close();
	}

	wxCheckBox *checkBoxHost;
	wxCheckBox *checkBoxJoin;
	wxTextCtrl *textName;
	wxTextCtrl *textAddress;
	wxButton *buttonSettings;
	wxButton *buttonStart;

};

class MenuApp : public wxApp {
	bool OnInit() {
		MainFrame *frame = new MainFrame();
		frame->Show(true);
		return true;
	}
};

IMPLEMENT_APP_NO_MAIN(MenuApp);

int main(int argc, char **argv) {
	wxEntry(argc, argv);

	if (!startGame) return 0;

	Server *server = 0;
	if (host) {
		server = new Server(4920);
		server->Launch();
	}

	Driver driver;
	driver.changeScreen(new GameScreen(driver,
			IPAddress.utf8_str(), 4920, playerName.utf8_str()));
	driver.run();

	if (server) {
		server->exit();
		delete server;
		server = 0;
	}

	return 0;
}
