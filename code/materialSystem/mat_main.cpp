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

// mat_main.cpp - renderer materials managment
#include "mat_local.h"
#include "mat_impl.h"
#include <api/coreAPI.h>
#include <api/vfsAPI.h>
#include <api/rbAPI.h>
#include <api/imgAPI.h>
#include <api/rAPI.h>
#include <shared/hashTableTemplate.h>
#include <shared/autoCmd.h>
#include <shared/tableList.h>
#include <shared/str.h>

struct matFile_s {
	str fname;
	str text;

	bool reloadMaterialSourceText() {
		text.clear();
		char *tmpFileData;
		u32 len = g_vfs->FS_ReadFile(this->fname,(void**)&tmpFileData);
		if(tmpFileData == 0) {
			g_core->RedWarning("matFile_s::reloadMaterialSourceText: failed to reload file %s\n",this->fname.c_str());
			return true; // error
		}
		this->text = tmpFileData;
		g_vfs->FS_FreeFile(tmpFileData);
		return false;
	}
	void iterateAllAvailableMaterialNames(void (*callback)(const char *s)) {
		const char *p = this->text.c_str();
		while(p && *p) {
			// skip comments and whitespaces
			p = G_SkipToNextToken(p);
			if(p == 0 || *p == 0)
				break;
			// skip tables
			if(!_strnicmp(p,"table",5) && G_isWS(p[5])) {
				p += 5; // skip 'table' token
				p = G_SkipToNextToken(p);
				// skip table name
				while(iswspace(*p) == false && *p && *p != '{')
					p++;
				p = G_SkipToNextToken(p);
				if(*p != '{') {
					g_core->RedWarning("Expected '{' after table\n");
					return;
				}
				p++;
				p = STR_SkipCurlyBracedSection(p);
			} else {
				// that's a material
				const char *nameStart = p;
				while(iswspace(*p) == false && *p)
					p++;
				str matName;
				matName.setFromTo(nameStart,p);
				//g_core->Print("Material: %s\n",matName.c_str());
				p = G_SkipToNextToken(p);
				if(*p != '{') {
					g_core->RedWarning("Expected '{' after material name %s\n",matName.c_str());
					return;
				}
				p++;
				p = STR_SkipCurlyBracedSection(p);
				callback(matName);
			}
		}	
	}
};

static hashTableTemplateExt_c<mtrIMPL_c> materials;
static arraySTD_c<matFile_s*> matFiles;

bool MAT_CacheMatFileText(const char *fname) {
	char *data;
	u32 len = g_vfs->FS_ReadFile(fname,(void**)&data);
	if(data == 0)
		return true;
	matFile_s *mf = new matFile_s;
	mf->fname = fname;
	mf->text = data;
	g_vfs->FS_FreeFile(data);
	matFiles.push_back(mf);
	return false;
}

