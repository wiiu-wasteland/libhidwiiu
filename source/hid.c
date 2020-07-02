#include "wiiu_types.h"

HidControllerKeys keyMap[32] = {
	0, 0, KEY_MINUS, KEY_PLUS, KEY_R, KEY_L, KEY_ZR, KEY_ZL,
	KEY_DDOWN, KEY_DUP, KEY_DRIGHT, KEY_DLEFT, KEY_Y, KEY_X,
	KEY_B, KEY_A, 0, KEY_R, KEY_L, 0, 0, 0, 0, KEY_RSTICK_DOWN,
	KEY_RSTICK_UP, KEY_RSTICK_RIGHT, KEY_RSTICK_LEFT, KEY_LSTICK_DOWN,
	KEY_LSTICK_UP, KEY_LSTICK_RIGHT, KEY_LSTICK_LEFT, 0, 0,
};

static u64 keysHeld;
static u64 keysDown;
static u64 keysUp;

static u32 touch_px;
static u32 touch_py;

void hidScanInput(void)
{
	VPADStatus t_vpad;
	VPADReadError e_vpad;
	VPADRead(VPAD_CHAN_0, &t_vpad, 1, &e_vpad);
	if (e_vpad == VPAD_READ_SUCCESS)
	{
		VPADTouchData tpdata;
		VPADGetTPCalibratedPoint(VPAD_CHAN_0, &tpdata, &t_vpad.tpFiltered1);

		u64 kHeld = 0;
		u64 kDown = 0;
		u64 kUp = 0;

		for (int i = 0; i < 32; i++)
		{
			kHeld |= (t_vpad.hold & BIT(i)) ? keyMap[i] : 0;
			kDown |= (t_vpad.trigger & BIT(i)) ? keyMap[i] : 0;
			kUp |= (t_vpad.release & BIT(i)) ? keyMap[i] : 0;
		}

		if (tpdata.touched)
		{
			if (!(keysHeld & KEY_TOUCH))
				kDown |= KEY_TOUCH;
			kHeld |= KEY_TOUCH;

			touch_px = (u32)tpdata.x;
			touch_py = (u32)tpdata.y;
		}
		else
		{
			if (keysHeld & KEY_TOUCH)
				kUp |= KEY_TOUCH;
		}

		keysHeld = kHeld;
		keysDown = kDown;
		keysUp = kUp;
	}
}

u64 hidKeysHeld(HidControllerID id)
{
	if ((id == CONTROLLER_P1_AUTO) || (id == CONTROLLER_PLAYER_1))
	{
		return keysHeld;
	}
	return 0;
}

u64 hidKeysDown(HidControllerID id)
{
	if ((id == CONTROLLER_P1_AUTO) || (id == CONTROLLER_PLAYER_1))
	{
		return keysDown;
	}
	return 0;
}

u64 hidKeysUp(HidControllerID id)
{
	if ((id == CONTROLLER_P1_AUTO) || (id == CONTROLLER_PLAYER_1))
	{
		return keysUp;
	}
	return 0;
}

u32 hidTouchCount(void)
{
	return !!(keysHeld & KEY_TOUCH);
}

void hidTouchRead(touchPosition *pos, u32 point_id)
{
	if (point_id == 0 && pos)
	{
		pos->px = touch_px;
		pos->py = touch_py;
		pos->dx = 1;
		pos->dy = 1;
	}
}
