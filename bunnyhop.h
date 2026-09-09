#pragma once

#include "main.h"

enum BhopType
{
	Fail,
	Ideal
};

class CBunnyHop
{
private:
	int iAirFrames;
	int iGroundFrames;
	int iBhopType;
	int iBhops;
	int iIdealBhops;
	int iIdealBhopsIAR;
	int iPostCmds;

	bool bDoPostCmds;

	float flIdealsBhopsPercent;
public:
	void HUD_Redraw(float time, int intermission);
	void CL_CreateMove(float frametime, usercmd_s* cmd, int active);
	void DoPostCmds(int iMinCmds, int iMaxCmds);

	int iScrollDirection = 120;

	bool bKey;
};

extern CBunnyHop BunnyHop;