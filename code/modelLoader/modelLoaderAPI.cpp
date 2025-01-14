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
// modelLoaderAPI.cpp - model loader interface

#include "modelLoaderLocal.h"
#include "skelModelImpl.h"
#include "skelAnimImpl.h"
#include "hl2MDLReader.h"
#include <api/iFaceMgrAPI.h>
#include <api/vfsAPI.h>
#include <api/cvarAPI.h>
#include <api/coreAPI.h>
#include <api/imgAPI.h>
#include <api/rAPI.h>
#include <api/rbAPI.h>
#include <api/modelLoaderDLLAPI.h>
#include <api/staticModelCreatorAPI.h>
#include <api/materialSystemAPI.h>
#include <shared/autoCvar.h>
#include <shared/autoCMD.h>
#include <shared/skc.h>

void MOD_CreateSphere(staticModelCreatorAPI_i *out, float radius, unsigned int rings, unsigned int sectors) {
	float const R = 1./(float)(rings-1);
	float const S = 1./(float)(sectors-1);
	int r, s;

	out->setNumSurfs(1);

	u32 numVeritces =  (rings * sectors * 3);
	out->resizeSurfaceVerts(0,numVeritces);
	u32 vertexIndex = 0;
	for(r = 0; r < rings; r++) {
		for(s = 0; s < sectors; s++) {
			float const y = sin( -M_PI_2 + M_PI * r * R );
			float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
			float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );

			float st[2];
			st[0] = s*S;
			st[1] = r*R;

			float xyz[3];
			xyz[0] = x * radius;
			xyz[1] = y * radius;
			xyz[2] = z * radius;

			
			out->setSurfaceVert(0,vertexIndex,xyz,st);

			vertexIndex++;
		}
	}
	u32 numIndices = ((rings-1) * (sectors-1) * 6);
	out->resizeIndices(numIndices);
	u32 index = 0;
	for(r = 0; r < rings-1; r++) 	{
		for(s = 0; s < sectors-1; s++) {
			u32 i0 = r * sectors + s;
			u32 i1 = r * sectors + (s+1);
			u32 i2 = (r+1) * sectors + (s+1);
			u32 i3 = (r+1) * sectors + s;

			out->setIndex(index,i0);
			index++;
			out->setIndex(index,i1);
			index++;
			out->setIndex(index,i2);
			index++;
			out->setIndex(index,i2);
			index++;
			out->setIndex(index,i3);
			index++;
			out->setIndex(index,i0);
			index++;
		}
	}
	out->countDuplicatedTriangles();
}
bool MOD_LoadModelFromHeightmap(const char *fname, staticModelCreatorAPI_i *out);
class modelLoaderDLLIMPL_c : public modelLoaderDLLAPI_i {
public:
	// for console commands and cvars
	virtual void initModelLoader() {
		AUTOCMD_RegisterAutoConsoleCommands();
		AUTOCVAR_RegisterAutoCvars();
	}
	virtual void shutdownModelLoader() {
		AUTOCMD_UnregisterAutoConsoleCommands();
		AUTOCVAR_UnregisterAutoCvars();
	}
	virtual bool isStaticModelFile(const char *fname) {
		// that's a procedural model
		if(fname[0] == '_')
			return true;
		const char *ext = strchr(fname,'.');
		if(ext == 0) {
			return false;
		}
		ext++;
		// Wavefront .obj static triangle model
		if(!_stricmp(ext,"obj"))
			return true;
		// raw .map file that will be converted to trimesh
		if(!_stricmp(ext,"map"))
			return true;
		// .ASE model (this format is used in Doom3)
		if(!_stricmp(ext,"ase"))
			return true;
		// we can load heightmaps here as well
		if(!_stricmp(ext,"png") || !_stricmp(ext,"jpg") || !_stricmp(ext,"tga") || !_stricmp(ext,"bmp"))
			return true;
		// Quake3 .md3 models (without animations; only first frame is loaded)
		if(!_stricmp(ext,"md3"))
			return true;
		// LWO, used in Doom3 along with ASE
		if(!_stricmp(ext,"lwo"))
			return true;
		// HL2 (Source Engine) .mdl (without animations)
		if(!_stricmp(ext,"mdl"))
			return true;
		// models can be created by mdlpp script
		if(!_stricmp(ext,"mdlpp"))
			return true;
		// 3ds, only for static models right now
		if(!_stricmp(ext,"3ds"))
			return true;
		return false;
	}
	virtual bool loadStaticModelFile(const char *fileNameWithExtraCommands, class staticModelCreatorAPI_i *out)  {
		// extra post process commands (like model scaling, rotating, etc)
		// can be applied directly in model name string, after special '|' character, eg:
		// "models/props/truck.obj|scale10|texturespecialTexture"
		const char *inlinePostProcessCommandMarker = strchr(fileNameWithExtraCommands,'|');
		// get raw filename
		str fname = fileNameWithExtraCommands;
		if(inlinePostProcessCommandMarker) {
			fname.capLen(inlinePostProcessCommandMarker-fileNameWithExtraCommands);
		}
		// see if it's a build-in model shape
		if(fname[0] == '_') {
			if(!_strnicmp(fname+1,"quadZ",5)) {
				// it's a flat, Z-oriented quad
				// (in Q3 Z axis is up-down)
				float x,y;
				sscanf(fname+1+5,"%fx%f",&x,&y);
				simpleVert_s points[4];
				points[0].xyz.set(x*0.5f,y*0.5,0);
				points[0].tc.set(1,0);
				points[1].xyz.set(x*0.5f,-y*0.5,0);
				points[1].tc.set(1,1);
				points[2].xyz.set(-x*0.5f,-y*0.5,0);
				points[2].tc.set(0,1);
				points[3].xyz.set(-x*0.5f,y*0.5,0);
				points[3].tc.set(0,0);
				out->addTriangle("nomaterial",points[0],points[1],points[2]);
				out->addTriangle("nomaterial",points[2],points[3],points[0]);
				if(inlinePostProcessCommandMarker) {
					MOD_ApplyInlinePostProcess(inlinePostProcessCommandMarker,out);
				}
				return false; // no error
			} else if(!_strnicmp(fname+1,"sphere",6)) {
				float radius;
				sscanf(fname+1+6,"%f",&radius);
				MOD_CreateSphere(out,radius,16,16);
				if(inlinePostProcessCommandMarker) {
					MOD_ApplyInlinePostProcess(inlinePostProcessCommandMarker,out);
				}
				return false; // no error
			}
		}
		const char *ext = strchr(fname,'.');
		if(ext == 0) {
			return true;
		}
		ext++; // skip '.'
		bool error;
		if(!_stricmp(ext,"obj")) {
			error = MOD_LoadOBJ(fname,out);
		} else if(!_stricmp(ext,"ase")) {
			error = MOD_LoadASE(fname,out);
		} else if(!_stricmp(ext,"map")) {
			error = MOD_LoadConvertMapFileToStaticTriMesh(fname,out);
		} else if(!_stricmp(ext,"png") || !_stricmp(ext,"jpg") || !_stricmp(ext,"tga") || !_stricmp(ext,"bmp")) {
			error = MOD_LoadModelFromHeightmap(fname,out);
		} else if(!_stricmp(ext,"md3")) {
			// load md3 as static model (first animation frame)
			error = MOD_LoadStaticMD3(fname,out);
		} else if(!_stricmp(ext,"lwo")) {
			error = MOD_LoadLWO(fname,out);
		} else if(!_stricmp(ext,"3ds")) {
			error = MOD_Load3DS(fname,out);
		} else if(!_stricmp(ext,"mdl")) {
			hl2MDLReader_c r;
			if(r.beginReading(fname) == false) {
				error = r.getStaticModelData(out);
			} else {
				error = true;
			}
		} else if(!_stricmp(ext,"mdlpp")) {
			error = MOD_CreateModelFromMDLPPScript(fname,out);
		} else {
			error = true;
		}
		if(error == false) {
			// apply model postprocess steps (scaling, rotating, etc)
			// defined in optional .mdlpp file
			if(_stricmp(ext,"mdlpp")) {
				MOD_ApplyPostProcess(fname,out);
			}
			if(inlinePostProcessCommandMarker) {
				MOD_ApplyInlinePostProcess(inlinePostProcessCommandMarker,out);
			}
			return false; // no error
		}
		return true;
	}
	virtual bool isKeyFramedModelFile(const char *fname) {
		const char *ext = strchr(fname,'.');
		if(ext == 0) {
			return false;
		}	
		ext++; // skip '.'
		if(!_stricmp(ext,"md3")) {
			u32 numFrames = MOD_ReadMD3FileFrameCount(fname);
			if(numFrames > 1) {
				return true;
			}
		}
		if(!_stricmp(ext,"md2")) {
			// TODO: read the number of md2 frame?
			return true;
		}
		if(!_stricmp(ext,"mdc")) {
			// TODO: read the number of mdc frame?
			return true;
		}
		if(!_stricmp(ext,"tan")) {
			// TODO: read the number of tan frame?
			return true;
		}
		return false;
	}
	virtual class kfModelAPI_i *loadKeyFramedModelFile(const char *fname) {
		const char *ext = strchr(fname,'.');
		if(ext == 0) {
			return 0;
		}	
		//if(!stricmp(ext,"md3")) {
		//	return MOD_LoadAnimatedMD3(fname);
		//}
		return KF_LoadKeyFramedModelAPI(fname);
		//return 0;
	}
	virtual bool isSkelModelFile(const char *fname) {
		const char *ext = strchr(fname,'.');
		if(ext == 0) {
			return false;
		}
		ext++;
		if(!_stricmp(ext,"md5mesh"))
			return true;
		if(!_stricmp(ext,"psk"))
			return true;
		// ET
		if(!_stricmp(ext,"mdm"))
			return true;
		// RTCW
		if(!_stricmp(ext,"mds"))
			return true;
		// Source (intermediate)
		if(!_stricmp(ext,"smd"))
			return true;
		// Wolfenstein 2009
		if(!_stricmp(ext,"md5r"))
			return true;
		// FAKK
		if(!_stricmp(ext,"skb"))
			return true;
		// MoHAA
		if(!_stricmp(ext,"skd"))
			return true;
		return false;
	}
	virtual class skelModelAPI_i *loadSkelModelFile(const char *fname) {
		// check for empty file name
		if(fname == 0 || fname[0] == 0)
			return 0;
		skelModelIMPL_c *skelModel = new skelModelIMPL_c;
		str tmp = fname;
		if(tmp.hasExt("md5mesh")) {
			if(skelModel->loadMD5Mesh(fname)) {
				delete skelModel;
				return 0;
			}		
		} else if(tmp.hasExt("psk")) {
			if(skelModel->loadPSK(fname)) {
				delete skelModel;
				return 0;
			}			
		} else if(tmp.hasExt("mdm")) {
			if(skelModel->loadMDM(fname)) {
				delete skelModel;
				return 0;
			}			
		} else if(tmp.hasExt("mds")) {
			if(skelModel->loadMDS(fname)) {
				delete skelModel;
				return 0;
			}			
		} else if(tmp.hasExt("smd")) {
			if(skelModel->loadSMD(fname)) {
				delete skelModel;
				return 0;
			}			
		} else if(tmp.hasExt("md5r")) {
			if(skelModel->loadMD5R(fname)) {
				delete skelModel;
				return 0;
			}			
		} else if(tmp.hasExt("skb")) {
			if(skelModel->loadSKB(fname)) {
				delete skelModel;
				return 0;
			}			
		} else if(tmp.hasExt("skd")) {
			if(skelModel->loadSKD(fname)) {
				delete skelModel;
				return 0;
			}
		} else {
			delete skelModel;
			return 0;
		}
	//	skelModel->recalcEdges();
		//if(skelModel) {
			// apply model postprocess steps (scaling, rotating, etc)
			// defined in optional .mdlpp file
			MOD_ApplyPostProcess(fname,skelModel);
		//}
		return skelModel;
	}
	virtual bool isSkelAnimFile(const char *fname) {
		const char *ext = strchr(fname,'.');
		if(ext == 0) {
			return false;
		}
		ext++;
		if(!_stricmp(ext,"md5anim"))
			return true;
		if(!_stricmp(ext,"mdx"))
			return true;
		if(!_stricmp(ext,"mds"))
			return true;
		if(!_stricmp(ext,"smd"))
			return true;
		if(!_stricmp(ext,"ska"))
			return true;
		// psk has a baseframe which can be used as single-frame animation
		if(!_stricmp(ext,"psk"))
			return true;
		if(!_stricmp(ext,"psa"))
			return true;
		if(!_stricmp(ext,"skc"))
			return true;
		return false;
	}
	virtual class skelAnimAPI_i *loadSkelAnimFile(const char *fname) {
		const char *ext = strchr(fname,'.');
		if(ext == 0) {
			return 0;
		}
		ext++; // skip '.'
		skelAnimAPI_i *ret = 0;
		if(!_stricmp(ext,"md5anim")) {
			skelAnimMD5_c *md5Anim = new skelAnimMD5_c;
			if(md5Anim->loadMD5Anim(fname)) {
				delete md5Anim;
				return 0;
			}
			ret = md5Anim;
		} else if(!_stricmp(ext,"mdx")) {
			skelAnimGeneric_c *mdxAnim = new skelAnimGeneric_c;
			if(mdxAnim->loadMDXAnim(fname)) {
				delete mdxAnim;
				return 0;
			}
			ret = mdxAnim;
		} else if(!_stricmp(ext,"mds")) {
			skelAnimGeneric_c *mdxAnim = new skelAnimGeneric_c;
			if(mdxAnim->loadMDSAnim(fname)) {
				delete mdxAnim;
				return 0;
			}
			ret = mdxAnim;
		} else if(!_stricmp(ext,"smd")) {
			skelAnimGeneric_c *smdAnim = new skelAnimGeneric_c;
			if(smdAnim->loadSMDAnim(fname)) {
				delete smdAnim;
				return 0;
			}
			ret = smdAnim;
		} else if(!_stricmp(ext,"ska")) {
			skelAnimGeneric_c *skaAnim = new skelAnimGeneric_c;
			if(skaAnim->loadSKAAnim(fname)) {
				delete skaAnim;
				return 0;
			}
			ret = skaAnim;
		} else if(!_stricmp(ext,"psk")) {
			skelAnimGeneric_c *pskAnim = new skelAnimGeneric_c;
			if(pskAnim->loadPSKAnim(fname)) {
				delete pskAnim;
				return 0;
			}
			ret = pskAnim;
		} else if(!_stricmp(ext,"psa")) {
			skelAnimGeneric_c *psaAnim = new skelAnimGeneric_c;
			if(psaAnim->loadPSAAnim(fname)) {
				delete psaAnim;
				return 0;
			}
			ret = psaAnim;
		} else if(!_stricmp(ext,"skc")) {
#if 0
			//skelAnimChannels_c *skcAnim = new skelAnimChannels_c;
			//if(skcAnim->loadSKCAnim(fname)) {
			//	delete skcAnim;
			//	return 0;
			//}
			//ret = skcAnim;
#else
			skelAnimGeneric_c *skcAnim = new skelAnimGeneric_c;
			if(skcAnim->loadSKCAnim(fname)) {
				delete skcAnim;
				return 0;
			}
			ret = skcAnim;
#endif
		} else {
			return 0;
		}
		SK_ApplyAnimPostProcess(fname,ret);
		return ret;
	}
	// read the number of animation frames in .md3 file (1 for non-animated models, 0 if model file does not exist)
	virtual u32 readMD3FrameCount(const char *fname) {
		return MOD_ReadMD3FileFrameCount(fname);
	}
	virtual bool convertToMD5Mesh(const char *fname, const char *out) {
		str tmpFName, tmpOut;
		skelModelAPI_i *a = loadSkelModelFile(fname);
		if(a == 0) {
			tmpFName = "models/";
			tmpFName.append(fname);
			a = loadSkelModelFile(tmpFName);
			if(a == 0) {
				return true;
			}
			fname = tmpFName;
		}
		str strOut;
		if(out == 0) {
			strOut = fname;
			strOut.setExtension("md5mesh");
		} else {
			strOut = out;
		}	
		a->writeMD5Mesh(strOut);
		return false;
	}
	virtual bool convertToMD5Anim(const char *fname, const char *out) {
		str tmpFName, tmpOut;
		skelAnimAPI_i *a = loadSkelAnimFile(fname);
		if(a == 0) {
			tmpFName = "models/";
			tmpFName.append(fname);
			a = loadSkelAnimFile(tmpFName);
			if(a == 0) {
				return true;
			}
			fname = tmpFName;
		}
		str strOut;
		if(out == 0) {
			strOut = fname;
			strOut.setExtension("md5anim");
		} else {
			strOut = out;
		}	
		a->convertToMD5Anim(strOut);
		return false;
	}
};

