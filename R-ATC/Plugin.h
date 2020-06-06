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
	VehicleState status_now;	// 車両状態値 (今フレーム)
	VehicleState status_previous;	// 車両状態値 (前フレーム)
	Hand handle_manual;	// 手動ハンドル状態
	Hand handle_control;	// ハンドル制御値 (前フレーム出力)
	std::array<bool, 16> key;	// 入力キー状態
	bool door;	// ドア状態
	int32_t signal;	// signal値

public:
	/* ----- functions ----- */
	// constructor
	Plugin();
	
	// destructor
	~Plugin();
	
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

	// save status in latest frame
	// called in Elapse(State, int*, int*)
	// arg : VehicleState st 
	//     vehicle status
	// return : Hand
	//     latest manually set handle position
	Hand Elapse(VehicleState st);

	// save status in latest frame
	// called in Elapse(State, int*, int*)
	// arg : State st 
	//     vehicle status
	// arg : int* p
	//     panel status
	// arg : int* s
	//     sound status
	// return : Hand
	//     latest manually set handle position
	Hand Elapse(State st, int* p, int* s);

	// save handle position in latest frame
	// called in Elapse(State, int*, int*)
	// arg : Hand h
	// return : Hand
	//     latest manually set handle position
	Hand Elapse(Hand h);

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
