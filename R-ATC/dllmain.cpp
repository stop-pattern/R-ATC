// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}


DE void SC Load(void) {

}

DE void SC Dispose(void) {

}

DE int SC GetPluginVersion(void) {

}

DE void SC SetVehicleSpec(Spec) {

}

DE void SC Initialize(int) {

}

DE Hand SC Elapse(State, int*, int*) {

}

DE void SC SetPower(int) {

}

DE void SC SetBrake(int) {

}

DE void SC SetReverser(int) {

}

DE void SC KeyDown(int) {

}

DE void SC KeyUp(int) {

}

DE void SC HornBlow(int) {

}

DE void SC DoorOpen(void) {

}

DE void SC DoorClose(void) {

}

DE void SC SetSignal(int) {

}

DE void SC SetBeaconData(Beacon) {

}