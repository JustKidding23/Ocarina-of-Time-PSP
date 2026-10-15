#include "oot_psp_enhancements.h"

#include <pspiofilemgr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "oot_psp_asset_loader.h"

#define OOT_PSP_ENHANCEMENTS_INI_BUFFER_SIZE 1024
#define OOT_PSP_ENHANCEMENTS_TEXT_SPEED 3

typedef struct OotPspEnhancementInfo {
    const char* iniName;
    const char* displayName;
    s32 defaultEnabled;
} OotPspEnhancementInfo;

static const OotPspEnhancementInfo sEnhancementInfo[OOT_PSP_ENH_COUNT] = {
    { "fast_text", "Faster text", true },
    { "skip_read_text", "Hold B skips read text", true },
    { "skip_logo", "Skip N64 logo", true },
    { "quiet_navi", "Quiet Navi calls", true },
    { "owl_default_no", "Owl repeat defaults to No", true },
    { "dampe_heart_piece", "Dampe heart piece first dig", true },
};

static const s32 sHudScales[] = { 100, 110, 120, 130 };
#define OOT_PSP_HUD_SCALE_DEFAULT 120

static s32 sEnabled[OOT_PSP_ENH_COUNT];
static s32 sHudScalePercent;

/* One bit per message ID. */
static u8 sSeenText[0x10000 / 8];
static s32 sSeenTextDirty;
static s32 sSeenTextLoaded;

static int OotPspEnhancements_StrIcmp(const char* a, const char* b) {
    unsigned char ca;
    unsigned char cb;

    while ((*a != '\0') && (*b != '\0')) {
        ca = (unsigned char)*a;
        cb = (unsigned char)*b;
        if ((ca >= 'A') && (ca <= 'Z')) {
            ca = (unsigned char)(ca - 'A' + 'a');
        }
        if ((cb >= 'A') && (cb <= 'Z')) {
            cb = (unsigned char)(cb - 'A' + 'a');
        }
        if (ca != cb) {
            return (int)ca - (int)cb;
        }
        a++;
        b++;
    }

    return (int)(unsigned char)*a - (int)(unsigned char)*b;
}

static char* OotPspEnhancements_Trim(char* text) {
    char* end;

    while ((*text == ' ') || (*text == '\t')) {
        text++;
    }
    end = text + strlen(text);
    while ((end > text) && ((end[-1] == ' ') || (end[-1] == '\t') || (end[-1] == '\r'))) {
        end--;
    }
    *end = '\0';
    return text;
}

static s32 OotPspEnhancements_ParseBool(const char* value) {
    return (OotPspEnhancements_StrIcmp(value, "on") == 0) || (OotPspEnhancements_StrIcmp(value, "true") == 0) ||
           (OotPspEnhancements_StrIcmp(value, "yes") == 0) || (strcmp(value, "1") == 0);
}

static void OotPspEnhancements_SetHudScale(s32 percent) {
    size_t i;

    sHudScalePercent = OOT_PSP_HUD_SCALE_DEFAULT;
    for (i = 0; i < sizeof(sHudScales) / sizeof(sHudScales[0]); i++) {
        if (sHudScales[i] == percent) {
            sHudScalePercent = percent;
            return;
        }
    }
}

void OotPspEnhancements_ResetDefaults(void) {
    s32 i;

    for (i = 0; i < OOT_PSP_ENH_COUNT; i++) {
        sEnabled[i] = sEnhancementInfo[i].defaultEnabled;
    }
    sHudScalePercent = OOT_PSP_HUD_SCALE_DEFAULT;
}

static void OotPspEnhancements_LoadSeenText(void) {
    char pathBuffer[384];
    const char* path =
        OotPsp_ResolveRootPath(OOT_PSP_ENHANCEMENTS_SEEN_TEXT_PATH, pathBuffer, sizeof(pathBuffer));
    SceUID fd;

    memset(sSeenText, 0, sizeof(sSeenText));
    sSeenTextDirty = false;
    sSeenTextLoaded = true;

    fd = sceIoOpen(path, PSP_O_RDONLY, 0);
    if (fd < 0) {
        return;
    }
    if (sceIoRead(fd, sSeenText, sizeof(sSeenText)) != (int)sizeof(sSeenText)) {
        memset(sSeenText, 0, sizeof(sSeenText));
    }
    sceIoClose(fd);
}

