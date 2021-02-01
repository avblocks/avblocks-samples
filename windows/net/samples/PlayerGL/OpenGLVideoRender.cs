/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

using System;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using SharpGL;
using SharpGL.Version;

namespace PlayerGLSample
{
    public class OpenGLVideoRender: UserControl
    {
        private System.ComponentModel.IContainer components = null;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        private void InitializeComponent()
        {
            this.SuspendLayout();
            // 
            // OpenGLVideoRender
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Name = "OpenGLVideoRender";
            this.ResumeLayout(false);
        }
        #endregion

        public OpenGLVideoRender()
        {
            InitializeComponent();

            //  Set the user draw styles.
            SetStyle(ControlStyles.AllPaintingInWmPaint, true);
            SetStyle(ControlStyles.UserPaint, true);
            SetStyle(ControlStyles.OptimizedDoubleBuffer, true);
        }

        private OpenGL gl;
        uint[] _texture = new uint[1];

        public bool Start()
        {
            Stop();

            gl = new OpenGL();
            //  Create the render context.
            gl.Create(OpenGLVersion.OpenGL1_1, RenderContextType.NativeWindow, Width, Height, 32, Handle);

            gl.Enable(OpenGL.GL_TEXTURE_2D);
            gl.GenTextures(1, _texture);
            gl.BindTexture(OpenGL.GL_TEXTURE_2D, _texture[0]);

            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_MIN_FILTER, OpenGL.GL_LINEAR);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_MAG_FILTER, OpenGL.GL_LINEAR);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_WRAP_S, OpenGL.GL_CLAMP);
            gl.TexParameter(OpenGL.GL_TEXTURE_2D, OpenGL.GL_TEXTURE_WRAP_T, OpenGL.GL_CLAMP);

            UpdateViewPort();

            return true;
        }

        public void Stop()
        {
            if (null != gl)
            {
                gl.DeleteTextures(1, _texture);
                gl.RenderContextProvider.Dispose();
                gl = null;
            }
        }

        public void Draw()
        {
            if (this.IsDisposed)
                return;

            if (null == gl)
                return;

            gl.Clear(OpenGL.GL_COLOR_BUFFER_BIT | OpenGL.GL_DEPTH_BUFFER_BIT);

            gl.BindTexture(OpenGL.GL_TEXTURE_2D, _texture[0]);

            gl.Begin(OpenGL.GL_QUADS);

            gl.TexCoord(0.0, 0.0); gl.Vertex(-1.0, -1.0);
            gl.TexCoord(1.0, 0.0); gl.Vertex(+1.0, -1.0);
            gl.TexCoord(1.0, 1.0); gl.Vertex(+1.0, +1.0);
            gl.TexCoord(0.0, 1.0); gl.Vertex(-1.0, +1.0);
            gl.End();

            using (Graphics g = CreateGraphics())
            {
                var hdc = g.GetHdc();
                gl.Blit(hdc);
                g.ReleaseHdc(hdc);
            }
        }

        public void SetFrame(byte[] pBuffer, int frameWidth, int frameHeight)
        {
            if (null == gl)
                return;

            gl.BindTexture(OpenGL.GL_TEXTURE_2D, _texture[0]);
            gl.TexImage2D(OpenGL.GL_TEXTURE_2D, 0, OpenGL.GL_RGB, frameWidth, frameHeight, 0, OpenGL.GL_BGR_EXT, OpenGL.GL_UNSIGNED_BYTE, pBuffer);
        }

        public void SetFrame(IntPtr pBuffer, int frameWidth, int frameHeight)
        {
            if (null == gl)
                return;

            gl.BindTexture(OpenGL.GL_TEXTURE_2D, _texture[0]);
            gl.TexImage2D(OpenGL.GL_TEXTURE_2D, 0, OpenGL.GL_RGB, frameWidth, frameHeight, 0, OpenGL.GL_BGR_EXT, OpenGL.GL_UNSIGNED_BYTE, pBuffer);
        }

        public void SetDisplayAspect(int width, int height)
        {
            _displayAspectWidth = width;
            _displayAspectHeight = height;

            UpdateViewPort();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            Draw();
        }

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);

            if (gl == null)
                return;

            //	Resize the DIB Surface.
            gl.SetDimensions(Width, Height);

            //	Set the viewport.
            UpdateViewPort();
        }

        int _displayAspectWidth = 1;
        int _displayAspectHeight = 1;

        void UpdateViewPort()
        {
            if (Width < 0 || Height < 0)
                return;

            if (gl == null)
                return;

            double windowAspect = (double)Width / (double)Height;
            double displayAspect = (double)_displayAspectWidth / (double)_displayAspectHeight;

            if (windowAspect < displayAspect)
            {
                int width = Width;
                int height = (int)(Width / displayAspect);
                gl.Viewport(0, (Height - height) / 2, width, height);
            }
            else
            {
                int width = (int)(Height * displayAspect);
                int height = Height;
                gl.Viewport((Width - width) / 2, 0, width, height);
            }
        }
    }
}
