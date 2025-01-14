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

#include <stdafx.h>
#include "qe3.h"

int	FindPoint (vec3_t point)
{
	int		i, j;

	for (i=0 ; i<g_qeglobals.d_numpoints ; i++)
	{
		for (j=0 ; j<3 ; j++)
			if (fabs(point[j] - g_qeglobals.d_points[i][j]) > 0.1)
				break;
			if (j == 3)
				return i;
	}

	g_qeglobals.d_points[g_qeglobals.d_numpoints] = point;
	if (g_qeglobals.d_numpoints < MAX_POINTS-1)
	{
		g_qeglobals.d_numpoints++;
	}

	return g_qeglobals.d_numpoints-1;
}

int FindEdge (int p1, int p2, face_s *f)
{
	int		i;

	for (i=0 ; i<g_qeglobals.d_numedges ; i++)
		if (g_qeglobals.d_edges[i].p1 == p2 && g_qeglobals.d_edges[i].p2 == p1)
		{
			g_qeglobals.d_edges[i].f2 = f;
			return i;
		}

	g_qeglobals.d_edges[g_qeglobals.d_numedges].p1 = p1;
	g_qeglobals.d_edges[g_qeglobals.d_numedges].p2 = p2;
	g_qeglobals.d_edges[g_qeglobals.d_numedges].f1 = f;

	if (g_qeglobals.d_numedges < MAX_EDGES-1)
	{
		g_qeglobals.d_numedges++;
	}

	return g_qeglobals.d_numedges-1;
}

void MakeFace (edBrush_c* b, face_s *f)
{
	texturedWinding_c	*w;
	int			i;
	int			pnum[128];

	w = b->makeFaceWinding(f);
	if (!w)
		return;
	for (i=0 ; i<w->size() ; i++)
		pnum[i] = FindPoint (w->getXYZ(i));
	for (i=0 ; i<w->size() ; i++)
		FindEdge (pnum[i], pnum[(i+1)%w->size()], f);

	free (w);
}

void SetupVertexSelection ()
{
	face_s	*f;
	edBrush_c *b;

	g_qeglobals.d_numpoints = 0;
	g_qeglobals.d_numedges = 0;

	for (b=selected_brushes.next ; b != &selected_brushes ; b=b->next)
  {
	  for (f=b->getFirstFace() ; f ; f=f->next)
		  MakeFace (b,f);
  }
}


void SelectFaceEdge (edBrush_c* b, face_s *f, int p1, int p2)
{
	texturedWinding_c	*w;
	int			i, j, k;
	int			pnum[128];

	w = b->makeFaceWinding(f);
	if (!w)
		return;
	for (i=0 ; i<w->size() ; i++)
		pnum[i] = FindPoint (w->getXYZ(i));

  for (i=0 ; i<w->size() ; i++)
		if (pnum[i] == p1 && pnum[(i+1)%w->size()] == p2)
		{
			f->planepts[0] = g_qeglobals.d_points[pnum[i]];
			f->planepts[1] = g_qeglobals.d_points[pnum[(i+1)%w->size()]];
			f->planepts[2] = g_qeglobals.d_points[pnum[(i+2)%w->size()]];
			for (j=0 ; j<3 ; j++)
			{
				for (k=0 ; k<3 ; k++)
				{
					f->planepts[j][k] = floor(f->planepts[j][k]/g_qeglobals.d_gridsize+0.5)*g_qeglobals.d_gridsize;
				}
			}

			AddPlanept (f->planepts[0]);
			AddPlanept (f->planepts[1]);
			break;
		}

	if (i == w->size())
		Sys_Printf ("SelectFaceEdge: failed\n");
	free (w);
}


void SelectVertex (int p1)
{
	edBrush_c		*b;
	texturedWinding_c	*w;
	int			i, j, k;
	face_s		*f;

	for (b=selected_brushes.next ; b != &selected_brushes ; b=b->next)
  {
	  for (f=b->getFirstFace() ; f ; f=f->next)
	  {
		  w =  b->makeFaceWinding(f);
		  if (!w)
			  continue;
		  for (i=0 ; i<w->size() ; i++)
		  {
			  if (FindPoint (w->getXYZ(i)) == p1)
			  {
				  f->planepts[0] = w->getXYZ((i+w->size()-1)%w->size());
				  f->planepts[1] = w->getXYZ(i);
				  f->planepts[2] = w->getXYZ((i+1)%w->size());
			    for (j=0 ; j<3 ; j++)
          {
				    for (k=0 ; k<3 ; k++)
            {
					    ;//f->planepts[j][k] = floor(f->planepts[j][k]/g_qeglobals.d_gridsize+0.5)*g_qeglobals.d_gridsize;
            } 
          }

			    AddPlanept (f->planepts[1]);
          //MessageBeep(-1);

			    break;
        }
		  }
		  delete (w);
	  }
  }
}

