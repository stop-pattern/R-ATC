#include "pch.h"
#include "R_ATC.h"

uint16_t R_ATC::calclateSpeed(VehicleState state) {
	uint16_t ret = UINT16_MAX;
	for (size_t i = 0; i < static_cast<size_t>(limitIndex::size); i++) {
		if (ret <= this->limit[i]) ret = this->limit[i];
	}
	return ret;
}

uint16_t R_ATC::calclateBrake(VehicleState state, uint16_t speed, uint8_t param) {
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

	// 10km/h�ȉ��̎��̗�O
	if (state.status.V < 10) {	// ����������ƍl���Ă���������
		ret = atsPlugin->getHandleControl().B;	// �O��o�͒l
		if(state.status.V < 0.25) if (state.status.A == 0) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 1) if (ret > atsPlugin->getSpec().J / 2) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 2.5) if (std::rand() % 4) ret--;
		else if (state.status.V < 5) if (std::rand() % 8) ret--;
		else if (std::rand() % 16) ret--;
	}

	// P�ڐG�EP���Z
	double bl = 5.00 / (atsPlugin->getSpec().E / 2);	// �u���[�L�v�Z�l
	// ����+5km/h�̎��ɔ��/2�̏o�͂ɂȂ�悤�Ɋe�i��P������
	for (size_t i = 0; i < atsPlugin->getSpec().E; i++) {
		if (state.status.V - bl * i < speed) {
			return static_cast<uint16_t>(i);
		}
		ret++;
	}

	return ret;	// �g��Ȃ����Ǖی�
}

R_ATC::R_ATC() {
}

R_ATC::~R_ATC() {
}

Hand R_ATC::Elapse(VehicleState state) {
	Hand ret = Hand();

	// �]���h�~
	if (atsPlugin->getDoor() && state.status.A) {
		ret.B = atsPlugin->getSpec().E / 2;	//�]���h�~����
		state.panel[static_cast<size_t>(panelIndex::Rolling)] = true;	// �]���h�~�_��
	} else if(atsPlugin->getHandleManual().B <= atsPlugin->getSpec().E / 2) {
		state.panel[static_cast<size_t>(panelIndex::Rolling)] = false;	// �]���h�~�œ�
	}

	ret.B = this->calclateBrake(state, 0/* �������x */);
	
	return ret;
}

// �p�^�[��������
// �������x�^��������n�b��Ƀp�^�[����G: P�ڋ�
// �����}0: B0
// ����+05: EB / 2
// ����+10: Bmax
// ����+15: EB
