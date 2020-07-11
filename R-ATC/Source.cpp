#include "pch.h"
#include "Header.h"


SpecPlus& SpecPlus::operator=(const SpecPlus& next) {
	if (this == &next) {
		this->E = next.B + 1;
		Spec::operator=(next);
	}
	return *this;
}

SpecPlus::SpecPlus() {
	int B = 0;	//ブレーキ段数
	int P = 0;	//ノッチ段数
	int A = 0;	//ATS確認段数
	int J = 0;	//常用最大段数
	int C = 0;	//編成車両数
	int E = 0;	// 非常ブレーキ
}

SpecPlus::SpecPlus(const Spec& old) {
	int B = old.B;	//ブレーキ段数
	int P = old.P;	//ノッチ段数
	int A = old.A;	//ATS確認段数
	int J = old.J;	//常用最大段数
	int C = old.C;	//編成車両数
	int E = old.J + 1;	// 非常ブレーキ
}

SpecPlus& SpecPlus::operator=(const Spec& next) {
	if (this == &next) {
		this->E = next.B + 1;
		Spec::operator=(next);
	}
	return *this;
}


StatePlus& StatePlus::operator=(const State& next) {
	if (this != &next) {
		this->dZ = next.Z - this->Z;
		this->dV = next.V - this->V;
		this->dT = next.T - this->T;
		this->dBC = next.BC - this->BC;
		this->dMR = next.MR - this->MR;
		this->dER = next.ER - this->ER;
		this->dBP = next.BP - this->BP;
		this->dSAP = next.SAP - this->SAP;
		this->dI = next.I - this->I;
		this->A = dV / dT;
		this->fps = 1.00 * 1000 / dT;
		State::operator=(next);
	}
	return *this;
}

StatePlus& StatePlus::operator=(const StatePlus& next) {
	if (this != &next) {
		this->dZ = next.Z - this->Z;
		this->dV = next.V - this->V;
		this->dT = next.T - this->T;
		this->dBC = next.BC - this->BC;
		this->dMR = next.MR - this->MR;
		this->dER = next.ER - this->ER;
		this->dBP = next.BP - this->BP;
		this->dSAP = next.SAP - this->SAP;
		this->dI = next.I - this->I;
		this->A = next.V - this->V;
		this->fps = 1.00 * 1000 / dT;
		State::operator=(next);
	}
	return *this;
}

StatePlus::StatePlus() {
	double Z = 0;	//列車位置[m]
	float V = 0;	//列車速度[km/h]
	int T = 0;		//0時からの経過時間[ms]
	float BC = 0;	//BC圧力[kPa]
	float MR = 0;	//MR圧力[kPa]
	float ER = 0;	//ER圧力[kPa]
	float BP = 0;	//BP圧力[kPa]
	float SAP = 0;	//SAP圧力[kPa]
	float I = 0;	//電流[A]
	double dZ = 0;	// 移動距離[m]
	float dV = 0;	//速度変化[km/h]
	int dT = 0;	// 経過時間[ms]
	float dBC = 0;	// BC圧力変化[kPa]
	float dMR = 0;	// MR圧力変化[kPa]
	float dER = 0;	// ER圧力変化[kPa]
	float dBP = 0;	// BP圧力変化[kPa]
	float dSAP = 0;	// SAP圧力変化[kPa]
	float dI = 0;	// 電流変化[A]
	float A = 0;	// 加速度[km/h/s]
	float fps = 0;	// フレーム数[frame/s]
}

VehicleState::VehicleState() {
	StatePlus status = StatePlus();	// 車両状態
	std::array<uint32_t, 256> panel = {};	// panel制御値
	std::array<int32_t, 256> sound = {};	// sound制御値
}

VehicleState::VehicleState(State st, int* p, int* s) {
	this->status = st;

	if (p == nullptr || s == nullptr) {
		this->panel = {};
		this->sound = {};
		return;
	}
	else for (size_t i = 0; i < 256; i++) {
		this->panel[i] = p[i];
		this->sound[i] = s[i];
	}
}

int ControlInfo::setControl(Hand* h, int* p, int* s) {
	try {
		// ハンドル代入
		if (this->Handle.count("B")) {
			if (h->B < this->Handle.find("B")->second) {
				h->B = this->Handle.find("B")->second;
			}
		}
		if (this->Handle.count("P")) {
			if (h->P < this->Handle.find("P")->second) {
				h->P = this->Handle.find("P")->second;
			}
		}
		if (this->Handle.count("R")) h->R = this->Handle.find("R")->second;
		if (this->Handle.count("C")) h->C = this->Handle.find("C")->second;
	}
	catch (const std::exception& e) {

	}

	try {
		// パネル代入
		for (auto iter = this->Panel.begin(); iter != this->Panel.end(); ++iter) {
			p[iter->first] = iter->second;
		}
	}
	catch (const std::out_of_range& e) {

	}

	try {
		// サウンド代入
		for (auto iter = this->Sound.begin(); iter != this->Sound.end(); ++iter) {
			s[iter->first] = iter->second;
		}
	}
	catch (const std::exception& e) {/*
		std::wstring emsg = std::wstring(e.what());
		DialogBoxW(NULL, emsg.c_str(), nullptr, 0);
	*/}

	return 0;
}

// 下からn桁目の数値をuint8_tで返す (nが1以外で0の時は10)
uint8_t getDigitF (float arg, uint16_t n)noexcept {
	// n桁目を抜き出すラムダ式
	auto gd = [](float arg, uint16_t n) -> uint8_t {
		float ret = std::fabs(arg);
		for (size_t i = 0; i < n - 1; i++) {
			ret /= 10;
		}
		return static_cast<uint8_t>(std::fmod(ret, 10));
	};

	if (n == 0) return static_cast<uint8_t>(arg);	// n==0の時はそのまま
	float ret = gd(arg, n);
	if (n == 1)return static_cast<uint8_t>(ret);	// n==1の時は1桁目そのまま
	if (ret != 0) return static_cast<uint8_t>(ret);	// ret!=0の時はそのまま
	if (gd(arg, n + 1) == 0) return 10;	// ret==0の時は上の桁が0なら10
	return static_cast<uint8_t>(ret);
};