const char *MAT_FindMaterialDefInText(const char *matName, const char *text) {
	u32 matNameLen = strlen(matName);
	const char *p = text;
	while(*p) {
#if 0
		if(!_strnicmp(p,matName,matNameLen) && G_isWS(p[matNameLen])) {
			const char *matNameStart = p;
			p += matNameLen;
			p = G_SkipToNextToken(p);
			if(*p != '{') {
				continue;
			}
			const char *brace = p;
			p++;
			// now we're sure that 'p' is at valid material text,
			// so we can start parsing
			return brace;
		}
		p++;
#else
		// parse the material file

		// skip comments
		if(p[0] == '/') {
			if(p[1] == '/') {
				p += 2; // skip "//"
				// skip single line comment
				while(*p != '\n') {
					if(*p == 0)
						return 0; // EOF hit
					p++;
				}
				p++; // skip '\n'
				continue;
			} else if(p[1] == '*') {
				p += 2; // skip "/*"
				// multiline
				while(!(p[0] == '*' && p[1] == '/')) {
					if(*p == 0)
						return 0; // EOF hit
					p++;
				}
				p += 2; // skip "*/"
				continue;
			}
			p++; // skip '/'
			continue;
		} else if(*p == '{') {
			p++; // skip '{'
			p = STR_SkipCurlyBracedSection(p);
			if(p == 0 || *p == 0)
				return 0; // EOF hit
			continue;
		} else if(G_isWS(*p)) {
			p++; // skip single whitespace
			continue;
		} else if(!stricmpn_slashes(p,matName,matNameLen) && G_isWS(p[matNameLen])) {
			const char *matNameStart = p;
			p += matNameLen;
			p = G_SkipToNextToken(p);
			if(*p != '{') {
				continue;
			}
			const char *brace = p;
			p++;
			// now we're sure that 'p' is at valid material text,
			// so we can start parsing
			return brace;
		} else {
			p++; // skip single character
		}
#endif
	}
	return 0;
}
bool MAT_FindMaterialText(const char *matName, matTextDef_s &out) {
	for(u32 i = 0; i < matFiles.size(); i++) {
		matFile_s *mf = matFiles[i];
		const char *p = MAT_FindMaterialDefInText(matName,mf->text);
		if(p) {
			out.p = p;
			out.textBase = mf->text;
			out.sourceFile = mf->fname;
			return true;
		}
	}
	return false;
}
const char *MAT_FindTableDefInText(const char *tableName, const char *text) {
	u32 tableNameLen = strlen(tableName);
	const char *p = text;
	while(*p) {
		if(!_strnicmp(p,"table",5) && G_isWS(p[5])) {
			p += 5;
			p = G_SkipToNextToken(p);
			if(!_strnicmp(p,tableName,tableNameLen) && G_isWS(p[tableNameLen])) {
				const char *tableNameStart = p;
				p += tableNameLen;
				p = G_SkipToNextToken(p);
				if(*p != '{') {
					continue;
				}
				const char *brace = p;
				p++;
				// now we're sure that 'p' is at valid material text,
				// so we can start parsing
				return brace;
			}
		}
		p++;
	}
	return 0;
}
bool MAT_FindTableText(const char *tableName, matTextDef_s &out) {
	for(u32 i = 0; i < matFiles.size(); i++) {
		matFile_s *mf = matFiles[i];
		const char *p = MAT_FindTableDefInText(tableName,mf->text);
		if(p) {
			out.p = p;
			out.textBase = mf->text;
			out.sourceFile = mf->fname;
			return true;
		}
	}
	return false;
}
bool MAT_FindTableText(const char *tableName, const char **p, const char **textBase, const char **sourceFileName) {
	matTextDef_s tmp;
	if(MAT_FindTableText(tableName,tmp)) {
		*p = tmp.p;
		*textBase = tmp.textBase;
		*sourceFileName = tmp.sourceFile;
		return true;
	}
	return false;
}
// tables list for Doom3/Quake4 material tables
static tableList_c mat_tableList(MAT_FindTableText);

u32 MAT_GetNumAllocatedMaterials() {
	return materials.size();
}
mtrAPI_i *MAT_GetAllocatedMaterial(u32 i) {
	return materials[i];
}
const char *MAT_GetAllocatedMaterialName(u32 i) {
	return materials[i]->getName();
}
const class tableListAPI_i *MAT_GetTablesAPI() {
	return &mat_tableList;
}
matFile_s *MAT_FindMatFileForName(const char *fname) {
	for(u32 i = 0; i < matFiles.size(); i++) {
		matFile_s *mf = matFiles[i];
		if(!_stricmp(mf->fname,fname)) {
			return mf;
		}
	}
	return 0;
}

