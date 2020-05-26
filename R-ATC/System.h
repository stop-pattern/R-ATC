#pragma once
class System {
private:
	/** ----- numbers ----- */
	// System::status �̃C���f�b�N�X
	enum struct statusIndex : int {
		getPath,
		iniLoad,
		size
	};

	/** ----- variants ----- */
	std::array<int, static_cast<size_t>(statusIndex::size)> status;	// �֐��̎��s����
	std::filesystem::path dllPath;	// ����dll�ւ̐�΃p�X
	std::filesystem::path iniPath;	// ����dll�Ɠ��K�w������ini�ւ̐�΃p�X
	std::map<std::string, std::map<std::string, int>> iniData;	// ini����ǂݏo�����l

	/** ----- functions ----- */
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

public:
	/** ----- functions ----- */
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
};

