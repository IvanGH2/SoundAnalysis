
#ifndef ProgressIndicatorCallbackH
#define ProgressIndicatorCallbackH

struct ProgIndicatorCallback{

void (__closure *pProgIndClbk)(unsigned);

void SetProgressIndicatorCallback(void (__closure *pCallback)(unsigned)){ pProgIndClbk = pCallback; }

};

#endif
