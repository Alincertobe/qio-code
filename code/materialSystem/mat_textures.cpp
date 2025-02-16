/*
============================================================================
Copyright (C) 2012 V.

This file is part of Qio source code.

Qio source code is free software; you can redistribute it 
and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

Qio source code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA,
or simply visit <http://www.gnu.org/licenses/>.
============================================================================
*/
// mat_textures.cpp - textures system frontend
#include <api/textureAPI.h>
#include <api/rbAPI.h>
#include <api/imgAPI.h>
#include <api/coreAPI.h>
#include <shared/str.h>
#include <shared/hashTableTemplate.h>
#include <shared/textureWrapMode.h>

class textureIMPL_c : public textureAPI_i {
	str name;
	union {
		u32 handleU32;
		void *handleV;
	};
	// second handle for DX10 backend
	void *extraHandle;
	u32 w, h;
	textureWrapMode_e wrapMode;
	textureIMPL_c *hashNext;
public:
	textureIMPL_c() {
		hashNext = 0;
		w = h = 0;
		handleV = 0;
		extraHandle = 0;
		wrapMode = TWM_REPEAT; // use GL_REPEAT by default
	}
	~textureIMPL_c() {
		if(rb == 0)
			return;
		rb->freeTextureData(this);
	}

	// returns the path to the texture file (with extension)
	virtual const char *getName() const {
		return name;
	}
	void setName(const char *newName) {
		name = newName;
	}
	void setWrapMode(textureWrapMode_e newWrapMode) {
		wrapMode = newWrapMode;
	}
	virtual u32 getWidth() const {
		return w;
	}
	virtual u32 getHeight() const {
		return h;
	}
	virtual void setWidth(u32 newWidth) {
		w = newWidth;
	}
	virtual void setHeight(u32 newHeight) {
		h = newHeight;
	}	
	virtual bool readTextureDataRGBA(byte *out) { 
		return rb->readTextureDataRGBA(this,out);
	}
	virtual bool setTextureDataRGBA(byte *out) { 
		return rb->setTextureDataRGBA(this,out);
	}
	// TWM_CLAMP_TO_EDGE should be set to true for skybox textures
	virtual textureWrapMode_e getWrapMode() const {
		return wrapMode;
	}
	virtual void *getInternalHandleV() const {
		return handleV;
	}
	virtual void setInternalHandleV(void *newHandle) {
		handleV = newHandle;
	}
	virtual u32 getInternalHandleU32() const {
		return handleU32;
	}
	virtual void setInternalHandleU32(u32 newHandle) {
		handleU32 = newHandle;
	}

	// second extra pointer for DX10 backend
	virtual void *getExtraUserPointer() const {
		return extraHandle;
	}
	virtual void setExtraUserPointer(void *newHandle) {
		extraHandle = newHandle;
	}

	textureIMPL_c *getHashNext() {
		return hashNext;
	}
	void setHashNext(textureIMPL_c *p) {
		hashNext = p;
	}
};

static hashTableTemplateExt_c<textureIMPL_c> mat_textures;
static textureIMPL_c *mat_defaultTexture = 0;

