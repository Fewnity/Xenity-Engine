/**
 * @file gu2gl.h
 * @author Nathan Bourgeois (iridescentrosesfall@gmail.com)
 * @brief
 * @version 1.0
 * @date 2022-09-06
 *
 * @copyright Copyright (c) 2022
 *
 */

// MIT License
//
// Copyright (c) 2022 Nathan Bourgeois
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>

#ifndef _GU2GL_INCLUDED_H
#define _GU2GL_INCLUDED_H

#ifdef __cplusplus
extern "C"
{
#endif

#define PSP_BUF_WIDTH (512)
#define PSP_SCR_WIDTH (480)
#define PSP_SCR_HEIGHT (272)

/* PI, float-sized */
#define GL_PI (3.141593f)

/* Boolean values for convenience */
#define GL_FALSE (0)
#define GL_TRUE (1)

/* Primitive types */
#define GL_POINTS (0)
#define GL_LINES (1)
#define GL_LINE_STRIP (2)
#define GL_TRIANGLES (3)
#define GL_TRIANGLE_STRIP (4)
#define GL_TRIANGLE_FAN (5)
#define GL_SPRITES (6)

/* States */
#define GL_ALPHA_TEST (0)
#define GL_DEPTH_TEST (1)
#define GL_SCISSOR_TEST (2)
#define GL_STENCIL_TEST (3)
#define GL_BLEND (4)
#define GL_CULL_FACE (5)
#define GL_DITHER (6)
#define GL_FOG (7)
#define GL_CLIP_PLANES (8)
#define GL_TEXTURE_2D (9)
#define GL_LIGHTING (10)
#define GL_LIGHT0 (11)
#define GL_LIGHT1 (12)
#define GL_LIGHT2 (13)
#define GL_LIGHT3 (14)
#define GL_LINE_SMOOTH (15)
#define GL_PATCH_CULL_FACE (16)
#define GL_COLOR_TEST (17)
#define GL_COLOR_LOGIC_OP (18)
#define GL_FACE_NORMAL_REVERSE (19)
#define GL_PATCH_FACE (20)
#define GL_FRAGMENT_2X (21)

/* Matrix modes */
#define GL_PROJECTION (0)
#define GL_VIEW (1)
#define GL_MODEL (2)
#define GL_TEXTURE (3)

/* Vertex Declarations Begin */
#define GL_TEXTURE_SHIFT(n) ((n) << 0)
#define GL_TEXTURE_8BIT GL_TEXTURE_SHIFT(1)
#define GL_TEXTURE_16BIT GL_TEXTURE_SHIFT(2)
#define GL_TEXTURE_32BITF GL_TEXTURE_SHIFT(3)
#define GL_TEXTURE_BITS GL_TEXTURE_SHIFT(3)

#define GL_COLOR_SHIFT(n) ((n) << 2)
#define GL_COLOR_5650 GL_COLOR_SHIFT(4)
#define GL_COLOR_5551 GL_COLOR_SHIFT(5)
#define GL_COLOR_4444 GL_COLOR_SHIFT(6)
#define GL_COLOR_8888 GL_COLOR_SHIFT(7)
#define GL_COLOR_BITS GL_COLOR_SHIFT(7)

#define GL_NORMAL_SHIFT(n) ((n) << 5)
#define GL_NORMAL_8BIT GL_NORMAL_SHIFT(1)
#define GL_NORMAL_16BIT GL_NORMAL_SHIFT(2)
#define GL_NORMAL_32BITF GL_NORMAL_SHIFT(3)
#define GL_NORMAL_BITS GL_NORMAL_SHIFT(3)

#define GL_VERTEX_SHIFT(n) ((n) << 7)
#define GL_VERTEX_8BIT GL_VERTEX_SHIFT(1)
#define GL_VERTEX_16BIT GL_VERTEX_SHIFT(2)
#define GL_VERTEX_32BITF GL_VERTEX_SHIFT(3)
#define GL_VERTEX_BITS GL_VERTEX_SHIFT(3)

#define GL_WEIGHT_SHIFT(n) ((n) << 9)
#define GL_WEIGHT_8BIT GL_WEIGHT_SHIFT(1)
#define GL_WEIGHT_16BIT GL_WEIGHT_SHIFT(2)
#define GL_WEIGHT_32BITF GL_WEIGHT_SHIFT(3)
#define GL_WEIGHT_BITS GL_WEIGHT_SHIFT(3)

#define GL_INDEX_SHIFT(n) ((n) << 11)
#define GL_INDEX_8BIT GL_INDEX_SHIFT(1)
#define GL_INDEX_16BIT GL_INDEX_SHIFT(2)
#define GL_INDEX_BITS GL_INDEX_SHIFT(3)

#define GL_WEIGHTS(n) ((((n)-1) & 7) << 14)
#define GL_WEIGHTS_BITS GL_WEIGHTS(8)
#define GL_VERTICES(n) ((((n)-1) & 7) << 18)
#define GL_VERTICES_BITS GL_VERTICES(8)

#define GL_TRANSFORM_SHIFT(n) ((n) << 23)
#define GL_TRANSFORM_3D GL_TRANSFORM_SHIFT(0)
#define GL_TRANSFORM_2D GL_TRANSFORM_SHIFT(1)
#define GL_TRANSFORM_BITS GL_TRANSFORM_SHIFT(1)
/* Vertex Declarations End */

/* Pixel Formats */
#define GL_PSM_5650 (0)	 /* Display, Texture, Palette */
#define GL_PSM_5551 (1)	 /* Display, Texture, Palette */
#define GL_PSM_4444 (2)	 /* Display, Texture, Palette */
#define GL_PSM_8888 (3)	 /* Display, Texture, Palette */
#define GL_PSM_T4 (4)	 /* Texture */
#define GL_PSM_T8 (5)	 /* Texture */
#define GL_PSM_T16 (6)	 /* Texture */
#define GL_PSM_T32 (7)	 /* Texture */
#define GL_PSM_DXT1 (8)	 /* Texture */
#define GL_PSM_DXT3 (9)	 /* Texture */
#define GL_PSM_DXT5 (10) /* Texture */

/* Spline Mode */
#define GL_FILL_FILL (0)
#define GL_OPEN_FILL (1)
#define GL_FILL_OPEN (2)
#define GL_OPEN_OPEN (3)

/* Shading Model */
#define GL_FLAT (0)
#define GL_SMOOTH (1)

/* Logical operation */
#define GL_CLEAR (0)
#define GL_AND (1)
#define GL_AND_REVERSE (2)
#define GL_COPY (3)
#define GL_AND_INVERTED (4)
#define GL_NOOP (5)
#define GL_XOR (6)
#define GL_OR (7)
#define GL_NOR (8)
#define GL_EQUIV (9)
#define GL_INVERTED (10)
#define GL_OR_REVERSE (11)
#define GL_COPY_INVERTED (12)
#define GL_OR_INVERTED (13)
#define GL_NAND (14)
#define GL_SET (15)

/* Texture Filter */
#define GL_NEAREST (0)
#define GL_LINEAR (1)
#define GL_NEAREST_MIPMAP_NEAREST (4)
#define GL_LINEAR_MIPMAP_NEAREST (5)
#define GL_NEAREST_MIPMAP_LINEAR (6)
#define GL_LINEAR_MIPMAP_LINEAR (7)

/* Texture Map Mode */
#define GL_TEXTURE_COORDS (0)
#define GL_TEXTURE_MATRIX (1)
#define GL_ENVIRONMENT_MAP (2)

/* Texture Level Mode */
#define GL_TEXTURE_AUTO (0)
#define GL_TEXTURE_CONST (1)
#define GL_TEXTURE_SLOPE (2)

/* Texture Projection Map Mode */
#define GL_POSITION (0)
#define GL_UV (1)
#define GL_NORMALIZED_NORMAL (2)
#define GL_NORMAL (3)

/* Wrap Mode */
#define GL_REPEAT (0)
#define GL_CLAMP (1)

/* Front Face Direction */
#define GL_CW (0)
#define GL_CCW (1)

/* Test Function */
#define GL_NEVER (0)
#define GL_ALWAYS (1)
#define GL_EQUAL (2)
#define GL_NOTEQUAL (3)
#define GL_LESS (4)
#define GL_LEQUAL (5)
#define GL_GREATER (6)
#define GL_GEQUAL (7)

/* Clear Buffer Mask */
#define GL_COLOR_BUFFER_BIT (1)
#define GL_STENCIL_BUFFER_BIT (2)
#define GL_DEPTH_BUFFER_BIT (4)
#define GL_FAST_CLEAR_BIT (16)

/* Texture Effect */
#define GL_TFX_MODULATE (0)
#define GL_TFX_DECAL (1)
#define GL_TFX_BLEND (2)
#define GL_TFX_REPLACE (3)
#define GL_TFX_ADD (4)

/* Texture Color Component */
#define GL_TCC_RGB (0)
#define GL_TCC_RGBA (1)

/* Blending Op */
#define GL_ADD (0)
#define GL_SUBTRACT (1)
#define GL_REVERSE_SUBTRACT (2)
#define GL_MIN (3)
#define GL_MAX (4)
#define GL_ABS (5)

/* Blending Factor */
#define GL_SRC_COLOR (0)
#define GL_ONE_MINUS_SRC_COLOR (1)
#define GL_SRC_ALPHA (2)
#define GL_ONE_MINUS_SRC_ALPHA (3)
#define GL_DST_COLOR (0)
#define GL_ONE_MINUS_DST_COLOR (1)
#define GL_DST_ALPHA (4)
#define GL_ONE_MINUS_DST_ALPHA (5)
#define GL_FIX (10)

/* Stencil Operations */
#define GL_KEEP (0)
#define GL_ZERO (1)
#define GL_REPLACE (2)
#define GL_INVERT (3)
#define GL_INCR (4)
#define GL_DECR (5)

/* Light Components */
#define GL_AMBIENT (1)
#define GL_DIFFUSE (2)
#define GL_SPECULAR (4)
#define GL_AMBIENT_AND_DIFFUSE (GL_AMBIENT | GL_DIFFUSE)
#define GL_DIFFUSE_AND_SPECULAR (GL_DIFFUSE | GL_SPECULAR)
#define GL_UNKNOWN_LIGHT_COMPONENT (8)

/* Light modes */
#define GL_SINGLE_COLOR (0)
#define GL_SEPARATE_SPECULAR_COLOR (1)

/* Light Type */
#define GL_DIRECTIONAL (0)
#define GL_POINTLIGHT (1)
#define GL_SPOTLIGHT (2)

/* Contexts */
#define GL_DIRECT (0)
#define GL_CALL (1)
#define GL_SEND (2)

/* List Queue */
#define GL_TAIL (0)
#define GL_HEAD (1)

/* Sync behavior (mode) */
#define GL_SYNC_FINISH (0)
#define GL_SYNC_SIGNAL (1)
#define GL_SYNC_DONE (2)
#define GL_SYNC_LIST (3)
#define GL_SYNC_SEND (4)

/* behavior (what) */
#define GL_SYNC_WAIT (0)
#define GL_SYNC_NOWAIT (1)

/* Sync behavior (what) [see pspge.h] */
#define GL_SYNC_WHAT_DONE (0)
#define GL_SYNC_WHAT_QUEUED (1)
#define GL_SYNC_WHAT_DRAW (2)
#define GL_SYNC_WHAT_STALL (3)
#define GL_SYNC_WHAT_CANCEL (4)

/* Signals */
#define GL_CALLBACK_SIGNAL (1)
#define GL_CALLBACK_FINISH (4)

/* Signal behavior */
#define GL_BEHAVIOR_SUSPEND (1)
#define GL_BEHAVIOR_CONTINUE (2)

/* Color Macros, maps 8 bit unsigned channels into one 32-bit value */
#define GL_ABGR(a, b, g, r) (((a) << 24) | ((b) << 16) | ((g) << 8) | (r))
#define GL_ARGB(a, r, g, b) GL_ABGR((a), (b), (g), (r))
#define GL_RGBA(r, g, b, a) GL_ARGB((a), (r), (g), (b))

/* Color Macro, maps floating point channels (0..1) into one 32-bit value */
#define GL_COLOR(r, g, b, a) GL_RGBA((u32)((r)*255.0f), (u32)((g)*255.0f), (u32)((b)*255.0f), (u32)((a)*255.0f))

	void guglInit(void *list);

	/**
	 * @brief Terminates the graphics context
	 *
	 */
	void guglTerm();

	/**
	 * @brief Start Frame
	 *
	 * @param list A valid GE list
	 * @param dialog Whether or not this frame is used for PSP Dialog
	 */
	void guglStartFrame(void *list, int dialog);

	/**
	 * @brief Swaps buffers and ends the frame
	 *
	 * @param vsync - Whether or not to wait for a VBLANK or not
	 * @param dialog - Whether or not this frame is used for PSP Dialog
	 */
	void guglSwapBuffers(int vsync, int dialog);

	void *getStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm);
	void *getStaticVramTexture(unsigned int width, unsigned int height, unsigned int psm);
	void glBlendFunc(int src, int dest);
	void glTranslatef(float x, float y, float z);
	// void glRotatef(float angle, float x, float y, float z);
	void glRotatefZYX(float z, float y, float x);
	void glRotatefXYZ(float x, float y, float z);
	void glScalef(float z, float y, float x);

