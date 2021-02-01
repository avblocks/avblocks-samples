/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "OpenGLVideoRender.h"

#pragma comment( lib, "opengl32.lib")


OpenGLVideoRender::OpenGLVideoRender(HINSTANCE hInstance, LPCTSTR title, int width, int height):
	PlayerWindow(hInstance, title, width, height), 
	_hDC(NULL), 
	_hRC(NULL), 
	_texture(0),
	_wndWidth(width),
	_wndHeight(height),
	_displayAspectWidth(1),
	_displayAspectHeight(1)
{

}

OpenGLVideoRender::~OpenGLVideoRender()
{
	Stop();
}

bool OpenGLVideoRender::Start()
{
	Stop();

	_hDC = GetDC(GetHWnd());

	// set the pixel format for the DC
	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cDepthBits = 16;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int  iFormat = ChoosePixelFormat(_hDC, &pfd);
	SetPixelFormat(_hDC, iFormat, &pfd);

	// create and enable the render context (RC)
	_hRC = wglCreateContext(_hDC);
	wglMakeCurrent(_hDC, _hRC);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	UpdateViewPort();

	return true;
}

void OpenGLVideoRender::Stop()
{
	if (NULL != _hDC)
	{
		glDeleteTextures(1, &_texture);

		wglMakeCurrent(_hDC, NULL);
		wglDeleteContext(_hRC);
		ReleaseDC(GetHWnd(), _hDC);

		_hDC = NULL;
		_hRC = NULL;
		_texture = 0;
	}
}

void OpenGLVideoRender::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindTexture(GL_TEXTURE_2D, _texture);

	glBegin(GL_QUADS);
		glTexCoord2d(0.0, 0.0); glVertex2d(-1.0, -1.0);
		glTexCoord2d(1.0, 0.0); glVertex2d(+1.0, -1.0);
		glTexCoord2d(1.0, 1.0); glVertex2d(+1.0, +1.0);
		glTexCoord2d(0.0, 1.0); glVertex2d(-1.0, +1.0);
	glEnd();

	SwapBuffers(_hDC);
}

void OpenGLVideoRender::SetFrame(unsigned char* pBuffer, unsigned int bufferSize, int frameWidth, int frameHeight)
{
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frameWidth, frameHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pBuffer);
}

void OpenGLVideoRender::SetDisplayAspect(int width, int height)
{
	_displayAspectWidth = width;
	_displayAspectHeight = height;

	UpdateViewPort();
}

void OpenGLVideoRender::UpdateViewPort()
{
	double windowAspect = (double)_wndWidth / (double)_wndHeight;
	double displayAspect = (double)_displayAspectWidth / (double)_displayAspectHeight;

	if(windowAspect < displayAspect)
	{
		int width = _wndWidth;
		int height = static_cast<int>(_wndWidth / displayAspect);
		glViewport(0, (_wndHeight - height) / 2, width, height);
	}
	else
	{
		int width = static_cast<int>(_wndHeight * displayAspect);
		int height = _wndHeight;
		glViewport((_wndWidth - width) / 2, 0, width, height);
	}
}

void OpenGLVideoRender::OnResize(int newWidth, int newHeight)
{
	_wndWidth = newWidth;
	_wndHeight = newHeight;

	UpdateViewPort();
}
