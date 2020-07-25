#include "pch.h"
#include "R_ATC.h"

float R_ATC::calclateSpeed(VehicleState state) {
	uint16_t ret = UINT16_MAX;
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

uint16_t R_ATC::calclateStopLimit(VehicleState state, double limit) {
	if (limit < 0) return this->unit + 1;
	for (size_t i = 0; i < this->unit; i++) {
		if (limit < this->dis * i) {
			if (i == 0) return this->unit + 1;
			return i;
		}
	}
	return this->unit + 2;
}

double R_ATC::getPreTrainPosition(VehicleState state) {
	double ret = 0.0;
	if (this->preTrain.size() <= 1) {
		if (this->preTrain.size() == 0) return ret;
		return this->preTrain[0].second;
	}
	if (this->preTrain[this->preTrain.size() - 1].first < std::chrono::milliseconds(state.status.T)) {
		return this->preTrain[this->preTrain.size() - 1].second;
	}
	for (size_t i = 0; i < this->preTrain.size(); i++) {
		if (i == this->preTrain.size() - 1) return this->preTrain[this->preTrain.size() - 1].second;
		if (std::chrono::milliseconds(state.status.T) < this->preTrain[i].first) {
			if (i == 0) return this->preTrain[i].second;
			double a = (preTrain[i + 1].second - preTrain[i].second) / (preTrain[i + 1].first.count() - preTrain[i].first.count());
			ret = a * (state.status.T - preTrain[i].first.count()) + preTrain[i].second;	// ���`��A
			// y = \frac{y_{1}-y_{0}}{x_{1}-x_{0}} (x-x_{0}) + y_{0}
			return ret;
		}
	}
	return ret;
}

R_ATC::R_ATC() {
	std::chrono::milliseconds temp = std::chrono::milliseconds(0);

	temp = std::chrono::hours(8) + std::chrono::minutes(53) + std::chrono::seconds(45);
	preTrain.push_back(std::make_pair(temp, 16625));

	temp = std::chrono::hours(8) + std::chrono::minutes(53) + std::chrono::seconds(52);
	preTrain.push_back(std::make_pair(temp, 16935));

	temp = std::chrono::hours(8) + std::chrono::minutes(54) + std::chrono::seconds(26);
	preTrain.push_back(std::make_pair(temp, 17775));

	temp = std::chrono::hours(8) + std::chrono::minutes(54) + std::chrono::seconds(55);
	preTrain.push_back(std::make_pair(temp, 18455));

	temp = std::chrono::hours(8) + std::chrono::minutes(56) + std::chrono::seconds(23);
	preTrain.push_back(std::make_pair(temp, 18905));

	temp = std::chrono::hours(8) + std::chrono::minutes(56) + std::chrono::seconds(37);
	preTrain.push_back(std::make_pair(temp, 19230));

	temp = std::chrono::hours(8) + std::chrono::minutes(57) + std::chrono::seconds(27);
	preTrain.push_back(std::make_pair(temp, 20425));

	temp = std::chrono::hours(9) + std::chrono::minutes(2) + std::chrono::seconds(20);
	preTrain.push_back(std::make_pair(temp, 20901));

	std::sort(preTrain.begin(), preTrain.end());	// �����\�[�g
}

R_ATC::~R_ATC() {
}

ControlInfo R_ATC::Elapse(VehicleState state) {
	ControlInfo ret = ControlInfo();

	// ATC�d��
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Power)] = true;

	// ��~���E
	const double stopLimit_d = this->stop < getPreTrainPosition(state) ? this->stop : getPreTrainPosition(state);	// ��~���E�ʒu
	const double stopLimit = stopLimit_d - state.status.Z >= 0 ? stopLimit_d - state.status.Z : 0;	// ��~���E�c����
	{
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
	}

	// �J�ʏ��
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Train)] = true;
	uint16_t stopLimit_ui = 0;
	if (stopLimit >= 0) stopLimit_ui = this->calclateStopLimit(state, stopLimit);	// ��~���E
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

	// �]���h�~
	if (atsPlugin->getDoor()) {
		ret.Handle["B"] = atsPlugin->getSpec().E / 2;	//�]���h�~����
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = 1;	// �]���h�~�_��
	} else if(atsPlugin->getHandleManual().B <= atsPlugin->getSpec().E / 2) {
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = 0;	// �]���h�~�œ�
	}

	// �u���[�L�o��
	uint16_t brake = this->calclateBrake(state, this->calclateSpeed(state));
	if (brake > ret.Handle["B"]) ret.Handle["B"] = brake;
	
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
