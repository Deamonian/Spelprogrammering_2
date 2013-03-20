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
	static bool FileRead(const String &filename,void *buffer,int &size);
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

#include <sstream>
template <typename T>
String ToString(T value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
};

class ConfigFile {
public:
	ConfigFile(const String &filename);

	void Set(const String &key,const String &value);
	bool HasKey(const String &param);
	float AsFloat(const String &param);
	int AsInt(const String &param);
	bool AsBool(const String &param);
	String AsString(const String &param);

private:
	void Parse(const String &filename);

private:
	typedef std::map<String,String> KeyValueMap;
	KeyValueMap m_kvmap;
};

#endif // USYSTEM_H_INCLUDED
