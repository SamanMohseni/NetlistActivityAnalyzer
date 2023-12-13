/*
*   Author: Seyed Saman Mohseni Sangtabi
*   Student number: 99210067
*   Optinal Assignment
*/

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <map>
#include <set>

using namespace std;

#define NOTSET -1

class Primitives{
public:
	static Primitives& getInstance(){
		static Primitives instance;
		return instance;
	}

	double getP1(string gateType, vector<double>& inputsP1){
		int numberOfInputs = inputsP1.size();
		if (numberOfInputs > 31){
			cerr << "ERROR [invalid circuit]: gates with more than 31 inputs are not supported." << endl;
			exit(1);
		}

		GateFunction gFunc = gateFunction[gateType];
		double p1 = 0;
		for (int i = 0; i < (1 << numberOfInputs); i++){ // generate all possible gate inputs
			bool logicResult = gFunc(i, numberOfInputs);
			if (logicResult){ // input combination results in logic 1 output
				// add the probability of this combination to p1
				double probability = 1;
				for (int bit = 0; bit < numberOfInputs; bit++){
					probability *= (i & (1 << bit)) ? inputsP1[bit] : (1 - inputsP1[bit]);
				}
				p1 += probability;
			}
		}

		return p1;
	}

	bool gateValid(string gateType){
		return (gateFunction.count(gateType) > 0);
	}

private:
	Primitives(){
		gateFunction.insert(make_pair("buf", Primitives::buf));
		gateFunction.insert(make_pair("not", Primitives::not));

		gateFunction.insert(make_pair("and", Primitives::and));
		gateFunction.insert(make_pair("nand", Primitives::nand));

		gateFunction.insert(make_pair("or", Primitives::or));
		gateFunction.insert(make_pair("nor", Primitives::nor));

		gateFunction.insert(make_pair("xor", Primitives::xor));
		gateFunction.insert(make_pair("xnor", Primitives::xnor));

		// you should insert defined primitive gate functions here: 


		//---------------------------------------------------------
	}

	Primitives(Primitives const&);
	void operator=(Primitives const&);

	static bool buf(unsigned int inputBits, unsigned int numberOfBits){
		if (numberOfBits != 1){
			cerr << "ERROR [invalid circuit]: more than 1 input connected to buf gate." << endl;
			exit(1);
		}
		return (inputBits & 1);
	}

	static bool not(unsigned int inputBits, unsigned int numberOfBits){
		if (numberOfBits != 1){
			cerr << "ERROR [invalid circuit]: more than 1 input connected to not gate." << endl;
			exit(1);
		}
		return !(inputBits & 1);
	}

	static bool and(unsigned int inputBits, unsigned int numberOfBits){
		return (inputBits == (1 << numberOfBits) - 1);
	}

	static bool nand(unsigned int inputBits, unsigned int numberOfBits){
		return (inputBits != (1 << numberOfBits) - 1);
	}

	static bool or(unsigned int inputBits, unsigned int numberOfBits){
		return (inputBits != 0);
	}

	static bool nor(unsigned int inputBits, unsigned int numberOfBits){
		return (inputBits == 0);
	}

	static bool xor(unsigned int inputBits, unsigned int numberOfBits){
		inputBits ^= inputBits >> 16;
		inputBits ^= inputBits >> 8;
		inputBits ^= inputBits >> 4;
		inputBits ^= inputBits >> 2;
		inputBits ^= inputBits >> 1;
		return (inputBits & 1);
	}

	static bool xnor(unsigned int inputBits, unsigned int numberOfBits){
		return !xor(inputBits, numberOfBits);
	}

	// you can defined more primitive gate functions here: 


	//----------------------------------------------------

	typedef bool(*GateFunction)(unsigned int inputBits, unsigned int numberOfBits);
	map<string, GateFunction> gateFunction;
};

struct Gate{
	string label;
	string type;
	vector<string> inputWireList;
};

class Circuit {
public:
	Circuit(){
	}

	void getNetList(){
		cout << "Enter the net list using the following format, without < and >." << endl;
		cout << "<gate type> <gate name> (<output wire label>, <input 1 wire label>, ..., <input n wire label>);" << endl;
		cout << "Finish the list by entering 'end' without quotes on a new line." << endl;
		cout << "Enter:" << endl << endl;

		while (true){
			string line;
			getline(cin, line);

			string gateType;
			string gateLabel;
			string outputWireLabel;
			vector<string> gateInputWireList;

			int index;
			index = pickUntil(line, ' ', '\n', 0, gateType).first;
			if (gateType.size() == 0){
				continue;
			}
			if (gateType == "end"){
				break;
			}
			if (!Primitives::getInstance().gateValid(gateType)){
				cerr << "ERROR [invalid input]: gate type not in primitives list." << endl;
				exit(1);
			}

			index = pickUntil(line, '(', '(', index + 1, gateLabel).first;
			if (gateList.count(gateLabel)){
				cerr << "ERROR [invalid input]: gate label already entered." << endl;
				exit(1);
			}
			gateList.insert(gateLabel);

			index = pickUntil(line, ',', ',', index + 1, outputWireLabel).first;
			if (!wireP1Fanout.count(outputWireLabel)){
				wireP1Fanout.insert(make_pair(outputWireLabel, make_pair(NOTSET, 0)));
			}

			while (true){
				string tmp;
				pair<int, char> target = pickUntil(line, ',', ')', index + 1, tmp);
				index = target.first;
				gateInputWireList.push_back(tmp);
				if (!wireP1Fanout.count(tmp)){
					wireP1Fanout.insert(make_pair(tmp, make_pair(NOTSET, 0)));
				}
				if (target.second == ')'){
					break;
				}
			}
			string tmp;
			pair<int, char> target = pickUntil(line, ';', ';', index + 1, tmp);
			index = target.first;
			if (target.second != ';' || tmp.size()){
				cerr << "ERROR [invalid input]: ';' expected." << endl;
				exit(1);
			}
			target = pickUntil(line, '\n', '\n', index + 1, tmp);
			if (tmp.size()){
				cerr << "ERROR [invalid input]: extra data after ';'." << endl;
				exit(1);
			}

			Gate gate;
			gate.type = gateType;
			gate.label = gateLabel;
			gate.inputWireList = gateInputWireList;

			if (parentGate.count(outputWireLabel)){
				cerr << "ERROR [invalid input]: 2 output wires conflict." << endl;
				exit(1);
			}
			
			parentGate.insert(make_pair(outputWireLabel, gate));
		}

		discoverInputs();
	}

