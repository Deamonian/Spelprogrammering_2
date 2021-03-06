// uSystem.cpp

#include "stdafx.h"
#pragma comment(lib,"winmm.lib")
#include <mmsystem.h>
#include <cstdarg>
#include <fstream>
#include "uSystem.h"

FILE *System::ms_logfile=nullptr;

void System::Notify(const String &message,const String &caption) {
	::MessageBoxA(NULL,message.c_str(),caption.c_str(),MB_OK|MB_ICONEXCLAMATION);
};

void System::Break() {
	__debugbreak();
};

void System::Log(const char *fmt,...) {
	va_list vl;
	va_start(vl,fmt);
	static char buffer[960];
	vsprintf_s(buffer,960,fmt,vl);
	va_end(vl);

	SYSTEMTIME st;
	::GetSystemTime(&st);

	static char timestamp[64];
	sprintf_s(timestamp,64,"[%4d%02d%02d %02d:%02d:%02d.%03d]",
		st.wYear,st.wMonth,st.wDay,
		st.wHour,st.wMinute,st.wSecond,
		st.wMilliseconds);

	static char output[1024];
	sprintf_s(output,"%s %s\n",timestamp,buffer);

	fwrite(output,strlen(output),1,ms_logfile);
	fflush(ms_logfile);
};

void System::OpenLog() {
	SYSTEMTIME st;
	::GetSystemTime(&st);

	static char filename[128];
	sprintf_s(filename,128,"log_%4d%02d%02d%02d%02d%02d.txt",st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
	fopen_s(&ms_logfile,filename,"w");
};

void System::CloseLog() {
	fclose(ms_logfile);
	ms_logfile=nullptr;
};

bool System::FileExists(const String &filename) {
	return GetFileAttributesA(filename.c_str())!=0xFFFFFFFF;
};

unsigned int System::FileSize(const String &filename) {
	WIN32_FILE_ATTRIBUTE_DATA data;
	::GetFileAttributesExA(filename.c_str(),GetFileExInfoStandard,&data);
	// NOTE: I am lazy, hence only know about files not greater than 2GBs
	return data.nFileSizeLow;
};

bool System::FileRead(const String &filename,void *buffer,int &size) {
	FILE *file=nullptr;
	fopen_s(&file,filename.c_str(),"rb");
	if(file==nullptr) {return false;};
	size=fread(buffer,1,size,file);
	fclose(file);
	return true;
};


Timer::Timer() {
	m_start=::timeGetTime();
	m_current=m_start;
	m_previous=m_current;
};

void Timer::Update() {
	m_previous=m_current;
	m_current=::timeGetTime();
	m_deltatime=(float)(m_current-m_previous)*0.001f;
};

float Timer::GetDelta() {
	return m_deltatime;
};

float Timer::Total() {
	return (float)(m_current-m_start)*0.001f;
};

// ------------------------------------------------
ConfigFile::ConfigFile(const String &filename) {
	Parse(filename);
};

void ConfigFile::Set(const String &key,const String &value) {
	m_kvmap[key]=value;
};

bool ConfigFile::HasKey(const String &param) {
	return m_kvmap.find(param)!=m_kvmap.end();
};

float ConfigFile::AsFloat(const String &param) {
	return (float)atof(m_kvmap.find(param)->second.c_str());
};

int ConfigFile::AsInt(const String &param) {
	return atoi(m_kvmap.find(param)->second.c_str());
};

bool ConfigFile::AsBool(const String &param){
	return atoi(m_kvmap.find(param)->second.c_str())>0;
};

String ConfigFile::AsString(const String &param){
	return m_kvmap.find(param)->second;
};

void ConfigFile::Parse(const String &filename) {
	std::ifstream stream;
	stream.open(filename.c_str());
	if(stream.fail()||stream.bad()) {return;};
	while(!stream.eof()) {
		String line;
		std::getline(stream,line);
		size_t pos=line.find_first_of(' ');
		String key=line.substr(0,pos);
		String value=line.substr(pos+1);
		m_kvmap.insert(std::pair<String,String>(key,value));
	};
};

