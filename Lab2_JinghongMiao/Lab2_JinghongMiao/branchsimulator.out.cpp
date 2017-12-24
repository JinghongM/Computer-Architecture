#define _GLIBCXX_USE_C99 1
#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
#include <sstream>
#include <cstdio>
#include <string>
#include <math.h>
using namespace std;
bitset<32> hexStringToBitset(string input)
{
	unsigned int PC;
	std::stringstream hexDecimalConvertor;
	hexDecimalConvertor << std::hex << input;
	hexDecimalConvertor >> PC;
	bitset<32> PCbitsets(PC);
	return PCbitsets;
}
bool judgement(int predictor)
{
	switch (predictor)
	{
	case 11: 
		//cout << "Strong taken" << endl;
		return 1;
	case 10: 
		//cout << "Weak taken" << endl;
		return 1;
	case 01:
		//cout << "Weak not taken" << endl;
		return 0;
	case 00:
		//cout << "Strong not taken" << endl;
		return 0;
	}
}
int getPCNumber(string tracename)
{
	int totalPC = 0;
	ifstream trace;
	string str;
	trace.open(tracename.c_str());
	if(trace.is_open())
	{
		while (getline(trace, str))
		{
			totalPC++;
		}
	}
	
	return totalPC;
}
int main(int argc,char**argv)
{ 
	//string config_newFileName = "./" + string(argv[1]);
	//string traceFileName = "./" + string(argv[2]);
	//cout<<config_newFileName<<endl;
	//cout<<traceFileName<<endl;	
	int m = 0;//LSB number
	int k = 0;//BHR number
	std::string str;
	//std::ifstream conf(config_newFileName.c_str());
	std::ifstream conf("./config_new.txt");
	int iOnly0And1 = 0;
	char insertArray[2][10];
	while(std::getline(conf, str))
	{
		if(iOnly0And1 ==0)
		{
			stringstream ss(str);
			ss>>m;
			iOnly0And1 = 1;
		}
		else
		{
			stringstream ss(str);
			ss>>k;
		}
		
	}
	//load m and k
	
	int** SC = new int*[(int)pow(2,m)];
	for (int row = 0; row < pow(2,m); row++)
	{
		SC[row] = new int[(int)pow(2,k)];
	}

	for (int row = 0; row < pow(2,m); row++)
	{
		for (int column = 0; column < pow(2,k); column++)
		{
			SC[row][column] = 11;
		}
	} 
	//create a 2*m rows, 2*k columns arrays.
	int* BHR = new int[k];
	for (int BHRindex = 0; BHRindex < k; BHRindex++)
	{
		BHR[BHRindex] = 1;
	}
	//int PCNumber = getPCNumber(traceFileName);
	int PCNumber = getPCNumber("./trace.txt");
	int *result = new int[PCNumber];
	//cout << "PCNumber=" << PCNumber << endl;
	//std::ifstream trace(traceFileName.c_str());
	std::ifstream trace("./trace.txt");
	int record = 0;
	int matchNumber = 0;
	while (std::getline(trace, str))
	{
		string arr[2];
		int i = 0;
		stringstream ssin(str);
		while (ssin.good() && i < 2)
		{
			ssin >> arr[i];
			++i;
		}
		bitset<32>PCbitset = hexStringToBitset(arr[0]);
		//cout << "PC bitset=" << PCbitset << endl;
		int row = 0;
		for (int index = 0; index < m; index++)
		{
			row = pow(2, index)*PCbitset[index] +row;
		}
		//cout << "row=" << row << endl;
		int column = 0;
		for (int index = 0; index < k; index++)
		{
			column = pow(2, k-index-1)*BHR[index] + column;
		}
		//cout << "BHR:" << endl;
		/*for (int BHRindex = 0; BHRindex < k; BHRindex++)
		{
			cout << BHR[BHRindex] << "    ";
		}*/
		//cout << endl;
		//cout << "column=" << column << endl;
		bool predictor = judgement(SC[row][column]);
		result[record] = predictor;
		for (int BHRindex = k-1; BHRindex >0; BHRindex--)
		{
			BHR[BHRindex] = BHR[BHRindex - 1];
		}
		//swap BHR to next slot
		stringstream recentBranchStream(arr[1]);
		int realBranch = 0;
		recentBranchStream >> realBranch;
		BHR[0] = realBranch;
		//cout << "New BHR:" << endl;
		/*for (int BHRindex = 0; BHRindex < k; BHRindex++)
		{
			cout << BHR[BHRindex] << "    ";
		}
		cout << endl;
		*/
		//put read branch desicion BRH[0] in realBranch
		//cout << "Real decision=" << arr[1] << endl;
		//cout << "Predict =" << predictor << endl;
		if (predictor == realBranch)
		{
			matchNumber++;
			switch (SC[row][column])
			{
			case 11: 
				//cout << "row="<< row<< "," << "column=" <<column << "=" << SC[row][column] << ",doing nothing" << endl;
				break;
			case 10:
				//cout << "Change" << "row=" << row << "," << "column=" << column << " from " << SC[row][column] << " to " << 11 << endl;
				SC[row][column] = 11;
				break;
			case 01:
				//cout << "Change" << "row=" << row << "," << "column=" << column << " from " << SC[row][column] << " to " << 00 << endl;
				SC[row][column] == 00;
				break;
			case 00:
				//cout << "row=" << row << "," << "column=" << column << "=" << SC[row][column] << ",doing nothing" << endl;
				break;
			}
		}
		else
		{
			switch (SC[row][column])
			{
			case 11:
				//cout << "Change" << "row=" << row << "," << "column=" << column << " from " << SC[row][column] << " to " << 10 << endl;
				SC[row][column]=10;
				break;
			case 10:
				//cout << "Change" << "row=" << row << "," << "column=" << column << " from " << SC[row][column] << " to " << 00 << endl;
				SC[row][column] = 00;
				break;
			case 01:
				//cout << "Change" << "row=" << row << "," << "column=" << column << " from " << SC[row][column] << " to " << 11 << endl;
				SC[row][column] == 11;
				break;
			case 00:
				//cout << "Change" << "row=" << row << "," << "column=" << column << " from " << SC[row][column] << " to " << 01 << endl;
				SC[row][column] == 01;
				break;
			}
		}
		record++;

	}
	ofstream rfout;
	std::ofstream outfile("trace.txt.out");
	rfout.open("trace.txt.out", std::ios_base::app);
	if (rfout.is_open())
	{
		for (int j = 0; j<PCNumber; j++)
		{
			rfout << result[j] << endl;
		}

	}
	else cout << "Unable to open file";
	rfout.close();
	cout << 1.0 - float(matchNumber)/float(PCNumber) << endl;

}
