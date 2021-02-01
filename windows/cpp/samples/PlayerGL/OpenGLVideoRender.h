/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once

#include <windows.h>
#include <GL/gl.h>
#include "PlayerWindow.h"


class OpenGLVideoRender: public PlayerWindow
{
public:
	OpenGLVideoRender(HINSTANCE hInstance, LPCTSTR title, int width, int height);
	~OpenGLVideoRender();

	bool Start();
	void Stop();

	void Draw();

	void SetFrame(unsigned char* pBuffer, unsigned int bufferSize, int frameWidth, int frameHeight);

	virtual void OnResize(int width, int height);

	void SetDisplayAspect(int width, int height);

private:

	int _wndWidth;
	int _wndHeight;

	int _displayAspectWidth;
	int _displayAspectHeight;

	void UpdateViewPort();

	HDC	 _hDC;
	HGLRC _hRC;
	GLuint _texture;
};
