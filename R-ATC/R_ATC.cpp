#include "pch.h"
#include "R_ATC.h"

R_ATC::R_ATC() {
}

R_ATC::~R_ATC() {
}

Hand R_ATC::Elapse(VehicleState state) {
	Hand ret = Hand();
	if (atsPlugin->getDoor()) {	// “]“®–h~
		ret.B = atsPlugin->getSpec().E / 2;
		ret.P = 0;
	}
	return ret;
}