void MAT_ScanForFiles(const char *path, const char *ext) {
	int numFiles;
	char **fnames = g_vfs->FS_ListFiles(path,ext,&numFiles);
	for(u32 i = 0; i < numFiles; i++) {
		const char *fname = fnames[i];
		str fullPath = path;
		fullPath.append(fname);
		MAT_CacheMatFileText(fullPath);
	}
	g_vfs->FS_FreeFileList(fnames);
}
void MAT_ScanForMaterialFiles() {
	g_core->Print("Magic sizeof(mtrIMPL_c) number: %i\n",sizeof(mtrIMPL_c));
	MAT_ScanForFiles("scripts/",".shader");
	MAT_ScanForFiles("materials/",".mtr");
}
void MAT_LoadMaterial(class mtrIMPL_c *mat, const char *alternateText = 0) {
	if(mat == 0) {
		// this should never happen
		g_core->RedWarning("MAT_LoadMaterial: NULL material pointer\n");
		return;
	}
	if(mat->getName() == 0 || mat->getName()[0] == 0) {
		// this should never happen
		g_core->RedWarning("MAT_LoadMaterial: material name not set! Cannot load material.\n");
		return;
	}
	// try to load from material text (.shader/.mtr files)
	matTextDef_s text;
	if(alternateText) {
		text.p = alternateText;
		text.sourceFile = "memory";
		text.textBase = alternateText;
		mat->loadFromText(text);
	} else if(MAT_FindMaterialText(mat->getName(),text)) {
		mat->loadFromText(text);
	} else {
		if(mat->isVMTMaterial()) {
			if(mat->loadFromVMTFile()) {
				mat->createFromImage();
			}
		} else {
			// create material directly from image
			mat->createFromImage();
		}
	}
}
mtrIMPL_c *MAT_RegisterMaterial(const char *inMatName) {
	// strip the image name extension (if any)
	str matName = inMatName;
	const char *ext = matName.getExt();
	// strip non-vmt extensions, but don't touch single-color materials
	if(inMatName[0] != '(') {
		if(ext && _stricmp(ext,"vmt")) {
			matName.stripExtension();
		}
	}
	mtrIMPL_c *ret = materials.getEntry(matName);
	if(ret) {
		return ret;
	}
	ret = new mtrIMPL_c;
	// set material name first and add it to hash table
	ret->setName(matName);
	materials.addObject(ret);
	// load material from material text (.shader/.mtr file)
	// or (if material text is not present) directly from texture if
	MAT_LoadMaterial(ret);
	return ret;
}
void MAT_ReloadSingleMaterial_internal(mtrIMPL_c *mat, const char *alternateText = 0) {
	// free old material data (but dont reset material name)
	mat->clear();
	// load material once again
	MAT_LoadMaterial(mat, alternateText);
}