#define glEnable sceGuEnable
#define glDisable sceGuDisable

#define glClear sceGuClear
#define glClearColor sceGuClearColor
#define glClearDepth sceGuClearDepth
#define glClearStencil sceGuClearStencil

#define glOffset sceGuOffset
#define glViewport sceGuViewport
#define glScissor sceGuScissor

#define glDrawElements sceGumDrawArray

#define glDepthFunc sceGuDepthFunc
#define glDepthMask sceGuDepthMask
#define glDepthOffset sceGuDepthOffset

#define glFog sceGuFog
#define glLight sceGuLight
#define glLightAtt sceGuLightAtt
#define glLightColor sceGuLightColor
#define glLightMode sceGuLightMode
#define glLightSpot sceGuLightSpot
#define glPixelMask sceGuPixelMask
#define glColor sceGuColor
#define glColorFunc sceGuColorFunc
#define glColorMaterial sceGuColorMaterial
#define glAlphaFunc sceGuAlphaFunc
#define glAmbient sceGuAmbient
#define glAmbientColor sceGuAmbientColor
	// #define glBlendFunc sceGuBlendFunc

#define glMaterial sceGuMaterial
#define glModelColor sceGuModelColor
#define glStencilFunc sceGuStencilFunc
#define glStencilOp sceGuStencilOp
#define glSpecular sceGuSpecular
#define glFrontFace sceGuFrontFace
#define glLogicalOp sceGuLogicalOp
#define glSetDither sceGuSetDither

