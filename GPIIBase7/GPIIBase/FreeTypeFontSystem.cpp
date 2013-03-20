// FreeTypeFontSystem.cpp

#include "stdafx.h"
#include <cassert>
#include "FreeTypeFontSystem.h"

#ifdef _DEBUG
#pragma comment(lib,"freetype2410MT_D.lib")
#else
#pragma comment(lib,"freetype2410MT.lib")
#endif // _DEBUG

struct TextVertex {
	Vector3 m_position;
	Vector2 m_texcoord;
	unsigned int m_color;
};

void FontBuilder::FTSystem::Init(int surfaceSize) {
	::memset(&m_library,0,sizeof(m_library));
	FT_Error error=FT_Init_FreeType(&m_library);
	assert(error==0);

	m_image.Create(FMT_R8G8B8A8,surfaceSize,surfaceSize);
	m_image.Clear(0x00);
	m_pack.Init(surfaceSize,surfaceSize);
};

void FontBuilder::FTSystem::Shut() {
	m_image.Release();
	std::list<FTFont>::iterator it=m_fonts.begin();
	while(it!=m_fonts.end()) {
		FT_Done_Face(it->m_face);
		it->m_cache.clear();
		++it;
	};
	m_fonts.clear();
	FT_Error error=FT_Done_FreeType(m_library);
	assert(error==0);
};

FontBuilder::FTFont *FontBuilder::FTSystem::LoadFontFace(const char *filename,int index) {
	std::list<FTFont>::iterator it=m_fonts.begin();
	while(it!=m_fonts.end()) {
		if(strcmp((*it).m_filename.c_str(),filename)==0) {
			return &(*it);
		};
	};

	FTFont font;
	font.m_filename=filename;
	::memset(&font.m_face,0,sizeof(font.m_face));
	FT_Error error=FT_New_Face(m_library,filename,index,&font.m_face);
	assert(error==0);
	m_fonts.push_back(font);
	return &m_fonts.back();
};

unsigned int do_argb(unsigned char a,unsigned char r,unsigned char g,unsigned char b) {
	return (a<<24)|(r<<16)|(g<<8)|(b);
};

unsigned int do_rgba(unsigned char r,unsigned char g,unsigned char b,unsigned char a) {
	return (a<<24)|(r<<16)|(g<<8)|(b);
};

bool FontBuilder::FTSystem::CacheCharacter(FTFont *font,int ch,int pointSize,const Vector3 &color) {
	assert(font);

	FTFont::GlyphEntry *cachedPos=font->FindCharIndex(ch,pointSize);
	if(cachedPos) return true;

	const int charHeight=pointSize<<6;
	const int dpi=300;
	FT_Error error=FT_Set_Char_Size(font->m_face,0,charHeight,dpi,dpi);
	//FT_Error error=FT_Set_Pixel_Sizes(font->m_face,0,pointSize);
	assert(error==0);

	int glyphIndex=FT_Get_Char_Index(font->m_face,ch);
	if(glyphIndex==0) {
		return false;
	};
	assert(glyphIndex!=0);
	const FT_Int32 loadFlags=FT_LOAD_DEFAULT;
	error=FT_Load_Glyph(font->m_face,glyphIndex,loadFlags);
	assert(error==0);

	const FT_Render_Mode renderMode=FT_RENDER_MODE_NORMAL;
	FT_Render_Glyph(font->m_face->glyph,renderMode);
	assert(font->m_face->glyph->format==FT_GLYPH_FORMAT_BITMAP);
	FT_Bitmap *bitmap=&font->m_face->glyph->bitmap;
	assert(bitmap);

	const int guardPadding=1;
	const int height=bitmap->rows;
	const int width=bitmap->width;
	if(width==0||height==0) return true;

	RectangleBinPack::Node *store=m_pack.Insert(width+2*guardPadding,height+2*guardPadding);
	if(!store) return false;

	for(int y=0;y<height;y++) {
		for(int x=0;x<width;x++) {
			ubyte i=bitmap->buffer[y*bitmap->pitch+x];
			ubyte r=(ubyte)(color.x*i);
			ubyte g=(ubyte)(color.y*i);
			ubyte b=(ubyte)(color.z*i);
			unsigned int c=do_argb(i,i,i,i);
			m_image.PutPixel4b(store->x+x+guardPadding, store->y+y+guardPadding, c);
		};
	};

	FTFont::GlyphEntry entry;
	entry.width=store->width;
	entry.height=store->height;

	entry.advx=font->m_face->glyph->advance.x>>6;

	entry.offx=font->m_face->glyph->bitmap_left;
	//entry.offx=(font->m_face->glyph->metrics.horiBearingX>>6);
	entry.offy=(font->m_face->glyph->metrics.vertAdvance>>6)-font->m_face->glyph->bitmap_top;

	entry.uv[0].x=(float)(store->x+guardPadding)/(float)m_image.GetWidth();
	entry.uv[0].y=(float)(store->y+guardPadding)/(float)m_image.GetHeight();
	entry.uv[1].x=(float)(store->width)/(float)m_image.GetWidth();
	entry.uv[1].y=(float)(store->height)/(float)m_image.GetHeight();

	font->m_cache.insert(std::make_pair(std::make_pair(ch,pointSize),entry));

	return true;
};

FontBuilder::float2 *FontBuilder::FTSystem::GetCharacterUVTopLeft(FTFont *font,unsigned int ch){

	return 0;
};

