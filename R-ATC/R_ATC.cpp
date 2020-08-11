#include "pch.h"
#include "R_ATC.h"

float R_ATC::calclateSpeed(VehicleState state) {
	uint16_t ret = UINT16_MAX;
	float dec = 18;	// �����萔
	float L = this->stop;	// ��~���E������
	if (state.status.Z > L) return 0;
	ret = std::sqrt(std::abs(L - state.status.Z) * dec);
	if (ret > 120) ret = 120;
	return ret;
}

ControlInfo R_ATC::calclateBrake(VehicleState state, float speed, uint8_t param) {
	ControlInfo ret = ControlInfo();	// �u���[�L�w�ߒl
	float psec = 2.5;	// P�ڋߕ\������[s]

	// ATC�\�����Z�b�g
	if (atsPlugin->getStatusPrevious().panel[static_cast<uint32_t>(panelIndex::ATC_02)] == true) ret.Panel[static_cast<uint32_t>(panelIndex::ATC_02)] = false;
	if (atsPlugin->getStatusPrevious().panel[static_cast<uint32_t>(panelIndex::ATC_01)] != 0) ret.Panel[static_cast<uint32_t>(panelIndex::ATC_01)] = false;
	if (atsPlugin->getStatusPrevious().sound[static_cast<size_t>(soundIndex::ATC)] == SoundInfo::PlayOnce) ret.Sound[static_cast<size_t>(soundIndex::ATC)] = SoundInfo::PlayContinue;

	// ����
	ret.Panel[static_cast<size_t>(panelIndex::Limit_1)] = static_cast<uint32_t>(speed);
	ret.Panel[static_cast<size_t>(panelIndex::Limit_5)] = static_cast<uint32_t>((speed + 5) / 5) * 5;

	// P�ڋ�
	if (state.status.V + state.status.A * psec < speed) {
		ret.Panel[static_cast<size_t>(panelIndex::Pattern)] = false;
		ret.Handle["B"] = 0;
		return ret;
	}
	if (atsPlugin->getStatusPrevious().panel[static_cast<size_t>(panelIndex::Pattern)] == SoundInfo::PlayContinue) {
		ret.Sound[static_cast<size_t>(panelIndex::Pattern)] = SoundInfo::PlayOnce;
	}
	ret.Panel[static_cast<size_t>(panelIndex::Pattern)] = true;

	// P��ڐG
	if (state.status.V < speed) {
		ret.Handle["B"] = 0;
		return ret;
	}

	// ATC�ݒ�
	if (state.status.V > speed + 10) param = 2;
	if (state.status.V == 0 && speed == 0) param = 1;

	// ATC��O
	switch (param) {
	default:
	case 0:
		break;
	case 1:	// ATC��p
		ret.Handle["B"] = atsPlugin->getSpec().J;
		ret.Panel[static_cast<uint32_t>(panelIndex::ATC_01)] = 1;
		if (atsPlugin->getStatusPrevious().sound[static_cast<size_t>(soundIndex::ATC)] == SoundInfo::PlayContinue) ret.Sound[static_cast<size_t>(soundIndex::ATC)] = SoundInfo::PlayOnce;
		return ret;
		break;
	case 2:	// ATC���
		ret.Handle["B"] = atsPlugin->getSpec().E;
		ret.Panel[static_cast<uint32_t>(panelIndex::ATC_01)] = 1;
		ret.Panel[static_cast<uint32_t>(panelIndex::ATC_02)] = true;
		if (atsPlugin->getStatusPrevious().sound[static_cast<size_t>(soundIndex::ATC)] == SoundInfo::PlayContinue) ret.Sound[static_cast<size_t>(soundIndex::ATC)] = SoundInfo::PlayOnce;
		return ret;
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
	if (state.status.V == 0) {
		ret.Handle["B"] = atsPlugin->getSpec().J;
		return ret;
	}

	// P�ڐG�EP���Z
	double bl = 4.00 / (atsPlugin->getSpec().E / 2);	// �u���[�L�v�Z�l
	// ����+4km/h�̎��ɔ��/2�̏o�͂ɂȂ�悤�Ɋe�i��P������
	for (size_t i = 0; i < atsPlugin->getSpec().E; i++) {
		if (state.status.V - bl * i < speed) {
			ret.Handle["B"] = static_cast<uint16_t>(i);
			return ret;
		}
		ret.Handle["B"]++;
	}

	return ret;	// �g��Ȃ����Ǖی�
}

uint16_t R_ATC::calclateStopLimit(VehicleState state) {
	for (size_t i = 0; i < this->unit; i++) {
		if (this->stop < state.status.Z + this->dis * i) {
			if (i == 0) return this->unit + 1;
			return i;
		}
	}
	return this->unit + 2;
}

R_ATC::R_ATC() {
}

R_ATC::~R_ATC() {
}

ControlInfo R_ATC::Elapse(VehicleState state) {
	ControlInfo ret = this->calclateBrake(state, this->calclateSpeed(state));

	// �M��
	//if (atsPlugin->getSignal() >= 50 && atsPlugin->getSignal() <= 60) {
	if (atsPlugin->getSignal() >= 200 && atsPlugin->getSignal() <= 219) {

		if (atsPlugin->getStatus().panel[static_cast<size_t>(panelIndex::Power)] == false) {
			ret.Sound[static_cast<size_t>(soundIndex::ATC)] = SoundInfo::PlayOnce;
		}
		else ret.Sound[static_cast<size_t>(soundIndex::ATC)] = SoundInfo::PlayContinue;

		// ATC�d��
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Power)] = true;

		// �u���[�L�o��
		if (ret.Handle["B"] < atsPlugin->getHandleManual().B) {
			ret.Handle.erase(ret.Handle.find("B"));
		}

		// 250[ms]�o�߂ōX�V
		const double stopLimit_d = this->stop;	// ��~���E�ʒu
		double stopLimit = stopLimit_d - state.status.Z >= 0 ? stopLimit_d - state.status.Z : 0;	// ��~���E�c����
		if ((atsPlugin->getStatus().status.T / 250 > atsPlugin->getStatusPrevious().status.T / 250)) {

			// ��~���E
			uint32_t temp = static_cast<uint32_t>(stopLimit / 1000) % 100;
			if (stopLimit / 100000 >= 1) {	// �ŏ�ʌ��̏ꍇ
				if (true) {	// 1�����[�h
					if (temp == 0) temp = 10;
				}
				else if (temp < 10) temp += 100;	// 2�����[�h
			}
			ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit1000)] = temp;

			if (temp == 0) {	// �ŏ�ʌ��̏ꍇ
				temp = static_cast<uint32_t>(stopLimit / 10) % 100;
				//if (temp == 0) temp = 100;
			}
			else {	// ���ʌ��̏ꍇ
				temp = static_cast<uint32_t>(stopLimit / 10) % 100;
				if (temp < 10) temp += 100;
				if (temp == 100) temp += 10;
			}
			ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit10)] = temp;

			temp = static_cast<uint32_t>(stopLimit * 10) % 100;
			ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit01)] = temp == 0 ? 100 : temp;

			temp = stopLimit;
			ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit)] = temp;

			// �J�ʏ��
			ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Train)] = true;
			uint16_t stopLimit_ui = 0;
			if (stopLimit >= 0) stopLimit_ui = this->calclateStopLimit(state);	// ��~���E
			ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Close)] = stopLimit_ui;

			// ����
			uint8_t index = static_cast<uint8_t>(R_ATC::panelIndex::Clossing_1);
			for (size_t i = static_cast<size_t>(R_ATC::panelIndex::Clossing_1); i <= static_cast<size_t>(R_ATC::panelIndex::Clossing_10); i++) {
				ret.Panel[i] = 0;
			}
			for (size_t i = 0; i < this->crossing.size(); i++) {
				if (crossing[i] < state.status.Z) continue;
				if (index >= static_cast<uint8_t>(R_ATC::panelIndex::Clossing_10) + 1) break;
				if (crossing[i] < state.status.Z + 1000) {
					ret.Panel[index] = static_cast<uint32_t>((crossing[i] - state.status.Z) / this->dis);
					index++;
				}
				else if (crossing[i] > state.status.Z + 1000) break;
			}

		}

		// �]���h�~
		if (atsPlugin->getDoor()) {
			ret.Handle["B"] = atsPlugin->getSpec().E / 2;	//�]���h�~����
			ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = 1;	// �]���h�~�_��
		} else if (atsPlugin->getHandleManual().B <= atsPlugin->getSpec().E / 2) {
			if (std::rand() % 2) ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = 0;	// �]���h�~�œ�
		}

	}
	else {	// �d����
		ret.Handle.erase(ret.Handle.find("B"));

		if (atsPlugin->getStatus().panel[static_cast<size_t>(panelIndex::Power)] == true) {
			ret.Sound[static_cast<size_t>(soundIndex::ATC)] = SoundInfo::PlayOnce;
			ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = false;
		}
		else ret.Sound[static_cast<size_t>(soundIndex::ATC)] = SoundInfo::PlayContinue;

		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Power)] = false;
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Limit_1)] = false;
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Limit_5)] = false;
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Pattern)] = false;
		ret.Panel[static_cast<uint32_t>(panelIndex::ATC_02)] = false;
		ret.Panel[static_cast<uint32_t>(panelIndex::ATC_01)] = 2;
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit1000)] = false;
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit10)] = false;
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit01)] = false;
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit)] = false;
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Train)] = false;
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Close)] = false;
		for (size_t i = static_cast<size_t>(R_ATC::panelIndex::Clossing_1); i <= static_cast<size_t>(R_ATC::panelIndex::Clossing_10); i++) {
			ret.Panel[i] = false;
		}
	}

	return ret;
}

double R_ATC::setCrossing(double distance) {
	this->crossing.push_back(distance);	// �v�f�ǉ�
	std::sort(this->crossing.begin(), this->crossing.end());	// �����\�[�g
	return distance;
}

double R_ATC::setStop(double distance) {
	this->stop = distance;
	return distance;
}

// �p�^�[��������
// �������x�^��������n�b��Ƀp�^�[����G: P�ڋ�
// �����}0: B0
// ����+05: EB / 2
// ����+10: Bmax
// ����+15: EB
