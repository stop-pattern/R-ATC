#pragma once
class R_ATC {
private:
	/* ----- numbers ----- */
	// R_ATC::limit のインデックス
	enum struct limitIndex : uint8_t {
		route,	// 線間最高
		curve,	// 曲線
		step2,	// 2段
		size	// 総数
	};

	// panelのインデックス
	enum struct panelIndex : uint8_t {
		Rolling = 65,	// 転動防止
		Limit_5 = 66,	// 速度矢印5k
		Limit_1 = 67,	// 速度矢印1k
		Pattern = 68,	// P接近
	};

	/* ----- variants ----- */
	std::vector<uint16_t> limit;	// 速度制限
	
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
