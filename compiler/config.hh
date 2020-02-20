#pragma once

//CPU architecture
enum CpuArch {
	Intel64,
	Intel32,
	Arm7
};

//The Assembler to use
enum Asm {
	Gas,		//The default
	Nasm
};

//Build type
enum BuildType {
	Exe,
	StaticLib,
	DynLib
};

//Configuration options
struct Config {
	std::string output;
	
	bool asm_only;
	bool obj_only;
	bool optimize;
	
	CpuArch arch;
	Asm asm_type;
	BuildType out_type;
};