void OotPspEnhancements_Load(void) {
    char pathBuffer[384];
    char iniBuffer[OOT_PSP_ENHANCEMENTS_INI_BUFFER_SIZE];
    const char* path = OotPsp_ResolveRootPath(OOT_PSP_ENHANCEMENTS_INI_PATH, pathBuffer, sizeof(pathBuffer));
    SceUID fd;
    int readSize;
    char* line;
    char* nextLine;

    OotPspEnhancements_ResetDefaults();
    OotPspEnhancements_LoadSeenText();

    fd = sceIoOpen(path, PSP_O_RDONLY, 0);
    if (fd < 0) {
        (void)OotPspEnhancements_Save();
        return;
    }
    readSize = sceIoRead(fd, iniBuffer, sizeof(iniBuffer) - 1);
    sceIoClose(fd);
    if (readSize < 0) {
        return;
    }
    iniBuffer[readSize] = '\0';

    for (line = iniBuffer; line != NULL; line = nextLine) {
        char* equals;
        char* key;
        char* value;
        s32 i;

        nextLine = strchr(line, '\n');
        if (nextLine != NULL) {
            *nextLine++ = '\0';
        }
        line = OotPspEnhancements_Trim(line);
        if ((line[0] == '\0') || (line[0] == '#') || (line[0] == ';') || (line[0] == '[')) {
            continue;
        }
        equals = strchr(line, '=');
        if (equals == NULL) {
            continue;
        }
        *equals = '\0';
        key = OotPspEnhancements_Trim(line);
        value = OotPspEnhancements_Trim(equals + 1);

        if (OotPspEnhancements_StrIcmp(key, "hud_scale") == 0) {
            OotPspEnhancements_SetHudScale(atoi(value));
            continue;
        }
        for (i = 0; i < OOT_PSP_ENH_COUNT; i++) {
            if (OotPspEnhancements_StrIcmp(key, sEnhancementInfo[i].iniName) == 0) {
                sEnabled[i] = OotPspEnhancements_ParseBool(value);
                break;
            }
        }
    }
}

s32 OotPspEnhancements_Save(void) {
    char pathBuffer[384];
    char buffer[OOT_PSP_ENHANCEMENTS_INI_BUFFER_SIZE];
    const char* path = OotPsp_ResolveRootPath(OOT_PSP_ENHANCEMENTS_INI_PATH, pathBuffer, sizeof(pathBuffer));
    int length = 0;
    int written;
    SceUID fd;
    s32 i;

    length += snprintf(buffer + length, sizeof(buffer) - length, "[enhancements]\n");
    for (i = 0; i < OOT_PSP_ENH_COUNT; i++) {
        length += snprintf(buffer + length, sizeof(buffer) - length, "%s = %s\n", sEnhancementInfo[i].iniName,
                           sEnabled[i] ? "on" : "off");
    }
    length += snprintf(buffer + length, sizeof(buffer) - length, "hud_scale = %d\n", (int)sHudScalePercent);
    if ((length < 0) || ((size_t)length >= sizeof(buffer))) {
        return -1;
    }

    fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
    if (fd < 0) {
        return fd;
    }
    written = sceIoWrite(fd, buffer, length);
    sceIoClose(fd);
    return (written == length) ? 0 : -1;
}

s32 OotPspEnhancements_IsEnabled(OotPspEnhancement enhancement) {
    return ((u32)enhancement < OOT_PSP_ENH_COUNT) && sEnabled[enhancement];
}

void OotPspEnhancements_Toggle(OotPspEnhancement enhancement) {
    if ((u32)enhancement < OOT_PSP_ENH_COUNT) {
        sEnabled[enhancement] = !sEnabled[enhancement];
    }
}

const char* OotPspEnhancements_GetName(OotPspEnhancement enhancement) {
    return ((u32)enhancement < OOT_PSP_ENH_COUNT) ? sEnhancementInfo[enhancement].displayName : "";
}

s32 OotPspEnhancements_GetHudScalePercent(void) {
    return (sHudScalePercent != 0) ? sHudScalePercent : 100;
}

void OotPspEnhancements_CycleHudScale(s32 direction) {
    s32 count = (s32)(sizeof(sHudScales) / sizeof(sHudScales[0]));
    s32 i;

    for (i = 0; i < count; i++) {
        if (sHudScales[i] == sHudScalePercent) {
            break;
        }
    }
    if (i == count) {
        i = 0;
    }
    i = (i + ((direction < 0) ? count - 1 : 1)) % count;
    sHudScalePercent = sHudScales[i];
}

s32 OotPspEnhancements_GetTextSpeed(void) {
    return OotPspEnhancements_IsEnabled(OOT_PSP_ENH_FAST_TEXT) ? OOT_PSP_ENHANCEMENTS_TEXT_SPEED : 1;
}

s32 OotPspEnhancements_HasSeenText(u16 textId) {
    return sSeenTextLoaded && ((sSeenText[textId >> 3] >> (textId & 7)) & 1);
}

void OotPspEnhancements_MarkTextSeen(u16 textId) {
    u8 bit = (u8)(1 << (textId & 7));

    if (!(sSeenText[textId >> 3] & bit)) {
        sSeenText[textId >> 3] |= bit;
        sSeenTextDirty = true;
    }
}

/* Called after the game writes its save, so the read-text list persists with progress. */
void OotPspEnhancements_SaveSeenText(void) {
    char pathBuffer[384];
    const char* path;
    SceUID fd;

    if (!sSeenTextLoaded || !sSeenTextDirty) {
        return;
    }
    path = OotPsp_ResolveRootPath(OOT_PSP_ENHANCEMENTS_SEEN_TEXT_PATH, pathBuffer, sizeof(pathBuffer));
    fd = sceIoOpen(path, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777);
    if (fd < 0) {
        return;
    }
    if (sceIoWrite(fd, sSeenText, sizeof(sSeenText)) == (int)sizeof(sSeenText)) {
        sSeenTextDirty = false;
    }
    sceIoClose(fd);
}
