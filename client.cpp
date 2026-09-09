#include "client.h"
#include "cvars.h"
#include "bunnyhop.h"

CCvars Cvars;
CBunnyHop BunnyHop;

bool FirstFrame = false;

void HookUserMessages()
{
	pUserMsgBase = (PUserMsg)offset.FindUserMsgBase();

	pResetHUD = HookUserMsg("ResetHUD", ResetHUD);
	pSetFOV = HookUserMsg("SetFOV", SetFOV);
	pTeamInfo = HookUserMsg("TeamInfo", TeamInfo);
	pCurWeapon = HookUserMsg("CurWeapon", CurWeapon);
	pDeathMsg = HookUserMsg("DeathMsg", DeathMsg);
}

void HookEngineMessages()
{
	pEngineMsgBase = (PEngineMsg)offset.FindSVCMessages();

	pSVC_sound = HookEngineMsg("svc_sound", SVC_Sound);
	pSVC_StuffText = HookEngineMsg("svc_stufftext", SVC_StuffText);
	pSVC_NewUserMsg = HookEngineMsg("svc_newusermsg", SVC_NewUserMsg);
	pSVC_UpdateUserInfo = HookEngineMsg("svc_updateuserinfo", SVC_UpdateUserInfo);
	pSVC_SendCvarValue = HookEngineMsg("svc_sendcvarvalue", SVC_SendCvarValue);
	pSVC_SendCvarValue2 = HookEngineMsg("svc_sendcvarvalue2", SVC_SendCvarValue2);
	pSVC_Director = HookEngineMsg("svc_director", SVC_Director);
}

void ConsolePrintColor(BYTE R, BYTE G, BYTE B, char* string)
{
	TColor24 DefaultColor;
	PColor24 Ptr;

	Ptr = Console_TextColor;
	DefaultColor = *Ptr;

	Ptr->R = R;
	Ptr->G = G;
	Ptr->B = B;

	g_Engine.Con_Printf("%s", string);
	*Ptr = DefaultColor;
}

void InitHack()
{
	font.InitText("Verdana", 12, 7);

	/* Hud Cvars */
	Cvars.Hud.ShowSpeed = g_Engine.pfnRegisterVariable("!kzh^hud_show_speed", "1", FCVAR_CLIENTDLL);
	/* Hud Cvars */

	/* BunnyHop Cvars */
	Cvars.BunnyHop.Toggle = g_Engine.pfnRegisterVariable("!kzh^bunnyhop_toggle", "1", FCVAR_CLIENTDLL);
	Cvars.BunnyHop.Session = g_Engine.pfnRegisterVariable("!kzh^bunnyhop_session", "50", FCVAR_CLIENTDLL);
	Cvars.BunnyHop.MaxIdealsIAR = g_Engine.pfnRegisterVariable("!kzh^bunnyhop_max_ideals_iar", "9", FCVAR_CLIENTDLL);
	Cvars.BunnyHop.MaxIdealsPercent = g_Engine.pfnRegisterVariable("!kzh^bunnyhop_max_ideals_percent", "70.0", FCVAR_CLIENTDLL);
	Cvars.BunnyHop.MinPostCmds = g_Engine.pfnRegisterVariable("!kzh^bunnyhop_min_post_cmds", "2", FCVAR_CLIENTDLL);
	Cvars.BunnyHop.MaxPostCmds = g_Engine.pfnRegisterVariable("!kzh^bunnyhop_max_post_cmds", "4", FCVAR_CLIENTDLL);
	/* BunnyHop Cvars */

	g_Engine.pfnClientCmd("toggleconsole");

	ConsolePrintColor(50, 255, 50, "\n\n\t\tKZH");
	ConsolePrintColor(255, 255, 255, " by ");
	ConsolePrintColor(255, 200, 200, "Cheatometry");
	ConsolePrintColor(255, 255, 255, " :-s\n");
}

void HUD_Frame(double time)
{
	if (!FirstFrame)
	{
		g_Screen.iSize = sizeof(SCREENINFO);

		offset.HLType = g_Studio.IsHardware() + 1;
		offset.ConsoleColorInitalize();

		HookUserMessages();
		HookEngineMessages();

		InitHack();
		FirstFrame = true;
	}

	g_Engine.pfnGetScreenInfo(&g_Screen);
	g_Client.HUD_Frame(time);
}

void HUD_Redraw(float time, int intermission)
{
	g_Client.HUD_Redraw(time, intermission);

	if (g_Local.bAlive)
	{
		if (Cvars.Hud.ShowSpeed->value)
		{
			static char szSpeed[5];
			sprintf_s(szSpeed, sizeof(szSpeed) - 1, "%.0f", g_Local.flSpeed);

			font.Print(round(g_Screen.iWidth * 0.5), round(g_Screen.iHeight * 0.90), 0, 255, 0, 255, 0, szSpeed);
		}

		BunnyHop.HUD_Redraw(time, intermission);
	}
}

int HUD_Key_Event(int down, int keynum, const char* pszCurrentBinding)
{
	if (Cvars.BunnyHop.Toggle->value)
	{
		if (g_Local.bAlive)
		{
			if (keynum == K_MOUSE2)
			{
				BunnyHop.bKey = !!down;
				return 0;
			}
		}

		if (keynum == K_KP_PLUS)
		{
			BunnyHop.iScrollDirection = BunnyHop.iScrollDirection == 120 ? -120 : 120;
			g_Engine.pfnPlaySoundByName(BunnyHop.iScrollDirection == 120 ? "vox/up.wav" : "vox/down.wav", 1.0);
		}
	}

	return g_Client.HUD_Key_Event(down, keynum, pszCurrentBinding);
}

void CL_CreateMove(float frametime, usercmd_s* cmd, int active)
{
	g_Client.CL_CreateMove(frametime, cmd, active);

	if (g_Local.bAlive)
	{
		BunnyHop.CL_CreateMove(frametime, cmd, active);
	}
}

void HUD_PlayerMove(struct playermove_s* ppmove, int server)
{
	g_Client.HUD_PlayerMove(ppmove, server);

	g_Local.iOldFlags = g_Local.iFlags;
	g_Local.iFlags = ppmove->flags;
	g_Local.iOldButtons = ppmove->oldbuttons;
	g_Local.iUseHull = ppmove->usehull;

	g_Local.flTime = ppmove->time;
	g_Local.flOldSpeed = g_Local.flSpeed;
	g_Local.flSpeed = sqrtf(ppmove->velocity.x * ppmove->velocity.x + ppmove->velocity.y * ppmove->velocity.y);
	g_Local.flMaxSpeed = ppmove->maxspeed;
	g_Local.flMaxPreStrafe = ppmove->maxspeed * 1.2f;
	g_Local.flFrameTime = ppmove->frametime;
	g_Local.fuser2 = ppmove->fuser2;

	g_Local.bAlive = ppmove->movetype == MOVETYPE_NONE ? false : true;

	g_Local.vVelocity = ppmove->velocity;
	g_Local.vOrigin = ppmove->origin;
}

void HookFunction()
{
	g_pClient->HUD_Redraw = HUD_Redraw;
	g_pClient->HUD_Key_Event = HUD_Key_Event;
	g_pClient->CL_CreateMove = CL_CreateMove;
	g_pClient->HUD_PlayerMove = HUD_PlayerMove;
	g_pClient->HUD_Frame = HUD_Frame;
}