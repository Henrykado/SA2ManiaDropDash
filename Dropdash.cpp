#include "pch.h"
#include "Dropdash.h"

// Drop Dash variables
Buttons dropdashButtons;
float dropdashSpeed = 0.0f;
int dropdashTimer = 0;
bool canChargeDropDash = false;
bool canSomersault = true;

// "Press to Bounce" variables
Buttons bounceButtons;
bool canBounce = true;
bool waitAFrame = false;

// config variables
std::string dropdashButton;
bool dropdashBounce;
bool fallDropdash;
bool sonicCanDropdash;
bool shadowCanDropdash;
bool amyCanDropdash;
bool metalCanDropdash;

// "Sonic New Tricks" compatibility variables
bool sonicNewTricks = false;
bool superBounce = false;
bool shBounce = false;;
Buttons SomersaultButton = Buttons_B;
Buttons BlackShieldButton = Buttons_Y;


bool canDropdash(char CharID2)
{
	switch (CharID2)
	{
		case Characters_Sonic: return sonicCanDropdash;
		case Characters_Shadow: return shadowCanDropdash;
		case Characters_Amy: return amyCanDropdash;
		case Characters_MetalSonic: return metalCanDropdash;
	}
}


bool canBounce2ElectricBoogaloo(CharObj2Base co2)
{
	if ((co2.Upgrades & Upgrades_SonicBounceBracelet) == 0)
		if (!shBounce || shBounce && co2.CharID != Characters_Shadow) // Sonic New Tricks option
			return false;

	if (co2.CharID2 == Characters_MetalSonic && BlackShieldButton == bounceButtons)
		return false;

	if (bounceButtons != dropdashButtons &&
		((bounceButtons != Buttons_Action && (bounceButtons & Controllers[co2.PlayerNum].press)) ||
		(bounceButtons == Buttons_Action && Action_Pressed[co2.PlayerNum])))
	{
		return true;
	}

	if (dropdashBounce && canBounce)
	{
		if ((dropdashButtons != Buttons_Action && (dropdashButtons & Controllers[co2.PlayerNum].release)) ||
			(dropdashButtons == Buttons_Action && Action_Released[co2.PlayerNum]))
		{
			return true;
		}
	}

	return false;
}


int Sonic_CheckBounceAttack_r(SonicCharObj2* a1, EntityData1* a2, int a3)
{
	if (canDropdash(a1->base.CharID2))
	{
		if (canBounce2ElectricBoogaloo(a1->base))
		{
			a1->base.AnimInfo.Next = 30;
			a2->Action = 69; // nice
			a2->Status |= Status_Attack | Status_Ball;
			if (superBounce) a1->base.Speed.y = -7.0;
			else a1->base.Speed.y = -5.0;
			*(WORD*)(a3 + 870) = 0; // (SonicCharObj2)->SomersaultTime = 0
			PlaySoundProbably(8202, 0, 0, 0);
			return true;
		}
		return false;
	}
	return Sonic_CheckBounceAttack_h.Original(a1, a2, a3);
}

int Sonic_CheckBounceAttack2_r(SonicCharObj2* a1, EntityData1* a2)
{
	if (canDropdash(a1->base.CharID2))
	{
		if (canBounce2ElectricBoogaloo(a1->base))
		{
			a1->base.AnimInfo.Next = 30;
			a2->Action = 69; // nice
			a2->Status |= Status_Attack | Status_Ball;
			a1->base.Speed.y = -7.0;
			PlaySoundProbably(8202, 0, 0, 0);
			return true;
		}
		return false;
	}
	return Sonic_CheckBounceAttack2_h.Original(a1, a2);
}


int Sonic_Somersault_r(SonicCharObj2* a1, EntityData1* a2, CharObj2Base* a3)
{
	if (!canSomersault) return false;
	else return Sonic_Somersault_h.Original(a1, a2, a3);
}



