#pragma once

struct SpecPlus : Spec {
	int E = 0;	// ���u���[�L
	SpecPlus& operator=(const Spec& next);
	SpecPlus& operator=(const SpecPlus& next);
};

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
};

struct VehicleState {
	SpecPlus spec;	// �ԗ����\
	StatePlus status_now;	// �ԗ���� (���t���[��)
	StatePlus status_previous;	// �ԗ���� (�O�t���[��)
	std::array<uint32_t, 256> panel;	// panel����l
	std::array<int32_t, 256> sound;	// sound����l
	Hand handle_manual;	// �蓮�n���h�����
	Hand handle_control;	// �n���h������l (�O�t���[���o��)
	std::array<bool, 16> key;	// ���̓L�[���
	bool door;	// �h�A���
	int32_t signal;	// signal�l
};