// interface manager (import)
class iFaceMgrAPI_i *g_iFaceMan = 0;
// imports
vfsAPI_s *g_vfs = 0;
cvarsAPI_s *g_cvars = 0;
coreAPI_s *g_core = 0;
materialSystemAPI_i *g_ms = 0;
imgAPI_i *g_img = 0;
rAPI_i *rf = 0;
rbAPI_i *rb = 0;
// exports
static modelLoaderDLLIMPL_c g_staticModelLoaderDLLAPI;
modelLoaderDLLAPI_i *g_modelLoader = &g_staticModelLoaderDLLAPI;

void SKC_TestAllFiles() {
	skc_c test;
	test.loadSKC("models/human/animation/weapon_mp44/mp44_reload.skc");


	int numSkcs;
	char **skcNames = g_vfs->FS_ListFiles( "models/", "skc", &numSkcs );
	for (u32 i = 0; i < numSkcs; i++) {
		str fn = "models/";
		fn.append(skcNames[i]);
		test.loadSKC(fn);
	}
	g_vfs->FS_FreeFileList( skcNames );
}
void ConvertToMD5Anim_f() {
	if(g_core->Argc() < 2) {
		g_core->Print("ConvertToMD5Anim_f: function required at least one argument (animation file name)\n");
		return;
	}
	const char *from = g_core->Argv(1);
	const char *to;
	if(g_core->Argc() >= 3) {
		to = g_core->Argv(2);
	} else {
		to = 0;
	}
	if(to) {
		g_core->Print("ConvertToMD5Anim: %s to %s\n",from,to);
	} else {
		g_core->Print("ConvertToMD5Anim: %s\n",from);
	}
	g_modelLoader->convertToMD5Anim(from,to);
}
void ConvertToMD5Mesh_f() {
	if(g_core->Argc() < 2) {
		g_core->Print("ConvertToMD5Mesh_f: function required at least one argument (model file name)\n");
		return;
	}
	const char *from = g_core->Argv(1);
	const char *to;
	if(g_core->Argc() >= 3) {
		to = g_core->Argv(2);
	} else {
		to = 0;;
	}
	if(to) {
		g_core->Print("ConvertToMD5Mesh: %s to %s\n",from,to);
	} else {
		g_core->Print("ConvertToMD5Mesh: %s\n",from);
	}
	g_modelLoader->convertToMD5Mesh(from,to);
}
static aCmd_c convertToMD5Anim("convertToMD5Anim",ConvertToMD5Anim_f);
static aCmd_c convertPSAToMD5Anim("convertPSAToMD5Anim",ConvertToMD5Anim_f);
static aCmd_c convertMDSToMD5Anim("convertMDSToMD5Anim",ConvertToMD5Anim_f);

