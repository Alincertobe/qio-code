/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.

This file is part of Quake III Arena source code.

Quake III Arena source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Quake III Arena source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__330BBF08_731C_11D1_B539_00AA00A410FC__INCLUDED_)
#define AFX_STDAFX_H__330BBF08_731C_11D1_B539_00AA00A410FC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#pragma warning(disable: 4305)            // For: double to float
#pragma warning(disable: 4800)            // For: performance warning on bool conversions

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC OLE automation classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__330BBF08_731C_11D1_B539_00AA00A410FC__INCLUDED_)

#include "MainFrm.h"
#include "PrefsDlg.h"
#include "FindTextureDlg.h"


extern CMainFrame* g_pParentWnd;
extern CString g_strAppPath;
extern CPrefsDlg& g_PrefsDlg;
extern CFindTextureDlg& g_dlgFind;

// layout styles
#define QR_SPLIT 0
#define QR_QE4 1
#define QR_4WAY 2
#define QR_SPLITZ 3


// externs
extern void AddSlash(CString&);
extern void DLLBuildDone();
extern void MFCCreate(HINSTANCE);
extern BOOL Init3Dfx();
extern void FindReplace(CString& strContents, const char* pTag, const char* pValue);
extern void CheckBspProcess();
extern void QE_CountBrushesAndUpdateStatusBar();
extern void	QE_CheckAutoSave();
extern mtrAPI_i	*current_texture;
extern BOOL SaveWindowState(HWND hWnd, const char *pszName);
extern BOOL DoMru(HWND, WORD);
extern void RunBsp (const char *command);
extern void Map_Snapshot();
extern void WXY_Print();
extern void AddProp( void );
extern bool DoColor(int iIndex);
extern entity_s	*edit_entity;
extern int inspector_mode;
extern bool g_bRotateMode;
extern bool g_bClipMode;
extern bool g_bScaleMode;
extern int g_nScaleHow;
extern bool g_bPathMode;
extern bool ByeByeSurfaceDialog();
extern void RunScript(char* pBuffer);
extern bool ExtractPath_and_Filename(const char* pPath, CString& strPath, CString& strFilename);
extern HINSTANCE g_hOpenGL32;
extern void Select_Scale(float x, float y, float z);
extern void Select_RotateTexture(int amt);
extern void Select_ScaleTexture(int x, int y);
extern void Select_ShiftTexture(int x, int y);
extern void FindReplaceTextures(const char* pFind, const char* pReplace, bool bSelected, bool bForce);
extern void DoProjectSettings();
extern bool region_active;
extern void Brush_Print(edBrush_c* b);
extern void	Texture_ShowDirectory (char* pPath, bool Linked = false);
extern void Map_ImportFile (char *filename);
extern void Map_SaveSelected(char* pFilename);
extern void UpdateSurfaceDialog();
extern bool g_bNewFace;
extern void Select_GetTrueMid (vec3_t mid);
extern bool g_bSwitch;
extern edBrush_c g_brFrontSplits;
extern edBrush_c g_brBackSplits;
extern CClipPoint g_Clip1;
extern CClipPoint g_Clip2;
extern edBrush_c* g_pSplitList;
extern CClipPoint g_PathPoints[256];
extern void AcquirePath(int nCount, PFNPathCallback* pFunc);
extern bool g_bScreenUpdates;
extern SCommandInfo g_Commands[];
extern int g_nCommandCount;
extern SKeyInfo g_Keys[];
extern int g_nKeyCount;
extern int inspector_mode;
extern const char	*bsp_commands[256];
extern void RunScriptByName(char*, bool);
extern void DoNewColor(int* i1, int* i2, int* i3);
extern void UpdateSurfaceDialog();
extern void CSG_SplitBrushByFace (edBrush_c *in, face_s *f, edBrush_c **front, edBrush_c **back);
extern void HandlePopup(CWnd* pWindow, unsigned int uId);
extern z_t z;
extern void Select_Scale(float x, float y, float z);
extern void NewBSP(char* pCommandLine, HWND);
extern void NewVIS(char* pCommandLine, HWND);
extern void NewRAD(char* pCommandLine, HWND);
extern void RunTools(char* pCommandLine, HWND, const char*);
extern void Clamp(float& f, int nClamp);
extern void MemFile_fprintf(CMemFile* pMemFile, const char* pText, ...);
extern void SaveWindowPlacement(HWND hwnd, const char* pName);
extern bool LoadWindowPlacement(HWND hwnd, const char* pName);
extern bool ConfirmModified ();
extern void DoPatchInspector();
void UpdatePatchInspector();
extern int BuildShortPathName(const char* pPath, char* pBuffer, int nBufferLen);
extern int g_nBrushId;


