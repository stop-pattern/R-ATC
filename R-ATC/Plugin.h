#pragma once
class Plugin {
private:
	/* ----- numbers ----- */
	// Plugin::status のインデックス
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

	int32_t initPos;	// 初期ブレーキ位置

	SpecPlus spec;	// 車両性能
	VehicleState status_now;	// 車両状態値 (今フレーム入力)
	VehicleState status_previous;	// 車両状態値 (前フレーム出力)
	Hand handle_manual;	// 手動ハンドル状態
	Hand handle_control;	// ハンドル制御値 (前フレーム出力)
	std::array<bool, 16> key;	// 入力キー状態
	bool door;	// ドア状態
	int32_t signal;	// signal値

public:
	/* ----- functions ----- */

	// --- system --- //

	// constructor
	Plugin();
	
	// destructor
	~Plugin();

	// use when dll was Attached
	// return : int
	//     whether inside function is succeeded (if success, 0)
	//     if failed, bit flag (return |= 1 << statusIndex)
	int Attach(HMODULE hModule);


	// --- for bve API --- //

	// save vehicle spec
	// called in SetVehicleSpec(Spec)
	// arg : Spec sp
	//     vehicle spec
	void SetVehicleSpec(Spec sp);

	// save latest initialize position
	// called in Initialize(int)
	// arg : int i
	//     initialize position
	void Initialize(int i);

	// save status in this frame
	// called at start of Elapse(State, int*, int*)
	// arg : State st 
	//     vehicle status
	// arg : int* p
	//     panel status
	// arg : int* s
	//     sound status
	// return : VehicleState
	//     vehicle state of this frame
	VehicleState beginElapse(State st, int* p, int* s);

	// save handle position in latest frame
	// called at last of Elapse(State, int*, int*)
	// arg : ControlInfo control
	//     vehicle control information
	// arg : int* p
	//     panel status
	// arg : int* s
	//     sound status
	// return : Hand
	//     latest handle position
	Hand endElapse(ControlInfo control, int* p, int* s);

	// save manual handle position
	// called in SetPower(int)
	// arg : int p
	//     power notch position
	void SetPower(int p);

	// save manual handle position
	// called in SetBrake(int)
	// arg : int p
	//     brake notch position
	void SetBrake(int b);

	// save manual reverser position
	// called in SetReverser(int)
	// arg : int p
	//     reverser position
	void SetReverser(int r);

	// save key status
	// called in KeyDown(int)
	// arg : int k
	//     key status
	void KeyDown(int k);

	// save key status
	// called in KeyUp(int)
	// arg : int k
	//     key status
	void KeyUp(int k);

	// save door status
	// called in DoorOpen(void)
	void DoorOpen(void);

	// save door status
	// called in DoorClose(void)
	void DoorClose(void);

	// save signal index
	// called in SetSignal(int)
	// arg : int s
	//     signal index
	void SetSignal(int s);


	// --- API for developer --- //

	// get vehicle spec
	// arg : void
	// return : SpecPlus
	//     vehicle spec
	SpecPlus getSpec(void);

	// get vehicle status
	// arg : void
	// return : VehicleState
	//     vehicle status
	VehicleState getStatus(void);

	// get vehicle status
	// arg : void
	// return : VehicleState
	//     vehicle status
	VehicleState getStatusPrevious(void);

	// get handle status
	// arg : void
	// return : Hand
	//     handle status
	Hand getHandleControl(void);

	// get handle status
	// arg : void
	// return : Hand
	//     handle status
	Hand getHandleManual(void);

	// get key status
	// arg : void
	// return : std::array<bool, 16>
	//     key status
	std::array<bool, 16> getKey(void);
	// get key status
	// arg : uint8_t i
	//     key index
	// return : bool
	//     key status
	bool getKey(uint8_t i);

	// get door status
	// arg : void
	// return : bool
	//     door status
	bool getDoor(void);

	// get signal index
	// arg : void
	// return : int32_t
	//     signal index
	int32_t getSignal(void);

	// get value from ini file
	// arg : std::string app
	//     ini [app] name
	// arg : std::string key
	//     ini key name
	// return : int
	//     whether app && key are found (if not found, -1)
	int getiniData(std::string app, std::string key);


private:
	// --- others --- //

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
