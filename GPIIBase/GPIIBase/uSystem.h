// uSystem.h

#ifndef USYSTEM_H_INCLUDED
#define USYSTEM_H_INCLUDED

class System {
	static FILE *ms_logfile;
public:
	static void Notify(const String &message,const String &caption);
	static void Break();

	static void Log(const char *fmt,...);
	static void OpenLog();
	static void CloseLog();

	static bool FileExists(const String &filename);
	static unsigned int FileSize(const String &filename);
};


class Timer {
public:
	Timer();

	void Update();
	float GetDelta();
	float Total();

private:
	unsigned int m_start;
	unsigned int m_previous;
	unsigned int m_current;
	float m_deltatime;
}; 

#endif // USYSTEM_H_INCLUDED
