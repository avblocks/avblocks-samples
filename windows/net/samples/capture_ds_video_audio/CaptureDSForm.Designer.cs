namespace CaptureDS
{
    partial class CaptureDSForm
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
            this.listAudioDev = new System.Windows.Forms.ComboBox();
            this.cmdAudioDevProp = new System.Windows.Forms.Button();
            this.listVideoDev = new System.Windows.Forms.ComboBox();
            this.cmdVideoDevProp = new System.Windows.Forms.Button();
            this.cmdVideoCaptureProp = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.cmdRecord = new System.Windows.Forms.Button();
            this.txtRecording = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.txtCurrentFPS = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.txtVDropped = new System.Windows.Forms.Label();
            this.txtVProcessed = new System.Windows.Forms.Label();
            this.txtVCallbacks = new System.Windows.Forms.Label();
            this.txtADropped = new System.Windows.Forms.Label();
            this.txtAProcessed = new System.Windows.Forms.Label();
            this.txtACallbacks = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.txtAverageFPS = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.txtNumNotDropped = new System.Windows.Forms.Label();
            this.txtNumDropped = new System.Windows.Forms.Label();
            this.labelNotDropped = new System.Windows.Forms.Label();
            this.labelDropped = new System.Windows.Forms.Label();
            this.txtRecTime = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.previewBox = new System.Windows.Forms.PictureBox();
            this.comboPresets = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.btnChooseOutput = new System.Windows.Forms.Button();
            this.txtOutput = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.txtAudioLog = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.previewBox)).BeginInit();
            this.SuspendLayout();
            // 
            // listAudioDev
            // 
            this.listAudioDev.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.listAudioDev.FormattingEnabled = true;
            this.listAudioDev.Location = new System.Drawing.Point(23, 21);
            this.listAudioDev.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.listAudioDev.Name = "listAudioDev";
            this.listAudioDev.Size = new System.Drawing.Size(261, 24);
            this.listAudioDev.TabIndex = 0;
            this.listAudioDev.SelectedIndexChanged += new System.EventHandler(this.listAudioDev_SelectedIndexChanged);
            // 
            // cmdAudioDevProp
            // 
            this.cmdAudioDevProp.Location = new System.Drawing.Point(23, 50);
            this.cmdAudioDevProp.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.cmdAudioDevProp.Name = "cmdAudioDevProp";
            this.cmdAudioDevProp.Size = new System.Drawing.Size(108, 30);
            this.cmdAudioDevProp.TabIndex = 1;
            this.cmdAudioDevProp.Text = "Device...";
            this.cmdAudioDevProp.UseVisualStyleBackColor = true;
            this.cmdAudioDevProp.Click += new System.EventHandler(this.cmdAudioDevProp_Click);
            // 
            // listVideoDev
            // 
            this.listVideoDev.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.listVideoDev.FormattingEnabled = true;
            this.listVideoDev.Location = new System.Drawing.Point(23, 21);
            this.listVideoDev.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.listVideoDev.Name = "listVideoDev";
            this.listVideoDev.Size = new System.Drawing.Size(261, 24);
            this.listVideoDev.TabIndex = 3;
            this.listVideoDev.SelectedIndexChanged += new System.EventHandler(this.listVideoDev_SelectedIndexChanged);
            // 
            // cmdVideoDevProp
            // 
            this.cmdVideoDevProp.Location = new System.Drawing.Point(23, 50);
            this.cmdVideoDevProp.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.cmdVideoDevProp.Name = "cmdVideoDevProp";
            this.cmdVideoDevProp.Size = new System.Drawing.Size(108, 30);
            this.cmdVideoDevProp.TabIndex = 4;
            this.cmdVideoDevProp.Text = "Device...";
            this.cmdVideoDevProp.UseVisualStyleBackColor = true;
            this.cmdVideoDevProp.Click += new System.EventHandler(this.cmdVideoDevProp_Click);
            // 
            // cmdVideoCaptureProp
            // 
            this.cmdVideoCaptureProp.Location = new System.Drawing.Point(147, 50);
            this.cmdVideoCaptureProp.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.cmdVideoCaptureProp.Name = "cmdVideoCaptureProp";
            this.cmdVideoCaptureProp.Size = new System.Drawing.Size(108, 30);
            this.cmdVideoCaptureProp.TabIndex = 5;
            this.cmdVideoCaptureProp.Text = "Capture...";
            this.cmdVideoCaptureProp.UseVisualStyleBackColor = true;
            this.cmdVideoCaptureProp.Click += new System.EventHandler(this.cmdVideoCaptureProp_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.listAudioDev);
            this.groupBox1.Controls.Add(this.cmdAudioDevProp);
            this.groupBox1.Location = new System.Drawing.Point(13, 6);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox1.Size = new System.Drawing.Size(299, 94);
            this.groupBox1.TabIndex = 6;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Audio Devices";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.listVideoDev);
            this.groupBox2.Controls.Add(this.cmdVideoDevProp);
            this.groupBox2.Controls.Add(this.cmdVideoCaptureProp);
            this.groupBox2.Location = new System.Drawing.Point(13, 102);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox2.Size = new System.Drawing.Size(299, 89);
            this.groupBox2.TabIndex = 7;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Video Devices";
            // 
            // cmdRecord
            // 
            this.cmdRecord.Location = new System.Drawing.Point(533, 402);
            this.cmdRecord.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.cmdRecord.Name = "cmdRecord";
            this.cmdRecord.Size = new System.Drawing.Size(139, 39);
            this.cmdRecord.TabIndex = 8;
            this.cmdRecord.Text = "Record";
            this.cmdRecord.UseVisualStyleBackColor = true;
            this.cmdRecord.Click += new System.EventHandler(this.cmdRecord_Click);
            // 
            // txtRecording
            // 
            this.txtRecording.AutoSize = true;
            this.txtRecording.ForeColor = System.Drawing.Color.Red;
            this.txtRecording.Location = new System.Drawing.Point(561, 380);
            this.txtRecording.Name = "txtRecording";
            this.txtRecording.Size = new System.Drawing.Size(85, 17);
            this.txtRecording.TabIndex = 9;
            this.txtRecording.Text = "Recording...";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.groupBox5);
            this.groupBox3.Controls.Add(this.groupBox4);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.txtCurrentFPS);
            this.groupBox3.Controls.Add(this.label10);
            this.groupBox3.Controls.Add(this.txtVDropped);
            this.groupBox3.Controls.Add(this.txtVProcessed);
            this.groupBox3.Controls.Add(this.txtVCallbacks);
            this.groupBox3.Controls.Add(this.txtADropped);
            this.groupBox3.Controls.Add(this.txtAProcessed);
            this.groupBox3.Controls.Add(this.txtACallbacks);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Controls.Add(this.label4);
            this.groupBox3.Controls.Add(this.txtAverageFPS);
            this.groupBox3.Controls.Add(this.label1);
            this.groupBox3.Controls.Add(this.txtNumNotDropped);
            this.groupBox3.Controls.Add(this.txtNumDropped);
            this.groupBox3.Controls.Add(this.labelNotDropped);
            this.groupBox3.Controls.Add(this.labelDropped);
            this.groupBox3.Controls.Add(this.txtRecTime);
            this.groupBox3.Controls.Add(this.label3);
            this.groupBox3.Location = new System.Drawing.Point(13, 332);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox3.Size = new System.Drawing.Size(457, 127);
            this.groupBox3.TabIndex = 12;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Statistics";
            // 
            // groupBox5
            // 
            this.groupBox5.Location = new System.Drawing.Point(175, 48);
            this.groupBox5.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox5.Size = new System.Drawing.Size(3, 70);
            this.groupBox5.TabIndex = 26;
            this.groupBox5.TabStop = false;
            // 
            // groupBox4
            // 
            this.groupBox4.Location = new System.Drawing.Point(337, 49);
            this.groupBox4.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox4.Size = new System.Drawing.Size(3, 70);
            this.groupBox4.TabIndex = 26;
            this.groupBox4.TabStop = false;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(367, 49);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(44, 17);
            this.label8.TabIndex = 25;
            this.label8.Text = "Video";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(264, 49);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(44, 17);
            this.label7.TabIndex = 24;
            this.label7.Text = "Audio";
            // 
            // txtCurrentFPS
            // 
            this.txtCurrentFPS.Location = new System.Drawing.Point(113, 100);
            this.txtCurrentFPS.Name = "txtCurrentFPS";
            this.txtCurrentFPS.Size = new System.Drawing.Size(56, 16);
            this.txtCurrentFPS.TabIndex = 21;
            this.txtCurrentFPS.Text = "[curfps]";
            this.txtCurrentFPS.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(11, 100);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(82, 17);
            this.label10.TabIndex = 20;
            this.label10.Text = "Current fps:";
            // 
            // txtVDropped
            // 
            this.txtVDropped.Location = new System.Drawing.Point(397, 100);
            this.txtVDropped.Name = "txtVDropped";
            this.txtVDropped.Size = new System.Drawing.Size(53, 16);
            this.txtVDropped.TabIndex = 19;
            this.txtVDropped.Text = "[vdrop]";
            this.txtVDropped.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // txtVProcessed
            // 
            this.txtVProcessed.Location = new System.Drawing.Point(397, 82);
            this.txtVProcessed.Name = "txtVProcessed";
            this.txtVProcessed.Size = new System.Drawing.Size(53, 16);
            this.txtVProcessed.TabIndex = 18;
            this.txtVProcessed.Text = "[vproc]";
            this.txtVProcessed.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // txtVCallbacks
            // 
            this.txtVCallbacks.Location = new System.Drawing.Point(409, 65);
            this.txtVCallbacks.Name = "txtVCallbacks";
            this.txtVCallbacks.Size = new System.Drawing.Size(41, 16);
            this.txtVCallbacks.TabIndex = 17;
            this.txtVCallbacks.Text = "[vcb]";
            this.txtVCallbacks.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // txtADropped
            // 
            this.txtADropped.Location = new System.Drawing.Point(281, 100);
            this.txtADropped.Name = "txtADropped";
            this.txtADropped.Size = new System.Drawing.Size(53, 16);
            this.txtADropped.TabIndex = 16;
            this.txtADropped.Text = "[adrop]";
            this.txtADropped.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // txtAProcessed
            // 
            this.txtAProcessed.Location = new System.Drawing.Point(281, 82);
            this.txtAProcessed.Name = "txtAProcessed";
            this.txtAProcessed.Size = new System.Drawing.Size(53, 16);
            this.txtAProcessed.TabIndex = 15;
            this.txtAProcessed.Text = "[aproc]";
            this.txtAProcessed.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // txtACallbacks
            // 
            this.txtACallbacks.Location = new System.Drawing.Point(293, 65);
            this.txtACallbacks.Name = "txtACallbacks";
            this.txtACallbacks.Size = new System.Drawing.Size(41, 16);
            this.txtACallbacks.TabIndex = 14;
            this.txtACallbacks.Text = "[acb]";
            this.txtACallbacks.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(193, 98);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(65, 17);
            this.label6.TabIndex = 10;
            this.label6.Text = "dropped:";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(180, 82);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(78, 17);
            this.label5.TabIndex = 9;
            this.label5.Text = "processed:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(185, 65);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(70, 17);
            this.label4.TabIndex = 8;
            this.label4.Text = "callbacks:";
            // 
            // txtAverageFPS
            // 
            this.txtAverageFPS.Location = new System.Drawing.Point(109, 81);
            this.txtAverageFPS.Name = "txtAverageFPS";
            this.txtAverageFPS.Size = new System.Drawing.Size(60, 16);
            this.txtAverageFPS.TabIndex = 7;
            this.txtAverageFPS.Text = "[avgfps]";
            this.txtAverageFPS.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(3, 81);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(88, 17);
            this.label1.TabIndex = 6;
            this.label1.Text = "Average fps:";
            // 
            // txtNumNotDropped
            // 
            this.txtNumNotDropped.Location = new System.Drawing.Point(101, 64);
            this.txtNumNotDropped.Name = "txtNumNotDropped";
            this.txtNumNotDropped.Size = new System.Drawing.Size(68, 16);
            this.txtNumNotDropped.TabIndex = 5;
            this.txtNumNotDropped.Text = "[dsvproc]";
            this.txtNumNotDropped.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // txtNumDropped
            // 
            this.txtNumDropped.Location = new System.Drawing.Point(101, 46);
            this.txtNumDropped.Name = "txtNumDropped";
            this.txtNumDropped.Size = new System.Drawing.Size(68, 16);
            this.txtNumDropped.TabIndex = 4;
            this.txtNumDropped.Text = "[dsvdrop]";
            this.txtNumDropped.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // labelNotDropped
            // 
            this.labelNotDropped.AutoSize = true;
            this.labelNotDropped.Location = new System.Drawing.Point(12, 64);
            this.labelNotDropped.Name = "labelNotDropped";
            this.labelNotDropped.Size = new System.Drawing.Size(79, 17);
            this.labelNotDropped.TabIndex = 3;
            this.labelNotDropped.Text = "Processed:";
            // 
            // labelDropped
            // 
            this.labelDropped.AutoSize = true;
            this.labelDropped.Location = new System.Drawing.Point(24, 46);
            this.labelDropped.Name = "labelDropped";
            this.labelDropped.Size = new System.Drawing.Size(67, 17);
            this.labelDropped.TabIndex = 2;
            this.labelDropped.Text = "Dropped:";
            // 
            // txtRecTime
            // 
            this.txtRecTime.AutoSize = true;
            this.txtRecTime.Location = new System.Drawing.Point(101, 23);
            this.txtRecTime.Name = "txtRecTime";
            this.txtRecTime.Size = new System.Drawing.Size(56, 17);
            this.txtRecTime.TabIndex = 1;
            this.txtRecTime.Text = "0:00:00";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(11, 23);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(76, 17);
            this.label3.TabIndex = 0;
            this.label3.Text = "Rec. Time:";
            // 
            // previewBox
            // 
            this.previewBox.BackColor = System.Drawing.SystemColors.ButtonShadow;
            this.previewBox.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.previewBox.Location = new System.Drawing.Point(331, 14);
            this.previewBox.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.previewBox.Name = "previewBox";
            this.previewBox.Size = new System.Drawing.Size(255, 176);
            this.previewBox.TabIndex = 15;
            this.previewBox.TabStop = false;
            // 
            // comboPresets
            // 
            this.comboPresets.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboPresets.FormattingEnabled = true;
            this.comboPresets.Location = new System.Drawing.Point(135, 220);
            this.comboPresets.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.comboPresets.Name = "comboPresets";
            this.comboPresets.Size = new System.Drawing.Size(289, 24);
            this.comboPresets.TabIndex = 17;
            this.comboPresets.SelectedIndexChanged += new System.EventHandler(this.comboPresets_SelectedIndexChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(22, 223);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(99, 17);
            this.label2.TabIndex = 16;
            this.label2.Text = "Output preset:";
            // 
            // btnChooseOutput
            // 
            this.btnChooseOutput.Location = new System.Drawing.Point(596, 258);
            this.btnChooseOutput.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnChooseOutput.Name = "btnChooseOutput";
            this.btnChooseOutput.Size = new System.Drawing.Size(76, 25);
            this.btnChooseOutput.TabIndex = 20;
            this.btnChooseOutput.Text = "...";
            this.btnChooseOutput.UseVisualStyleBackColor = true;
            this.btnChooseOutput.Click += new System.EventHandler(this.btnChooseOutput_Click);
            // 
            // txtOutput
            // 
            this.txtOutput.Location = new System.Drawing.Point(135, 259);
            this.txtOutput.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txtOutput.Name = "txtOutput";
            this.txtOutput.ReadOnly = true;
            this.txtOutput.Size = new System.Drawing.Size(451, 22);
            this.txtOutput.TabIndex = 19;
            this.txtOutput.TabStop = false;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(44, 259);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(77, 17);
            this.label9.TabIndex = 18;
            this.label9.Text = "Output file:";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(50, 292);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(71, 17);
            this.label11.TabIndex = 18;
            this.label11.Text = "Audio log:";
            // 
            // txtAudioLog
            // 
            this.txtAudioLog.Location = new System.Drawing.Point(135, 292);
            this.txtAudioLog.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.txtAudioLog.Name = "txtAudioLog";
            this.txtAudioLog.ReadOnly = true;
            this.txtAudioLog.Size = new System.Drawing.Size(451, 22);
            this.txtAudioLog.TabIndex = 19;
            this.txtAudioLog.TabStop = false;
            // 
            // CaptureDSForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(708, 489);
            this.Controls.Add(this.btnChooseOutput);
            this.Controls.Add(this.txtAudioLog);
            this.Controls.Add(this.txtOutput);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.comboPresets);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.previewBox);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.txtRecording);
            this.Controls.Add(this.cmdRecord);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.MaximizeBox = false;
            this.Name = "CaptureDSForm";
            this.Text = "capture_ds_audio_video (C#)";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.RecorderForm_FormClosing);
            this.Load += new System.EventHandler(this.RecorderForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.previewBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox listAudioDev;
        private System.Windows.Forms.Button cmdAudioDevProp;
        private System.Windows.Forms.ComboBox listVideoDev;
        private System.Windows.Forms.Button cmdVideoDevProp;
        private System.Windows.Forms.Button cmdVideoCaptureProp;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button cmdRecord;
        private System.Windows.Forms.Label txtRecording;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label txtRecTime;
        private System.Windows.Forms.Label labelNotDropped;
        private System.Windows.Forms.Label labelDropped;
        private System.Windows.Forms.Label txtNumNotDropped;
        private System.Windows.Forms.Label txtNumDropped;
        private System.Windows.Forms.Label txtAverageFPS;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label txtADropped;
        private System.Windows.Forms.Label txtAProcessed;
        private System.Windows.Forms.Label txtACallbacks;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label txtVDropped;
        private System.Windows.Forms.Label txtVProcessed;
        private System.Windows.Forms.Label txtVCallbacks;
        private System.Windows.Forms.Label txtCurrentFPS;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.PictureBox previewBox;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.ComboBox comboPresets;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btnChooseOutput;
        private System.Windows.Forms.TextBox txtOutput;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox txtAudioLog;
    }
}

