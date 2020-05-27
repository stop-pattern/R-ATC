#include "pch.h"
#include "Header.h"


SpecPlus& SpecPlus::operator=(const SpecPlus& next) {
	if (this == &next) {
		this->E = next.B + 1;
		Spec::operator=(next);
	}
	return *this;
}

SpecPlus& SpecPlus::operator=(const Spec& next) {
	if (this == &next) {
		this->E = next.B + 1;
		Spec::operator=(next);
	}
	return *this;
}


StatePlus& StatePlus::operator=(const State& next) {
	if (this != &next) {
		this->dZ = next.Z - this->Z;
		this->dV = next.V - this->V;
		this->dT = next.T - this->T;
		this->dBC = next.BC - this->BC;
		this->dMR = next.MR - this->MR;
		this->dER = next.ER - this->ER;
		this->dBP = next.BP - this->BP;
		this->dSAP = next.SAP - this->SAP;
		this->dI = next.I - this->I;
		this->A = dV / dT;
		this->fps = 1.00 * 1000 / dT;
		State::operator=(next);
	}
	return *this;
}

StatePlus& StatePlus::operator=(const StatePlus& next) {
	if (this != &next) {
		this->dZ = next.Z - this->Z;
		this->dV = next.V - this->V;
		this->dT = next.T - this->T;
		this->dBC = next.BC - this->BC;
		this->dMR = next.MR - this->MR;
		this->dER = next.ER - this->ER;
		this->dBP = next.BP - this->BP;
		this->dSAP = next.SAP - this->SAP;
		this->dI = next.I - this->I;
		this->A = next.V - this->V;
		this->fps = 1.00 * 1000 / dT;
		State::operator=(next);
	}
	return *this;
}