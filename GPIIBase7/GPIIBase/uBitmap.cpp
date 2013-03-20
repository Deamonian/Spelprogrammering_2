// uBitmap.cpp

#include "stdafx.h"
#include "uBitmap.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#pragma pack(push,1)
struct TgaHeader {
	u8  desc_len;
	u8  cmap_type;
	u8  image_type;
	u16 cmap_start;
	u16 cmap_entries;
	u8  cmap_bits;
	u16 x_offset;
	u16 y_offset;
	u16 width;
	u16 height;
	u8  bpp;
	u8  attrib;
};

struct DDSHeader {
	u32 dwMagic;
	u32 dwSize;
	u32 dwFlags;
	u32 dwHeight;
	u32 dwWidth;
	u32 dwPitchOrLinearSize;
	u32 dwDepth; 
	u32 dwMipMapCount;
	u32 dwReserved[11];
	struct {
        u32 dwSize;
		u32 dwFlags;
		u32 dwFourCC;
		u32 dwRGBBitCount;
		u32 dwRBitMask;
		u32 dwGBitMask;
		u32 dwBBitMask;
		u32 dwRGBAlphaBitMask; 
	} ddpfPixelFormat;
	struct {
		u32 dwCaps1;
		u32 dwCaps2;
		u32 Reserved[2];
	} ddsCaps;
	u32 dwReserved2;
};

struct DDSHeaderDX10 {
    u32 dxgiFormat;
    u32 resourceDimension;
    u32 miscFlag;
    u32 arraySize;
    u32 reserved;
};
#pragma pack(pop)



uint Bitmap::GetFormatSize(const EBmpFmt fmt) {
	switch(fmt) {
	case FMT_R8: return sizeof(ubyte);
	case FMT_R8G8: return sizeof(ubyte)*2;
	case FMT_R8G8B8: return sizeof(ubyte)*3;
	case FMT_R8G8B8A8: return sizeof(ubyte)*4;
	};
	return 0;
};

Bitmap::Bitmap() {
	m_format=FMT_UNKNOWN;
	m_width=0;
	m_height=0;
	m_depth=0;
	m_pixels=nullptr;
};

Bitmap::~Bitmap() {
	Release();
};

static EBmpFmt convert_bpp_to_fmt(uint bpp) {
	switch(bpp) {
	case 8: return FMT_R8;
	case 16: return FMT_R8G8;
	case 24: return FMT_R8G8B8;
	case 32: return FMT_R8G8B8A8;
	};
	return FMT_UNKNOWN;
};

bool Bitmap::Load(const String &filename) {
	size_t pos=filename.find_last_of('.');
	if(pos==String::npos) {return false;};
	String ext=filename.substr(pos+1);

	// TODO: Add more formats
	if(strcmp(ext.c_str(),"tga")==0) {return LoadTga(filename);};

	return false;
};

bool Bitmap::Create(const EBmpFmt fmt,const uint width,const uint height) {
	m_format=fmt;
	m_width=width;
	m_height=height;
	uint size=m_width*m_height*Bitmap::GetFormatSize(m_format);
	m_pixels=new ubyte[size];
	for(uint i=0;i<size;i++) {m_pixels[i]=0x00;};
	return true;
};

void Bitmap::Release() {
	if(m_pixels) delete[] m_pixels;
	m_format=FMT_UNKNOWN;
	m_width=0;
	m_height=0;
	m_pixels=nullptr;
};

uint Bitmap::GetWidth() const {
	return m_width;
};

uint Bitmap::GetHeight() const {
	return m_height;
};

EBmpFmt Bitmap::GetFormat() const {
	return m_format;
};

ubyte *Bitmap::GetData() {
	return m_pixels;
};

const ubyte *Bitmap::GetData() const {
	return m_pixels;
};

bool Bitmap::Is1D() const {
	return m_width>1&&m_height==1&&m_depth==1;
};

bool Bitmap::Is2D() const {
	return m_width>1&&m_height>1&&m_depth==1;
};

bool Bitmap::Is3D() const {
	return m_width>1&&m_height>1&&m_depth>0;
};

void Bitmap::Clear(ubyte color) {
	uint size=m_width*m_height*GetFormatSize(m_format);
	for(uint i=0;i<size;i++) {m_pixels[i]=color;};
};

