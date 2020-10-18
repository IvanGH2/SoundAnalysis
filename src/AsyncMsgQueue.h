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

#ifndef AsyncMsgQueueH
#define AsyncMsgQueueH

#include "vector.h"
/*
this class is used when using background threads
typically GetValue() is used by the main thread, whereas AddValue
by a background thread
*/
class AsyncMsgQueue{

	vector<unsigned> vMsgVals;
	unsigned msgSize;
public:

	AsyncMsgQueue(){ msgSize=0; }

	void AddValue(unsigned value){
	   vMsgVals.push_back(value);
	   msgSize++;
	}
	//GetValue returns the last value and deletes it from the message queue
	unsigned GetValue() {

		unsigned value=0;
		if( msgSize>0 ) {
		value = vMsgVals.at(msgSize-1);
		vMsgVals.pop_back();
		msgSize--;
		}
		return value;
	}
};

#endif