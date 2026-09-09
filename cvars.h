#pragma once

#include "main.h"

class CCvars
{
private:

public:
	class CHud
	{
	public:
		cvar_s *ShowSpeed;
	};
	
	CHud Hud;

	class CBunnyHop
	{
	public:
		cvar_s *Toggle;
		cvar_s *Session;
		cvar_s *MaxIdealsIAR;
		cvar_s *MaxIdealsPercent;
		cvar_s *MinPostCmds;
		cvar_s *MaxPostCmds;
	};

	CBunnyHop BunnyHop;
	
};

extern CCvars Cvars;