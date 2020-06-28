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


	// 下からn桁目の数値をuint8_tで返す (nが1以外で0の時は10)
	auto getDigitF = [&](float arg, uint16_t n)noexcept -> uint8_t {
		if (n == 0) return arg;
		float ret = arg;
		for (size_t i = 0; i < n - 1; i++) {
			ret /= 10;
		}
		ret = static_cast<int>(ret) % 10;
		if (n == 1)return ret;
		if (ret == 0)return 10;
		return ret;
	};

	float A = atsPlugin->getStatus().status.A;

	// TIMS代替
	p[21] = st.V < 0 ? 2 : st.V > 0 ? 1 : 0;
	p[22] = getDigitF(st.V, 4);
	p[23] = getDigitF(st.V, 3);
	p[24] = getDigitF(st.V, 2);
	p[25] = getDigitF(st.V, 1);
	p[26] = getDigitF(st.V, 0);
	p[30] = ret.B;
	p[37] = st.T / 3600000;
	p[38] = st.T / 60000;
	p[39] = st.T / 1000;
	p[41] = A < 0 ? 2 : A > 0 ? 1 : 0;
	p[42] = A < 0 ? 2 : A > 0 ? 1 : 0;
	p[43] = A < 0 ? 2 : A > 0 ? 1 : 0;
	p[44] = A < 0 ? 2 : A > 0 ? 1 : 0;
	p[45] = getDigitF(st.V, 3);	// 3
	p[46] = getDigitF(st.V, 2);	// 2
	p[47] = getDigitF(st.V, 1);	// 1
	p[50] = getDigitF(st.V, 0);	// needle
	p[100] = getDigitF(st.V, 3);
	p[101] = getDigitF(st.V, 2);
	p[102] = getDigitF(st.V, 1);
	p[103] = 0;
	p[104] = 0;
	p[105] = A < 0 ? 2 : A > 0 ? 1 : 0;
	p[106] = A < 0 ? 2 : A > 0 ? 1 : 0;
	p[107] = A < 0 ? 2 : A > 0 ? 1 : 0;
	p[108] = A < 0 ? 2 : A > 0 ? 1 : 0;
	p[109] = A < 0 ? 2 : A > 0 ? 1 : 0;
	p[122] = atsPlugin->getDoor() ? 0 : st.BC < 200 ? 1 : 0;
	p[122] = atsPlugin->getDoor() ? 0 : st.BC < 200 ? 1 : 0;

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