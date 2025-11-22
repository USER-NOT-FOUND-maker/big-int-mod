#include <stdint.h>
#include <string>
#include <iostream>

using namespace std;

typedef int8_t i8;
typedef unsigned int uint32;
typedef int int32;

#define aoc amountOfChunks
#define false 0
#define true 1 

// we need to do this because we are NOT wasting compile-time on linking "stdbool.h"

int32 cmpStrings(string s1, string s2){
	if (s1[0] == '-' && s2[0] != '-') {
		return -1;
	}
	else if (s1[0] != '-' && s2[0] == '-') {
		return 1;
	}

	if (s1.size() > s2.size()) { return -1; }
	else if (s1.size() < s2.size()) { return 1; }

	for (uint32 i = 0; i < s1.size(); i++){ // this absolutely has to change (to support numbers with more than 2^32 digits, will do once addNums is working
		int codeOne = (int) s1[i];
		int codeTwo = (int) s2[i];

		if (codeOne > codeTwo){
			return -1;
		}
		else if (codeOne < codeTwo){
			return 1;
		}
	}

	return 0;
}

void printIntArr(int* arr, uint32 len){
	for (uint32 i = 0; i < len; i++){
		printf("[%d], ",*(arr+i));
	}
}

bool isDig(char c){
	return (int) c > 57 || (int) c < 48;
}

bool isNum(string val){
    char* ptr = &val[0];

    if (isDig(*ptr) && *ptr != '-') { return false; }

    while (*ptr++ != '\0') {
        if (isDig(*ptr) && *ptr != '\0') { return false; }
    }

    return true;
}

string constructNString(char c, uint32 n){
	string s;

	for (uint32 i = 0; i < n; i++){
		s += c;
	}
	return s;
}

string leftPad(string s, char c, uint32 n){
	return constructNString(c,n) + s;
}

string removeTrailingZeros(string s){
	if (s == "0" && s.size() == 1) {
		return s;
	}

	uint32 index = 0; // stores where the last 0 is

	while (s[index] == '0' && index != (s.size() - 1)) {
		index++;
	}

	return s.substr(index);
}

class number{
private:
    i8* chunks;
    uint32 amountOfChunks;
	int32 isNeg;

public:
    number(string val="0"){
        if (!(isNum(val))) { val = "0"; }

		val = removeTrailingZeros(val);

        chunks = (i8*) malloc(val.length());
        aoc = val.size();

        for (uint32 i = 0; i < aoc; i++){
            *(chunks + i) = (i8) ((i8) val[i]) - 48;
        }

		if (val[0] == '-') {
			isNeg = true;
		}
		else {
			isNeg = false;
		}
    }

    void setVal(string value){
		cout << "VALUE BEFORE ISNUM " << value << endl;

        if (!isNum(value)) { value = "0"; }

		cout << "VALUE AFTER ISNUM " << value << endl;

		value = removeTrailingZeros(value);

        chunks = (i8*) realloc(chunks, value.length());
        aoc = value.length();

        for (uint32 i = 0; i < aoc; i++){
            *(chunks + i) = (i8) ((i8) value[i]) - 48;
        }

		if (value[0] == '-') {
			isNeg = true;
		}
		else {
			isNeg = false;
		}
    }

    string getString() const{
        string representative;

        for (uint32 i = 0; i < aoc; i++){
            representative += (char) (*(chunks+i) + 48);
        }

		if (isNeg){
			return '-' + representative;
		}

        return representative;
    }

	void fixNum(){
		i8* chunkCopy = (i8*) malloc(aoc+1);
		
		*(chunkCopy) = 0;
		for (uint32 i = 0; i < aoc; i++){
			*(chunkCopy + i + 1) = *(chunks + i);
		}

		i8* endOfChunk = (i8*) (chunkCopy + aoc);

		while (endOfChunk != chunkCopy){
			if (*endOfChunk > 9){
				*endOfChunk -= 10;
				*(endOfChunk - 1) += 1;
			}
			endOfChunk--;
		}

		chunks = (i8*) realloc(chunks,aoc+1);
		aoc += 1;

		uint32 index = 0; // stores what index the last '0' is

		while (*(chunkCopy + index) == 0) { index++; }

		for (uint32 i = 0; i < aoc; i++){
			*(chunks + i) = *(chunkCopy + i);
		}

		for (uint32 i = index; i < aoc; i++){
			*(chunks - index + i) = *(chunks + i);
		}

		free(chunkCopy);

		chunks = (i8*) realloc(chunks,aoc-index);
		aoc -= index;
	}

    void debugInfo() const {
		cout << "THE DEBUG INFORMATION OF THIS NUMBER BELOW" << endl;

        string representative = getString();

        cout << endl << "this numbers representative is " << representative << endl;
        cout << endl << "this numbers 'amountOfChunks' variable is " << aoc << endl;
        cout << endl << "this numbers array of digits is ";
        for (uint i = 0; i < aoc; i++){
            printf("[%d], ",*(chunks+i)); // THIS time we have to use printf so we can catch garbage values if they occur
        }
		cout << endl << "this numbers chunks = " << (void *) chunks;
		cout << endl;
    }

    friend ostream& operator<<(ostream& os, const number& n){
        for (uint32 i = 0; i < n.aoc; i++) {
            os << (char)(n.chunks[i] + 48);
		}
        return os;
    }

	number& operator=(number const& other){
		this->setVal(other.getString());
		return *this;
	}

	bool operator==(number const other) const{
		return this->getString() == other.getString();
	}

