// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include "Plugin.h"
#include "R_ATC.h"

Plugin* atsPlugin = new Plugin();	// 現在状態保存
R_ATC* atcR = new R_ATC();	// R-ATC

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		atsPlugin->Attach(hModule);
		break;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		delete atsPlugin;
		break;
	}
	return TRUE;
}


DE void SC Load(void) {

}

DE void SC Dispose(void) {

}

DE int SC GetPluginVersion(void) {
	return PI_VERSION;
}

DE void SC SetVehicleSpec(Spec sp) {
	atsPlugin->SetVehicleSpec(sp);
}

DE void SC Initialize(int) {

}

DE Hand SC Elapse(State st, int* p, int* s) {
	VehicleState status = VehicleState(st, s, p);
	Hand manual = atsPlugin->Elapse(status);
	Hand ret = manual;

	/* ----- todo: add optional code here ----- */
	// example:
	if (atsPlugin->getDoor()) {
		ret.P = 0;
	}
	if (std::abs(atsPlugin->getStatus().status.A) >= 10) {
		ret.B = atsPlugin->getSpec().E;
	}
	// R-ATC example:
	atcR->Elapse(status);

	return atsPlugin->Elapse(ret);
}

DE void SC SetPower(int p) {
	atsPlugin->SetPower(p);
}

DE void SC SetBrake(int b) {
	atsPlugin->SetBrake(b);
}

DE void SC SetReverser(int r) {
	atsPlugin->SetReverser(r);
}

DE void SC KeyDown(int k) {
	atsPlugin->KeyDown(k);
}

DE void SC KeyUp(int k) {
	atsPlugin->KeyUp(k);
}

DE void SC HornBlow(int h) {

}

DE void SC DoorOpen(void) {
	atsPlugin->DoorOpen();
}

DE void SC DoorClose(void) {
	atsPlugin->DoorClose();
}

DE void SC SetSignal(int s) {
	atsPlugin->SetSignal(s);
}

DE void SC SetBeaconData(Beacon b) {

}