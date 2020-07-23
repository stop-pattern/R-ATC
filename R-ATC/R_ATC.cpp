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

uint16_t R_ATC::calclateStopLimit(VehicleState state) {
	for (size_t i = 0; i < this->unit; i++) {
		if (this->stop < state.status.Z + this->dis * i) {
			if (i == 0) return this->unit + 1;
			return i;
		}
	}
	return this->unit + 2;
}

R_ATC::R_ATC() {
}

R_ATC::~R_ATC() {
}

ControlInfo R_ATC::Elapse(VehicleState state) {
	ControlInfo ret = ControlInfo();

	// ATC電源
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Power)] = true;

	// 停止限界
	const double stopLimit_d = this->stop;	// 停止限界位置
	double stopLimit = stopLimit_d - state.status.Z >= 0 ? stopLimit_d - state.status.Z : 0;	// 停止限界残距離
	{
		uint32_t temp = static_cast<uint32_t>(stopLimit / 1000) % 100;
		if (stopLimit / 100000 >= 1) {	// 最上位桁の場合
			if (true) {	// 1桁モード
				if (temp == 0) temp = 10;
			}
			else if (temp < 10) temp += 100;	// 2桁モード
		}
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit1000)] = temp;

		if (temp == 0) {	// 最上位桁の場合
			temp = static_cast<uint32_t>(stopLimit / 10) % 100;
			//if (temp == 0) temp = 100;
		}
		else {	// 中位桁の場合
			temp = static_cast<uint32_t>(stopLimit / 10) % 100;
			if (temp < 10) temp += 100;
			if (temp == 100) temp += 10;
		}
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit10)] = temp;

		temp = static_cast<uint32_t>(stopLimit * 10) % 100;
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit01)] = temp == 0 ? 100 : temp;

		temp = stopLimit;
		ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit)] = temp;
	}


	// 踏切
	uint8_t index = static_cast<uint8_t>(R_ATC::panelIndex::Clossing_1);
	for (size_t i = static_cast<size_t>(R_ATC::panelIndex::Clossing_1); i <= static_cast<size_t>(R_ATC::panelIndex::Clossing_10); i++) {
		ret.Panel[i] = 0;
	}
	for (size_t i = 0; i < this->crossing.size(); i++) {
		if (crossing[i] < state.status.Z) continue;
		if (index >= static_cast<uint8_t>(R_ATC::panelIndex::Clossing_10) + 1) break;
		if (crossing[i] < state.status.Z + 1000) {
			ret.Panel[index] = static_cast<uint32_t>((crossing[i] - state.status.Z) / this->dis);
			index++;
		}
		else if (crossing[i] > state.status.Z + 1000) break;
	}

	// 転動防止
	if (atsPlugin->getDoor() && state.status.A) {
		ret.Handle["B"] = atsPlugin->getSpec().E / 2;	//転動防止動作
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = true;	// 転動防止点灯
	} else if(atsPlugin->getHandleManual().B <= atsPlugin->getSpec().E / 2) {
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = false;	// 転動防止滅灯
	}

	ret.Handle["B"] = this->calclateBrake(state, 0/* 制限速度 */);
	
	return ret;
}

double R_ATC::setCrossing(double distance) {
	this->crossing.push_back(distance);	// 要素追加
	std::sort(this->crossing.begin(), this->crossing.end());	// 昇順ソート
	return distance;
}

double R_ATC::setStop(double distance) {
	this->stop = distance;
	return distance;
}

// パターン引き方
// 等加速度運動をしてn秒後にパターン抵触: P接近
// 制限±0: B0
// 制限+05: EB / 2
// 制限+10: Bmax
// 制限+15: EB
