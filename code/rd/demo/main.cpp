//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath>

#include <vector>
#include <string>

#include "imgui.h"
#include "imguiRenderGL.h"

#include "Recast.h"
#include "RecastDebugDraw.h"
#include "InputGeom.h"
#include "TestCase.h"
#include "Filelist.h"
#include "SlideShow.h"
#include "Sample_SoloMesh.h"
#include "Sample_TileMesh.h"
#include "Sample_TempObstacles.h"
#include "Sample_Debug.h"

#ifdef WIN32
#	define snprintf _snprintf
#	define putenv _putenv
#endif

using std::string;
using std::vector;

struct SampleItem
{
	Sample* (*create)();
	const string name;
};
Sample* createSolo() { return new Sample_SoloMesh(); }
Sample* createTile() { return new Sample_TileMesh(); }
Sample* createTempObstacle() { return new Sample_TempObstacles(); }
Sample* createDebug() { return new Sample_Debug(); }
static SampleItem g_samples[] =
{
	{ createSolo, "Solo Mesh" },
	{ createTile, "Tile Mesh" },
	{ createTempObstacle, "Temp Obstacles" },
//	{ createDebug, "Debug" },
};
static const int g_nsamples = sizeof(g_samples) / sizeof(SampleItem);
//
//int main(int /*argc*/, char** /*argv*/)
//{
//	// Init SDL
//	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
//	{
//		printf("Could not initialise SDL\n");
//		return -1;
//	}
//	
//	// Center window
//	char env[] = "SDL_VIDEO_CENTERED=1";
//	putenv(env);
//
//	// Init OpenGL
//	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
//	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
//	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
////#ifndef WIN32
//	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
//	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
////#endif
//
//	const SDL_VideoInfo* vi = SDL_GetVideoInfo();
//
//	bool presentationMode = false;
//
//	int width, height;
//	SDL_Surface* screen = 0;
//	
//	if (presentationMode)
//	{
//		width = vi->current_w;
//		height = vi->current_h;
//		screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL|SDL_FULLSCREEN);
//	}
//	else
//	{	
//		width = rcMin(vi->current_w, (int)(vi->current_h * 16.0 / 9.0));
//		width = width - 80;
//		height = vi->current_h - 80;
//		screen = SDL_SetVideoMode(width, height, 0, SDL_OPENGL);
//	}
//	
//	if (!screen)
//	{
//		printf("Could not initialise SDL opengl\n");
//		return -1;
//	}
//
//	glEnable(GL_MULTISAMPLE);
//
//	SDL_WM_SetCaption("Recast Demo", 0);
//	
//	if (!imguiRenderGLInit("DroidSans.ttf"))
//	{
//		printf("Could not init GUI renderer.\n");
//		SDL_Quit();
//		return -1;
//	}
//	
//	float t = 0.0f;
//	float timeAcc = 0.0f;
//	Uint32 prevFrameTime = SDL_GetTicks();
//	int mousePos[2] = {0, 0};
//	int origMousePos[2] = {0, 0}; // Used to compute mouse movement totals across frames.
//	
//	float cameraEulers[] = {45, -45};
//	float cameraPos[] = {0, 0, 0};
//	float camr = 1000;
//	float origCameraEulers[] = {0, 0}; // Used to compute rotational changes across frames.
//	
//	float moveW = 0, moveS = 0, moveA = 0, moveD = 0;
//	
//	float scrollZoom = 0;
//	bool rotate = false;
//	bool movedDuringRotate = false;
//	float rayStart[3];
//	float rayEnd[3];
//	bool mouseOverMenu = false;
//	
//	bool showMenu = !presentationMode;
//	bool showLog = false;
//	bool showTools = true;
//	bool showLevels = false;
//	bool showSample = false;
//	bool showTestCases = false;
//
//	// Window scroll positions.
//	int propScroll = 0;
//	int logScroll = 0;
//	int toolsScroll = 0;
//	
//	string sampleName = "Choose Sample...";
//	
//	vector<string> files;
//	const string meshesFolder = "Meshes";
//	string meshName = "Choose Mesh...";
//	
//	float markerPosition[3] = {0, 0, 0};
//	bool markerPositionSet = false;
//	
//	SlideShow slideShow;
//	slideShow.init("slides/");
//	
//	InputGeom* geom = 0;
//	Sample* sample = 0;
//
//	const string testCasesFolder = "TestCases";
//	TestCase* test = 0;
//
//	BuildContext ctx;
//	
//	// Fog.
//	float fogColor[4] = { 0.32f, 0.31f, 0.30f, 1.0f };
//	glEnable(GL_FOG);
//	glFogi(GL_FOG_MODE, GL_LINEAR);
//	glFogf(GL_FOG_START, camr * 0.1f);
//	glFogf(GL_FOG_END, camr * 1.25f);
//	glFogfv(GL_FOG_COLOR, fogColor);
//	
//	glEnable(GL_CULL_FACE);
//	glDepthFunc(GL_LEQUAL);
//	
//	bool done = false;
//	while(!done)
//	{
//		// Handle input events.
//		int mouseScroll = 0;
//		bool processHitTest = false;
//		bool processHitTestShift = false;
//		SDL_Event event;
//		
//		while (SDL_PollEvent(&event))
//		{
//			switch (event.type)
//			{
//				case SDL_KEYDOWN:
//					// Handle any key presses here.
//					if (event.key.keysym.sym == SDLK_ESCAPE)
//					{
//						done = true;
//					}
//					else if (event.key.keysym.sym == SDLK_t)
//					{
//						showLevels = false;
//						showSample = false;
//						showTestCases = true;
//						scanDirectory(testCasesFolder, ".txt", files);
//					}
//					else if (event.key.keysym.sym == SDLK_TAB)
//					{
//						showMenu = !showMenu;
//					}
//					else if (event.key.keysym.sym == SDLK_SPACE)
//					{
//						if (sample)
//							sample->handleToggle();
//					}
//					else if (event.key.keysym.sym == SDLK_1)
//					{
//						if (sample)
//							sample->handleStep();
//					}
//					else if (event.key.keysym.sym == SDLK_9)
//					{
//						if (sample && geom)
//						{
//							string savePath = meshesFolder + "/";
//							BuildSettings settings;
//							memset(&settings, 0, sizeof(settings));
//
//							rcVcopy(settings.navMeshBMin, geom->getNavMeshBoundsMin());
//							rcVcopy(settings.navMeshBMax, geom->getNavMeshBoundsMax());
//
//							sample->collectSettings(settings);
//
//							geom->saveGeomSet(&settings);
//						}
//					}
//					else if (event.key.keysym.sym == SDLK_RIGHT)
//					{
//						slideShow.nextSlide();
//					}
//					else if (event.key.keysym.sym == SDLK_LEFT)
//					{
//						slideShow.prevSlide();
//					}
//					break;
//			
//				case SDL_MOUSEBUTTONDOWN:
//					if (event.button.button == SDL_BUTTON_RIGHT)
//					{
//						if (!mouseOverMenu)
//						{
//							// Rotate view
//							rotate = true;
//							movedDuringRotate = false;
//							origMousePos[0] = mousePos[0];
//							origMousePos[1] = mousePos[1];
//							origCameraEulers[0] = cameraEulers[0];
//							origCameraEulers[1] = cameraEulers[1];
//						}
//					}
//					else if (event.button.button == SDL_BUTTON_WHEELUP)
//					{
//						if (mouseOverMenu)
//							mouseScroll--;
//						else
//							scrollZoom -= 1.0f;
//					}
//					else if (event.button.button == SDL_BUTTON_WHEELDOWN)
//					{
//						if (mouseOverMenu)
//							mouseScroll++;
//						else
//							scrollZoom += 1.0f;
//					}
//					break;
//					
//				case SDL_MOUSEBUTTONUP:
//					// Handle mouse clicks here.
//					if (event.button.button == SDL_BUTTON_RIGHT)
//					{
//						rotate = false;
//						if (!mouseOverMenu)
//						{
//							if (!movedDuringRotate)
//							{
//								processHitTest = true;
//								processHitTestShift = true;
//							}
//						}
//					}
//					else if (event.button.button == SDL_BUTTON_LEFT)
//					{
//						if (!mouseOverMenu)
//						{
//							processHitTest = true;
//							processHitTestShift = (SDL_GetModState() & KMOD_SHIFT) ? true : false;
//						}
//					}
//					
//					break;
//					
//				case SDL_MOUSEMOTION:
//					mousePos[0] = event.motion.x;
//					mousePos[1] = height-1 - event.motion.y;
//					
//					if (rotate)
//					{
//						int dx = mousePos[0] - origMousePos[0];
//						int dy = mousePos[1] - origMousePos[1];
//						cameraEulers[0] = origCameraEulers[0] - dy * 0.25f;
//						cameraEulers[1] = origCameraEulers[1] + dx * 0.25f;
//						if (dx * dx + dy * dy > 3 * 3)
//						{
//							movedDuringRotate = true;
//						}
//					}
//					break;
//					
//				case SDL_QUIT:
//					done = true;
//					break;
//					
//				default:
//					break;
//			}
//		}
//
//		unsigned char mouseButtonMask = 0;
//		if (SDL_GetMouseState(0, 0) & SDL_BUTTON_LMASK)
//			mouseButtonMask |= IMGUI_MBUT_LEFT;
//		if (SDL_GetMouseState(0, 0) & SDL_BUTTON_RMASK)
//			mouseButtonMask |= IMGUI_MBUT_RIGHT;
//		
//		Uint32 time = SDL_GetTicks();
//		float dt = (time - prevFrameTime) / 1000.0f;
//		prevFrameTime = time;
//		
//		t += dt;
//
//		// Hit test mesh.
//		if (processHitTest && geom && sample)
//		{
//			float hitTime;
//			bool hit = geom->raycastMesh(rayStart, rayEnd, hitTime);
//			
//			if (hit)
//			{
//				if (SDL_GetModState() & KMOD_CTRL)
//				{
//					// Marker
//					markerPositionSet = true;
//					markerPosition[0] = rayStart[0] + (rayEnd[0] - rayStart[0]) * hitTime;
//					markerPosition[1] = rayStart[1] + (rayEnd[1] - rayStart[1]) * hitTime;
//					markerPosition[2] = rayStart[2] + (rayEnd[2] - rayStart[2]) * hitTime;
//				}
//				else
//				{
//					float pos[3];
//					pos[0] = rayStart[0] + (rayEnd[0] - rayStart[0]) * hitTime;
//					pos[1] = rayStart[1] + (rayEnd[1] - rayStart[1]) * hitTime;
//					pos[2] = rayStart[2] + (rayEnd[2] - rayStart[2]) * hitTime;
//					sample->handleClick(rayStart, pos, processHitTestShift);
//				}
//			}
//			else
//			{
//				if (SDL_GetModState() & KMOD_CTRL)
//				{
//					// Marker
//					markerPositionSet = false;
//				}
//			}
//		}
//		
//		// Update sample simulation.
//		const float SIM_RATE = 20;
//		const float DELTA_TIME = 1.0f / SIM_RATE;
//		timeAcc = rcClamp(timeAcc + dt, -1.0f, 1.0f);
//		int simIter = 0;
//		while (timeAcc > DELTA_TIME)
//		{
//			timeAcc -= DELTA_TIME;
//			if (simIter < 5 && sample)
//			{
//				sample->handleUpdate(DELTA_TIME);
//			}
//			simIter++;
//		}
//
//		// Clamp the framerate so that we do not hog all the CPU.
//		const float MIN_FRAME_TIME = 1.0f / 40.0f;
//		if (dt < MIN_FRAME_TIME)
//		{
//			int ms = (int)((MIN_FRAME_TIME - dt) * 1000.0f);
//			if (ms > 10) ms = 10;
//			if (ms >= 0) SDL_Delay(ms);
//		}
//		
//		// Set the viewport.
//		glViewport(0, 0, width, height);
//		GLint viewport[4];
//		glGetIntegerv(GL_VIEWPORT, viewport);
//		
//		// Clear the screen
//		glClearColor(0.3f, 0.3f, 0.32f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		glDisable(GL_TEXTURE_2D);
//		glEnable(GL_DEPTH_TEST);
//		
//		// Compute the projection matrix.
//		glMatrixMode(GL_PROJECTION);
//		glLoadIdentity();
//		gluPerspective(50.0f, (float)width/(float)height, 1.0f, camr);
//		GLdouble projectionMatrix[16];
//		glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
//		
//		// Compute the modelview matrix.
//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();
//		glRotatef(cameraEulers[0], 1, 0, 0);
//		glRotatef(cameraEulers[1], 0, 1, 0);
//		glTranslatef(-cameraPos[0], -cameraPos[1], -cameraPos[2]);
//		GLdouble modelviewMatrix[16];
//		glGetDoublev(GL_MODELVIEW_MATRIX, modelviewMatrix);
//		
//		// Get hit ray position and direction.
//		GLdouble x, y, z;
//		gluUnProject(mousePos[0], mousePos[1], 0.0f, modelviewMatrix, projectionMatrix, viewport, &x, &y, &z);
//		rayStart[0] = (float)x;
//		rayStart[1] = (float)y;
//		rayStart[2] = (float)z;
//		gluUnProject(mousePos[0], mousePos[1], 1.0f, modelviewMatrix, projectionMatrix, viewport, &x, &y, &z);
//		rayEnd[0] = (float)x;
//		rayEnd[1] = (float)y;
//		rayEnd[2] = (float)z;
//		
//		// Handle keyboard movement.
//		Uint8* keystate = SDL_GetKeyState(NULL);
//		moveW = rcClamp(moveW + dt * 4 * (keystate[SDLK_w] ? 1 : -1), 0.0f, 1.0f);
//		moveS = rcClamp(moveS + dt * 4 * (keystate[SDLK_s] ? 1 : -1), 0.0f, 1.0f);
//		moveA = rcClamp(moveA + dt * 4 * (keystate[SDLK_a] ? 1 : -1), 0.0f, 1.0f);
//		moveD = rcClamp(moveD + dt * 4 * (keystate[SDLK_d] ? 1 : -1), 0.0f, 1.0f);
//		
//		float keybSpeed = 22.0f;
//		if (SDL_GetModState() & KMOD_SHIFT)
//			keybSpeed *= 4.0f;
//		
//		float movex = (moveD - moveA) * keybSpeed * dt;
//		float movey = (moveS - moveW) * keybSpeed * dt + scrollZoom * 2.0f;
//		scrollZoom = 0;
//		
//		cameraPos[0] += movex * (float)modelviewMatrix[0];
//		cameraPos[1] += movex * (float)modelviewMatrix[4];
//		cameraPos[2] += movex * (float)modelviewMatrix[8];
//		
//		cameraPos[0] += movey * (float)modelviewMatrix[2];
//		cameraPos[1] += movey * (float)modelviewMatrix[6];
//		cameraPos[2] += movey * (float)modelviewMatrix[10];
//
//		glEnable(GL_FOG);
//
//		if (sample)
//			sample->handleRender();
//		if (test)
//			test->handleRender();
//		
//		glDisable(GL_FOG);
//		
//		// Render GUI
//		glDisable(GL_DEPTH_TEST);
//		glMatrixMode(GL_PROJECTION);
//		glLoadIdentity();
//		gluOrtho2D(0, width, 0, height);
//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();
//		
//		mouseOverMenu = false;
//		
//		imguiBeginFrame(mousePos[0], mousePos[1], mouseButtonMask, mouseScroll);
//		
//		if (sample)
//		{
//			sample->handleRenderOverlay((double*)projectionMatrix, (double*)modelviewMatrix, (int*)viewport);
//		}
//		if (test)
//		{
//			if (test->handleRenderOverlay((double*)projectionMatrix, (double*)modelviewMatrix, (int*)viewport))
//				mouseOverMenu = true;
//		}
//
//		// Help text.
//		if (showMenu)
//		{
//			const char msg[] = "W/S/A/D: Move  RMB: Rotate";
//			imguiDrawText(280, height-20, IMGUI_ALIGN_LEFT, msg, imguiRGBA(255,255,255,128));
//		}
//		
//		if (showMenu)
//		{
//			if (imguiBeginScrollArea("Properties", width-250-10, 10, 250, height-20, &propScroll))
//				mouseOverMenu = true;
//
//			if (imguiCheck("Show Log", showLog))
//				showLog = !showLog;
//			if (imguiCheck("Show Tools", showTools))
//				showTools = !showTools;
//
//			imguiSeparator();
//			imguiLabel("Sample");
//			if (imguiButton(sampleName.c_str()))
//			{
//				if (showSample)
//				{
//					showSample = false;
//				}
//				else
//				{
//					showSample = true;
//					showLevels = false;
//					showTestCases = false;
//				}
//			}
//			
//			imguiSeparator();
//			imguiLabel("Input Mesh");
//			if (imguiButton(meshName.c_str()))
//			{
//				if (showLevels)
//				{
//					showLevels = false;
//				}
//				else
//				{
//					showSample = false;
//					showTestCases = false;
//					showLevels = true;
//					scanDirectory(meshesFolder, ".obj", files);
//					scanDirectoryAppend(meshesFolder, ".gset", files);
//				}
//			}
//			if (geom)
//			{
//				char text[64];
//				snprintf(text, 64, "Verts: %.1fk  Tris: %.1fk",
//						 geom->getMesh()->getVertCount()/1000.0f,
//						 geom->getMesh()->getTriCount()/1000.0f);
//				imguiValue(text);
//			}
//			imguiSeparator();
//
//			if (geom && sample)
//			{
//				imguiSeparatorLine();
//				
//				sample->handleSettings();
//
//				if (imguiButton("Build"))
//				{
//					ctx.resetLog();
//					if (!sample->handleBuild())
//					{
//						showLog = true;
//						logScroll = 0;
//					}
//					ctx.dumpLog("Build log %s:", meshName.c_str());
//					
//					// Clear test.
//					delete test;
//					test = 0;
//				}
//
//				imguiSeparator();
//			}
//			
//			if (sample)
//			{
//				imguiSeparatorLine();
//				sample->handleDebugMode();
//			}
//
//			imguiEndScrollArea();
//		}
//		
//		// Sample selection dialog.
//		if (showSample)
//		{
//			static int levelScroll = 0;
//			if (imguiBeginScrollArea("Choose Sample", width-10-250-10-200, height-10-250, 200, 250, &levelScroll))
//				mouseOverMenu = true;
//
//			Sample* newSample = 0;
//			for (int i = 0; i < g_nsamples; ++i)
//			{
//				if (imguiItem(g_samples[i].name.c_str()))
//				{
//					newSample = g_samples[i].create();
//					if (newSample)
//						sampleName = g_samples[i].name;
//				}
//			}
//			if (newSample)
//			{
//				delete sample;
//				sample = newSample;
//				sample->setContext(&ctx);
//				if (geom && sample)
//				{
//					sample->handleMeshChanged(geom);
//				}
//				showSample = false;
//			}
//
//			if (geom || sample)
//			{
//				const float* bmin = 0;
//				const float* bmax = 0;
//				if (geom)
//				{
//					bmin = geom->getNavMeshBoundsMin();
//					bmax = geom->getNavMeshBoundsMax();
//				}
//				// Reset camera and fog to match the mesh bounds.
//				if (bmin && bmax)
//				{
//					camr = sqrtf(rcSqr(bmax[0]-bmin[0]) +
//								 rcSqr(bmax[1]-bmin[1]) +
//								 rcSqr(bmax[2]-bmin[2])) / 2;
//					cameraPos[0] = (bmax[0] + bmin[0]) / 2 + camr;
//					cameraPos[1] = (bmax[1] + bmin[1]) / 2 + camr;
//					cameraPos[2] = (bmax[2] + bmin[2]) / 2 + camr;
//					camr *= 3;
//				}
//				cameraEulers[0] = 45;
//				cameraEulers[1] = -45;
//				glFogf(GL_FOG_START, camr*0.1f);
//				glFogf(GL_FOG_END, camr*1.25f);
//			}
//			
//			imguiEndScrollArea();
//		}
//		
//		// Level selection dialog.
//		if (showLevels)
//		{
//			static int levelScroll = 0;
//			if (imguiBeginScrollArea("Choose Level", width - 10 - 250 - 10 - 200, height - 10 - 450, 200, 450, &levelScroll))
//				mouseOverMenu = true;
//			
//			vector<string>::const_iterator fileIter = files.begin();
//			vector<string>::const_iterator filesEnd = files.end();
//			vector<string>::const_iterator levelToLoad = filesEnd;
//			for (; fileIter != filesEnd; ++fileIter)
//			{
//				if (imguiItem(fileIter->c_str()))
//				{
//					levelToLoad = fileIter;
//				}
//			}
//			
//			if (levelToLoad != filesEnd)
//			{
//				meshName = *levelToLoad;
//				showLevels = false;
//				
//				delete geom;
//				geom = 0;
//				
//				string path = meshesFolder + "/" + meshName;
//				
//				geom = new InputGeom;
//				if (!geom->load(&ctx, path))
//				{
//					delete geom;
//					geom = 0;
//					
//					showLog = true;
//					logScroll = 0;
//					ctx.dumpLog("Geom load log %s:", meshName.c_str());
//				}
//				if (sample && geom)
//				{
//					sample->handleMeshChanged(geom);
//				}
//
//				if (geom || sample)
//				{
//					const float* bmin = 0;
//					const float* bmax = 0;
//					if (geom)
//					{
//						bmin = geom->getNavMeshBoundsMin();
//						bmax = geom->getNavMeshBoundsMax();
//					}
//					// Reset camera and fog to match the mesh bounds.
//					if (bmin && bmax)
//					{
//						camr = sqrtf(rcSqr(bmax[0]-bmin[0]) +
//									 rcSqr(bmax[1]-bmin[1]) +
//									 rcSqr(bmax[2]-bmin[2])) / 2;
//						cameraPos[0] = (bmax[0] + bmin[0]) / 2 + camr;
//						cameraPos[1] = (bmax[1] + bmin[1]) / 2 + camr;
//						cameraPos[2] = (bmax[2] + bmin[2]) / 2 + camr;
//						camr *= 3;
//					}
//					cameraEulers[0] = 45;
//					cameraEulers[1] = -45;
//					glFogf(GL_FOG_START, camr * 0.1f);
//					glFogf(GL_FOG_END, camr * 1.25f);
//				}
//			}
//			
//			imguiEndScrollArea();
//			
//		}
//		
//		// Test cases
//		if (showTestCases)
//		{
//			static int testScroll = 0;
//			if (imguiBeginScrollArea("Choose Test To Run", width-10-250-10-200, height-10-450, 200, 450, &testScroll))
//				mouseOverMenu = true;
//
//			vector<string>::const_iterator fileIter = files.begin();
//			vector<string>::const_iterator filesEnd = files.end();
//			vector<string>::const_iterator testToLoad = filesEnd;
//			for (; fileIter != filesEnd; ++fileIter)
//			{
//				if (imguiItem(fileIter->c_str()))
//				{
//					testToLoad = fileIter;
//				}
//			}
//			
//			if (testToLoad != filesEnd)
//			{
//				string path = testCasesFolder + "/" + *testToLoad;
//				test = new TestCase;
//				if (test)
//				{
//					// Load the test.
//					if (!test->load(path))
//					{
//						delete test;
//						test = 0;
//					}
//
//					// Create sample
//					Sample* newSample = 0;
//					for (int i = 0; i < g_nsamples; ++i)
//					{
//						if (g_samples[i].name == test->getSampleName())
//						{
//							newSample = g_samples[i].create();
//							if (newSample)
//								sampleName = g_samples[i].name;
//						}
//					}
//					if (newSample)
//					{
//						delete sample;
//						sample = newSample;
//						sample->setContext(&ctx);
//						showSample = false;
//					}
//
//					// Load geom.
//					meshName = test->getGeomFileName();
//					
//					delete geom;
//					geom = 0;
//					
//					path = meshesFolder + "/" + meshName;
//					
//					geom = new InputGeom;
//					if (!geom || !geom->load(&ctx, path))
//					{
//						delete geom;
//						geom = 0;
//						showLog = true;
//						logScroll = 0;
//						ctx.dumpLog("Geom load log %s:", meshName.c_str());
//					}
//					if (sample && geom)
//					{
//						sample->handleMeshChanged(geom);
//					}
//
//					// This will ensure that tile & poly bits are updated in tiled sample.
//					if (sample)
//						sample->handleSettings();
//
//					ctx.resetLog();
//					if (sample && !sample->handleBuild())
//					{
//						ctx.dumpLog("Build log %s:", meshName.c_str());
//					}
//					
//					if (geom || sample)
//					{
//						const float* bmin = 0;
//						const float* bmax = 0;
//						if (geom)
//						{
//							bmin = geom->getNavMeshBoundsMin();
//							bmax = geom->getNavMeshBoundsMax();
//						}
//						// Reset camera and fog to match the mesh bounds.
//						if (bmin && bmax)
//						{
//							camr = sqrtf(rcSqr(bmax[0] - bmin[0]) +
//										 rcSqr(bmax[1] - bmin[1]) +
//										 rcSqr(bmax[2] - bmin[2])) / 2;
//							cameraPos[0] = (bmax[0] + bmin[0]) / 2 + camr;
//							cameraPos[1] = (bmax[1] + bmin[1]) / 2 + camr;
//							cameraPos[2] = (bmax[2] + bmin[2]) / 2 + camr;
//							camr *= 3;
//						}
//						cameraEulers[0] = 45;
//						cameraEulers[1] = -45;
//						glFogf(GL_FOG_START, camr * 0.2f);
//						glFogf(GL_FOG_END, camr * 1.25f);
//					}
//					
//					// Do the tests.
//					if (sample)
//						test->doTests(sample->getNavMesh(), sample->getNavMeshQuery());
//				}
//			}				
//				
//			imguiEndScrollArea();
//		}
//
//		
//		// Log
//		if (showLog && showMenu)
//		{
//			if (imguiBeginScrollArea("Log", 250 + 20, 10, width - 300 - 250, 200, &logScroll))
//				mouseOverMenu = true;
//			for (int i = 0; i < ctx.getLogCount(); ++i)
//				imguiLabel(ctx.getLogText(i));
//			imguiEndScrollArea();
//		}
//		
//		// Left column tools menu
//		if (!showTestCases && showTools && showMenu) // && geom && sample)
//		{
//			if (imguiBeginScrollArea("Tools", 10, 10, 250, height - 20, &toolsScroll))
//				mouseOverMenu = true;
//
//			if (sample)
//				sample->handleTools();
//			
//			imguiEndScrollArea();
//		}
//		
//		slideShow.updateAndDraw(dt, (float)width, (float)height);
//		
//		// Marker
//		if (markerPositionSet && gluProject((GLdouble)markerPosition[0], (GLdouble)markerPosition[1], (GLdouble)markerPosition[2],
//								  modelviewMatrix, projectionMatrix, viewport, &x, &y, &z))
//		{
//			// Draw marker circle
//			glLineWidth(5.0f);
//			glColor4ub(240,220,0,196);
//			glBegin(GL_LINE_LOOP);
//			const float r = 25.0f;
//			for (int i = 0; i < 20; ++i)
//			{
//				const float a = (float)i / 20.0f * RC_PI*2;
//				const float fx = (float)x + cosf(a)*r;
//				const float fy = (float)y + sinf(a)*r;
//				glVertex2f(fx,fy);
//			}
//			glEnd();
//			glLineWidth(1.0f);
//		}
//		
//		imguiEndFrame();
//		imguiRenderGLDraw();		
//		
//		glEnable(GL_DEPTH_TEST);
//		SDL_GL_SwapBuffers();
//	}
//	
//	imguiRenderGLDestroy();
//	
//	SDL_Quit();
//	
//	delete sample;
//	delete geom;
//	
//	return 0;
//}
