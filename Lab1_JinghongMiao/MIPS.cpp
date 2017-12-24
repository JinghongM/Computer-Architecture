#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include <sstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define LW 
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

class RF
{
public:
	bitset<32> ReadData1, ReadData2; //Rs, Rt
	RF()
	{
		Registers.resize(32);
		Registers[0] = bitset<32>(0);
	}

	void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
	{
		Registers[0] = 0;
		if (WrtEnable == 1)
		{
			Registers[(int)(WrtReg.to_ulong())] = WrtData;
		}
		// implement the funciton by you.  
		else
		{
			ReadData1 = Registers[(int)(RdReg1.to_ulong())];
			ReadData2 = Registers[(int)(RdReg2.to_ulong())];
		}

	}


	void OutputRF()
	{
		ofstream rfout;
		std::ofstream outfile("RFresult.txt");
		rfout.open("RFresult.txt", std::ios_base::app);
		if (rfout.is_open())
		{
			rfout << "A state of RF:" << endl;
			for (int j = 0; j<32; j++)
			{
				rfout << Registers[j] << endl;
			}

		}
		else cout << "Unable to open file";
		rfout.close();

	}
private:
	vector<bitset<32> >Registers;

};

class ALU
{
public:
	bitset<32> ALUresult;
	bitset<32> RALUOperation(bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
	{
		int i;
		// implement the ALU operations by you. 
		int func = (int)ALUOP.to_ulong();
		switch (func)
		{
		case ADDU:
		{	cout << "ADDU" << endl;
		ALUresult = bitset<32>((unsigned long long)oprand1.to_ulong() + (unsigned long long)oprand2.to_ulong());
		cout << ALUresult << endl;
		break;
		}
		case SUBU:
		{	cout << "SUBU" << endl;
		ALUresult = bitset<32>((unsigned long long)oprand1.to_ulong() - (unsigned long long)oprand2.to_ulong());
		break;
		}
		case AND:
		{	cout << "AND" << endl;
		ALUresult = oprand1 & oprand2;
		break;
		}
		case OR:
		{	cout << "OR" << endl;
		ALUresult = oprand1 | oprand1;
		break;
		}
		case NOR:
		{
			cout << "NOR" << endl;
			ALUresult = ~(oprand1 | oprand2);
			break;
		}
		}
		return ALUresult;
	}
	bitset<32> IALUOperation(bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2, bitset<32> signImmediate)
	{
		switch ((int)ALUOP.to_ulong())
		{
		case 1:
		{//addiu only add oprand1 to signImmed
			ALUresult = bitset<32>((unsigned long long)oprand1.to_ulong() + (unsigned long long)signImmediate.to_ulong());//addiu
			return ALUresult;
		}
		case 4:
		{//beq
			if (oprand1 == oprand2)
			{
				for (int i = 0; i < 30; i++)
				{
					signImmediate[31 - i] = signImmediate[29 - i];
				}
				signImmediate[0], signImmediate[1] = 0, 0;
				return signImmediate;
			}
			else
			{
				return 0;
			}
		}
		}
	}
	bitset<32> JALUOperation(bitset<32> PC, bitset<26> address)
	{
		bitset<32> newPC = 0;
		for (int i = 0; i < 4; i++)
		{
			newPC[31 - i] = bitset<32>((int)PC.to_ulong() + 4)[31 - i];
		}
		for (int i = 0; i < 26; i++)
		{
			newPC[27 - i] = address[25 - i];
		}
		return newPC;

	}
};

class INSMem
{
public:
	bitset<32> Instruction;
	INSMem()
	{
		IMem.resize(MemSize);
		ifstream imem;
		string line;
		int i = 0;
		imem.open("imem.txt");
		if (imem.is_open())
		{
			cout << "Instruction load complete" << endl;
			while (getline(imem, line))
			{
				IMem[i] = bitset<8>(line);
				i++;

			}

		}
		else cout << "Unable to open file";
		imem.close();
	}