void MAT_PrintMaterialSourceFileName(const char *matName) {
	mtrIMPL_c *mat = materials.getEntry(matName);
	if(mat == 0) {
		g_core->RedWarning("MAT_PrintMaterialSourceFileName: material %s was not loaded.\n",matName);
		return;
	}
	// see if we have to reload entire .mtr text file
	const char *sourceFileName = mat->getSourceFileName();
	g_core->Print("Material %s was loaded from file %s\n",matName,sourceFileName);
}
// reload entire material source text 
// but recreate only material with given name
// (matName is the name of single material)
void MAT_ReloadSingleMaterial(const char *matName, const char *alternateText) {
	mtrIMPL_c *mat = materials.getEntry(matName);
	if(mat == 0) {
		g_core->RedWarning("MAT_ReloadMaterial: material %s was not loaded.\n",matName);
		return;
	}
	if(alternateText == 0) {
		// see if we have to reload entire .mtr text file
		const char *sourceFileName = mat->getSourceFileName();
		matFile_s *mtrTextFile = MAT_FindMatFileForName(sourceFileName);
		if(mtrTextFile) {
			// reload mtr text file
			mtrTextFile->reloadMaterialSourceText();
		}
	}
	// reparse specified material text / reload its single image
	MAT_ReloadSingleMaterial_internal(mat,alternateText);
}
// reload entire material source text
// and recreate all of the present materials using it
// (mtrSourceFileName is the name of .shader / .mtr file)
void MAT_ReloadMaterialFileSource(const char *mtrSourceFileName) {
	matFile_s *mtrTextFile = MAT_FindMatFileForName(mtrSourceFileName);
	if(mtrTextFile) {
		g_core->Print("MAT_ReloadMaterialFileSource: refreshing material file %s\n",mtrSourceFileName);
		// reload existing mtr text file
		mtrTextFile->reloadMaterialSourceText();
		// reload materials
		u32 c_reloadedMats = 0;
		for(u32 i = 0; i < materials.size(); i++) {
			mtrIMPL_c *m = materials[i];
			if(!_stricmp(m->getSourceFileName(),mtrSourceFileName)) {
				MAT_ReloadSingleMaterial_internal(m);
				c_reloadedMats++;
			}
		}
		// if none found, try to assume that user skipped the directory name
		if(c_reloadedMats == 0) {
			for(u32 i = 0; i < materials.size(); i++) {
				mtrIMPL_c *m = materials[i];
				const char *fname = m->getSourceFileName();
				const char *p = strchr(fname,'/');
				if(!_stricmp(p,mtrSourceFileName)) {
					MAT_ReloadSingleMaterial_internal(m);
					c_reloadedMats++;
				}
			}
		}
		g_core->Print("MAT_ReloadMaterialFileSource: reloaded %i materials for source file %s\n",c_reloadedMats,mtrSourceFileName);
	} else {
		g_core->Print("MAT_ReloadMaterialFileSource: loading NEW materials source file %s\n",mtrSourceFileName);
		// add a NEW material file
		if(MAT_CacheMatFileText(mtrSourceFileName)) {
			// try to fix the path
			str np = "materials/";
			np.append(mtrSourceFileName);
			if(MAT_CacheMatFileText(np)) {
				g_core->RedWarning("MAT_ReloadMaterialFileSource: %s does not exist\n",mtrSourceFileName);
				return;
			}
		}
	}
}
void MAT_IterateAllAvailableMaterialNames(void (*callback)(const char *s)) {
	for(u32 i = 0; i < matFiles.size(); i++) {
		matFiles[i]->iterateAllAvailableMaterialNames(callback);
	}
}
void MAT_IterateAllAvailableMaterialFileNames(void (*callback)(const char *s)) {
	for(u32 i = 0; i < matFiles.size(); i++) {
		callback(matFiles[i]->fname.c_str());
	}
}
u32 MAT_GetNumCachedMaterialFiles() {
	return matFiles.size();
}
const char *MAT_GetMaterialFileName(u32 i) {
	return matFiles[i]->fname;
}
void MAT_CacheMaterial(const char *fname) {
	g_core->Print("Caching material %s...\n",fname);
	MAT_RegisterMaterial(fname);
}
void MAT_ClearMaterialInUseFlags() {
	for(u32 i = 0; i < materials.size(); i++) {
		mtrIMPL_c *m = materials[i];
		m->markAsNotUsed();
	}
}
void MAT_FreeUnusedMaterials() {
	// make sure that renderer materials are marked as used
	rf->markUsedMaterials();

	u32 c_removed = 0;
	u32 c_left = 0;
	for(int i = 0; i < materials.size(); i++) {
		mtrIMPL_c *m = materials[i];
		if(m->isMarkedAsUsed() == false) {
			g_core->Print("MAT_FreeUnusedMaterials: material %s is not used - removing\n",m->getName());
			materials.removeEntry(m);
			i--;
			delete m;
			c_removed++;
		} else {
			g_core->Print("MAT_FreeUnusedMaterials: material %s is still used.\n",m->getName());
			c_left++;
		}
	}
	g_core->Print("MAT_FreeUnusedMaterials: %i removed, %i left\n",c_removed,c_left);
}
void MAT_CacheAllMaterialsFromMatFile(const char *fname) {
	for(u32 i = 0; i < matFiles.size(); i++) {
		if(!stricmp(matFiles[i]->fname,fname)) {
			matFiles[i]->iterateAllAvailableMaterialNames(MAT_CacheMaterial);
		}
	}
}
class mtrAPI_i *MAT_CreateHLBSPTexture(const char *newMatName, const byte *pixels, u32 width, u32 height, const byte *palette) {
	mtrIMPL_c *ret = materials.getEntry(newMatName);
	if(ret == 0) {
		ret = new mtrIMPL_c;
		// set material name first and add it to hash table
		ret->setName(newMatName);
		materials.addObject(ret);
	} else {
		// just clear the existing material
		ret->clear();
	}
	byte *converted = 0;
	u32 convertedW, convertedH;
	g_img->convert8BitImageToRGBA32(&converted,&convertedW,&convertedH,pixels,width,height,palette);
	textureAPI_i *tex = MAT_CreateTexture(newMatName,converted,convertedW,convertedH,4);
	g_img->freeImageData(converted);
	ret->createFromTexturePointer(tex);
	return ret;
}
class mtrAPI_i *MAT_RegisterMaterialAPI(const char *matName) {
	return MAT_RegisterMaterial(matName);
}
class mtrAPI_i *MAT_FindLoadedMaterialAPI(const char *matName) {
	mtrIMPL_c *ret = materials.getEntry(matName);
	if(ret) {
		return ret;
	}
	return 0;;
}
bool MAT_IsMaterialOrImagePresent(const char *matName) {
	// try to load from material text (.shader/.mtr files)
	matTextDef_s text;
	if(MAT_FindMaterialText(matName,text)) {
		return true; // OK, found
	}
	// see if the image with such name (or similiar, extension can be different!) exists
	byte *data = 0;
	u32 w, h;
	const char *fixedPath = g_img->loadImage(matName,&data,&w,&h);
	if(data == 0) {
		return false;
	}
	g_img->freeImageData(data);
	return true; // OK, texture image exists
}
void MAT_FreeAllMaterials() {
	for(u32 i = 0; i < materials.size(); i++) {
		mtrIMPL_c *m = materials[i];
		delete m;
		materials[i] = 0;
	}
	materials.clear();
}
void MAT_FreeCachedMaterialsTest() {
	for(u32 i = 0; i < matFiles.size(); i++) {
		matFile_s *mf = matFiles[i];
		delete mf;
		matFiles[i] = 0;
	}
	matFiles.clear();
}

