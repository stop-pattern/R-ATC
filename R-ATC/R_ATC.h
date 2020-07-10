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
		Rolling = 65,	// �]���h�~
		Limit_5 = 66,	// ���x���5k
		Limit_1 = 67,	// ���x���1k
		Pattern = 68,	// P�ڋ�
	};

	/* ----- variants ----- */
	std::vector<uint16_t> limit;	// ���x����
	
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
};
