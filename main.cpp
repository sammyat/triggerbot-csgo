#include <thread>
#include <iostream>
#include "memory.h"

namespace offset
{
	//client offsets
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DFEF0C;
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDE997C;
	constexpr ::std::ptrdiff_t dwForceAttack = 0x322CD48;

	//entity offsets
	constexpr ::std::ptrdiff_t m_iHealth = 0x100;
	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;
	constexpr ::std::ptrdiff_t m_iCrosshairId = 0x11838;
}

//Entry point
int main()
{
	//gets the client address and prints it to the console
	const auto memory = Memory{ "csgo.exe" };
	const auto client = memory.GetModuleAddress("client.dll");
	
	std::cout << std::hex << "client.dll address -> 0x" << client << std::dec << std::endl;

	//triggerbot loop
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		//if the shift key is pressed, it will do the rest
		if (!GetAsyncKeyState(VK_SHIFT))
			continue;

		const auto& localPlayer = memory.Read<std::uintptr_t>(client + offset::dwLocalPlayer);
		const auto& localHealth = memory.Read<std::int32_t>(localPlayer + offset::m_iHealth);

		//skip if dead

		if (!localHealth)
			continue;

		const auto& crosshairId = memory.Read<std::int32_t>(localPlayer + offset::m_iCrosshairId);

		//skip if looking at a wall | explanation: player is only from 1 to 64, everything above is an object not a player
		if (!crosshairId || crosshairId > 64)
			continue;

		const auto& player = memory.Read<std::uintptr_t>(client + offset::dwEntityList + (crosshairId - 1) * 0x10);

		//if the player is alive
		if (!memory.Read<std::int32_t>(player + offset::m_iHealth))
			continue;

		//skips loop if the player is on your team
		if (memory.Read<std::int32_t>(player + offset::m_iTeamNum) ==
			memory.Read<std::int32_t>(localPlayer + offset::m_iTeamNum))
			continue;

		// shoots
		memory.Write<std::uintptr_t>(client + offset::dwForceAttack, 6);
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
		memory.Write<std::uintptr_t>(client + offset::dwForceAttack, 4);
	}
	//terminates the program
	return 0;
}