	bitset<32> ReadMemory(bitset<32> ReadAddress)
	{
		// implement by you. (Read the byte at the ReadAddress and the following three byte).

		bitset<8> Byte0 = IMem[(int)ReadAddress.to_ulong()];
		bitset<8> Byte1 = IMem[(int)ReadAddress.to_ulong() + 1];
		bitset<8> Byte2 = IMem[(int)ReadAddress.to_ulong() + 2];
		bitset<8> Byte3 = IMem[(int)ReadAddress.to_ulong() + 3];
		for (int i = 0; i < 8; i++)
		{
			Instruction[31 - i] = Byte0[7 - i];
			Instruction[23 - i] = Byte1[7 - i];
			Instruction[15 - i] = Byte2[7 - i];
			Instruction[7 - i] = Byte3[7 - i];
		}
		// cout << Instruction << endl;
		return Instruction;
	}



private:
	vector<bitset<8> > IMem;

};

class DataMem
{
public:
	bitset<32> readdata;
	DataMem()
	{
		DMem.resize(MemSize);
		ifstream dmem;
		string line;
		int i = 0;
		dmem.open("dmem.txt");
		if (dmem.is_open())
		{
			cout << "Data load complete" << endl;
			while (getline(dmem, line))
			{
				DMem[i] = bitset<8>(line);
				i++;
			}
		}
		else cout << "Unable to open file";
		dmem.close();

	}
	bitset<32> MemoryAccess(bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem)
	{
		if (readmem == 1 && writemem == 0)
		{
			bitset<8> Byte0 = (bitset<8>)(DMem[(int)Address.to_ulong()].to_ulong());
			bitset<8> Byte1 = (bitset<8>)(DMem[(int)Address.to_ulong() + 1].to_ulong());
			bitset<8> Byte2 = (bitset<8>)(DMem[(int)Address.to_ulong() + 2].to_ulong());
			bitset<8> Byte3 = (bitset<8>)(DMem[(int)Address.to_ulong() + 3].to_ulong());

			//readdata = Byte0 << 24 | Byte1 << 16 | Byte2 << 8 | Byte3;
			for (int i = 0; i < 8; i++)
			{
				readdata[31 - i] = Byte0[7 - i];
				readdata[23 - i] = Byte1[7 - i];
				readdata[15 - i] = Byte2[7 - i];
				readdata[7 - i] = Byte3[7 - i];
			}
		}
		if (writemem == 1 && readmem == 0)
		{
			bitset<8> Byte0;
			bitset<8> Byte1;
			bitset<8> Byte2;
			bitset<8> Byte3;
			for (int i = 0; i < 8; i++)
			{
				Byte0[i] = WriteData[i];
				Byte1[i] = WriteData[i + 8];
				Byte2[i] = WriteData[i + 16];
				Byte1[i] = WriteData[i + 24];
			}

			DMem[(int)Address.to_ulong()] = Byte3;
			DMem[(int)Address.to_ulong() + 1] = Byte2;
			DMem[(int)Address.to_ulong() + 2] = Byte1;
			DMem[(int)Address.to_ulong() + 3] = Byte0;
		}
		// implement by you.
		return readdata;

	}

