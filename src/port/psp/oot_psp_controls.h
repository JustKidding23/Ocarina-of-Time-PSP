#ifndef OOT_PSP_CONTROLS_H
#define OOT_PSP_CONTROLS_H

#include <stddef.h>
#include <stdint.h>

#include "ultra64/ultratypes.h"

#define OOT_PSP_CONTROLS_INI_PATH "controls.ini"
#define OOT_PSP_CONTROLS_DEADZONE_MIN 0
#define OOT_PSP_CONTROLS_DEADZONE_MAX 80
/* PSP sticks often rest 10-20 units off centre (of 128). */
#define OOT_PSP_CONTROLS_DEADZONE_DEFAULT 20

void OotPspControls_InitDefaults(void);
s32 OotPspControls_Load(void);
s32 OotPspControls_Save(void);
void OotPspControls_ResetDefaults(void);

u16 OotPspControls_MapButtons(u32 pspButtons);
void OotPspControls_MapStick(u8 rawX, u8 rawY, s8* outX, s8* outY);

int OotPspControls_GetBindingCount(void);
const char* OotPspControls_GetBindingName(int index);
void OotPspControls_GetBindingValueText(int index, char* buffer, size_t bufferSize);
void OotPspControls_CycleBinding(int index, int direction);
int OotPspControls_GetDeadzone(void);
void OotPspControls_SetDeadzone(int deadzone);
void OotPspControls_AdjustDeadzone(int delta);
int OotPspControls_IsN64StickScalingEnabled(void);
void OotPspControls_SetN64StickScalingEnabled(int enabled);

#endif
