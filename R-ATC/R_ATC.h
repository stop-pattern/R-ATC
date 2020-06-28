#pragma once
class R_ATC {
private:
	/* ----- variants ----- */

public:
	/* ----- functions ----- */

	// constructor
	R_ATC();

	// destructor
	~R_ATC();

	// control R-ATC at every frame
	// arg : VehicleState state
	//     vehicle status
	// return : Hand
	//     output handle position
	Hand Elapse(VehicleState state);
};
