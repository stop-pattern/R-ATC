#include "pch.h"
#include "System.h"



/* ----- functions ----- */

System::System() {
	std::array<int, static_cast<size_t>(statusIndex::size)> init_a = {};
	this->status = init_a;
	
	this->dllPath = "";
	
	this->iniPath = "";

	std::map<std::string, std::map<std::string, int>> init_m = { {} };
	this->iniData = init_m;

	this->vehicleStatus = VehicleState();
	this->vehicleStatus.spec = {0, 0, 0, 0, 0, 0};
	this->vehicleStatus.status_now = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	this->vehicleStatus.status_previous = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	std::array<uint32_t, 256> init_u = {};
	this->vehicleStatus.panel = init_u;
	std::array<int32_t, 256> init_s = {};
	this->vehicleStatus.sound = init_s;
	Hand init_h = {};
	this->vehicleStatus.handle_manual = init_h;
	this->vehicleStatus.handle_control = init_h;
	std::array<bool, 16> init_b = {};
	this->vehicleStatus.key = init_b;
	this->vehicleStatus.door = false;
	this->vehicleStatus.signal = 0;
}

System::~System() {
}

int System::Attach(HMODULE hModule) {
	this->status[static_cast<size_t>(statusIndex::getPath)] = this->getPath(hModule);
	if (this->dllPath.is_absolute()) {
		this->status[static_cast<size_t>(statusIndex::iniLoad)] = this->iniLoad();
	}

	int ret = 0;
	for (size_t i = 0; i < static_cast<size_t>(statusIndex::size); i++) {
		if (this->status[i] == 0) continue;
		ret |= 1 << i;
	}
	return ret;
}

int System::getiniData(std::string app, std::string key) {
	if (this->iniData.count(app) && this->iniData.count(key)) {	// 存在判定
		return this->iniData[app][key];
	}
	return -1;	// 指定した値がないとき
}

void System::SetVehicleSpec(Spec sp) {
	sp = this->vehicleStatus.spec;
}

void System::SetPower(int p) {
	p = this->vehicleStatus.handle_manual.P;
}

void System::SetBrake(int b) {
	b = this->vehicleStatus.handle_manual.B;
}

void System::SetReverser(int r) {
	r = this->vehicleStatus.handle_manual.R;
}

void System::KeyDown(int k) {
	this->vehicleStatus.key[k] = true;
}

void System::KeyUp(int k) {
	this->vehicleStatus.key[k] = false;
}

void System::DoorOpen(void) {
	this->vehicleStatus.door = true;
}

void System::DoorClose(void) {
	this->vehicleStatus.door = false;
}

void System::SetSignal(int s) {
	this->vehicleStatus.signal = s;
}

int System::getPath(HMODULE hModule) {
	LPSTR FilePath;	// = _T("");
	char buf[100];
	FilePath = &buf[0];

	try {
		GetModuleFileName(hModule, FilePath, MAX_PATH);	// ファイルパス取得
	}
	catch (const std::exception & e) {
		return -1;	// エラー握りつぶす => 後で実装する
	}

	this->dllPath = FilePath;

	this->iniPath = this->dllPath;
	this->iniPath.replace_extension(".ini");

	return 0;
}

int System::iniLoad() {
	try {
		readIni("app", "key", 0);	// Example
	}
	catch (const std::exception & e) {
		return -1;	// エラー握りつぶす => 後で実装する
	}
	return 0;
}

int System::readIni(std::string app, std::string key, uint32_t def) {
	int ret = 0;
	try {
		//GetPrivateProfileIntA("AppName", "KeyName", default, "FileName");
		ret = GetPrivateProfileInt(app.c_str(), key.c_str(), 0, this->iniPath.string().c_str());
		this->iniData[app][key] = ret;
	}
	catch (const std::exception & e) {
		return -1;	// エラー握りつぶす => 後で実装する
	}
	return ret;
}
