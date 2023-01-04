#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <map>
#include <string>

#include <libconfig.h++>

#define SUCCESS EXIT_SUCCESS
#define FAILURE EXIT_FAILURE

using namespace std;
using namespace libconfig;

class MapKey
{
private:
	int keycode;
	unsigned int modifiers;

public:
	MapKey(int keycode, unsigned int modifiers)
		: keycode(keycode)
		, modifiers(modifiers) { }

	operator unsigned long() const {
		return (((unsigned long)keycode) << 32) | modifiers;
	}
};

class Hotkey
{
public:
	int keycode;
	unsigned int modifiers;
	string key;
	string command;

	Hotkey() {
		keycode = -1;
		modifiers = 0;
		key = "";
		command = "true";
	}

	Hotkey(const Hotkey& other) {
		keycode = other.keycode;
		modifiers = other.modifiers;
		key = other.key;
		command = other.command;
	}

	Hotkey(int keycode, string key, unsigned int modifiers, string command)
		: keycode(keycode)
		, key(key)
		, modifiers(modifiers)
		, command(command) { }
};

void grabKeys(Display* display, std::map<unsigned long, Hotkey>& hotkeys) {
	Window root = DefaultRootWindow(display);

	for(auto it : hotkeys) {
		Hotkey hotkey = it.second;

		Bool owner_events = False;
		int pointer_mode = GrabModeAsync;
		int keyboard_mode = GrabModeAsync;

		XGrabKey(display, hotkey.keycode, hotkey.modifiers, root, owner_events, pointer_mode, keyboard_mode);
	}

	XSelectInput(display, root, KeyPressMask);
	XEvent event;
	for(;;) {
		XNextEvent(display, &event);
		switch(event.type) {
			case KeyPress: {
				XKeyEvent& key_event = *((XKeyEvent*)&event);
				MapKey mapKey(key_event.keycode, key_event.state);

				if(hotkeys.count(mapKey)) {
					Hotkey hotkey = hotkeys[mapKey];
					system(hotkey.command.c_str());
				}
			} break;
			default:
				break;
		}
	}
}

int readConfig(Config& cfg, string name) {
	// Read the file. If there is an error, report it and exit.
	try {
		cfg.readFile(name.c_str());
	} catch(const FileIOException& fioex) {
		std::cerr << "Cannot read file `" << name << "'" << std::endl;
		return FAILURE;
	} catch(const ParseException& pex) {
		std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine() << " - " << pex.getError() << std::endl;
		return FAILURE;
	}

	return SUCCESS;
}

int main(int argc, char** argv) {
	Config cfg;

	cout << "xhotkey v1.1.0" << endl;

	int numErrors = 0;

	if(readConfig(cfg, string(getenv("HOME")) + string("/.xhotkey")) == SUCCESS) {
		const Setting& root = cfg.getRoot();

		Display* display = XOpenDisplay(0);

		try {
			const Setting& hotkeyEntries = root["hotkeys"];
			int count = hotkeyEntries.getLength();

			std::map<unsigned long, Hotkey> hotkeys;
			for(int i = 0; i < count; ++i) {
				const Setting& hotkeyEntry = hotkeyEntries[i];
				string key, command;

				// Only process a hotkeyEntry if all of the required fields are present
				if(hotkeyEntry.lookupValue("key", key) && hotkeyEntry.lookupValue("command", command)) {
					unsigned int modifiers = 0;

					if(hotkeyEntry.exists("modifiers")) {
						const Setting& modifierEntries = hotkeyEntry["modifiers"];
						for(int me_i = 0; me_i < modifierEntries.getLength(); me_i++) {
							const Setting& modifierEntry = modifierEntries[me_i];
							string modifierName = modifierEntry;

							if(modifierName == "Control") {
								modifiers |= ControlMask;
							} else if(modifierName == "Shift") {
								modifiers |= ShiftMask;
							} else if(modifierName == "Alt") {
								modifiers |= Mod1Mask;
							} else if(modifierName == "Super") {
								modifiers |= Mod4Mask;
							} else {
								cerr << modifierEntry.getSourceFile() << ": Unsupported modifier: `" << modifierName
									 << "' at line " << modifierEntry.getSourceLine()
									 << ". Must be one of: Control, Shift, Alt, Super" << endl;

								numErrors++;
							}
						}
					}

					KeySym keysym = XStringToKeysym(key.c_str());
					int keycode = XKeysymToKeycode(display, keysym);

					hotkeys[MapKey(keycode, modifiers)] = Hotkey(keycode, key, modifiers, command);
				} else {
					cerr << hotkeyEntry.getSourceFile() << ": Invalid hotkey entry at line "
						 << hotkeyEntry.getSourceLine() << endl;

					numErrors++;
				}
			}

			if(numErrors == 0) {
				grabKeys(display, hotkeys);
			}

		} catch(const SettingNotFoundException& snfe) {
			numErrors++;
		}

		XCloseDisplay(display);

		return numErrors == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
	}

	return EXIT_FAILURE;
}