bool FontBuilder::FTSystem::Fill(FTFont *font,void *buffer,int size,unsigned int &pos,float x,float y,float z,unsigned int color,String *str) {
	if(font==nullptr||buffer==nullptr||str==nullptr) {return false;};

	bool tag=false;
	TextVertex *vertices=(TextVertex*)buffer;
	int len=str->length();
	for(int i=0;i<len;i++) {
		/*if(!tag) {
			if(str->at(i)=='<') {
				tag=true;
			};
		}
		else {
			if(str->at(i)=='>') {
				tag=false;
			};
		};*/
		if(/*!tag&&*/str->at(i)==' ') {
			x+=(float)size*1.25f;
			continue;
		};
		unsigned int c=(unsigned int)ubyte(str->at(i));
		FTFont::GlyphEntry *e=font->FindCharIndex(c,size);

		if(!e) {
			CacheCharacter(font,c,size,Vector3(1.0f,1.0f,1.0f));
			e=font->FindCharIndex(c,size);
			if(!e) {e=font->FindCharIndex(ubyte('?'),size);};
		};

		float pw=(float)e->width;
		float ph=(float)e->height;
		float px=x+float(e->offx);
		float py=y+float(e->offy);

		float ux=e->uv[0].x;
		float uy=e->uv[0].y;
		float uw=e->uv[1].x;
		float uh=e->uv[1].y;

		vertices[pos+0].m_position=Vector3(px,py,z);
		vertices[pos+0].m_texcoord=Vector2(ux,uy);
		vertices[pos+0].m_color=color;

		vertices[pos+1].m_position=Vector3(px+pw,py,z);
		vertices[pos+1].m_texcoord=Vector2(ux+uw,uy);
		vertices[pos+1].m_color=color;

		vertices[pos+2].m_position=Vector3(px+pw,py+ph,z);
		vertices[pos+2].m_texcoord=Vector2(ux+uw,uy+uh);
		vertices[pos+2].m_color=color;

		vertices[pos+3].m_position=Vector3(px+pw,py+ph,z);
		vertices[pos+3].m_texcoord=Vector2(ux+uw,uy+uh);
		vertices[pos+3].m_color=color;

		vertices[pos+4].m_position=Vector3(px,py+ph,z);
		vertices[pos+4].m_texcoord=Vector2(ux,uy+uh);
		vertices[pos+4].m_color=color;

		vertices[pos+5].m_position=Vector3(px,py,z);
		vertices[pos+5].m_texcoord=Vector2(ux,uy);
		vertices[pos+5].m_color=color;

		pos+=6;
		x+=e->advx;
	};

	return true;
};

int FontBuilder::FTSystem::CalcWidth(FTFont *font,int size,String *str) {
	int width=0;
	int len=str->length();
	for(int i=0;i<len;i++) {
		unsigned int c=(unsigned int)ubyte(str->at(i));
		if(c==32) {
			width+=size<<1;
			continue;
		};
		FTFont::GlyphEntry *e=font->FindCharIndex(c,size);
		if(!e) {
			CacheCharacter(font,c,size,Vector3(1.0f,1.0f,1.0f));
			e=font->FindCharIndex(c,size);
			if(!e) {e=font->FindCharIndex(ubyte('?'),size);};
		};
		width+=e->advx;
	};
	return width;
};


// ***************************************************************** //
static uint s_id_gen=1;
FontBuilder::FontBuilder() {
	m_cache.m_font=nullptr;
	m_cache.m_id=-1;
};

FontBuilder::~FontBuilder() {
};

bool FontBuilder::Init(int bitmap_size) {
	m_system.Init(bitmap_size);
	return true;
};

void FontBuilder::Shut() {
	m_entries.clear();
	m_system.Shut();
};

FontId FontBuilder::LoadFontFace(const String &filename,int index) {
	FTFont *font=m_system.LoadFontFace(filename.c_str(),index);
	if(font==nullptr) {return -1;};
	FontEntry entry;
	entry.id=s_id_gen++;
	entry.font=font;
	m_entries.push_back(entry);
	return entry.id;
};

void FontBuilder::CacheFace(const FontId id,unsigned int character,int size,uint color) {
	if(m_cache.m_id!=id) {
		m_cache.m_font=GetFont(id);
		m_cache.m_id=id;
	};
	if(m_cache.m_font==nullptr) {return;};

	const float inv=0.00390625f;
	Vector3 c(0.0f,0.0f,0.0f);
	c.x=(float)(ubyte(color>>16)&0xff)*inv;
	c.y=(float)(ubyte(color>>8)&0xff)*inv;
	c.z=(float)(ubyte(color)&0xff)*inv;
	if(!m_system.CacheCharacter(m_cache.m_font,character,size,c)) {
		System::Log("Warning, could not cache character (ID:%d,%d,%d)!",id,character,size);
	};
};

void FontBuilder::FillBuffer(const FontId id,int size,String *str,const Vector3 &position,unsigned int color,void *vertex_buffer,unsigned int &pos) {
	FTFont *font=GetFont(id);
	m_system.Fill(font,vertex_buffer,size,pos,position.x,position.y,position.z,color,str);
};

int FontBuilder::CalcWidth(const FontId id,int size,String *str) {
	FTFont *font=GetFont(id);
	return m_system.CalcWidth(font,size,str);
};

Bitmap *FontBuilder::GetImage() {
	return m_system.GetCacheImage();
};

FontBuilder::FTFont *FontBuilder::GetFont(const FontId id) {
	for(uint i=0;i<m_entries.size();i++) {
		if(m_entries[i].id==id) {return m_entries[i].font;};
	};
	return nullptr;
};
