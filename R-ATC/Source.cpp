#include "pch.h"
#include "Header.h"


SpecPlus& SpecPlus::operator=(const SpecPlus& next) {
	if (this != &next) {
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

SpecPlus& SpecPlus::operator=(const Spec& next) {
	if (this != &next) {
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

	if (p == nullptr) this->panel = {};
	else for (size_t i = 0; i < 256; i++) {
		this->panel[i] = p[i];
	}

	if(s == nullptr) this->sound = {};
	else for (size_t i = 0; i < 256; i++) {
		this->sound[i] = s[i];
	}
}

int ControlInfo::setControl(Hand* h, int* p, int* s) {
	try {
		// ハンドル代入
		if (this->Handle.count(static_cast<uint8_t>(handleIndex::B))) h->B = this->Handle.find(static_cast<uint8_t>(handleIndex::B))->second;
		if (this->Handle.count(static_cast<uint8_t>(handleIndex::P))) h->P = this->Handle.find(static_cast<uint8_t>(handleIndex::P))->second;
		if (this->Handle.count(static_cast<uint8_t>(handleIndex::R))) h->R = this->Handle.find(static_cast<uint8_t>(handleIndex::R))->second;
		if (this->Handle.count(static_cast<uint8_t>(handleIndex::C))) h->C = this->Handle.find(static_cast<uint8_t>(handleIndex::C))->second;
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
