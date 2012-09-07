/****************************************************************************
 * Snes9x Nintendo Wii/Gamecube Port
 *
 * softdev July 2006
 * crunchy2 May 2007-July 2007
 * Michniewski 2008
 * Tantric 2008-2010
 *
 * snes9xgx.h
 *
 * This file controls overall program flow. Most things start and end here!
 ***************************************************************************/

#ifndef _SNES9XGX_H_
#define _SNES9XGX_H_

#define MAXPATHLEN 1024

#include "FreeTypeGX.h"
#include "filelist.h"

#define APPNAME 		"NullDc"
#define APPVERSION 		"libxenon"
#define APPFOLDER 		"snes9xgx"
#define PREF_FILE_NAME 	"settings.xml"

#define NOTSILENT 0
#define SILENT 1

const char pathPrefix[9][8] =
//{ "", "sd:/", "usb:/", "dvd:/", "smb:/", "carda:/", "cardb:/" };
{ "", "uda:/", "sda0:/", "dvd:/", "smb:/", };

enum {
	DEVICE_AUTO,
	DEVICE_USB,
        DEVICE_HDD,
	DEVICE_DVD,
	DEVICE_SMB,
};

enum {
	FILE_SRAM,
	FILE_SNAPSHOT,
	FILE_ROM,
	FILE_CHEAT
};

enum
{
	CTRL_PAD,
	CTRL_MOUSE,
	CTRL_SCOPE,
	CTRL_JUST,
	CTRL_PAD2,
	CTRL_PAD4,
	CTRL_LENGTH
};

const char ctrlName[6][24] =
{ "SNES Controller", "SNES Mouse", "Superscope", "Justifier", "SNES Controllers (2)", "SNES Controllers (4)" };

enum {
	LANG_JAPANESE = 0,
	LANG_ENGLISH,
	LANG_GERMAN,
	LANG_FRENCH,
	LANG_SPANISH,
	LANG_ITALIAN,
	LANG_DUTCH,
	LANG_SIMP_CHINESE,
	LANG_TRAD_CHINESE,
	LANG_KOREAN,
	LANG_PORTUGUESE,
	LANG_BRAZILIAN_PORTUGUESE,
	LANG_CATALAN,
	LANG_TURKISH,
	LANG_LENGTH
};

struct SGCSettings{
    int		AutoLoad;
    int		AutoSave;
    int		LoadMethod; // For ROMS: Auto, SD, DVD, USB, Network (SMB)
	int		SaveMethod; // For SRAM, Freeze, Prefs: Auto, SD, USB, SMB
	char	LoadFolder[MAXPATHLEN]; // Path to game files
	char	SaveFolder[MAXPATHLEN]; // Path to save files
	char	CheatFolder[MAXPATHLEN]; // Path to cheat files

	char	smbip[80];
	char	smbuser[20];
	char	smbpwd[20];
	char	smbshare[20];

	float	zoomHor; // horizontal zoom amount
	float	zoomVert; // vertical zoom amount
	int		videomode; // 0 - automatic, 1 - NTSC (480i), 2 - Progressive (480p), 3 - PAL (50Hz), 4 - PAL (60Hz)
	int		render;		// 0 - original, 1 - filtered, 2 - unfiltered
	int		FilterMethod; // convert to RenderFilter
	int		Controller;
	int		crosshair;
	int		widescreen;	// 0 - 4:3 aspect, 1 - 16:9 aspect
	int		xshift;	// video output shift
	int		yshift;
	int		WiimoteOrientation;
	int		ExitAction;
	int		MusicVolume;
	int		SFXVolume;
	int		Rumble;
	int 	language;
};

void S9xInitNetPlay();
void ExitApp();
void ShutdownWii();
bool SupportedIOS(u32 ios);
bool SaneIOS(u32 ios);
extern struct SGCSettings GCSettings;
extern int ScreenshotRequested;
extern int ShutdownRequested;
extern int ExitRequested;
extern char appPath[];
extern char loadedFile[];
extern FreeTypeGX *fontSystem[];

extern int exitThreads;
#endif
