// uAllocator.h

#ifndef UALLOCATOR_H_INCLUDED
#define UALLOCATOR_H_INCLUDED

template <typename T>
class Allocator {
public:
	Allocator(unsigned int count=1024) {
		m_count=count;
		m_current=0;
		m_block=new T[m_count];
		for(unsigned int i=0;i<m_count;i++) {
			m_free.push_back((m_block+i));
		};
	};

	~Allocator() {
		if(m_block) {
			delete[] m_block;
			m_block=nullptr;
		};
	};

	T *Allocate() {
		assert(!m_free.empty());
		T *obj=m_free.front();
		m_free.pop_front();
		return obj;
	};

	void Free(T *obj) {
		assert((void*)obj>(void*)m_block&&(void*)obj<(void*)(m_block+(m_count*sizeof(T))));
		m_free.push_back(obj);
	};

private:
	unsigned int m_count;
	unsigned int m_current;
	T *m_block;
	std::list<T*> m_free;
};

#endif // UALLOCATOR_H_INCLUDED
