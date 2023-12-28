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

FunctionPointer(void, Sonic_ChecksForDamage_p, (EntityData1* a1, EntityData2* a2, CharObj2Base* a3, SonicCharObj2* a5), 0x719920);
FunctionHook<void, EntityData1*, EntityData2*, CharObj2Base*, SonicCharObj2*> Sonic_ChecksForDamage_h(Sonic_ChecksForDamage_p);

char (*QueueSound_DualEntity)(int a1, ObjectMaster* a2, char a3, char a4, __int16 a5) = GenerateUsercallWrapper<char (*)(int a1, ObjectMaster* a2, char a3, char a4, __int16 a5)>(rAL, 0x4374D0, rEDI, stack4, stack1, stack1, stack2);