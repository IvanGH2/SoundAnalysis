/*	Copyright 2020 Ivan Balen
	This file is part of the Sound analysis library.
	The Sound analysis library is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	The Sound analysis library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the Lesser GNU General Public License
	along with the Sound analysis library.  If not, see <http://www.gnu.org/licenses/>.*/
//---------------------------------------------------------------------------

#ifndef DebugH
#define DebugH
#include "stdio.h"
#include <iostream>
#include <fstream>
#include <sstream>
//---------------------------------------------------------------------------
class Debugger{

	ofstream &outFile;
	const char newLine;
	ofstream &GetOutStream(){ return *( new ofstream("debug.log") ); }
	Debugger();

    public:

	~Debugger(){
	   delete &outFile;
	}
	static Debugger &GetInstance();

	void Log(const char *v, unsigned len);

	void Log(const char *k, float v, unsigned len);

	void Log(const std::string& );

	void Close() { outFile.close(); }

};
//Debugger *Debugger::pDebugger = 0;
#endif
