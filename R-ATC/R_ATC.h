#pragma once
class R_ATC {
private:
	/* ----- numbers ----- */
	// R_ATC::limit �̃C���f�b�N�X
	enum struct limitIndex : uint8_t {
		route,	// ���ԍō�
		curve,	// �Ȑ�
		step2,	// 2�i
		size	// ����
	};

	// panel�̃C���f�b�N�X
	enum struct panelIndex : uint8_t {
		Power = 52,	// ATC�d��
		Rolling = 65,	// �]���h�~
		Limit_5 = 66,	// ���x���5k
		Limit_1 = 67,	// ���x���1k
		Pattern = 68,	// P�ڋ�
		StopLimit1000 = 76,	// ��~���E10000-1000
		StopLimit10 = 77,	// ��~���E100-10
		StopLimit01 = 78,	// ��~���E1-0.1
		StopLimit = 79,	// ��~���E
		Train = 94,	// �����
		Open = 237,	// �J�ʋ��
		Close = 238,	// ���J��
		Station_start = 239,	// �w��n�_
		Station_end = 240,	// �w��I�_
		Clossing_1 = 241,	// ����
	};

	/* ----- variants ----- */
	const uint16_t unit = 100;	// ��~���E�\���������ɋ�؂邩[��]
	const uint8_t dis = 10;	// 1��؂蓖����̋���[m]
	std::vector<uint16_t> limit;	// ���x����
	double stop = 0.0;	// ��~���E
	
	/* ----- functions ----- */
	// calclate speed limit
	// arg : VehicleState state
	//     vehicle status
	// return : uint16_t
	//     output speed limit
	uint16_t calclateSpeed(VehicleState state);

	// calclate brake notch
	// arg : VehicleState state
	//     vehicle status
	// arg : uint16_t speed
	//     limit speed
	// arg : uint8_t param
	//     ATC parameter
	// return : uint16_t
	//     output brake notch
	uint16_t calclateBrake(VehicleState state, uint16_t speed, uint8_t param = 0);

	// calclate stop limit
	// arg : VehicleState state
	//     vehicle status
	// return : uint16_t
	//     stop limit index
	uint16_t calclateStopLimit(VehicleState state);

public:
	// constructor
	R_ATC();

	// destructor
	~R_ATC();

	// control R-ATC at every frame
	// arg : VehicleState state
	//     vehicle status
	// return : ControlInfo
	//     output status
	ControlInfo Elapse(VehicleState state);

	// set stop limit
	// arg : double distance
	//     distance of stop limit 
	// return : double
	//     arg
	double setStop(double distance);
};
