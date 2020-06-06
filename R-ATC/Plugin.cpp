#include "pch.h"
#include "Plugin.h"



/* ----- functions ----- */

Plugin::Plugin() {
	std::array<int, static_cast<size_t>(statusIndex::size)> init_a = {};
	this->status = init_a;
	
	this->dllPath = "";
	
	this->iniPath = "";

	std::map<std::string, std::map<std::string, int>> init_m = { {} };
	this->iniData = init_m;

	this->spec = SpecPlus();
	this->status_now = VehicleState();
	this->status_previous = VehicleState();
	Hand init_h = {};
	this->handle_manual = init_h;
	this->handle_control = init_h;
	std::array<bool, 16> init_b = {};
	this->key = init_b;
	this->door = false;
	this->signal = 0;
}

Plugin::~Plugin() {
}

int Plugin::Attach(HMODULE hModule) {
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

int Plugin::getiniData(std::string app, std::string key) {
	if (this->iniData.count(app) && this->iniData.count(key)) {	// 存在判定
		return this->iniData[app][key];
	}
	return -1;	// 指定した値がないとき
}

void Plugin::SetVehicleSpec(Spec sp) {
	this->spec = sp;
}

void Plugin::Initialize(int i) {
	this->initPos;
}

Hand Plugin::Elapse(State st, int* p, int* s) {
	this->status_previous = this->status_now;
	this->status_now.status = st;
	for (size_t i = 0; i < 256; i++) {
		this->status_now.panel[i] = p[i];
		this->status_now.sound[i] = s[i];
	}
	return this->handle_manual;
}

Hand Plugin::Elapse(Hand h) {
	this->handle_control = h;
	return this->handle_control;
}

void Plugin::SetPower(int p) {
	this->handle_manual.P = p;
}

void Plugin::SetBrake(int b) {
	this->handle_manual.B = b;
}

void Plugin::SetReverser(int r) {
	this->handle_manual.R = r;
}

void Plugin::KeyDown(int k) {
	this->key[k] = true;
}

void Plugin::KeyUp(int k) {
	this->key[k] = false;
}

void Plugin::DoorOpen(void) {
	this->door = true;
}

void Plugin::DoorClose(void) {
	this->door = false;
}

void Plugin::SetSignal(int s) {
	this->signal = s;
}

int Plugin::getPath(HMODULE hModule) {
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

int Plugin::iniLoad() {
	try {
		readIni("app", "key", 0);	// Example
	}
	catch (const std::exception & e) {
		return -1;	// エラー握りつぶす => 後で実装する
	}
	return 0;
}

int Plugin::readIni(std::string app, std::string key, uint32_t def) {
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
