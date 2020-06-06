#pragma once

// �ԗ��X�y�b�N�g��
struct SpecPlus : Spec {
	int E = 0;	// ���u���[�L
	SpecPlus& operator=(const Spec& next);
	SpecPlus& operator=(const SpecPlus& next);
	SpecPlus();
};

// �ԗ���Ԋg��
struct StatePlus : State {
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
	StatePlus& operator=(const State& next);
	StatePlus& operator=(const StatePlus& next);
	StatePlus();
};

// ���t���[�����
struct VehicleState {
	StatePlus status = StatePlus();	// �ԗ����
	std::array<uint32_t, 256> panel = {};	// panel����l
	std::array<int32_t, 256> sound = {};	// sound����l
	VehicleState();
	VehicleState(State st, int* p, int* s);
};