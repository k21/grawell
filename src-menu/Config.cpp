#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/stat.h>

#include "Config.h"

static char *getConfigFilePath() {
	char *prefix = 0;
	bool dotFile;

#if defined(__WXMSW__)
	if (!prefix || !*prefix) {
		prefix = getenv("APPDATA");
		dotFile = false;
	}
#elif defined(__UNIX__)
	if (!prefix || !*prefix) {
		prefix = getenv("XDG_DATA_HOME");
		dotFile = false;
	}
	if (!prefix || !*prefix) {
		prefix = getenv("HOME");
		dotFile = true;
	}
#else
#error "Unknown OS"
#endif

	if (!prefix || !*prefix) {
		return 0;
	}

	char *res = new char[strlen(prefix) + 64];
	if (!res) return 0;

	sprintf(res, "%s/%sgrawel.conf", prefix, dotFile ? "." : "");

	return res;
}

int save(bool host, const wxString &address, const wxString &name,
		int32_t resX, int32_t resY, bool fullscreen) {
	char *path = getConfigFilePath();
	if (!path) return 1;

	FILE *fp = fopen(path, "w");
	delete [] path;
	if (!fp) return 1;

	int ret = 0;
	char buffer[128];
	if (ret >= 0) ret = fprintf(fp, "%d\n", (int)host);
	if (ret >= 0) {
		strncpy(buffer, address.utf8_str(), sizeof buffer);
		buffer[(sizeof buffer) - 1] = '\0';
		ret = fprintf(fp, "%s\n", buffer);
	}
	if (ret >= 0) {
		strncpy(buffer, name.utf8_str(), sizeof buffer);
		buffer[(sizeof buffer) - 1] = '\0';
		ret = fprintf(fp, "%s\n", buffer);
	}
	if (ret >= 0) ret = fprintf(fp, "%ld\n",(long)resX);
	if (ret >= 0) ret = fprintf(fp, "%ld\n",(long)resY);
	if (ret >= 0) ret = fprintf(fp, "%d\n", (int)fullscreen);
	if (ret < 0) return 1;

	if (fclose(fp)) return 1;

	return 0;
}

int load(bool &host, wxString &address, wxString &name,
		int32_t &resX, int32_t &resY, bool &fullscreen) {
	char *path = getConfigFilePath();
	if (!path) return 1;

	FILE *fp = fopen(path, "r");
	delete [] path;
	if (!fp) {
		if (errno == ENOENT) {
			host = false;
			address = _("");
			name = _("");
			resX = 800;
			resY = 600;
			fullscreen = false;
			return 0;
		}
		return 1;
	}

	char buffer[128];
	int line = 0;
	while (fgets(buffer, sizeof buffer, fp)) {
		int i = strlen(buffer) - 1;
		while (i >= 0 && (buffer[i] == '\n' || buffer[i] == '\r')) {
			buffer[i--] = '\0';
		}

		if (line == 0) host = (buffer[0] != '0');
		if (line == 1) address = wxString::FromUTF8(buffer);
		if (line == 2) name = wxString::FromUTF8(buffer);
		if (line == 3) {
			errno = 0;
			resX = strtol(buffer, 0, 10);
			if (errno) return 1;
		}
		if (line == 4) {
			errno = 0;
			resY = strtol(buffer, 0, 10);
			if (errno) return 1;
		}
		if (line == 5) fullscreen = (buffer[0] != '0');
		++line;
	}
	if (ferror(fp)) return 1;

	if (fclose(fp)) return 1;

	return 0;
}
