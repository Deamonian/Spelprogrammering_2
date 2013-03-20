// FreeTypeFontSystem.h

#ifndef FREETYPEFONTSYSTEM_H_INCLUDED
#define FREETYPEFONTSYSTEM_H_INCLUDED

#include "RectangleBinPack.h"
#include "uBitmap.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H 
#include FT_BITMAP_H


typedef int FontId;

class FontBuilder {
	struct float2 {
		float x,y;
	};

	class FTFont {
	public:
		String m_filename;
		FT_Face m_face;

		struct GlyphEntry {
			float2 uv[2];
			int advx;
			int width;
			int height;
			int offx;
			int offy;
		};

		typedef std::map<std::pair<int,int>,GlyphEntry> GlyphCacheMap;
		GlyphCacheMap m_cache;

		GlyphEntry *FindCharIndex(int ch,int pointSize) {
			GlyphCacheMap::iterator it=m_cache.find(std::make_pair(ch,pointSize));
			if(it!=m_cache.end()) {
				return &it->second;
			};
			return nullptr;
		};
	};

	struct FontEntry {
		int id;
		FTFont *font;
	};

	class FTSystem {
	public:
		void Init(int surfaceSize=1024);
		void Shut();

		FTFont *LoadFontFace(const char *filename,int index=0);
		bool CacheCharacter(FTFont *font,int ch,int pointSize,const Vector3 &color);
		float2 *GetCharacterUVTopLeft(FTFont *font,unsigned int ch);
		Bitmap *GetCacheImage() {return &m_image;};

		bool Fill(FTFont *font,void *buffer,int size,unsigned int &pos,float x,float y,float z,unsigned int color,String *str);
		int CalcWidth(FTFont *font,int size,String *str);

	private:
		FT_Library m_library;
		std::list<FTFont> m_fonts;
		Bitmap m_image;
		RectangleBinPack m_pack;
	};

public:
	FontBuilder();
	~FontBuilder();

	bool Init(int bitmap_size=512);
	void Shut();

	FontId LoadFontFace(const String &filename,int index=0);
	void CacheFace(const FontId id,unsigned int character,int size,uint color);

	void FillBuffer(const FontId id,int size,String *str,const Vector3 &position,unsigned int color,void *vertex_buffer,unsigned int &pos);
	int CalcWidth(const FontId id,int size,String *str);

	Bitmap *GetImage();

private:
	FTFont *GetFont(const FontId id);

private:
	FTSystem m_system;
	std::vector<FontEntry> m_entries;

	struct {
		int m_id;
		FTFont *m_font;
	} m_cache;
};

#endif // FREETYPEFONTSYSTEM_H_INCLUDED