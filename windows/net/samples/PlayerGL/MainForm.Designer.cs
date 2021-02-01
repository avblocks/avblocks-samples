namespace PlayerGLSample
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.OpenGLVideoRender = new PlayerGLSample.OpenGLVideoRender();
            this.SuspendLayout();
            // 
            // OpenGLVideoRender
            // 
            this.OpenGLVideoRender.Dock = System.Windows.Forms.DockStyle.Fill;
            this.OpenGLVideoRender.Location = new System.Drawing.Point(0, 0);
            this.OpenGLVideoRender.Name = "OpenGLVideoRender";
            this.OpenGLVideoRender.Size = new System.Drawing.Size(784, 561);
            this.OpenGLVideoRender.TabIndex = 0;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(784, 561);
            this.Controls.Add(this.OpenGLVideoRender);
            this.Name = "MainForm";
            this.Text = "AVBlocks SDK for .NET - PlayerGL Sample";
            this.ResumeLayout(false);

        }

        #endregion

        public OpenGLVideoRender OpenGLVideoRender;

    }
}