	void OutputDataMem()
	{
		ofstream dmemout;
		dmemout.open("dmemresult.txt");
		if (dmemout.is_open())
		{
			for (int j = 0; j< 1000; j++)
			{
				dmemout << DMem[j] << endl;
			}

		}
		else cout << "Unable to open file";
		dmemout.close();

	}

private:
	vector<bitset<8> > DMem;

};

void dumpResults(bitset<32> pc = 0, bitset<5> WrRFAdd = 0, bitset<32> WrRFData = 0, bitset<1> RFWrEn = 0, bitset<32> WrMemAdd = 0, bitset<32> WrMemData = 0, bitset<1> WrMemEn = 0)
{
	ofstream fileout;

	fileout.open("Results.txt", std::ios_base::app);
	if (fileout.is_open())
	{
		cout << "PC " << pc << endl << "WrtRFAdd " << WrRFAdd << endl << "WrtRFData " << WrRFData <<
			endl << "RFWrEn " << RFWrEn << endl << "WrMemADD" << WrMemAdd << endl << "WrMemData" << WrMemData << endl << "WrMemEn" << WrMemEn << endl;

		fileout << pc << ' ' << WrRFAdd << ' ' << WrRFData << ' ' << RFWrEn << ' ' << WrMemAdd << ' ' << WrMemData << ' ' << WrMemEn << endl;

	}
	else cout << "Unable to open file";
	fileout.close();

}
bitset<32> ExtendSignImm(bitset<16> Imm)
{
	bitset<32> ExtendSignImm = 0;
	if (Imm[15] == 1)
	{
		ExtendSignImm = 0xFFFFFFFF;
		for (int i = 0; i < 15; i++)
		{
			ExtendSignImm[15 - i] = Imm[15 - i];
		}
	}
	else
	{
		ExtendSignImm = 0;
		for (int i = 0; i < 15; i++)
		{
			ExtendSignImm[15 - i] = Imm[15 - i];
		}
	}
	return ExtendSignImm;
}


bitset<5> Rs_rTypeAssign(bitset<32> Ins)
{
	bitset<5> Rs = 0;
	for (int i = 0; i < 5; i++)
	{
		Rs[4 - i] = Ins[25 - i];
	}
	return Rs;
}
bitset<5> Rt_rTypeAssign(bitset<32> Ins)
{
	bitset<5> Rt = 0;
	for (int i = 0; i < 5; i++)
	{
		Rt[4 - i] = Ins[20 - i];
	}
	return Rt;
}
bitset<5> Rd_rTypeAssign(bitset<32> Ins)
{
	bitset<5> Rd = 0;
	for (int i = 0; i < 5; i++)
	{
		Rd[4 - i] = Ins[15 - i];
	}
	return Rd;
}
bitset<3> ALUOp_rTypeAssign(bitset<32> Ins)
{
	bitset<3> ALUOp = 0;
	for (int i = 0; i < 3; i++)
	{
		ALUOp[2 - i] = Ins[2 - i];
	}
	return ALUOp;
}
bitset<5> Rs_iTypeAssign(bitset<32> Ins)
{
	bitset<5> Rs = 0;
	for (int i = 0; i < 5; i++)
	{
		Rs[4 - i] = Ins[25 - i];
	}
	return Rs;
}
bitset<5> Rt_iTypeAssign(bitset<32> Ins)
{
	bitset<5> Rt = 0;
	for (int i = 0; i < 5; i++)
	{
		Rt[4 - i] = Ins[20 - i];
	}
	return Rt;
}
bitset<32> SignextendImm_iTypeAssign(bitset<32> Ins)
{
	bitset<32> SignExtendimm = 0;
	if (Ins[15] == 0)
	{
		for (int i = 0; i < 16; i++) {
			SignExtendimm[15 - i] = Ins[15 - i];
		}
	}
	else
	{
		for (int i = 0; i < 16; i++) {
			SignExtendimm[15 - i] = Ins[15 - i];
		}
		for (int i = 0; i < 16; i++)
		{
			SignExtendimm[31 - i] = 1;
		}
	}
	return SignExtendimm;
}
bitset<3> ALUOp_iTypeAssign(bitset<32> Ins)
{
	bitset<3> ALUOp = 0;
	for (int i = 0; i < 3; i++)
	{
		ALUOp[2 - i] = Ins[28 - i];
	}
	return ALUOp;
}
bitset<3> ALUOp_jTypeAssign(bitset<32> Ins)
{
	bitset<3> ALUOp = 0;
	for (int i = 0; i < 3; i++)
	{
		ALUOp[2 - i] = Ins[28 - i];
	}
	return ALUOp;
}
bitset<26> Address_jTypeAssign(bitset<32> Ins)
{
	bitset<26> Address = 0;
	for (int i = 0; i < 26; i++) {
		Address[25 - i] = Ins[25 - i];
	}
	return Address;
}

int main()
{
	RF myRF;
	ALU myALU;
	INSMem myInsMem;
	DataMem myDataMem;
	bitset<32> PC = 0;
	PC = bitset<32>(-4);
	bitset<32> Ins;
	bitset<6> OPcode;
	bitset<3> ALUOp;
	bitset<16> Imm = 0;
	bitset<32> SignExtendimm;
	bitset<32> result;
	bitset<26> address;
	bitset<5> Rd;
	bitset<5> Rt;
	bitset<5> Rs;
	int loopNumber = 0;
	int endloop = 4;
	bool PCchange;
	bitset<5> WrRFAdd;
	bitset<32> WrRFData;
	bitset<1> RFWrEn;
	bitset<32> WrMemAdd;
	bitset<32> WrMemData;
	bitset<1> WrMemEn;
	int i;
	while (1) //each loop body corresponds to one clock cycle.
	{
		WrRFAdd = 0;
		WrRFData = 0;
		RFWrEn = 0;
		WrMemAdd = 0;
		WrMemData = 0;
		WrMemEn = 0;
		if (endloop != 4)
		{
			endloop--;
			cout << "Loop ends in " << endloop << endl;
		}
		if (endloop == 1)
		{
			cout << "Loop ends" << endl;
			break;
		}
		PCchange = false;
		if (loopNumber > 1)
		{
			//stage 3: DataMemory/RF write or read
			if (OPcode == bitset<6>(0))
			{//R-type
				cout << "Stage 3 executes R-type, saving data in RF" << endl;
				WrRFAdd = Rd;
				WrRFData = result;
				RFWrEn = 1;
				WrMemEn = 0;
				myRF.ReadWrite(0, 0, Rd, result, 1);
			}
			else if (OPcode == bitset<6>(2))
			{
				WrMemEn = 0;
				cout << "Stage 3 executes J-type, doing nothing" << endl;
				//J-type doing nothing to the RF and Mem

			}
			else
			{//I-type
				if (OPcode == bitset<6>(9))
				{//addiu
					cout << "Stage 3 executes I-type, saving ADDIU result into RF" << endl;
					WrRFAdd = Rt;
					WrRFData = result;
					RFWrEn = 1;
					WrMemEn = 0;
					myRF.ReadWrite(0, 0, Rt, result, 1);
				}
				else if (OPcode == bitset<6>(4))
				{//beq doing nothing to read or write
					WrMemEn = 0;
					cout << "Stage 3 executes I-type, BEQ doing nothing" << endl;
				}
				else if (OPcode == bitset<6>(35))
				{//lw
					cout << "Stage 3 executes I-type, saving LW result to RF" << endl;
					bitset<32> memoryAddressToRead = myALU.IALUOperation(bitset<3>(1), myRF.ReadData1, myRF.ReadData2, SignExtendimm);
					bitset<32> memeoryValueToRead = myDataMem.MemoryAccess(memoryAddressToRead, 0, 1, 0);
					WrRFAdd = Rt;
					WrRFData = memeoryValueToRead;
					RFWrEn = 1;
					WrMemEn = 0;
					myRF.ReadWrite(0, 0, Rt, memeoryValueToRead, 1);
				}
				else if (OPcode == bitset<6>(43))
				{//sw
					cout << "Stage 3 executes I-type, saving SW result to Data Memory" << endl;
					RFWrEn = 0;
					myRF.ReadWrite(Rt, Rs, 0, 0, 0);
					bitset<32> writeData = myRF.ReadData1;
					bitset<32> memoryAddressToWrite = myALU.IALUOperation(bitset<3>(9), myRF.ReadData2, myRF.ReadData1, SignExtendimm);
					WrMemAdd = memoryAddressToWrite;
					WrMemData = writeData;
					WrMemEn = 1;
					myDataMem.MemoryAccess(memoryAddressToWrite, writeData, 0, 1);
				}
			}
			cout << "RF and Memory write complete" << endl;
		}
		//Stage 2: instruction Decoder and Execute
		if (endloop != 4)
		{
			cout << "HALT detected, stop executing anything" << endl;
			cout << "////////////////////////////////////////////////////////////////////////////////" << endl;
			continue;
		}

		if (loopNumber > 0)
		{
			Ins = myInsMem.ReadMemory(PC); //where is changed
			cout << "Instruction = " << Ins << endl;
			for (int i = 0; i < 6; i++)
			{
				OPcode[5 - i] = Ins[31 - i];

			}
			bitset<1> IsItype;
			if (OPcode == bitset<6>(0))
			{//R type

				cout << "Stage 2 executes R-type" << endl;
				Rs = Rs_rTypeAssign(Ins);
				Rt = Rt_rTypeAssign(Ins);
				Rd = Rd_rTypeAssign(Ins);
				ALUOp = ALUOp_rTypeAssign(Ins);
				myRF.ReadWrite(Rs, Rt, 0, 0, 0);
				result = myALU.RALUOperation(ALUOp, myRF.ReadData1, myRF.ReadData2);
			}

			else if (OPcode == bitset<6>(2))
			{//J type
				cout << "Stage 2 executes J-type" << endl;
				ALUOp = ALUOp_jTypeAssign(Ins);
				address = Address_jTypeAssign(Ins);
				bitset<32> newPC = myALU.JALUOperation(PC, address);
				PC = newPC;
				PCchange = true;

			}
			else if (OPcode == bitset<6>(63))
			{//detect HALT instruction
				cout << "HALT instruction, doing nothing" << endl;
			}
			else
			{//I type
				cout << "Stage 2 executes I-type" << endl;
				Rs = Rs_iTypeAssign(Ins);
				Rt = Rt_iTypeAssign(Ins);
				SignExtendimm = SignextendImm_iTypeAssign(Ins);
				ALUOp = ALUOp_iTypeAssign(Ins);
				myRF.ReadWrite(Rs, Rt, 0, 0, 0);
				switch ((int)OPcode.to_ulong())
				{
				case 9:
				{//addiu
					cout << "Stage 2 executes ADDIU" << endl;
					result = myALU.IALUOperation(ALUOp, myRF.ReadData1, myRF.ReadData2, SignExtendimm);
					break;
				}
				case 4:
				{//beq
					cout << "Stage 2 executes BEQ" << endl;
					PCchange = true;
					PC = bitset<32>((int)PC.to_ulong() + 4 +
						(int)((myALU.IALUOperation(ALUOp, myRF.ReadData1, myRF.ReadData2, SignExtendimm)).to_ulong()));
					break;
				}
				case 35:
				{//lw won't execute here
					cout << "Stage 2 executes LW, doing nothing" << endl;
					break;
				}
				case 43:
				{//sw won't execute here
					cout << "Stage 2 executes SW, doing nothing" << endl;
					break;
				}
				}
			}
			cout << " Register file getting data and Execute complete" << endl;
		}
		//Stage 1: Instruction fetch
		if (endloop == 2) { continue; }
		if (Ins == bitset<32>(0xFFFFFFFF))
		{
			cout << "Get HALT Instruction" << endl;
			//break;
			endloop = 3;
		}

		if (PCchange == false)
		{
			cout << "PC not change" << endl;
			PC = bitset<32>((int)PC.to_ulong() + 4);
		}

		cout << "PC load complete" << endl;
		dumpResults(PC, WrRFAdd, WrRFData, RFWrEn, WrMemAdd, WrMemData, WrMemEn);
		cout << "////////////////////////////////////////////////////////////////////////////////" << endl;
		loopNumber++;
		// At the end of each cycle, fill in the corresponding data into "dumpResults" function to output files.
		// The first dumped pc value should be 0.	
	}
	myRF.OutputRF();
	myDataMem.OutputDataMem();
	return 0;


}
