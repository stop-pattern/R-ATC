#include "pch.h"
#include "R_ATC.h"

uint16_t R_ATC::calclateSpeed(VehicleState state) {
	uint16_t ret = UINT16_MAX;
	for (size_t i = 0; i < static_cast<size_t>(limitIndex::size); i++) {
		if (ret <= this->limit[i]) ret = this->limit[i];
	}
	return ret;
}

uint16_t R_ATC::calclateBrake(VehicleState state, uint16_t speed, uint8_t param) {
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

	// 10km/h以下の時の例外
	if (state.status.V < 10) {	// もうちょっと考えてもいいかも
		ret = atsPlugin->getHandleControl().B;	// 前回出力値
		if(state.status.V < 0.25) if (state.status.A == 0) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 1) if (ret > atsPlugin->getSpec().J / 2) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 2.5) if (std::rand() % 4) ret--;
		else if (state.status.V < 5) if (std::rand() % 8) ret--;
		else if (std::rand() % 16) ret--;
	}

	// P接触・P演算
	double bl = 5.00 / (atsPlugin->getSpec().E / 2);	// ブレーキ計算値
	// 制限+5km/hの時に非常/2の出力になるように各段のPを引く
	for (size_t i = 0; i < atsPlugin->getSpec().E; i++) {
		if (state.status.V - bl * i < speed) {
			return static_cast<uint16_t>(i);
		}
		ret++;
	}

	return ret;	// 使わないけど保険
}

R_ATC::R_ATC() {
}

R_ATC::~R_ATC() {
}

Hand R_ATC::Elapse(VehicleState state) {
	Hand ret = Hand();

	// 転動防止
	if (atsPlugin->getDoor() && state.status.A) {
		ret.B = atsPlugin->getSpec().E / 2;	//転動防止動作
		state.panel[static_cast<size_t>(panelIndex::Rolling)] = true;	// 転動防止点灯
	} else if(atsPlugin->getHandleManual().B <= atsPlugin->getSpec().E / 2) {
		state.panel[static_cast<size_t>(panelIndex::Rolling)] = false;	// 転動防止滅灯
	}

	ret.B = this->calclateBrake(state, 0/* 制限速度 */);
	
	return ret;
}

// パターン引き方
// 等加速度運動をしてn秒後にパターン抵触: P接近
// 制限±0: B0
// 制限+05: EB / 2
// 制限+10: Bmax
// 制限+15: EB
