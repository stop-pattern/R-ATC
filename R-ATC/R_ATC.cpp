#include "pch.h"
#include "R_ATC.h"

float R_ATC::calclateSpeed(VehicleState state) {
	uint16_t ret = UINT16_MAX;
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
	double limit = this->stop;	// 停止限界距離
	if (this->getPreTrainPosition(state) < limit) limit = this->getPreTrainPosition(state);
	for (size_t i = 0; i < this->unit; i++) {
		if (limit < state.status.Z + this->dis * i) {
			if (i == 0) return this->unit + 1;
			return i;
		}
	}
	return this->unit + 2;
}

double R_ATC::getPreTrainPosition(VehicleState state) {
	double ret = 0.0;
	if (this->preTrain.size() <= 1) {
		if (this->preTrain.size() == 0) return ret;
		return this->preTrain[0].second;
	}
	if (this->preTrain[this->preTrain.size() - 1].first < std::chrono::milliseconds(state.status.T)) {
		return this->preTrain[this->preTrain.size() - 1].second;
	}
	for (size_t i = 0; i < this->preTrain.size(); i++) {
		if (std::chrono::milliseconds(state.status.T) < this->preTrain[i].first) {
			double a = (preTrain[i + 1].second - preTrain[i].second) / (preTrain[i + 1].first.count() - preTrain[i].first.count());
			ret = a * (state.status.T - preTrain[i].first.count()) + preTrain[i].second;	// 線形回帰
			return ret;
		}
	}
	return ret;
}

R_ATC::R_ATC() {
	std::chrono::milliseconds temp = std::chrono::milliseconds(0);

	temp = std::chrono::hours(8) + std::chrono::minutes(53) + std::chrono::seconds(45);
	preTrain.push_back(std::make_pair(temp, 16625));

	temp = std::chrono::hours(8) + std::chrono::minutes(53) + std::chrono::seconds(52);
	preTrain.push_back(std::make_pair(temp, 16935));

	temp = std::chrono::hours(8) + std::chrono::minutes(54) + std::chrono::seconds(26);
	preTrain.push_back(std::make_pair(temp, 17775));

	temp = std::chrono::hours(8) + std::chrono::minutes(54) + std::chrono::seconds(55);
	preTrain.push_back(std::make_pair(temp, 18455));

	temp = std::chrono::hours(8) + std::chrono::minutes(56) + std::chrono::seconds(23);
	preTrain.push_back(std::make_pair(temp, 18905));

	temp = std::chrono::hours(8) + std::chrono::minutes(56) + std::chrono::seconds(37);
	preTrain.push_back(std::make_pair(temp, 19230));

	temp = std::chrono::hours(8) + std::chrono::minutes(57) + std::chrono::seconds(27);
	preTrain.push_back(std::make_pair(temp, 20425));

	temp = std::chrono::hours(9) + std::chrono::minutes(2) + std::chrono::seconds(20);
	preTrain.push_back(std::make_pair(temp, 20901));


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

	// 開通情報
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Train)] = true;
	uint16_t stopLimit_ui = 0;
	if (stopLimit >= 0) stopLimit_ui = this->calclateStopLimit(state);	// 停止限界
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Close)] = stopLimit_ui;

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
	if (atsPlugin->getDoor()) {
		ret.Handle["B"] = atsPlugin->getSpec().E / 2;	//転動防止動作
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = 1;	// 転動防止点灯
	} else if(atsPlugin->getHandleManual().B <= atsPlugin->getSpec().E / 2) {
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = 0;	// 転動防止滅灯
	}

	// ブレーキ出力
	uint16_t brake = this->calclateBrake(state, this->calclateSpeed(state));
	if (brake > ret.Handle["B"]) ret.Handle["B"] = brake;
	
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
