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

#ifndef WorkerThreadH
#define WorkerThreadH

#include <vcl.h>

class TWorkerThread : public TThread {

	void (__closure * pWorkerMethod)();

	void (__closure * pAllThreadsFinished)();

	void (__closure * pCurrThreadFinished)(unsigned);

	int arg, tId;

	static unsigned numThreads;

	static TWorkerThread * pCurrThread;

   public:

	__fastcall TWorkerThread( bool suspended ): TThread( suspended ){
		numThreads++;
		tId = numThreads;
		FreeOnTerminate = true;
	}

	virtual void __fastcall Execute();

	void SetWorkerMethodCallback(void (__closure *p)()) {
		pWorkerMethod = p;
	}

	void SetArgument(int a){ arg = a; }


	void __fastcall AllThreadsFinished(){

	  if(pAllThreadsFinished)   pAllThreadsFinished();
	}
	void NotifyAllThreadsFinished(){

	  Synchronize(&AllThreadsFinished);
	}
	
	void __fastcall CurrentThreadFinished(){

	  if(pCurrThreadFinished)   pCurrThreadFinished(tId);
	}
	void NotifyCurrentThreadFinished(){

	  Synchronize(&CurrentThreadFinished);
	}

	void SetAllThreadsFinishedCallback(void (__closure *pC)()){
		pAllThreadsFinished = pC;
	}

	void SetCurrentThreadFinishedCallback(void (__closure *pC)(unsigned)){
		pCurrThreadFinished = pC;
	}

	unsigned GetThreadId() const { return tId; }

	static unsigned GetNumThreads() { return numThreads; }

	static void ResetThreadCounter() { numThreads = 0; }

	static TWorkerThread &GetCurrentThread() { return *pCurrThread; }

	static unsigned GetCurrentThreadId() { return pCurrThread->tId; }

	static unsigned GetNumCores();

};
unsigned TWorkerThread::numThreads = 0;
TWorkerThread *TWorkerThread::pCurrThread = 0;
#endif
