// uBitmap.h

#pragma once
#ifndef UBITMAP_H_INCLUDED
#define UBITMAP_H_INCLUDED

typedef unsigned int uint;
typedef unsigned char ubyte;

enum EBmpFmt {FMT_UNKNOWN,FMT_R8,FMT_R8G8,FMT_R8G8B8,FMT_R8G8B8A8};

class Bitmap {
public:
	Bitmap();
	~Bitmap();

	bool Load(const String &filename);
	bool Create(const EBmpFmt fmt,const uint width,const uint height);
	void Release();

	uint GetWidth() const;
	uint GetHeight() const;
	EBmpFmt GetFormat() const;

	ubyte *GetData();
	const ubyte *GetData() const;

	bool Is1D() const;
	bool Is2D() const;
	bool Is3D() const;

	void Clear(ubyte color=0x00);
	void PutPixel4b(uint x,uint y,uint color);

	static uint GetFormatSize(const EBmpFmt fmt);

private:
	bool LoadTga(const String &filename);
	bool LoadDds(const String &filename);

private:
	EBmpFmt m_format;
	uint m_width;
	uint m_height;
	uint m_depth;
	ubyte *m_pixels;
};


#endif // UBITMAP_H_INCLUDED