	void getInputsP1(){
		cout << endl;
		cout << "Enter wire label followed by it's P1 value, seperated by space." << endl;
		cout << inputWires.size() << " input wires detected, " << inputWires.size() << " wireLabel P1 pairs expected." << endl;
		cout << "Enter:" << endl;

		for (int i = 0; i < inputWires.size(); i++){
			string wireLabel;
			double p1;
			cin >> wireLabel >> p1;
			if (!inputWires.count(wireLabel)){
				cerr << "ERROR [invalid input]: the specified wire is not an input." << endl;
				exit(1);
			}
			if (wireP1Fanout[wireLabel].first != NOTSET){
				cerr << "ERROR [invalid input]: the wire's P1 is already specified." << endl;
				exit(1);
			}
			wireP1Fanout[wireLabel].first = p1;
		}
	}

	double getActivity(){
		for (map<string, pair<double, int>>::iterator it = wireP1Fanout.begin(); it != wireP1Fanout.end(); ++it){
			processWire(it->first);
		}

		double totalActivity = 0;
		for (map<string, pair<double, int>>::iterator it = wireP1Fanout.begin(); it != wireP1Fanout.end(); ++it){
			double p1 = it->second.first;
			int fanout = it->second.second;
			totalActivity += p1 * (1 - p1) * fanout;
		}

		return totalActivity;
	}

	void showActivityDetails(){
		cout << "Activity Details: " << "//////////////////////////////////////" << endl;
		for (map<string, pair<double, int>>::iterator it = wireP1Fanout.begin(); it != wireP1Fanout.end(); ++it){
			double p1 = it->second.first;
			int fanout = it->second.second;
			double alpha = p1 * (1 - p1);
			double wireActitty = alpha * fanout;

			cout << "Wire " << it->first << ": P1 = " << p1 << ", Fanout = " << fanout << 
				", alpha = " << alpha << ", Actitty = " << wireActitty << endl;
		}
		cout << "////////////////////////////////////////////////////////" << endl;
	}

private:
	double processWire(string wireLabel){
		if (wireP1Fanout[wireLabel].first != NOTSET){ // check if P1 already set
			return wireP1Fanout[wireLabel].first;
		}
		Gate parent = parentGate[wireLabel];
		vector<double> inputsP1;
		for (int i = 0; i < parent.inputWireList.size(); i++){
			wireP1Fanout[parent.inputWireList[i]].second++;
			inputsP1.push_back(processWire(parent.inputWireList[i]));
		}

		wireP1Fanout[wireLabel].first = Primitives::getInstance().getP1(parent.type, inputsP1);
		return wireP1Fanout[wireLabel].first;
	}

	void discoverInputs(){
		for (map<string, pair<double, int>>::iterator it = wireP1Fanout.begin(); it != wireP1Fanout.end(); ++it){
			string wireLabel = it->first;
			if (!parentGate.count(wireLabel)){
				inputWires.insert(wireLabel);
			}
		}
	}

	pair<int, char> pickUntil(string& input, char target1, char target2, int startIndex, string& pick){
		pick.clear();
		int i;
		for (i = startIndex; i < input.size(); i++){
			if (input[i] == target1 || input[i] == target2){
				return make_pair(i, input[i]);
			}
			char c = input[i];
			if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '$' || c == ' ')){
				cerr << "ERROR [invalid input]: only alphanumeric characters plus '_' and '$' are allowed." << endl;
				exit(1);
			}
			if (input[i] != ' '){
				pick.push_back(input[i]);
			}
		}
		return make_pair(i, 0);
	}

private:
	map<string, Gate> parentGate; // maps wire_label to its source gate: <gate_output_label, gate>
	map<string, pair<double, int>> wireP1Fanout; // maps wire_lebel to <its probability of being logic 1, and fanout>

	set<string> gateList;
	set<string> inputWires;
};

int main(){

	Circuit c;
	c.getNetList();
	c.getInputsP1();

	cout << endl << endl;
	cout << "////////////////////////////////////////////////////////" << endl;
	cout << "Total Activity: " << c.getActivity() << endl;
	cout << "////////////////////////////////////////////////////////" << endl << endl;

	c.showActivityDetails();

	return 0;
}