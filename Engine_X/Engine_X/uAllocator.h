// uAllocator.h

#ifndef UALLOCATOR_H_INCLUDED
#define UALLOCATOR_H_INCLUDED

template <typename T>
class Allocator {
public:
	Allocator(unsigned int count=1024) 
	{
		mCount = count;
		mCurrent = 0;
		mBlock = new T[mCount];
		for(unsigned int i = 0; i < mCount; ++i) 
		{
			mFree.push_back((mBlock + i));
		}
	};

	~Allocator() 
	{
		if(mBlock) 
		{
			delete[] mBlock;
			mBlock = nullptr;
		}
	};

	T* Allocate() 
	{
		assert(!mFree.empty());
		T* obj = mFree.front();
		mFree.pop_front();
		return obj;
	};

	void Free(T* obj) 
	{
		assert((void*)obj > (void*)mBlock && (void*)obj < (void*)(mBlock + (mCount * sizeof(T))));
		mFree.push_back(obj);
	};

private:
	unsigned int mCount;
	unsigned int mCurrent;
	T* mBlock;
	std::list<T*> mFree;
};

#endif // UALLOCATOR_H_INCLUDED
