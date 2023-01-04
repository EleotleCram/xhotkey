# XHotkey - Global Hotkey Application For Xorg

## What

*xhotkey* is a small standalone global hotkey application for Xorg (X11 protocol). It is meant to be small, light and it fits in nicely with the rest of the x application family (xscreensaver, xclip, xkill, etc.)

## Why

If you run a minimalist Xorg with only a window manager and no session manager, but you do want to configure global hotkeys, then xhotkey may be for you.

## Example configuration

```
$ cat ~/.xhotkey
// Here be hotkeys...
hotkeys = ({
	key = "XF86AudioPrev";
	command = "playerctl previous";
},{
	key = "XF86AudioPlay";
	command = "playerctl play-pause";
},{
	key = "XF86AudioNext";
	command = "playerctl next";
},{
	key = "XF86AudioLowerVolume";
	command = "amixer -D pulse set Master 10%-";
},{
	key = "XF86AudioMute";
	command = "amixer -D pulse set Master toggle";
},{
	key = "XF86AudioRaiseVolume";
	command = "amixer -D pulse set Master 10%+";
},{
	key = "l";
	modifiers = ("Control", "Shift", "Alt");
	command = "ls";
});
```

## Usage

Add the following line to your _.xinitrc_ or _startup_ file:

```
xhotkey &
```

## Build instructions

Install the prerequisites:
```
$ sudo apt install "libx11-dev"
$ sudo apt install "libconfig++-dev"
```

Then build using make:
```
$ make
```

The resulting binary can be found in _bin/_.
```
ls -lah bin/
total 76K
drwxrwxr-x 2 user user 4.0K Jan  4 14:57 .
drwxrwxr-x 6 user user 4.0K Jan  4 14:59 ..
-rw-rw-r-- 1 user user	0 Jan  3 23:31 .gitkeep
-rwxrwxr-x 1 user user  52K Jan  4 14:57 xhotkey
```