	bool operator>(number const other) const{
		string num1 = this->getString();
		string num2 = other.getString();

		return cmpStrings(num1,num2) == -1;
	}

	bool operator<(number const other) const{
		string num1 = this->getString();
		string num2 = other.getString();

		return cmpStrings(num1,num2) == 1;
	}

	bool operator!=(number const other) const{
		return !(this->getString() == other.getString());
	}

	number operator+(number n){
		number result; // stores the result of the addition
		string resultNum; // stores the default number of result (but does change, can and probably will be omitted
		number thisCP;
		number nCP;

		if (this->aoc > n.aoc) {
			result.chunks = (i8* ) realloc(result.chunks,this->aoc + 1);
			result.aoc = this->aoc+1;
			resultNum = constructNString('0',result.aoc);
			result.setVal(resultNum);

			thisCP = *this;

			nCP.chunks = (i8*) realloc(nCP.chunks,this->aoc);
			nCP.aoc = this->aoc;
			nCP.setVal(leftPad(n.getString(),'0',nCP.aoc - n.aoc));
		}
		else if (this->aoc < n.aoc) {
			result.chunks = (i8* ) realloc(result.chunks,n.aoc + 1);
			result.aoc = n.aoc+1;
			resultNum = constructNString('0',result.aoc);
			result.setVal(resultNum);

			nCP = n;

			thisCP.chunks = (i8*) realloc(thisCP.chunks,n.aoc);
			thisCP.aoc = n.aoc;
			thisCP.setVal(leftPad(this->getString(),'0',thisCP.aoc - this->aoc));
		}
		else {
			result.chunks = (i8* ) realloc(result.chunks,n.aoc + 1);
			result.aoc = n.aoc+1;
			resultNum = constructNString('0',result.aoc);
			result.setVal(resultNum);

			thisCP = *this;
			nCP = n;
		}

	for (uint32 i = 0; i < nCP.aoc; i++){
		*(result.chunks + i + 1) = *(nCP.chunks + i) + *(thisCP.chunks + i);
	}

	result.fixNum();

	return result;
	}

	number& operator+=(number const& n){
		*this = *this + n;
		return *this;
	}


	number operator*(const number other){

/*

YOU CAN TRY THIS NEW IDEA

IF YOU DOING FOR EXAMPLE 12 * 12

SPLIT THE 12 INTO 10 AND 2

THEN DO (12 * 10) + (12 * 2)

SINCE OBVIOUSLY THIS ISNT AS SIMPLE AS YOU WANT

OBVIOUSLY, there are issues, that being that if we have for example 12 * 212, when we do

12 * 200

we have to add the appropriate 0s and THEN multiply by, so something like

(12 add the zeros) * 2
*/
		number thisCP(this->getString());
		number iteration("1");

		string thisString,otherString;

		thisString = thisCP.getString();
		otherString = other.getString();

		if (thisString == "0" || otherString == "0") { 
			thisCP.setVal("0");
			return thisCP;
		}

		if (thisString == "1"){
			return other;
		}
		else if (otherString == "1"){
			return thisCP;
		}

		while (iteration < other){
			thisCP += *this;
			iteration += number("1");
		}
		return thisCP;
	}

	number operator-(number n){
		number result; // stores the result of the addition
		string resultNum; // stores the default number of result (but does change, can and probably will be omitted
		number thisCP;
		number nCP;

		if (n == *this) {
			return thisCP;
		}

		if (this->aoc > n.aoc) {
			result.chunks = (i8* ) realloc(result.chunks,this->aoc + 1);
			result.aoc = this->aoc+1;
			resultNum = constructNString('0',result.aoc);
			result.setVal(resultNum);

			thisCP = *this;

			nCP.chunks = (i8*) realloc(nCP.chunks,this->aoc);
			nCP.aoc = this->aoc;
			nCP.setVal(leftPad(n.getString(),'0',nCP.aoc - n.aoc));
		}
		else if (this->aoc < n.aoc) {
			result.chunks = (i8* ) realloc(result.chunks,n.aoc + 1);
			result.aoc = n.aoc+1;
			resultNum = constructNString('0',result.aoc);
			result.setVal(resultNum);

			nCP = n;

			thisCP.chunks = (i8*) realloc(thisCP.chunks,n.aoc);
			thisCP.aoc = n.aoc;
			thisCP.setVal(leftPad(this->getString(),'0',thisCP.aoc - this->aoc));
		}
		else {
			result.chunks = (i8* ) realloc(result.chunks,n.aoc + 1);
			result.aoc = n.aoc+1;
			resultNum = constructNString('0',result.aoc);
			result.setVal(resultNum);

			thisCP = *this;
			nCP = n;
		}

	for (uint32 i = 0; i < nCP.aoc; i++){
		*(result.chunks + i + 1) = *(thisCP.chunks + i) - *(nCP.chunks + i);
	}

	result.fixNum();

	return result;
	}

};

void fibonacci(){
	number a;
	number b("1");
	number tmp;

	for (uint32 i = 0; i < 10000; i++){
		tmp = b;
		b = b + a;
		a = tmp;

		cout << b << "\n";
	}
}

int main(){
	number a;
	number b;

	string aS, bS;

	cout << "enter aS: ";
	cin >> aS;
	cout << endl << "enter bS: ";
	cin >> bS;

	a.setVal(aS);
	b.setVal(bS);

	number c = a - b;

//	cout << endl << c << endl;

    return 0;
}
