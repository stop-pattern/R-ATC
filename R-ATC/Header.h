#pragma once

// 車両スペック拡張
struct SpecPlus : Spec {
	int E = 0;	// 非常ブレーキ
	SpecPlus& operator=(const Spec& next);
	SpecPlus& operator=(const SpecPlus& next);
	SpecPlus();
};

// 車両状態拡張
struct StatePlus : State {
	double dZ = 0;	// 移動距離[m]
	float dV = 0;	//速度変化[km/h]
	int dT = 0;	// 経過時間[ms]
	float dBC = 0;	// BC圧力変化[kPa]
	float dMR = 0;	// MR圧力変化[kPa]
	float dER = 0;	// ER圧力変化[kPa]
	float dBP = 0;	// BP圧力変化[kPa]
	float dSAP = 0;	// SAP圧力変化[kPa]
	float dI = 0;	// 電流変化[A]
	float A = 0;	// 加速度[km/h/s]
	float fps = 0;	// フレーム数[frame/s]
	StatePlus& operator=(const State& next);
	StatePlus& operator=(const StatePlus& next);
	StatePlus();
};

// 毎フレーム状態
struct VehicleState {
	StatePlus status = StatePlus();	// 車両状態
	std::array<uint32_t, 256> panel = {};	// panel制御値
	std::array<int32_t, 256> sound = {};	// sound制御値
	VehicleState();
	VehicleState(State st, int* p, int* s);
};

// 制御情報
struct ControlInfo {
	std::unordered_map<std::string, int8_t, std::less<>> Handle = { {} };	// ハンドル制御値
	std::unordered_map<uint8_t, uint32_t> Panel = { {} };	// パネル制御値
	std::unordered_map<uint8_t, int32_t> Sound = { {} };	// サウンド制御値
	int setControl(Hand* h, int* p, int* s);	// 状態伝達
};
};

// 下からn桁目の数値をuint8_tで返す
// ※nが1以外で0の時は上位桁が10なら10
uint8_t getDigitF(float arg, uint16_t n)noexcept;
