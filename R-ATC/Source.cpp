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
	int B = 0;	//�u���[�L�i��
	int P = 0;	//�m�b�`�i��
	int A = 0;	//ATS�m�F�i��
	int J = 0;	//��p�ő�i��
	int C = 0;	//�Ґ��ԗ���
	int E = 0;	// ���u���[�L
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
	double Z = 0;	//��Ԉʒu[m]
	float V = 0;	//��ԑ��x[km/h]
	int T = 0;		//0������̌o�ߎ���[ms]
	float BC = 0;	//BC����[kPa]
	float MR = 0;	//MR����[kPa]
	float ER = 0;	//ER����[kPa]
	float BP = 0;	//BP����[kPa]
	float SAP = 0;	//SAP����[kPa]
	float I = 0;	//�d��[A]
	double dZ = 0;	// �ړ�����[m]
	float dV = 0;	//���x�ω�[km/h]
	int dT = 0;	// �o�ߎ���[ms]
	float dBC = 0;	// BC���͕ω�[kPa]
	float dMR = 0;	// MR���͕ω�[kPa]
	float dER = 0;	// ER���͕ω�[kPa]
	float dBP = 0;	// BP���͕ω�[kPa]
	float dSAP = 0;	// SAP���͕ω�[kPa]
	float dI = 0;	// �d���ω�[A]
	float A = 0;	// �����x[km/h/s]
	float fps = 0;	// �t���[����[frame/s]
}

VehicleState::VehicleState() {
	StatePlus status = StatePlus();	// �ԗ����
	std::array<uint32_t, 256> panel = {};	// panel����l
	std::array<int32_t, 256> sound = {};	// sound����l
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
		// �n���h�����
		if (this->Handle.count(static_cast<uint8_t>(handleIndex::B))) h->B = this->Handle.find(static_cast<uint8_t>(handleIndex::B))->second;
		if (this->Handle.count(static_cast<uint8_t>(handleIndex::P))) h->P = this->Handle.find(static_cast<uint8_t>(handleIndex::P))->second;
		if (this->Handle.count(static_cast<uint8_t>(handleIndex::R))) h->R = this->Handle.find(static_cast<uint8_t>(handleIndex::R))->second;
		if (this->Handle.count(static_cast<uint8_t>(handleIndex::C))) h->C = this->Handle.find(static_cast<uint8_t>(handleIndex::C))->second;
	}
	catch (const std::exception& e) {

	}

	try {
		// �p�l�����
		for (auto iter = this->Panel.begin(); iter != this->Panel.end(); ++iter) {
			p[iter->first] = iter->second;
		}
	}
	catch (const std::out_of_range& e) {

	}

	try {
		// �T�E���h���
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