static void MAT_RefreshSingleMaterial_f() {
	if(g_core->Argc() < 2) {
		g_core->Print("usage: \"mat_refreshSingleMaterial <matName>\"\n");
		return;
	}
	const char *matName = g_core->Argv(1);
	MAT_ReloadSingleMaterial(matName);
}
static void MAT_RefreshMaterialSourceFile_f() {
	if(g_core->Argc() < 2) {
		g_core->Print("usage: \"mat_refreshMaterialSourceFile <mtrFileName>\"\n");
		return;
	}
	const char *mtrFileName = g_core->Argv(1);
	MAT_ReloadMaterialFileSource(mtrFileName);
}
static void MAT_PrintMaterialSourceFileName_f() {
	if(g_core->Argc() < 2) {
		g_core->Print("usage: \"mat_printMaterialSourceFileName <matName>\"\n");
		return;
	}
	const char *mtrFileName = g_core->Argv(1);
	MAT_PrintMaterialSourceFileName(mtrFileName);
}
static void MAT_PrintLoadedMaterialNames_f() {
	g_core->Print("%i materials loaded\n",materials.size());
	for(u32 i = 0; i < materials.size(); i++) {
		mtrIMPL_c *mat = materials[i];
		g_core->Print("%i/%i - %s from %s\n",i,materials.size()
			,mat->getName(),mat->getSourceFileName());
	}
	g_core->Print("%i materials loaded\n",materials.size());
}
static void MAT_PrintLoadedMaterialFileNames_f() {
	g_core->Print("%i matFiles loaded\n",matFiles.size());
	for(u32 i = 0; i < matFiles.size(); i++) {
		matFile_s *mat = matFiles[i];
		g_core->Print("%i/%i - %s\n",i,materials.size()
			,mat->fname.c_str());
	}
	g_core->Print("%i materials loaded\n",materials.size());
}
static void MAT_PreviewMaterialText_f() {
	if(g_core->Argc() < 2) {
		g_core->Print("usage: \"mat_previewMaterialText <matName> <text>\"\n");
		return;
	}
	const char *matName = g_core->Argv(1);
	g_core->Print("Previewing new text of material %s\n",matName);
	const char *txt = g_core->ArgsFrom(2);
	g_core->Print("Text %s\n",txt);
	//MAT_SetMaterialText(matName,txt);
	MAT_ReloadSingleMaterial(matName,txt);
}

static aCmd_c mat_refreshSingleMaterial_f("mat_refreshSingleMaterial",MAT_RefreshSingleMaterial_f);
static aCmd_c mat_refreshMaterialSourceFile_f("mat_refreshMaterialSourceFile",MAT_RefreshMaterialSourceFile_f);
static aCmd_c mat_printMaterialSourceFileName("mat_printMaterialSourceFileName",MAT_PrintMaterialSourceFileName_f);
static aCmd_c mat_printLoadedMaterialNames("mat_printLoadedMaterialNames",MAT_PrintLoadedMaterialNames_f);
static aCmd_c mat_printLoadedMaterialFileNames("mat_printLoadedMaterialFileNames",MAT_PrintLoadedMaterialFileNames_f);
static aCmd_c mat_previewMaterialText("mat_previewMaterialText",MAT_PreviewMaterialText_f);