class textureAPI_i *MAT_GetDefaultTexture() {
	if(mat_defaultTexture == 0) {
		g_core->Print("Magic sizeof(textureIMPL_c) number: %i\n",sizeof(textureIMPL_c));
		mat_defaultTexture = new textureIMPL_c;
		mat_defaultTexture->setName("default");
		byte *data;
		u32 w, h;
		if(g_img == 0) {
			static byte tmp[4] = { 255, 255, 255, 255 };
			h = w = 1;
			data = tmp;
		} else {
			g_img->getDefaultImage(&data,&w,&h);
		}
		rb->uploadTextureRGBA(mat_defaultTexture,data,w,h);
		// we must not free the *default* texture data
	}
	return mat_defaultTexture;
}
class textureAPI_i *MAT_CreateLightmap(int index, const byte *data, u32 w, u32 h, bool rgba) {
	// for lightmaps
	textureIMPL_c *nl =  new textureIMPL_c;
	rb->uploadLightmap(nl,data,w,h, rgba);
	nl->setName(va("*lightmap%i",index));
	if(mat_textures.addObject(nl)) {
		g_core->Print("Warning: %s added more than once.\n",nl->getName());
		mat_textures.addObject(nl,true);
	}
	return nl;
}
// texString can contain doom3-like modifiers
// TODO: what if a texture is reused with different picmip setting?
class textureAPI_i *MAT_RegisterTexture(const char *texString, enum textureWrapMode_e wrapMode) {
	textureIMPL_c *ret = mat_textures.getEntry(texString);
	if(ret) {
		return ret;
	}
	if(!_stricmp(texString,"default")) {
		return MAT_GetDefaultTexture();
	}
	// special case for image lib module not present
	if(g_img == 0) {
		return MAT_GetDefaultTexture();
	}
	byte *data = 0;
	u32 w, h;
	const char *fixedPath = g_img->loadImage(texString,&data,&w,&h);
	if(data == 0) {
		g_core->RedWarning("MAT_RegisterTexture: using default texture for %s\n",texString);
		return MAT_GetDefaultTexture();
	}
	ret = new textureIMPL_c;
	ret->setName(fixedPath);
	ret->setWrapMode(wrapMode);
	rb->uploadTextureRGBA(ret,data,w,h);
	g_img->freeImageData(data);
	if(mat_textures.addObject(ret)) {
		g_core->Print("Warning: %s added more than once.\n",texString);
		mat_textures.addObject(ret,true);
	}
	return ret;
}
class textureAPI_i *MAT_CreateTexture(const char *texName, const byte *picData, u32 w, u32 h, u32 bpp) {
	textureIMPL_c *ret = mat_textures.getEntry(texName);
	if(ret) {
		return ret;
	}
	arraySTD_c<byte> b;
	if(picData == 0) {
		bpp = 4;
		b.resize(w*h*4);
		for(u32 j = 0; j < w; j++) {
			for(u32 i = 0; i < h; i++) {
				u32 index = 4*(i * w + j);
				if(i < w*0.1 || j < h*0.1 || i > w*0.9 || j > h*0.9) {
					b[index+0] = 0;
					b[index+1] = 255;
					b[index+2] = 0;
					b[index+3] = 255;
				} else {
					b[index+0] = 255;
					b[index+1] = 0;
					b[index+2] = 0;
					b[index+3] = 255;
				}
			}
		}
		picData = b.getArray();
	}
	ret = new textureIMPL_c;
	ret->setName(texName);
	ret->setWrapMode(TWM_REPEAT);
	if(bpp == 4) {
		rb->uploadTextureRGBA(ret,picData,w,h);
	} else {
		arraySTD_c<byte> fixed;
		u32 numPixels = w * h;
		fixed.resize(numPixels*4);
		byte *o = fixed.getArray();
		const byte *in = picData;
		for(u32 i = 0; i < numPixels; i++) {
			*o = 255;
			o++;
			*o = 255;
			o++;
			*o = 255;
			o++;
			*o = *in;
			o++; 
			in++;
		}
		rb->uploadTextureRGBA(ret,fixed.getArray(),w,h);
	}
	if(mat_textures.addObject(ret)) {
		g_core->Print("Warning: %s added more than once.\n",texName);
		mat_textures.addObject(ret,true);
	}
	return ret;
}
//void MAT_FreeTexture(class textureAPI_i **p) {
//	textureAPI_i *ptr = *p;
//	if(ptr == 0)
//		return;
//	(*p) = 0;
//	textureIMPL_c *impl = (textureIMPL_c*) ptr;
//	mat_textures.removeEntry(impl);
//	delete impl;
//}
void MAT_FreeAllTextures() {
	for(u32 i = 0; i < mat_textures.size(); i++) {
		textureIMPL_c *t = mat_textures[i];
		delete t;
		mat_textures[i] = 0;
	}
	mat_textures.clear();
	// free the default, build-in image as well
	if(mat_defaultTexture) {
		delete mat_defaultTexture;
		mat_defaultTexture = 0;
	}
}