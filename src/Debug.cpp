//---------------------------------------------------------------------------


#pragma hdrstop

#include "Debug.h"

//---------------------------------------------------------------------------

Debugger::Debugger() :
outFile(GetOutStream()),
newLine('\n')
{
}
Debugger &Debugger::GetInstance(){

	static Debugger debugger;

	return debugger;
}
void Debugger::Log(const char *d, unsigned len){

   outFile.write((const char *)d, len);
   outFile << newLine;
}
/*void Debugger::Log(const char *k, const char *v){

   outFile << k << ": " << v << newLine;
}   */
void Debugger::Log(const char *k, float v, unsigned len){

   char val[6];
   sprintf(val, "%f", v);
   outFile.write((const char *)k, len) ;
   outFile << ": " << val << newLine;
}
void Debugger::Log(const std::string &str){

   outFile <<  str << newLine;
}
#pragma package(smart_init)
