#pragma once
class System {
private:
	/* ----- numbers ----- */
	// System::status のインデックス
	enum struct statusIndex : int {
		getPath,
		iniLoad,
		size
	};

	/* ----- variants ----- */
	std::array<int, static_cast<size_t>(statusIndex::size)> status;	// 関数の実行結果
	std::filesystem::path dllPath;	// このdllへの絶対パス
	std::filesystem::path iniPath;	// このdllと同階層同名のiniへの絶対パス
	std::map<std::string, std::map<std::string, int>> iniData;	// iniから読み出した値
	VehicleState vehicleStatus;	// 車両状態値

public:
	/* ----- functions ----- */
	// constructor
	System();
	
	// destructor
	~System();
	
	// use when dll was Attached
	// return : int
	//     whether inside function is succeeded (if success, 0)
	//     if failed, bit flag (return |= 1 << statusIndex)
	int Attach(HMODULE hModule);
	
	// get value from ini file
	// arg : std::string app
	//     ini [app] name
	// arg : std::string key
	//     ini key name
	// return : int
	//     whether app && key are found (if not found, -1)
	int getiniData(std::string app, std::string key);

	// save vehicle spec to vehicleStatus
	// called in SetVehicleSpec(Spec)
	// arg : Spec sp
	//     vehicle spec
	void SetVehicleSpec(Spec sp);

	// save manual handle position to vehicleStatus
	// called in SetPower(int)
	// arg : int p
	//     power notch position
	void SetPower(int p);

	// save manual handle position to vehicleStatus
	// called in SetBrake(int)
	// arg : int p
	//     brake notch position
	void SetBrake(int p);

	// save manual reverser position to vehicleStatus
	// called in SetReverser(int)
	// arg : int p
	//     reverser position
	void SetReverser(int p);

	// save key status to vehicleStatus
	// called in KeyDown(int)
	// arg : int k
	//     key status
	void KeyDown(int k);

	// save key status to vehicleStatus
	// called in KeyUp(int)
	// arg : int k
	//     key status
	void KeyUp(int k);

	// save door status to vehicleStatus
	// called in DoorOpen(void)
	void DoorOpen(void);

	// save door status to vehicleStatus
	// called in DoorClose(void)
	void DoorClose(void);

	// save signal index to vehicleStatus
	// called in SetSignal(int)
	// arg : int s
	//     signal index
	void SetSignal(int s);

private:

	// get dll path (with ini path)
	// arg : HMODULE hModule
	//     ->dllMain
	// return : int
	//     whether this function is succeeded (if failed, -1)
	int getPath(HMODULE hModule);

	// load initialize value from ini file
	// return : int
	//     whether this function is succeeded (if failed, -1)
	int iniLoad();

	// read initialize value from ini file
	// arg : std::string app
	//     ini [app] name
	// arg : std::string key
	//     ini key name
	// arg : uint32_t def = 0
	//     ini default value (used when app or name is not found)
	// return : int
	//     whether this function is succeeded (if failed, -1)
	int readIni(std::string app, std::string key, uint32_t def = 0);
};
