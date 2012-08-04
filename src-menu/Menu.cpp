#include <cstdint>

#include <wx/wx.h>

#include "../src/Driver.h"
#include "../src/GameScreen.h"
#include "../src/Server.h"

static bool startGame = false;

static bool host = false;
static wxString IPAddress;
static wxString playerName;

static int32_t resolutionX;
static int32_t resolutionY;
static bool fullscreen;

class SettingsFrame : public wxFrame {
public:
	SettingsFrame():
			wxFrame(0, wxID_ANY, _("GraWell Settings"),
					wxDefaultPosition, wxSize(200,200)) {
		wxPanel *panel = new wxPanel(this, wxID_ANY);

		wxBoxSizer *boxParts = new wxBoxSizer(wxVERTICAL);
		{
			wxFlexGridSizer *gridTextConfig = new wxFlexGridSizer(2, 2, 5, 10);
			{
				wxStaticText *labelResolution = new wxStaticText(
						panel, wxID_ANY, _("Screen resolution"));
				wxStaticText *labelFullscreen = new wxStaticText(
						panel, wxID_ANY, _("Fullscreen"));

				wxBoxSizer *boxResolution = new wxBoxSizer(wxHORIZONTAL);
				{
					textResolutionX = new wxTextCtrl(panel, wxID_ANY);
					textResolutionY = new wxTextCtrl(panel, wxID_ANY);
					wxStaticText *labelX = new wxStaticText(
							panel, wxID_ANY, _("X"));

					boxResolution->Add(textResolutionX, 1, wxEXPAND);
					boxResolution->Add(labelX, 0, wxALIGN_CENTER_VERTICAL);
					boxResolution->Add(textResolutionY, 1, wxEXPAND);
				}

				checkboxFullscreen = new wxCheckBox(panel, wxID_ANY, _("XXX"));

				gridTextConfig->Add(labelResolution, 0,
						wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				gridTextConfig->Add(boxResolution, 1, wxEXPAND);
				gridTextConfig->Add(labelFullscreen, 0,
						wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				gridTextConfig->Add(checkboxFullscreen, 1, wxEXPAND);

				gridTextConfig->AddGrowableCol(1, 1);
			}
			boxParts->Add(gridTextConfig, 0, wxEXPAND | wxALL, 10);

			wxBoxSizer *boxButtons = new wxBoxSizer(wxHORIZONTAL);
			{
				buttonUndo = new wxButton(panel, wxID_ANY, _("Undo Changes"));
				boxButtons->Add(buttonUndo, 1, wxEXPAND | wxLEFT, 10);

				buttonExit = new wxButton(panel, wxID_ANY, _("XXX"));
				boxButtons->Add(buttonExit, 1, wxEXPAND | wxLEFT, 10);
			}
			boxParts->Add(boxButtons, 1, wxEXPAND | (wxALL - wxLEFT), 10);
		}
		panel->SetSizer(boxParts);

		/* TODO: load values from file */
		resolutionX = 800;
		resolutionY = 600;
		fullscreen = false;

		UndoChanges();

		Connect(textResolutionX->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
				wxCommandEventHandler(SettingsFrame::OnChange));
		Connect(textResolutionY->GetId(), wxEVT_COMMAND_TEXT_UPDATED,
				wxCommandEventHandler(SettingsFrame::OnChange));
		Connect(checkboxFullscreen->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
				wxCommandEventHandler(SettingsFrame::OnCheckboxFullscreenClick));
		Connect(buttonUndo->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
				wxCommandEventHandler(SettingsFrame::OnButtonUndoClick));
		Connect(buttonExit->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
				wxCommandEventHandler(SettingsFrame::OnButtonExitClick));
		Connect(this->GetId(), wxEVT_CLOSE_WINDOW,
				wxCommandEventHandler(SettingsFrame::OnClose));
	}

	void OnChange(wxCommandEvent &) {
		OnChange();
	}

	void OnCheckboxFullscreenClick(wxCommandEvent &) {
		OnChange();
		UpdateFullscreenLabel();
	}

	void OnButtonUndoClick(wxCommandEvent &) {
		UndoChanges();
	}

	void OnButtonExitClick(wxCommandEvent &) {
		if (changed) {
			/* TODO */
			AfterSaveOrUndo();
		} else {
			Close();
		}
	}

	void OnClose(wxCommandEvent &) {
		MakeModal(false);
		Destroy();
	}

	void OnChange() {
		buttonExit->SetLabel(_("Save Changes"));
		buttonUndo->Enable(true);
		changed = true;
	}

	void AfterSaveOrUndo() {
		buttonExit->SetLabel(_("Leave Settings"));
		buttonUndo->Enable(false);
		changed = false;
	}

	void UpdateFullscreenLabel() {
		if (checkboxFullscreen->GetValue()) {
			checkboxFullscreen->SetLabel(_("Yes"));
		} else {
			checkboxFullscreen->SetLabel(_("No"));
		}
	}

	void UndoChanges() {
		textResolutionX->SetValue(wxString::Format(_("%d"), resolutionX));
		textResolutionY->SetValue(wxString::Format(_("%d"), resolutionY));
		checkboxFullscreen->SetValue(fullscreen);
		UpdateFullscreenLabel();
		AfterSaveOrUndo();
	}

	wxTextCtrl *textResolutionX;
	wxTextCtrl *textResolutionY;
	wxCheckBox *checkboxFullscreen;
	wxButton *buttonUndo;
	wxButton *buttonExit;

	bool changed;

};

class MainFrame : public wxFrame {
public:
	MainFrame():
			wxFrame(0, wxID_ANY, _("GraWell Launcher"),
					wxDefaultPosition, wxSize(200,200)) {
		wxPanel *panel = new wxPanel(this, wxID_ANY);

		wxBoxSizer *boxParts = new wxBoxSizer(wxVERTICAL);
		{
			wxFlexGridSizer *gridTextConfig = new wxFlexGridSizer(3, 2, 5, 10);
			{
				wxStaticText *labelMode = new wxStaticText(
						panel, wxID_ANY, _("Network mode"));
				wxStaticText *labelName = new wxStaticText(
						panel, wxID_ANY, _("Player name"));
				wxStaticText *labelAddress = new wxStaticText(
						panel, wxID_ANY, _("Server address"));

				wxBoxSizer *boxHostJoin = new wxBoxSizer(wxHORIZONTAL);
				{
					checkBoxHost = new wxCheckBox(panel, wxID_ANY, _("Host"));
					checkBoxJoin = new wxCheckBox(panel, wxID_ANY, _("Join"));
					checkBoxJoin->SetValue(true);

					boxHostJoin->Add(checkBoxHost, 0, wxALL, 10);
					boxHostJoin->Add(checkBoxJoin, 0, wxALL, 10);
				}

				textName = new wxTextCtrl(panel, wxID_ANY);
				textAddress = new wxTextCtrl(panel, wxID_ANY);

				gridTextConfig->Add(labelMode, 0,
						wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				gridTextConfig->Add(boxHostJoin, 1, wxEXPAND);
				gridTextConfig->Add(labelName, 0,
						wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				gridTextConfig->Add(textName, 1, wxEXPAND);
				gridTextConfig->Add(labelAddress, 0,
						wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL);
				gridTextConfig->Add(textAddress, 1, wxEXPAND);

				gridTextConfig->AddGrowableCol(1, 1);
			}
			boxParts->Add(gridTextConfig, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

			wxBoxSizer *boxButtons = new wxBoxSizer(wxHORIZONTAL);
			{
				buttonExit = new wxButton(panel, wxID_ANY, _("Exit"));
				boxButtons->Add(buttonExit, 1, wxEXPAND | wxLEFT, 10);

				buttonSettings = new wxButton(panel, wxID_ANY, _("Settings"));
				boxButtons->Add(buttonSettings, 1, wxEXPAND | wxLEFT, 10);

				buttonStart = new wxButton(panel, wxID_ANY, _("Start Game"));
				boxButtons->Add(buttonStart, 1, wxEXPAND | wxLEFT, 10);
			}
			boxParts->Add(boxButtons, 1, wxEXPAND | (wxALL - wxLEFT), 10);
		}
		panel->SetSizer(boxParts);

		Connect(checkBoxHost->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
				wxCommandEventHandler(MainFrame::OnSelectHost));
		Connect(checkBoxJoin->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED,
				wxCommandEventHandler(MainFrame::OnSelectJoin));
		Connect(buttonExit->GetId(), wxEVT_COMMAND_BUTTON_CLICKED,
				wxCommandEventHandler(MainFrame::OnButtonExitClick));
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

	void OnButtonExitClick(wxCommandEvent &) {
		Close();
	}

	void OnButtonSettingsClick(wxCommandEvent &) {
		SettingsFrame *frame = new SettingsFrame();
		frame->MakeModal(true);
		frame->Show(true);
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
	wxButton *buttonExit;
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
