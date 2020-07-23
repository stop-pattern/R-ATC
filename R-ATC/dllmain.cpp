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
		break;
	}
	return TRUE;
}


DE void SC Load(void) {

}

DE void SC Dispose(void) {
	delete atsPlugin;
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
	ControlInfo control = atcR->Elapse(status);

	if (control.Handle["P"] < atsPlugin->getHandleManual().P) {
		control.Handle["P"] = atsPlugin->getHandleManual().P;
	}
	if (control.Handle["B"] < atsPlugin->getHandleManual().B) {
		control.Handle["B"] = atsPlugin->getHandleManual().B;
	}

	float A = atsPlugin->getStatus().status.A;
	int8_t u = 0;
	if (control.Handle["B"] == 0) {
		if (control.Handle["P"] == 0) u = 0;
		else u = 1;
	} else u = 2;

	// TIMS代替
	p[21] = st.V < 0 ? 2 : st.V > 0 ? 1 : 0;
	p[22] = getDigitF(st.V, 4);
	p[23] = getDigitF(st.V, 3);
	p[24] = getDigitF(st.V, 2);
	p[25] = getDigitF(st.V, 1);
	p[26] = std::fabs(st.V);
	p[30] = control.Handle["B"];
	p[37] = st.T / 3600000;
	p[38] = st.T / 60000;
	p[39] = st.T / 1000;
	if (std::rand() % 2) p[41] = u;
	if (std::rand() % 2) p[42] = u;
	if (std::rand() % 2) p[43] = u;
	if (std::rand() % 2) p[44] = u;
	p[45] = getDigitF(st.V, 3);
	p[46] = getDigitF(st.V, 2);
	p[47] = getDigitF(st.V, 1);
	p[50] = std::fabs(st.V);
	p[100] = getDigitF(st.V, 3);
	p[101] = getDigitF(st.V, 2);
	p[102] = getDigitF(st.V, 1);
	p[103] = 0;
	p[104] = 0;
	if (std::rand() % 2) p[105] = u;
	if (std::rand() % 2) p[106] = u;
	if (std::rand() % 2) p[107] = u;
	if (std::rand() % 2) p[108] = u;
	p[109] = atsPlugin->getStatus().status.dZ < 0 ? 2 : atsPlugin->getStatus().status.dZ > 0 ? 1 : 0;
	p[122] = atsPlugin->getDoor() ? 0 : st.BC < 200 ? 1 : 0;
	p[123] = st.BC / 20 > 00 ? st.BC / 20 < 10 ? st.BC / 20 - 00 : 10 : 10;
	p[124] = st.BC / 20 > 10 ? st.BC / 20 < 20 ? st.BC / 20 - 10 : 10 : 10;
	p[125] = st.BC / 20 > 20 ? st.BC / 20 < 30 ? st.BC / 20 - 20 : 10 : 10;
	p[126] = st.BC / 20 > 30 ? st.BC / 20 < 40 ? st.BC / 20 - 30 : 10 : 10;
	p[127] = (st.MR-750) / 5 > 00 ? (st.MR - 750) / 5 < 10 ? (st.MR - 750) / 20 - 00 : 10 : 10;
	p[128] = (st.MR - 750) / 5 > 10 ? (st.MR - 750) / 5 < 20 ? (st.MR - 750) / 20 - 10 : 10 : 10;
	p[129] = (st.MR - 750) / 5 > 20 ? (st.MR - 750) / 5 < 30 ? (st.MR - 750) / 20 - 20 : 10 : 10;

	Hand ret = atsPlugin->Elapse(status);
	control.setControl(&ret, p, s);

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
	switch (b.Num) {
	case 210:
		atcR->setStop(b.Z + atsPlugin->getStatus().status.Z);	// D-ATC互換機能
		break;
	default:
		break;
	}
}