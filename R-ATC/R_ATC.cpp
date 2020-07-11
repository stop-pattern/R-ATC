#include "pch.h"
#include "R_ATC.h"

float R_ATC::calclateSpeed(VehicleState state) {
	uint16_t ret = UINT16_MAX;
	/*
	for (size_t i = 0; i < static_cast<size_t>(limitIndex::size); i++) {
		if (ret <= this->limit[i]) ret = this->limit[i];
	}
	return ret;
	*/
	float dec = 18;	// �����萔
	float L = 225400;	// ��~���E������
	if (state.status.Z > L) return 0;
	ret = std::sqrt(std::abs(L - state.status.Z) * dec);
	if (ret > 120) ret = 120;
	return ret;
}

uint16_t R_ATC::calclateBrake(VehicleState state, float speed, uint8_t param) {
	uint16_t ret = 0;	// �u���[�L�w�ߒl
	float psec = 2.5;	// P�ڋߕ\������[s]

	// P�ڋ�
	if (state.status.V + state.status.A * psec < speed) {
		state.panel[static_cast<size_t>(panelIndex::Pattern)] = false;
		return 0;
	}
	state.panel[static_cast<size_t>(panelIndex::Pattern)] = true;

	// P��ڐG
	if (state.status.V < speed) return 0;

	// ATC�ݒ�
	if (state.status.V > speed + 10) param = 2;
	if (state.status.V == 0 && speed == 0) param = 1;

	// ATC��O
	switch (param) {
	default:
	case 0:
		break;
	case 1:	// ATC��p
		return atsPlugin->getSpec().J;
		break;
	case 2:	// ATC���
		return atsPlugin->getSpec().E;
		break;
	}

	/*
	// 10km/h�ȉ��̎��̗�O
	if (state.status.V < 10) {	// ����������ƍl���Ă���������
		ret = atsPlugin->getHandleControl().B;	// �O��o�͒l
		if(state.status.V < 0.25) if (state.status.A == 0) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 0.5) if (ret > atsPlugin->getSpec().J / 2) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 1) if (std::rand() % 4) ret--;
		else if (state.status.V < 5) if (std::rand() % 8) ret--;
		else if (std::rand() % 100) ret--;
		if (ret < 0) ret = 0;
	}
	*/
	if (state.status.V == 0) return atsPlugin->getSpec().J;

	// P�ڐG�EP���Z
	double bl = 4.00 / (atsPlugin->getSpec().E / 2);	// �u���[�L�v�Z�l
	// ����+4km/h�̎��ɔ��/2�̏o�͂ɂȂ�悤�Ɋe�i��P������
	for (size_t i = 0; i < atsPlugin->getSpec().E; i++) {
		if (state.status.V - bl * i < speed) {
			return static_cast<uint16_t>(i);
		}
		ret++;
	}

	return ret;	// �g��Ȃ����Ǖی�
}

uint16_t R_ATC::calclateStopLimit(VehicleState state) {
	const uint16_t unit = 100;	// ��~���E�\���������ɋ�؂邩[��]
	const uint8_t dis = 10;	// 1��؂蓖����̋���[m]
	double limit;	// ��~���E����
	//limit = dis * (unit - state.status.V);	// �f�o�b�N
	limit = this->stop;	// �f�o�b�N
	for (size_t i = 0; i < unit; i++) {
		if (limit < state.status.Z + dis * i) {
			if (i == 0) return unit;
			return i;
		}
	}
	return 0;
}

R_ATC::R_ATC() {
}

R_ATC::~R_ATC() {
}

ControlInfo R_ATC::Elapse(VehicleState state) {
	ControlInfo ret = ControlInfo();

	// ��~���E
	const double stopLimit_d = std::rand();
	const double stopLimit = stopLimit_d - state.status.Z;
	uint16_t stopLimit_ui = this->calclateStopLimit(state);
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Close)] = stopLimit_ui;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit)] = stopLimit;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit01)] = static_cast<uint32_t>(stopLimit * 10) % 100;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit10)] = static_cast<uint32_t>(stopLimit / 10) % 100;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit1000)] = static_cast<uint32_t>(stopLimit / 1000) % 1000;

	// �]���h�~
	if (atsPlugin->getDoor() && state.status.A) {
		ret.Handle["B"] = atsPlugin->getSpec().E / 2;	//�]���h�~����
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = true;	// �]���h�~�_��
	} else if(atsPlugin->getHandleManual().B <= atsPlugin->getSpec().E / 2) {
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = false;	// �]���h�~�œ�
	}

	ret.Handle["B"] = this->calclateBrake(state, this->calclateSpeed(state));
	
	return ret;
}

// �p�^�[��������
// �������x�^��������n�b��Ƀp�^�[����G: P�ڋ�
// �����}0: B0
// ����+05: EB / 2
// ����+10: Bmax
// ����+15: EB
