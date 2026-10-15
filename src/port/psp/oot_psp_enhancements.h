#ifndef OOT_PSP_ENHANCEMENTS_H
#define OOT_PSP_ENHANCEMENTS_H

#include "ultra64/ultratypes.h"

#define OOT_PSP_ENHANCEMENTS_INI_PATH "enhancements.ini"
#define OOT_PSP_ENHANCEMENTS_SEEN_TEXT_PATH "seen_text.bin"

/* Optional quality-of-life changes, set from the HOME menu and enhancements.ini. */
typedef enum OotPspEnhancement {
    OOT_PSP_ENH_FAST_TEXT,      /* draw text several characters per frame */
    OOT_PSP_ENH_SKIP_READ_TEXT, /* hold B to skip through text already read */
    OOT_PSP_ENH_SKIP_LOGO,      /* skip the N64 logo at boot */
    OOT_PSP_ENH_QUIET_NAVI,     /* no "Hey! Listen!" voice; C-Up still flashes */
    OOT_PSP_ENH_OWL_DEFAULT_NO, /* Kaepora Gaebora's repeat question starts on "No" */
    OOT_PSP_ENH_DAMPE_HEART,    /* first valid grave dig gives Dampe's heart piece */
    OOT_PSP_ENH_COUNT
} OotPspEnhancement;

void OotPspEnhancements_Load(void);
s32 OotPspEnhancements_Save(void);
void OotPspEnhancements_ResetDefaults(void);

s32 OotPspEnhancements_IsEnabled(OotPspEnhancement enhancement);
void OotPspEnhancements_Toggle(OotPspEnhancement enhancement);
const char* OotPspEnhancements_GetName(OotPspEnhancement enhancement);

/* HUD size in percent (100 = original). */
s32 OotPspEnhancements_GetHudScalePercent(void);
void OotPspEnhancements_CycleHudScale(s32 direction);

/* Characters revealed per frame when fast text is on. */
s32 OotPspEnhancements_GetTextSpeed(void);

/* Message IDs the player has finished reading, for hold-B skipping. */
s32 OotPspEnhancements_HasSeenText(u16 textId);
void OotPspEnhancements_MarkTextSeen(u16 textId);
void OotPspEnhancements_SaveSeenText(void);

#endif
