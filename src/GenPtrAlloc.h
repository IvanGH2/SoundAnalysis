#ifndef GenPtrAllocH
#define GenPtrAllocH

template<class T>
class GenPtrAlloc{

	 T **ptr, *p;
	 bool contMem; //contiguous memory array
	 int numRows;
	 enum ObjType { OBJECT, SINGLE_ARRAY, DOUBLE_ARRAY, DOUBLE_ARRAY_CONT };
	 ObjType objType;
	 bool ownershipTransferred; //true -the caller is responsible for deallocating the memory, false - GenPtrAlloc will deallocate the memory

	 public:

	 void FreeResources(){

		if(ownershipTransferred) return; //the caller should deallocate memory

		switch(objType){

			case DOUBLE_ARRAY:
				if(ptr){
					for(int i=0;i<numRows;i++){
						delete [] ptr[i];
						ptr[i] = 0;
					}
				delete [] ptr;
				}
				break;
			case DOUBLE_ARRAY_CONT:
				if(ptr){
					if(ptr[0]) delete [] ptr[0];

					delete [] ptr;  ptr=0;
				}
				break;
			case SINGLE_ARRAY:
				if(p) { delete [] p;  p=0; }
				break;
			case OBJECT:
				if(p) delete p;
		}

	 }
	 ~GenPtrAlloc(){
		 FreeResources();
	 }
	 GenPtrAlloc() : ownershipTransferred(true) {}

	 GenPtrAlloc(unsigned r, unsigned c) : ownershipTransferred(true) {
		numRows = r;
		ptr = new T*[r];
		for(int i=0; i<r;i++)
			ptr[i] = new T[c];
		objType = DOUBLE_ARRAY;
	 }
	 GenPtrAlloc(unsigned r, unsigned c, bool cont) : ownershipTransferred(true) {

		contMem = cont;
		numRows = r;
		ptr = new T*[r];
		T *pMemPool = new T[r*c];
		for(int i=0,j=0; i<r;i++, j+=c)
			ptr[i] = pMemPool+j;
		objType = DOUBLE_ARRAY_CONT;
	 }
	 GenPtrAlloc(unsigned numElem) : ownershipTransferred(true){
		p = new T[numElem];
		objType = SINGLE_ARRAY;
	 }

	 GenPtrAlloc(unsigned numElem, bool ownTransferred) {
		p = new T[numElem];
		objType = SINGLE_ARRAY;
		ownershipTransferred = ownTransferred;
	 }
	 /*GenPtrAlloc(): ownershipTransferred(true){
		p = new T;
		objType = OBJECT;
	 } */
	 T *GetPtr(unsigned numElem){
		p = new T[numElem];
		objType = SINGLE_ARRAY;

		return p;
	 }

	 T **GetPtrDbl() 	const 	{ return ptr; 		}

	 T  *GetPtr() 		const 	{ return p; }

	 void SetPtr(T *ptr) { p = ptr; }

	 void SetPtrDbl(T **ptr) { this->ptr = ptr; }

	 template<class T>
	 static void DeallocateArray(T **p, unsigned numRows){ //deallocating a double array
		if(p && numRows){
			for(int i=0;i<numRows;i++){
				delete [] p[i];
				}
			delete [] p;
			}
	 }

	 template<class T>
	 static void DeallocateArray(T **p){  //deallocating a double cont array
		if(p){
			if(p[0])  delete [] p[0];

				delete [] p;
			}
	 }

	 template<class T>
	 static void DeallocateArray(T *p){  //deallocating a single array
		if(p){
			delete [] p;
			}
	 }
	 template<class T>
	 static void DeallocateObject(T *p){  //deallocating an object
		if(p)	delete  p;
	 }
	 void DeallocateArray(){ FreeResources(); }

	 void TransferOwnershipToCaller(bool transferred){ ownershipTransferred = transferred; }

	 bool IsOwnershipTransferredToCaller() const { return ownershipTransferred; }
};

#endif
