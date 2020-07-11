#pragma once
class R_ATC {
private:
	/* ----- numbers ----- */
	// R_ATC::limit ‚ÌƒCƒ“ƒfƒbƒNƒX
	enum struct limitIndex : uint8_t {
		route,	// üŠÔÅ‚
		curve,	// ‹Èü
		step2,	// 2’i
		size	// ‘”
	};

	// panel‚ÌƒCƒ“ƒfƒbƒNƒX
	enum struct panelIndex : uint8_t {
		Rolling = 65,	// “]“®–h~
		Limit_5 = 66,	// ‘¬“x–îˆó5k
		Limit_1 = 67,	// ‘¬“x–îˆó1k
		Pattern = 68,	// PÚ‹ß
		StopLimit1000 = 76,	// ’â~ŒÀŠE10000-1000
		StopLimit10 = 77,	// ’â~ŒÀŠE100-10
		StopLimit01 = 78,	// ’â~ŒÀŠE1-0.1
		StopLimit = 79,	// ’â~ŒÀŠE
		Train = 94,	// ©—ñÔ
		Open = 237,	// ŠJ’Ê‹æŠÔ
		Close = 238,	// –¢ŠJ’Ê
		Station_start = 239,	// ‰w‹æn“_
		Station_end = 240,	// ‰w‹æI“_
		Clossing_1 = 241,	// “¥Ø
	};

	/* ----- variants ----- */
	std::vector<uint16_t> limit;	// ‘¬“x§ŒÀ
	
	/* ----- functions ----- */
	// calclate speed limit
	// arg : VehicleState state
	//     vehicle status
	// return : uint16_t
	//     output speed limit
	float calclateSpeed(VehicleState state);

	// calclate brake notch
	// arg : VehicleState state
	//     vehicle status
	// arg : uint16_t speed
	//     limit speed
	// arg : uint8_t param
	//     ATC parameter
	// return : uint16_t
	//     output brake notch
	uint16_t calclateBrake(VehicleState state, float speed, uint8_t param = 0);

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
};