void Bitmap::PutPixel4b(uint x,uint y,uint color) {
	assert(x<m_width||y<m_height);
	uint *p=(uint*)m_pixels;
	p[y*m_width+x]=color;
};

// -- private ----------------------------------------
bool Bitmap::LoadTga(const String &filename) {
	FILE *file=nullptr;
	fopen_s(&file,filename.c_str(),"rb");
	if(file==nullptr) {return false;};

	fseek(file,0,SEEK_END);
	uint size=ftell(file);
	fseek(file,0,SEEK_SET);

	TgaHeader header;
	fread(&header,sizeof(TgaHeader),1,file);

	m_width=(uint)header.width;
	m_height=(uint)header.height;
	m_depth=1;

	uint pixel_size=header.bpp/8;

	uint pal_len=header.desc_len+header.cmap_entries*header.cmap_bits/8;
	ubyte palette[768];
	if(pal_len>0) {fread(palette,sizeof(palette),1,file);};

	ubyte *lbuff=new ubyte[size-sizeof(header)-pal_len];
	fread(lbuff,size-sizeof(header)-pal_len,1,file);
	fclose(file);

	size=m_width*m_height*header.bpp/8;
	ubyte *tmp_buff=new ubyte[size];
	ubyte *dst=nullptr,*src=nullptr;

	if((header.image_type&0x08)>0) {
		uint c,cnt;

		dst=tmp_buff;
		src=lbuff;

		while(size>0) {
			c=*src++;
			cnt=(c&0x7f)+1;
			size-=cnt*pixel_size;
			if((c&0x80)>0) {
				do {
					::memcpy(dst,src,pixel_size);
					dst+=pixel_size;
				} while(--cnt);
			}
			else {
				cnt*=pixel_size;
				::memcpy(dst,src,cnt);
				dst+=cnt;
				src+=cnt;
			};
		};
		src=tmp_buff;
	}
	else {
		src=lbuff;
	};

	uint tmp_pixel=0;
	src+=pixel_size*m_width*(m_height-1);
	switch(header.bpp) {
	case 8:
		if(pal_len>0) {
			m_format=FMT_R8G8B8;
			dst=m_pixels=new ubyte[m_width*m_height*3];
			for(uint y=0;y<m_height;y++) {
				for(uint x=0;x<m_width;x++) {
					tmp_pixel=3*(*src++);
					*dst++=palette[tmp_pixel+2];
					*dst++=palette[tmp_pixel+1];
					*dst++=palette[tmp_pixel];
				};
				src-=m_width*2;
			};
		}
		else {
			m_format=FMT_R8;
			dst=m_pixels=new ubyte[m_width*m_height];
			for(uint y=0;y<m_height;y++) {
				::memcpy(dst,src,m_width);
				dst+=m_width;
				src-=m_width;
			};
		};
		break;
	case 16:
		m_format=FMT_R8G8B8A8;
		dst=m_pixels=new ubyte[m_width*m_height*4];
		for(uint y=0;y<m_height;y++) {
			for(uint x=0;x<m_width;x++) {
				tmp_pixel=*((u16*)src);
				dst[0]=((tmp_pixel>>10)&0x1f)<<3;
				dst[1]=((tmp_pixel>>5 )&0x1f)<<3;
				dst[2]=((tmp_pixel    )&0x1f)<<3;
				dst[3]=((tmp_pixel>>15)?0xff:0);
				dst+=4;
				src+=2;
			};
			src-=m_width*4;
		};
		break;
	case 24:
		m_format=FMT_R8G8B8;
		dst=m_pixels=new ubyte[m_width*m_height*3];
		for(uint y=0;y<m_height;y++) {
			for(uint x=0;x<m_width;x++) {
				*dst++=src[2];
				*dst++=src[1];
				*dst++=src[0];
				src+=3;
			};
			src-=m_width*6;
		};
		break;
	case 32:
		m_format=FMT_R8G8B8A8;
		dst=m_pixels=new ubyte[m_width*m_height*4];
		for(uint y=0;y<m_height;y++) {
			for(uint x=0;x<m_width;x++) {
				*dst++=src[2];
				*dst++=src[1];
				*dst++=src[0];
				*dst++=src[3];
				src+=4;
			};
			src-=m_width*8;
		};
		break;
	};

	delete[] lbuff;
	delete[] tmp_buff;

	return true;
};

bool Bitmap::LoadDds(const String &filename) {
	return false;
};
