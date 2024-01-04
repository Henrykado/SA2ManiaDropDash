#pragma once

#include "pch.h"

/*static const void* const QueueSound_DualEntityPtr = (void*)0x4374D0;
static inline char QueueSound_DualEntity(int a1, ObjectMaster* a2, char a3, char a4, __int16 a5)
{
	char result;
	__asm
	{
		push[a5]
		push[a4]
		push[a3]
		push[a2]
		mov edi, [a1]
		call QueueSound_DualEntityPtr
		mov result, al
		add esp, 16
	}
	return result;
}*/

const Buttons Buttons_None = (Buttons)0;
const Buttons Buttons_Action = (Buttons)(Buttons_B | Buttons_X);

static const Buttons ButtonsList[]
{
	Buttons_B,
	Buttons_X,
	Buttons_Y,
	Buttons_Action,
};

UsercallFunc(int, Sonic_Somersault_h, (SonicCharObj2* a1, EntityData1* a2, CharObj2Base* a3), (a1, a2, a3), 0x723880, rEAX, rEAX, stack4, stack4);

UsercallFunc(int, Sonic_CheckBounceAttack_h, (SonicCharObj2* a1, EntityData1* a2, int a3), (a1, a2, a3), 0x725270, rEAX, rEAX, rECX, stack4);
UsercallFunc(int, Sonic_CheckBounceAttack2_h, (SonicCharObj2* a1, EntityData1* a2), (a1, a2), 0x7252D0, rEAX, rEAX, rECX);

FunctionPointer(void, Sonic_ChecksForDamage_p, (EntityData1* a1, EntityData2* a2, CharObj2Base* a3, SonicCharObj2* a5), 0x719920);
FunctionHook<void, EntityData1*, EntityData2*, CharObj2Base*, SonicCharObj2*> Sonic_ChecksForDamage_h(Sonic_ChecksForDamage_p);

char (*QueueSound_DualEntity)(int a1, ObjectMaster* a2, char a3, char a4, __int16 a5) = GenerateUsercallWrapper<char (*)(int a1, ObjectMaster* a2, char a3, char a4, __int16 a5)>(rAL, 0x4374D0, rEDI, stack4, stack1, stack1, stack2);