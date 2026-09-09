#include "bunnyhop.h"
#include "cvars.h"
#include "calculations.h"
#include "functions.h"

void CBunnyHop::HUD_Redraw(float time, int intermission)
{
	if (g_Local.iFlags & FL_ONGROUND)
	{
		if (iAirFrames)
			iAirFrames = 0;

		if (bDoPostCmds)
		{
			if (iPostCmds)
				iPostCmds = 0;

			bDoPostCmds = false;
		}

		iGroundFrames++;

		if (bKey)
		{
			if (iGroundFrames <= 5)
			{
				if (~g_Local.iOldButtons & IN_JUMP)
				{
					switch (iBhopType)
					{
					case Fail:
						if ((g_Local.flSpeed > g_Local.flMaxPreStrafe && iGroundFrames == 1) || (g_Local.flSpeed < g_Local.flMaxPreStrafe && g_Local.flOldSpeed < g_Local.flMaxPreStrafe && iGroundFrames >= 2))
						{
							SimulateMouse(iScrollDirection);
							DoPostCmds(Cvars.BunnyHop.MinPostCmds->value, Cvars.BunnyHop.MaxPostCmds->value);
						}
						break;
					case Ideal:
						if (g_Local.flSpeed < g_Local.flMaxPreStrafe && (iGroundFrames == 1 || iGroundFrames >= 2 && g_Local.flOldSpeed > g_Local.flMaxPreStrafe))
						{
							SimulateMouse(iScrollDirection);
							DoPostCmds(Cvars.BunnyHop.MinPostCmds->value, Cvars.BunnyHop.MaxPostCmds->value);
						}
						break;
					}
				}
			}
			else
			{
				if (~g_Local.iOldButtons & IN_JUMP)
				{
					SimulateMouse(iScrollDirection);
					DoPostCmds(Cvars.BunnyHop.MinPostCmds->value, Cvars.BunnyHop.MaxPostCmds->value);
				}
			}
		}
	}
	else
	{
		if (iGroundFrames)
			iGroundFrames = 0;

		iAirFrames++;

		if (iAirFrames == 1)
		{
			if ((int)g_Engine.pfnRandomLong(1, 100) <= (int)Cvars.BunnyHop.MaxIdealsPercent->value && iIdealBhopsIAR < Cvars.BunnyHop.MaxIdealsIAR->value && flIdealsBhopsPercent < Cvars.BunnyHop.MaxIdealsPercent->value)
				iBhopType = Ideal;
			else
				iBhopType = Fail;
		}

		if (bDoPostCmds)
		{
			if (~g_Local.iOldButtons & IN_JUMP)
			{
				SimulateMouse(iScrollDirection);

				if (--iPostCmds == 0)
				{
					bDoPostCmds = false;
				}
			}
		}
	}
}

void CBunnyHop::CL_CreateMove(float frametime, usercmd_s* cmd, int active)
{
	if (g_Local.iFlags & FL_ONGROUND)
	{
		if (cmd->buttons & IN_JUMP && ~g_Local.iOldButtons & IN_JUMP)
		{
			if (iGroundFrames <= 5)
			{
				iBhops++;

				if (g_Local.flSpeed < g_Local.flMaxPreStrafe && (iGroundFrames == 1 || iGroundFrames >= 2 && g_Local.flOldSpeed > g_Local.flMaxPreStrafe))
				{
					iIdealBhops++;
					iIdealBhopsIAR++;
				}
				else
				{
					if (iIdealBhopsIAR)
						iIdealBhopsIAR = 0;
				}

				flIdealsBhopsPercent = (float)iIdealBhopsIAR * 100.0 / (float)iBhops;

				if (iBhops >= Cvars.BunnyHop.Session->value)
				{
					iBhops = 0;

					if (iIdealBhops)
					{
						iIdealBhops = 0;
						iIdealBhopsIAR = 0;
						flIdealsBhopsPercent = 0.0;
					}
				}
			}
		}
	}
}

void CBunnyHop::DoPostCmds(int iMinCmds, int iMaxCmds)
{
	iPostCmds = (int)g_Engine.pfnRandomLong(iMinCmds, iMaxCmds);

	bDoPostCmds = true;
}