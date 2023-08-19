#include "pkm_helper.h"
#include "wfETC.h"

static int stbi__pkm_test(stbi__context *s)
{
	//	check the magic number
	if (stbi__get8(s) != 'P') {
		stbi__rewind(s);
		return 0;
	}

	if (stbi__get8(s) != 'K') {
		stbi__rewind(s);
		return 0;
	}

	if (stbi__get8(s) != 'M') {
		stbi__rewind(s);
		return 0;
	}

	if (stbi__get8(s) != ' ') {
		stbi__rewind(s);
		return 0;
	}

	if (stbi__get8(s) != '1') {
		stbi__rewind(s);
		return 0;
	}

	if (stbi__get8(s) != '0') {
		stbi__rewind(s);
		return 0;
	}

	stbi__rewind(s);
	return 1;
}

#ifndef STBI_NO_STDIO

int stbi__pkm_test_filename(char const *filename)
{
   int r;
   FILE *f = fopen(filename, "rb");
   if (!f) return 0;
   r = stbi__pkm_test_file(f);
   fclose(f);
   return r;
}

int stbi__pkm_test_file(FILE *f)
{
   stbi__context s;
   int r,n = ftell(f);
   stbi__start_file(&s,f);
   r = stbi__pkm_test(&s);
   fseek(f,n,SEEK_SET);
   return r;
}
#endif

int stbi__pkm_test_memory(stbi_uc const *buffer, int len)
{
   stbi__context s;
   stbi__start_mem(&s,buffer, len);
   return stbi__pkm_test(&s);
}

int stbi__pkm_test_callbacks(stbi_io_callbacks const *clbk, void *user)
{
   stbi__context s;
   stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
   return stbi__pkm_test(&s);
}

static TextureInfo* stbi__pkm_info(stbi__context *s)
{
	PKMHeader header;
	unsigned int width, height;

	stbi__getn( s, (stbi_uc*)(&header), sizeof(PKMHeader) );

	if (0 != strncmp( header.aName, "PKM 10", sizeof(header.aName))) {
		stbi__rewind(s);
		return 0;
	}

	width = (header.iWidthMSB << 8) | header.iWidthLSB;
	height = (header.iHeightMSB << 8) | header.iHeightLSB;

	int x = s->img_x = width;
	int y = s->img_y = height;
	int comp = s->img_n = 3;

	stbi__rewind(s);

	TextureInfo* ti = malloc(sizeof(TextureInfo));
	ti->width = x;
	ti->height = y;
	ti->type = PKM;
	ti->numOfElements = comp;
	ti->data = NULL;
	ti->extraInfo = NULL;

	return ti;
}

TextureInfo* stbi__pkm_info_from_memory(stbi_uc const *buffer, int len )
{
	stbi__context s;
	stbi__start_mem(&s,buffer, len);
	return stbi__pkm_info(&s);
}

TextureInfo* stbi__pkm_info_from_callbacks(stbi_io_callbacks const *clbk, void *user)
{
	stbi__context s;
	stbi__start_callbacks(&s, (stbi_io_callbacks *) clbk, user);
	return stbi__pkm_info( &s);
}

#ifndef STBI_NO_STDIO
TextureInfo* stbi__pkm_info_from_path(char const *filename)
{
   FILE *f = fopen(filename, "rb");
   if (!f) return 0;
   TextureInfo* res = stbi__pkm_info_from_file(f);
   fclose(f);
   return res;
}

TextureInfo* stbi__pkm_info_from_file(FILE *f)
{
   stbi__context s;
   long n = ftell(f);
   stbi__start_file(&s, f);
   TextureInfo* res = stbi__pkm_info(&s);
   fseek(f, n, SEEK_SET);
   return res;
}
#endif

static TextureInfo* stbi__pkm_load(stbi__context *s, int req_comp)
{
	stbi_uc *pkm_data = NULL;
	stbi_uc *pkm_res_data = NULL;
	PKMHeader header;
	unsigned int width;
	unsigned int height;
	unsigned int compressed_size;

	stbi__getn( s, (stbi_uc*)(&header), sizeof(PKMHeader) );

	if (0 != strncmp( header.aName, "PKM 10", sizeof(header.aName))) {
		return NULL;
	}

	width = (header.iWidthMSB << 8) | header.iWidthLSB;
	height = (header.iHeightMSB << 8) | header.iHeightLSB;

	int x = s->img_x = width;
	int y = s->img_y = height;
	int comp = s->img_n = 4;

	compressed_size = (((width + 3) & ~3) * ((height + 3) & ~3)) >> 1;

	pkm_data = (stbi_uc *)malloc(compressed_size);
	stbi__getn( s, pkm_data, compressed_size );

	pkm_res_data = (stbi_uc *)malloc(width * height * s->img_n);

	wfETC1_DecodeImage(pkm_data, pkm_res_data, width, height);

	free( pkm_data );

	if ( NULL != pkm_res_data ) {
		if( (req_comp < 4) && (req_comp >= 1) ) {
			//	user has some requirements, meet them
			if( req_comp != s->img_n ) {
				pkm_res_data = stbi__convert_format( pkm_res_data, s->img_n, req_comp, s->img_x, s->img_y );
				comp = req_comp;
			}
		}

		TextureInfo* ti = malloc(sizeof(TextureInfo));
		ti->width = x;
		ti->height = y;
		ti->type = PKM;
		ti->numOfElements = comp;
		ti->data = pkm_res_data;
		ti->extraInfo = NULL;

		return ti;

	} else {
		free( pkm_res_data );
	}

	return NULL;
}

#ifndef STBI_NO_STDIO
TextureInfo* stbi__pkm_load_from_file(FILE *f, int req_comp)
{
	stbi__context s;
	stbi__start_file(&s,f);
	return stbi__pkm_load(&s, req_comp);
}

TextureInfo* stbi__pkm_load_from_path(char const*filename, int req_comp)
{
	FILE *f = fopen(filename, "rb");
	if (!f) return NULL;
	TextureInfo* res = stbi__pkm_load_from_file(f, req_comp);
	fclose(f);
	return res;
}
#endif

TextureInfo* stbi__pkm_load_from_memory(stbi_uc const *buffer, int len, int req_comp)
{
	stbi__context s;
	stbi__start_mem(&s, buffer, len);
	return stbi__pkm_load(&s, req_comp);
}

TextureInfo* stbi__pkm_load_from_callbacks(stbi_io_callbacks const *clbk, void *user, int req_comp)
{
	stbi__context s;
	stbi__start_callbacks(&s, (stbi_io_callbacks *)clbk, user);
	return stbi__pkm_load(&s, req_comp);
}