#define glTexEnvColor sceGuTexEnvColor
#define glTexFilter sceGuTexFilter
#define glTexFlush sceGuTexFlush
#define glTexFunc sceGuTexFunc
#define glTexImage sceGuTexImage
#define glTexLevelMode sceGuTexLevelMode
#define glTexMapMode sceGuTexMapMode
#define glTexMode sceGuTexMode
#define glTexOffset sceGuTexOffset
#define glTexScale sceGuTexScale
#define glTexSlope sceGuTexSlope
#define glDepthFunc sceGuDepthFunc
#define glDepthMask sceGuDepthMask
#define glDepthOffset sceGuDepthOffset
#define glDepthRange sceGuDepthRange
#define glTexSync sceGuTexSync
#define glTexWrap sceGuTexWrap

	/**
	 * Matrix functions
	 */

#define glMatrixMode sceGumMatrixMode
#define glLoadIdentity sceGumLoadIdentity
#define glLoadMatrix sceGumLoadMatrix
#define glOrtho sceGumOrtho
#define glPerspective sceGumPerspective
#define glPushMatrix sceGumPushMatrix
#define glPopMatrix sceGumPopMatrix

/**
 * GLU Functions
 */
#define gluLookAt sceGumLookAt
#define gluMultMatrix sceGumMultMatrix
#define gluRotateX sceGumRotateX
#define gluRotateY sceGumRotateY
#define gluRotateZ sceGumRotateZ
#define gluRotateXYZ sceGumRotateXYZ
// #define gluRotateZYX sceGumRotateZYX
// #define gluScale sceGumScale
// #define gluTranslate sceGumTranslate
#define gluFullInverse sceGumFullInverse
#define gluFastInverse sceGumFastInverse