void SelectEdgeByRay (const vec3_c &org, const vec3_c &dir)
{
	int		i, j, besti;
	float	d, bestd;
	vec3_c	mid, temp;
	pedge_t	*e;

	// find the edge closest to the ray
	besti = -1;
	bestd = 8;

	for (i=0 ; i<g_qeglobals.d_numedges ; i++)
	{
		for (j=0 ; j<3 ; j++)
			mid[j] = 0.5*(g_qeglobals.d_points[g_qeglobals.d_edges[i].p1][j] + g_qeglobals.d_points[g_qeglobals.d_edges[i].p2][j]);

		temp = mid - org;
		d = temp.dotProduct(dir);
		temp.vectorMA (org, d, dir);
		temp = mid - temp;
		d = temp.vectorLength();
		if (d < bestd)
		{
			bestd = d;
			besti = i;
		}
	}

	if (besti == -1)
	{
		Sys_Printf ("Click didn't hit an edge\n");
		return;
	}
	Sys_Printf ("hit edge\n");

	// make the two faces that border the edge use the two edge points
	// as primary drag points
	g_qeglobals.d_num_move_points = 0;
	e = &g_qeglobals.d_edges[besti];
	for (edBrush_c* b=selected_brushes.next ; b != &selected_brushes ; b=b->next)
  {
    SelectFaceEdge (b, e->f1, e->p1, e->p2);
	  SelectFaceEdge (b, e->f2, e->p2, e->p1);
  }



}

void SelectVertexByRay3D (const vec3_c &org, const vec3_c &dir)
{
	int		i, besti;
	float	d, bestd;
	vec3_c	temp;

	// find the point closest to the ray
	besti = -1;
	bestd = 8;

	for (i=0 ; i<g_qeglobals.d_numpoints ; i++)
	{
		temp = g_qeglobals.d_points[i] - org;
		d = temp.dotProduct(dir);
		temp.vectorMA (org, d, dir);
		temp = g_qeglobals.d_points[i] - temp;
		d = temp.vectorLength();
		if (d < bestd)
		{
			bestd = d;
			besti = i;
		}
	}

	if (besti == -1)
	{
		Sys_Printf ("Click didn't hit a vertex\n");
		return;
	}
	Sys_Printf ("hit vertex\n");
	g_qeglobals.d_move_points[g_qeglobals.d_num_move_points++] = g_qeglobals.d_points[besti];
	//SelectVertex (besti);
}


void SelectVertexByRay2D (const vec3_c &org, const vec3_c &dir)
{
	int		i, besti;
	float	d, bestd;
	vec3_c	temp;

	// find the point closest to the ray
	besti = -1;
	bestd = 8;
	arraySTD_c<int> bests;

	int ax = dir.getLargestAxisAbs();

	for (i=0 ; i<g_qeglobals.d_numpoints ; i++)
	{
		temp = g_qeglobals.d_points[i] - org;
		d = temp.dotProduct(dir);
		temp.vectorMA (org, d, dir);
		temp = g_qeglobals.d_points[i] - temp;
		temp[ax] = 0;
		d = temp.vectorLength();
		if(abs(d-bestd) < 1.f)
		{
			bests.push_back(i);
		}
		else if (d < bestd)
		{
			bests.clear();
			bestd = d;
			besti = i;
			bests.push_back(i);
		}
	}

	if (besti == -1)
	{
		Sys_Printf ("Click didn't hit a vertex\n");
		return;
	}
	Sys_Printf ("hit 2d vertex\n");
	for(u32 i = 0; i < bests.size(); i++)
	{
		g_qeglobals.d_move_points[g_qeglobals.d_num_move_points++] = g_qeglobals.d_points[bests[i]];
		//SelectVertex (besti);
	}
}


extern void AddPatchMovePoint(const vec3_c &v, bool bMulti, bool bFull);
void SelectCurvePointByRay (const vec3_c &org, vec3_t dir, int buttons)
{
	int		i, besti;
	float	d, bestd;
	vec3_c	temp;

	// find the point closest to the ray
	besti = -1;
	bestd = 8;

	for (i=0 ; i<g_qeglobals.d_numpoints ; i++)
	{
		temp = g_qeglobals.d_points[i] - org;
		d = temp.dotProduct(dir);
		temp.vectorMA (org, d, dir);
		temp = g_qeglobals.d_points[i] - temp;
		d = temp.vectorLength();
		if (d <= bestd)
		{
			bestd = d;
			besti = i;
		}
	}

	if (besti == -1)
	{
		if (g_pParentWnd->ActiveXY()->AreaSelectOK())
		{
			g_qeglobals.d_select_mode = sel_area;
			g_qeglobals.d_vAreaTL = org;
			g_qeglobals.d_vAreaBR = org;
		}
		return;
	}
	//Sys_Printf ("hit vertex\n");
	AddPatchMovePoint(g_qeglobals.d_points[besti], buttons & MK_CONTROL, buttons & MK_SHIFT);
}