static aCmd_c convertToMD5Mesh("convertToMD5Mesh",ConvertToMD5Mesh_f);
static aCmd_c convertPSKToMD5Mesh("convertPSKToMD5Mesh",ConvertToMD5Mesh_f);
static aCmd_c convertMDSToMD5Mesh("convertMDSToMD5Mesh",ConvertToMD5Mesh_f);


void ShareAPIs(iFaceMgrAPI_i *iFMA) {
	g_iFaceMan = iFMA;

	// exports
	g_iFaceMan->registerInterface((iFaceBase_i *)(void*)g_modelLoader,MODELLOADERDLL_API_IDENTSTR);

	// imports
	g_iFaceMan->registerIFaceUser(&g_vfs,VFS_API_IDENTSTR);
	g_iFaceMan->registerIFaceUser(&g_cvars,CVARS_API_IDENTSTR);
	g_iFaceMan->registerIFaceUser(&g_core,CORE_API_IDENTSTR);
	g_iFaceMan->registerIFaceUser(&g_ms,MATERIALSYSTEM_API_IDENTSTR);
	g_iFaceMan->registerIFaceUser(&g_img,IMG_API_IDENTSTR);
	g_iFaceMan->registerIFaceUser(&rf,RENDERER_API_IDENTSTR);
	g_iFaceMan->registerIFaceUser(&rb,RENDERER_BACKEND_API_IDENTSTR);
}

qioModule_e IFM_GetCurModule() {
	return QMD_MODELLOADER;
}

