#include "pch.h"
#include "System.h"



/* ----- functions ----- */

int System::getPath(HMODULE hModule) {
	LPSTR FilePath;	// = _T("");
	char buf[100];
	FilePath = &buf[0];

	try {
		GetModuleFileName(hModule, FilePath, MAX_PATH);	// ファイルパス取得
	}
	catch (const std::exception& e) {
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
	catch (const std::exception& e) {
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
	catch (const std::exception& e) {
		return -1;	// エラー握りつぶす => 後で実装する
	}
	return ret;
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