/// IMPLEMENTATION
#ifdef GUGL_IMPLEMENTATION
	void guglInit(void *list)
	{
		void *fbp0 = getStaticVramBuffer(PSP_BUF_WIDTH, PSP_SCR_HEIGHT, GU_PSM_8888);
		void *fbp1 = getStaticVramBuffer(PSP_BUF_WIDTH, PSP_SCR_HEIGHT, GU_PSM_8888);
		void *zbp = getStaticVramBuffer(PSP_BUF_WIDTH, PSP_SCR_HEIGHT, GU_PSM_4444);

		sceGuInit();

		sceGuStart(GU_DIRECT, list);
		sceGuDrawBuffer(GU_PSM_8888, fbp0, PSP_BUF_WIDTH);
		sceGuDispBuffer(PSP_SCR_WIDTH, PSP_SCR_HEIGHT, fbp1, PSP_BUF_WIDTH);
		sceGuDepthBuffer(zbp, PSP_BUF_WIDTH);
		sceGuOffset(2048 - (PSP_SCR_WIDTH / 2), 2048 - (PSP_SCR_HEIGHT / 2));
		sceGuViewport(2048, 2048, PSP_SCR_WIDTH, PSP_SCR_HEIGHT);
		sceGuDepthRange(65535, 0);
		sceGuScissor(0, 0, PSP_SCR_WIDTH, PSP_SCR_HEIGHT);
		sceGuEnable(GU_SCISSOR_TEST);
		sceGuDepthFunc(GU_GEQUAL);
		sceGuEnable(GU_DEPTH_TEST);
		sceGuFrontFace(GU_CW);
		sceGuShadeModel(GU_SMOOTH);
		sceGuEnable(GU_CULL_FACE);
		// sceGuDisable(GU_CULL_FACE);
		sceGuEnable(GU_TEXTURE_2D);
		sceGuEnable(GU_CLIP_PLANES);
		sceGuFinish();
		sceGuSync(0, 0);

		sceDisplayWaitVblankStart();
		sceGuDisplay(GU_TRUE);
	};

	void glBlendFunc(int src, int dest)
	{
		sceGuBlendFunc(GL_ADD, src, dest, 0, 0);
	}

	void glTranslatef(float x, float y, float z)
	{
		ScePspFVector3 v = {x, y, z};
		sceGumTranslate(&v);
	}

