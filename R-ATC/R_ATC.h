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
		Power = 52,	// ATC電源
		Rolling = 65,	// 転動防止
		Limit_5 = 66,	// 速度矢印5k
		Limit_1 = 67,	// 速度矢印1k
		Pattern = 68,	// P接近
		StopLimit1000 = 76,	// 停止限界10000-1000
		StopLimit10 = 77,	// 停止限界100-10
		StopLimit01 = 78,	// 停止限界1-0.1
		StopLimit = 79,	// 停止限界
		Train = 94,	// 自列車
		Open = 237,	// 開通区間
		Close = 238,	// 未開通
		Station_start = 239,	// 駅区始点
		Station_end = 240,	// 駅区終点
		Clossing_1 = 241,	// 踏切
		Clossing_2 = 242,	// 踏切
		Clossing_3 = 243,	// 踏切
		Clossing_4 = 244,	// 踏切
		Clossing_5 = 245,	// 踏切
		Clossing_6 = 246,	// 踏切
		Clossing_7 = 247,	// 踏切
		Clossing_8 = 248,	// 踏切
		Clossing_9 = 249,	// 踏切
		Clossing_10 = 250,	// 踏切
	};

	// soundのインデックス
	enum struct soundIndex : uint8_t {
		ATC = 50, // ATCベル

	};

	/* ----- variants ----- */
	const uint16_t unit = 100;	// 停止限界表示をいくつに区切るか[個]
	const uint8_t dis = 10;	// 1区切り当たりの距離[m]
	std::vector<uint16_t> limit;	// 速度制限
	std::vector<double> crossing;	// 踏切
	double stop = 0.0;	// 停止限界
	
	/* ----- functions ----- */
	// calclate speed limit
	// arg : VehicleState state
	//     vehicle status
	// return : float
	//     output speed limit
	float calclateSpeed(VehicleState state);

	// calclate brake notch
	// arg : VehicleState state
	//     vehicle status
	// arg : float speed
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

	// set crossing position
	// arg : double distance
	//     vehicle status
	// return : double
	//     arg
	double setCrossing(double distance);

	// set stop limit
	// arg : double distance
	//     distance of stop limit 
	// return : double
	//     arg
	double setStop(double distance);
};
