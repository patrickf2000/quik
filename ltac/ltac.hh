#pragma once

enum class LTAC {
	//Functions
	Label,
	Ret,
	Call,
	Setup,
	Leave,
	
	//Data comparison
	Mov,
	Cmp,
	
	//Branching
	Jmp,
	Je,
	Jne,
	Jg,
	Jl,
	Jge,
	Jle,
	
	//Math
	Add,
	Sub,
	Mul,
	Div,
	Mod
	
	//Vector math
	VAdd,
	VSub,
	VMul,
	VDiv,
	
	//Registers
	Reg,
	SpReg,
	BpReg,
	WReg,
	
	//Memory
	Mem,
	
	//Data
	Byte,
	Int,
	Float
};
