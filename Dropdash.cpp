#include "pch.h"
#include "Dropdash.h"

float dropdashSpeed = 0.0f;
int dropdashTimer = 0;
bool canChargeDropDash = false;

std::string dropdashButton;

void Sonic_ChecksForDamage_r(EntityData1* ed1, EntityData2* ed2, CharObj2Base* co2, SonicCharObj2* sco2)
{
	if (ed1->Action == 6) // Jump
	{
		Buttons buttons;
		if (dropdashButton == "B")             buttons = Buttons_B;
		else if (dropdashButton == "X")        buttons = Buttons_X;
		else if (dropdashButton == "Y")        buttons = Buttons_Y;
		
		if (canChargeDropDash == false && (buttons & Controllers[co2->PlayerNum].press))
			canChargeDropDash = true;

		if (canChargeDropDash && (buttons & Controllers[co2->PlayerNum].on))
			dropdashTimer++;
		else {
			canChargeDropDash = false;
			dropdashTimer = 0;
		}

		if (dropdashTimer == 15)
		{
			ed1->Status |= Status_Ball;
			co2->AnimInfo.Next = 12;
		}

		if (dropdashTimer >= 15)
			QueueSound_DualEntity(8206, (ObjectMaster*)ed1, 1, 0, 2);
	}
	else
	{
		canChargeDropDash = false;
		dropdashTimer = 0;
	}

	Sonic_ChecksForDamage_h.Original(ed1, ed2, co2, sco2);

	if (dropdashTimer > 15 && (ed1->Action == 1 || ed1->Action == 0)) // Walk or Idle
	{
		ed1->Status |= (Status_Attack | Status_Ball);
		dropdashSpeed = 6 + min(dropdashTimer / 20, 3);
		
		// Release Spindash code
		ed1->Action = 4;
		co2->Speed.x = dropdashSpeed;
		co2->AnimInfo.Next = 12;
		PlaySoundProbably(8203, 0, 0, 0);
		sco2->SomersaultTime = 0;
	}
}

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");
		dropdashButton = config->getString("", "dropdashButton", "Y");
		delete config;

		Sonic_ChecksForDamage_h.Hook(Sonic_ChecksForDamage_r);
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer }; // This is needed for the Mod Loader to recognize the DLL.
}