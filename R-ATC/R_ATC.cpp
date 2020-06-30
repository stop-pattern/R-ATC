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

	// 10km/hˆÈ‰º‚Ì‚Ì—áŠO
	if (state.status.V < 10) {	// ‚à‚¤‚¿‚å‚Á‚Æl‚¦‚Ä‚à‚¢‚¢‚©‚à
		ret = atsPlugin->getHandleControl().B;	// ‘O‰ño—Í’l
		if(state.status.V < 0.25) if (state.status.A == 0) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 1) if (ret > atsPlugin->getSpec().J / 2) ret = atsPlugin->getSpec().J / 2;
		else if (state.status.V < 2.5) if (std::rand() % 4) ret--;
		else if (state.status.V < 5) if (std::rand() % 8) ret--;
		else if (std::rand() % 16) ret--;
	}

	// PÚGEP‰‰Z
	double bl = 5.00 / (atsPlugin->getSpec().E / 2);	// ƒuƒŒ[ƒLŒvZ’l
	// §ŒÀ+5km/h‚Ì‚É”ñí/2‚Ìo—Í‚É‚È‚é‚æ‚¤‚ÉŠe’i‚ÌP‚ğˆø‚­
	for (size_t i = 0; i < atsPlugin->getSpec().E; i++) {
		if (state.status.V - bl * i < speed) {
			return static_cast<uint16_t>(i);
		}
		ret++;
	}

	return ret;	// g‚í‚È‚¢‚¯‚Ç•ÛŒ¯
}

R_ATC::R_ATC() {
}

R_ATC::~R_ATC() {
}

Hand R_ATC::Elapse(VehicleState state) {
	Hand ret = Hand();

	// “]“®–h~
	if (atsPlugin->getDoor() && state.status.A) {
		ret.B = atsPlugin->getSpec().E / 2;	//“]“®–h~“®ì
		state.panel[static_cast<size_t>(panelIndex::Rolling)] = true;	// “]“®–h~“_“”
	} else if(atsPlugin->getHandleManual().B <= atsPlugin->getSpec().E / 2) {
		state.panel[static_cast<size_t>(panelIndex::Rolling)] = false;	// “]“®–h~–Å“”
	}

	ret.B = this->calclateBrake(state, 0/* §ŒÀ‘¬“x */);
	
	return ret;
}

// ƒpƒ^[ƒ“ˆø‚«•û
// “™‰Á‘¬“x‰^“®‚ğ‚µ‚Än•bŒã‚Éƒpƒ^[ƒ“’ïG: PÚ‹ß
// §ŒÀ}0: B0
// §ŒÀ+05: EB / 2
// §ŒÀ+10: Bmax
// §ŒÀ+15: EB
