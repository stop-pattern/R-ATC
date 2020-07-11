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
	float dec = 18;	// Œ¸‘¬’è”
	float L = 225400;	// ’â~ŒÀŠE‹——£’ö
	if (state.status.Z > L) return 0;
	ret = std::sqrt(std::abs(L - state.status.Z) * dec);
	if (ret > 120) ret = 120;
	return ret;
}

uint16_t R_ATC::calclateBrake(VehicleState state, float speed, uint8_t param) {
	uint16_t ret = 0;	// ƒuƒŒ[ƒLw—ß’l
	float psec = 2.5;	// PÚ‹ß•\¦ŠÔ[s]

	// PÚ‹ß
	if (state.status.V + state.status.A * psec < speed) {
		state.panel[static_cast<size_t>(panelIndex::Pattern)] = false;
		return 0;
	}
	state.panel[static_cast<size_t>(panelIndex::Pattern)] = true;

	// P”ñÚG
	if (state.status.V < speed) return 0;

	// ATCİ’è
	if (state.status.V > speed + 10) param = 2;
	if (state.status.V == 0 && speed == 0) param = 1;

	// ATC—áŠO
	switch (param) {
	default:
	case 0:
		break;
	case 1:	// ATCí—p
		return atsPlugin->getSpec().J;
		break;
	case 2:	// ATC”ñí
		return atsPlugin->getSpec().E;
		break;
	}

	/*
	// 10km/hˆÈ‰º‚Ì‚Ì—áŠO
	if (state.status.V < 10) {	// ‚à‚¤‚¿‚å‚Á‚Æl‚¦‚Ä‚à‚¢‚¢‚©‚à
		ret = atsPlugin->getHandleControl().B;	// ‘O‰ño—Í’l
		if(state.status.V < 0.25) if (state.status.A == 0) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 0.5) if (ret > atsPlugin->getSpec().J / 2) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 1) if (std::rand() % 4) ret--;
		else if (state.status.V < 5) if (std::rand() % 8) ret--;
		else if (std::rand() % 100) ret--;
		if (ret < 0) ret = 0;
	}
	*/
	if (state.status.V == 0) return atsPlugin->getSpec().J;

	// PÚGEP‰‰Z
	double bl = 4.00 / (atsPlugin->getSpec().E / 2);	// ƒuƒŒ[ƒLŒvZ’l
	// §ŒÀ+4km/h‚Ì‚É”ñí/2‚Ìo—Í‚É‚È‚é‚æ‚¤‚ÉŠe’i‚ÌP‚ğˆø‚­
	for (size_t i = 0; i < atsPlugin->getSpec().E; i++) {
		if (state.status.V - bl * i < speed) {
			return static_cast<uint16_t>(i);
		}
		ret++;
	}

	return ret;	// g‚í‚È‚¢‚¯‚Ç•ÛŒ¯
}

uint16_t R_ATC::calclateStopLimit(VehicleState state) {
	const uint16_t unit = 100;	// ’â~ŒÀŠE•\¦‚ğ‚¢‚­‚Â‚É‹æØ‚é‚©[ŒÂ]
	const uint8_t dis = 10;	// 1‹æØ‚è“–‚½‚è‚Ì‹——£[m]
	double limit;	// ’â~ŒÀŠE‹——£
	//limit = dis * (unit - state.status.V);	// ƒfƒoƒbƒN
	limit = this->stop;	// ƒfƒoƒbƒN
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

	// ’â~ŒÀŠE
	const double stopLimit_d = std::rand();
	const double stopLimit = stopLimit_d - state.status.Z;
	uint16_t stopLimit_ui = this->calclateStopLimit(state);
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::Close)] = stopLimit_ui;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit)] = stopLimit;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit01)] = static_cast<uint32_t>(stopLimit * 10) % 100;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit10)] = static_cast<uint32_t>(stopLimit / 10) % 100;
	ret.Panel[static_cast<uint8_t>(R_ATC::panelIndex::StopLimit1000)] = static_cast<uint32_t>(stopLimit / 1000) % 1000;

	// “]“®–h~
	if (atsPlugin->getDoor() && state.status.A) {
		ret.Handle["B"] = atsPlugin->getSpec().E / 2;	//“]“®–h~“®ì
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = true;	// “]“®–h~“_“”
	} else if(atsPlugin->getHandleManual().B <= atsPlugin->getSpec().E / 2) {
		ret.Panel[static_cast<size_t>(panelIndex::Rolling)] = false;	// “]“®–h~–Å“”
	}

	ret.Handle["B"] = this->calclateBrake(state, this->calclateSpeed(state));
	
	return ret;
}

// ƒpƒ^[ƒ“ˆø‚«•û
// “™‰Á‘¬“x‰^“®‚ğ‚µ‚Än•bŒã‚Éƒpƒ^[ƒ“’ïG: PÚ‹ß
// §ŒÀ}0: B0
// §ŒÀ+05: EB / 2
// §ŒÀ+10: Bmax
// §ŒÀ+15: EB
