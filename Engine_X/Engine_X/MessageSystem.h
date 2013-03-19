// MessageSystem.h

#ifndef MESSAGESYSTEM_H_INCLUDED
#define MESSAGESYSTEM_H_INCLUDED

#include "MessageSystemPrereq.h"

class MessageSystem 
{
	class ICallback 
	{
	public:
		virtual ~ICallback() {};
		virtual void operator()(IEvent*) = 0;
		virtual bool Compare(void *) = 0;
	};

	template <class T>
	class Callback : public ICallback 
	{
		T* mObj; 
		void(T::*mMtd)(IEvent*);
	public:
		Callback(T* obj, void(T::*mtd)(IEvent*)) : mObj(obj), mMtd(mtd) {};
		void operator()(IEvent* evt) { (*mObj.*mMtd)(evt); }
		bool Compare(void* obj) { return mObj == obj; } 
	};

public:
	MessageSystem();
	~MessageSystem();

	void Notify(IEvent* event);
	void Unregister(EEventType type, void* object);

	template <class T>
	void Register(EEventType type, T* object, void(T::*method)(IEvent*)) 
	{
		mMessageLists[(int)type].push_back(new Callback<T>(object, method));
	}

private:
	void Shut();

private:
	typedef std::vector<ICallback*> CallbackList;
	CallbackList mMessageLists[EVT_MAX];
};

#endif // MESSAGESYSTEM_H_INCLUDED
