#ifndef CONFIG_H_
#define CONFIG_H_

#include <cstdint>

#include <wx/wx.h>

int save(bool host, const wxString &address, const wxString &name,
		int32_t resX, int32_t resY, bool fullscreen);

int load(bool &host, wxString &address, wxString &name,
		int32_t &resX, int32_t &resY, bool &fullscreen);

#endif