#define gluScale sceGumScale
	void glRotatefZYX(float z, float y, float x)
	{
		ScePspFVector3 v = {z, y, x};
		sceGumRotateZYX(&v);
	}

	void glRotatefXYZ(float x, float y, float z)
	{
		ScePspFVector3 v = {x, y, z};
		sceGumRotateXYZ(&v);
	}

	void glScalef(float x, float y, float z)
	{
		ScePspFVector3 v = {x, y, z};
		sceGumScale(&v);
	}

	void guglTerm()
	{
		sceGuTerm();
	}

	void guglStartFrame(void *list, int dialog)
	{
		sceGuStart(GU_DIRECT, list);

		if (dialog)
		{
			sceGuFinish();
			sceGuSync(0, 0);
		}
	}

	void guglSwapBuffers(int vsync, int dialog)
	{
		if (!dialog)
		{
			sceGuFinish();
			sceGuSync(0, 0);
		}

		if (vsync)
		{
			sceDisplayWaitVblankStart();
		}

		sceGuSwapBuffers();
	}

	static unsigned int getMemorySize(unsigned int width, unsigned int height, unsigned int psm)
	{
		switch (psm)
		{
		case GU_PSM_T4:
			return (width * height) >> 1;

		case GU_PSM_T8:
			return width * height;

		case GU_PSM_5650:
		case GU_PSM_5551:
		case GU_PSM_4444:
		case GU_PSM_T16:
			return 2 * width * height;

		case GU_PSM_8888:
		case GU_PSM_T32:
			return 4 * width * height;

		default:
			return 0;
		}
	}

	void *getStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm)
	{
		static unsigned int staticOffset = 0;
		unsigned int memSize = getMemorySize(width, height, psm);
		void *result = (void *)staticOffset;
		staticOffset += memSize;

		return result;
	}

	void *getStaticVramTexture(unsigned int width, unsigned int height, unsigned int psm)
	{
		void *result = getStaticVramBuffer(width, height, psm);
		return (void *)(((unsigned int)result) + ((unsigned int)sceGeEdramGetAddr()));
	}

#endif

#ifdef __cplusplus
}
#endif
#endif