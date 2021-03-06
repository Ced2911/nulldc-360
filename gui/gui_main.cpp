/****************************************************************************
 * Snes9x Nintendo Wii/Gamecube Port
 *
 * softdev July 2006
 * crunchy2 May 2007-July 2007
 * Michniewski 2008
 * Tantric 2008-2010
 *
 * snes9xgx.cpp
 *
 * This file controls overall program flow. Most things start and end here!
 ***************************************************************************/
#include <xetypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include "w_input.h"
#include <libfat/fat.h>
#include <debug.h>
#include <sys/iosupport.h>
#include <diskio/ata.h>
#include <usb/usbmain.h>
#include <xenon_soc/xenon_power.h>
#include <xenon_sound/sound.h>
#include <xenon_smc/xenon_smc.h>
#include <debug.h>

#include "video.h"
#include "audio.h"
#include "menu.h"
#include "freeze.h"
#include "preferences.h"
#include "button_mapping.h"
#include "fileop.h"
#include "filebrowser.h"
#include "input.h"
#include "FreeTypeGX.h"

#include "filelist.h"
#include "mount.h"
#include "emu.h"

int ScreenshotRequested = 0;
int ShutdownRequested = 0;
int ExitRequested = 0;
char loadedFile[1024] = {0};
static int currentMode;
int exitThreads = 0;

/****************************************************************************
 * Shutdown / Reboot / Exit
 ***************************************************************************/

void ExitCleanup()
{
	UnmountAllFAT();
}

void ExitApp()
{
	SavePrefs(true);
	xenon_smc_power_shutdown();
}

int main(int argc, char *argv[])
{
	xenon_make_it_faster(XENON_SPEED_FULL);
	InitVideo();
	
	usb_init();
	usb_do_poll();
	xenon_ata_init();
	
	xenon_sound_init();
	
	SetupPads();
	mount_all_devices();
	// Set defaults
	DefaultSettings(); 
	// Initialize font system
	InitFreeType((u8*)font_ttf, font_ttf_size); 
	
	browserList = (BROWSERENTRY *)malloc(sizeof(BROWSERENTRY)*MAX_BROWSER_SIZE);
		
	while (1) // main loop
	{
		MainMenu(MENU_GAMESELECTION);
		EmuLaunch();
		
		if(EmuRunning)
			MainMenu(MENU_GAME);
		else
			MainMenu(MENU_GAMESELECTION);		

		if(EmuRunning)
			EmuResume();
		else
			EmuLaunch();
		
		if(EmuConfigRequested) {				
			EmuConfigRequested = 0;
			break;
		}
		if(EmuResetRequested) {
			EmuResetRequested = 0;
			EmuReset();
		}
	}
	
	return 0;
}

void InGameMenu(){
	MainMenu(MENU_GAME);
	EmuResume();
}