
#ifndef ProgressIndicatorCallbackH
#define ProgressIndicatorCallbackH

class OperationCallback{

protected:

	void (__closure *pProgressCallback)(void*,unsigned);

	virtual void SetProgressCallback(void (__closure *p)(void*, unsigned))=0;// { pProgressCallback = p; } ;

};

 #endif