void Sonic_ChecksForDamage_r(EntityData1* ed1, EntityData2* ed2, CharObj2Base* co2, SonicCharObj2* sco2)
{
	if (!canDropdash(co2->CharID2))
		return Sonic_ChecksForDamage_h.Original(ed1, ed2, co2, sco2);


	if (!canSomersault && ed1->Action != 4)
		if (Action_Released[co2->PlayerNum])
			canSomersault = true;

	if (sonicNewTricks)
	{
		if ( SomersaultButton == Buttons_Y || 
			(ed1->Action == 1 && (SomersaultButton == Buttons_B || SomersaultButton == Buttons_Y) &&
			SomersaultButton & Controllers[co2->PlayerNum].press) )
		{
			canSomersault = true;
		}
	}


	if (ed1->Action == 6 || ed1->Action == 70 || (fallDropdash && ed1->Action == 10)) // Jump, Bounce or (Fall if fallDropdash is true)
	{
		if (canChargeDropDash == false && (dropdashButtons & Controllers[co2->PlayerNum].press))
		{
			canChargeDropDash = true;
			canBounce = true;
		}

		if (waitAFrame)
		{
			waitAFrame = false;
			canBounce = true;
		}

		if (canChargeDropDash && (dropdashButtons & Controllers[co2->PlayerNum].on))
			dropdashTimer++;
		else if (dropdashTimer > 0) {
			dropdashTimer = 0;
			canChargeDropDash = false;
			ed1->Status &= ~Status_Ball;
			waitAFrame = true; // used to delay setting canBounce to true so the character doesn't immediatly bounce after releasing the drop dash button
		}

		if (dropdashTimer == 15) 
		{
			ed1->Status |= Status_Ball;
			co2->AnimInfo.Next = 12;
			if (ed1->Action == 10) ed1->Action = 6;
			canBounce = false;
		}

		if (dropdashTimer >= 15) 
			QueueSound_DualEntity(8206, (ObjectMaster*)ed1, 1, 0, 2);
	}
	else
	{
		dropdashTimer = 0;
		canChargeDropDash = false;
		canBounce = false;
	}

	Sonic_ChecksForDamage_h.Original(ed1, ed2, co2, sco2);

	if (dropdashTimer > 15 && (ed1->Action == 1 || ed1->Action == 0)) // Walk or Idle
	{
		ed1->Status |= (Status_Attack | Status_Ball);
		dropdashSpeed = 7 + floor(min((dropdashTimer - 15) / 60, 2));
		
		// Release Spindash code
		ed1->Action = 4;
		co2->Speed.x = dropdashSpeed;
		co2->AnimInfo.Next = 12;
		PlaySoundProbably(8203, 0, 0, 0);

		canSomersault = false;
	}
}


extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
		dropdashButton = config->getString("", "dropdashButton", "Y");
		dropdashBounce = config->getBool("", "dropdashBounce", false);
		fallDropdash = config->getBool("", "fallDropdash", false);
		sonicCanDropdash = config->getBool("", "sonicCanDropdash", true);
		shadowCanDropdash = config->getBool("", "shadowCanDropdash", true);
		amyCanDropdash = config->getBool("", "amyCanDropdash", true);
		metalCanDropdash = config->getBool("", "metalCanDropdash", true);
		delete config;

		if (dropdashButton == "Y") {
			dropdashButtons = Buttons_Y;
			bounceButtons = Buttons_Action;
		}
		else if (dropdashButton == "B and X") {
			dropdashButtons = Buttons_Action;
			bounceButtons = Buttons_None;
		}
		else if (dropdashButton == "B") {
			dropdashButtons = Buttons_B;
			bounceButtons = Buttons_X;
		}
		else if (dropdashButton == "X") {
			dropdashButtons = Buttons_X;
			bounceButtons = Buttons_B;
		}

		if (GetModuleHandle(L"SA2-Sonic-Tricks") != nullptr)
		{
			const Mod* sonicNewTricksMod = helperFunctions.Mods->find_by_dll(GetModuleHandle(L"SA2-Sonic-Tricks"));

			const IniFile* config = new IniFile(std::string(sonicNewTricksMod->Folder) + "\\config.ini");
			superBounce = config->getBool("General", "superBounce", true);
			shBounce = config->getBool("Shadow", "shBounce", true);
			bounceButtons = ButtonsList[config->getInt("ActionRemap", "BounceButton", 1)];
			SomersaultButton = ButtonsList[config->getInt("ActionRemap", "SomersaultButton", 0)];
			BlackShieldButton = ButtonsList[config->getInt("ActionRemap", "shieldButton", 2)];
			delete config;

			if (bounceButtons == Buttons_Action)
			{
				if (dropdashButtons == Buttons_B) bounceButtons = Buttons_X;
				else if (dropdashButtons == Buttons_X) bounceButtons = Buttons_B;
				else if (dropdashButtons == Buttons_Action) bounceButtons = Buttons_None;
			}

			sonicNewTricks = true;
		}
		
		Sonic_Somersault_h.Hook(Sonic_Somersault_r);
		Sonic_CheckBounceAttack_h.Hook(Sonic_CheckBounceAttack_r);
		Sonic_CheckBounceAttack2_h.Hook(Sonic_CheckBounceAttack2_r);
		Sonic_ChecksForDamage_h.Hook(Sonic_ChecksForDamage_r);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer }; // This is needed for the Mod Loader to recognize the DLL.
}