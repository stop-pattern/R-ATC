#include "pch.h"
#include "R_ATC.h"

float R_ATC::calclateSpeed(VehicleState state) {
	uint16_t ret = UINT16_MAX;
	/*
	for (size_t i = 0; i < static_cast<size_t>(limitIndex::size); i++) {
		if (ret <= this->limit[i]) ret = this->limit[i];
	}
	return ret;
	*/
	float dec = 18;	// 減速定数
	float L = 225400;	// 停止限界距離程
	if (state.status.Z > L) return 0;
	ret = std::sqrt(std::abs(L - state.status.Z) * dec);
	if (ret > 120) ret = 120;
	return ret;
}

uint16_t R_ATC::calclateBrake(VehicleState state, float speed, uint8_t param) {
	uint16_t ret = 0;	// ブレーキ指令値
	float psec = 2.5;	// P接近表示時間[s]

	// P接近
	if (state.status.V + state.status.A * psec < speed) {
		state.panel[static_cast<size_t>(panelIndex::Pattern)] = false;
		return 0;
	}
	state.panel[static_cast<size_t>(panelIndex::Pattern)] = true;

	// P非接触
	if (state.status.V < speed) return 0;

	// ATC設定
	if (state.status.V > speed + 10) param = 2;
	if (state.status.V == 0 && speed == 0) param = 1;

	// ATC例外
	switch (param) {
	default:
	case 0:
		break;
	case 1:	// ATC常用
		return atsPlugin->getSpec().J;
		break;
	case 2:	// ATC非常
		return atsPlugin->getSpec().E;
		break;
	}

	/*
	// 10km/h以下の時の例外
	if (state.status.V < 10) {	// もうちょっと考えてもいいかも
		ret = atsPlugin->getHandleControl().B;	// 前回出力値
		if(state.status.V < 0.25) if (state.status.A == 0) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 0.5) if (ret > atsPlugin->getSpec().J / 2) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 1) if (std::rand() % 4) ret--;
		else if (state.status.V < 5) if (std::rand() % 8) ret--;
		else if (std::rand() % 100) ret--;
		if (ret < 0) ret = 0;
	}
	*/
	if (state.status.V == 0) return atsPlugin->getSpec().J;

	// P接触・P演算
	double bl = 4.00 / (atsPlugin->getSpec().E / 2);	// ブレーキ計算値
	// 制限+4km/hの時に非常/2の出力になるように各段のPを引く
	for (size_t i = 0; i < atsPlugin->getSpec().E; i++) {
		if (state.status.V - bl * i < speed) {
			return static_cast<uint16_t>(i);
		}
		ret++;
	}

	return ret;	// 使わないけど保険
}

uint16_t R_ATC::calclateStopLimit(VehicleState state) {
	const uint16_t unit = 100;	// 停止限界表示をいくつに区切るか[個]
	const uint8_t dis = 10;	// 1区切り当たりの距離[m]
	double limit;	// 停止限界距離
	//limit = dis * (unit - state.status.V);	// デバック
	limit = this->stop;	// デバック
	for (size_t i = 0; i < unit; i++) {
		if (limit < state.status.Z + dis * i) {
			if (i == 0) return unit;
			return i;
		}
	}
	return 0;
}

R_ATC::R_ATC() {
}

R_ATC::~R_ATC() {
}

ControlInfo R_ATC::Elapse(VehicleState state) {
	ControlInfo ret = ControlInfo();

	// 停止限界
	const double stopLimit_d = this->stop;	// 停止限界位置
	const double stopLimit = stopLimit_d - state.status.Z;	// 停止限界残距離
	uint16_t stopLimit_ui = this->calclateStopLimit(state);	// 停止限界
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Close)] = stopLimit_ui;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit)] = stopLimit;
	uint32_t temp = static_cast<uint32_t>(stopLimit * 10) % 100;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit01)] = temp == 0 ? 100 : temp;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit10)] = static_cast<uint32_t>(stopLimit / 10) % 100;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit1000)] = static_cast<uint32_t>(stopLimit / 1000) % 1000;

	// 転動防止
	if (atsPlugin->getDoor() && state.status.A) {
		ret.Handle["B"] = atsPlugin->getSpec().E / 2;	//転動防止動作
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = true;	// 転動防止点灯
	} else if(atsPlugin->getHandleManual().B <= atsPlugin->getSpec().E / 2) {
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = false;	// 転動防止滅灯
	}

	ret.Handle["B"] = this->calclateBrake(state, this->calclateSpeed(state));
	
	return ret;
}

// パターン引き方
// 等加速度運動をしてn秒後にパターン抵触: P接近
// 制限±0: B0
// 制限+05: EB / 2
// 制限+10: Bmax
// 制限+15: EB
