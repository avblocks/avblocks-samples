Namespace CaptureDS
	Partial Public Class CaptureDSForm
		''' <summary>
		''' Required designer variable.
		''' </summary>
		Private components As System.ComponentModel.IContainer = Nothing

		''' <summary>
		''' Clean up any resources being used.
		''' </summary>
		''' <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		Protected Overrides Sub Dispose(ByVal disposing As Boolean)
			If disposing AndAlso (components IsNot Nothing) Then
				components.Dispose()
			End If
			MyBase.Dispose(disposing)
		End Sub

		#Region "Windows Form Designer generated code"

		''' <summary>
		''' Required method for Designer support - do not modify
		''' the contents of this method with the code editor.
		''' </summary>
		Private Sub InitializeComponent()
            Me.listAudioDev = New System.Windows.Forms.ComboBox()
            Me.cmdAudioDevProp = New System.Windows.Forms.Button()
            Me.listVideoDev = New System.Windows.Forms.ComboBox()
            Me.cmdVideoDevProp = New System.Windows.Forms.Button()
            Me.cmdVideoCaptureProp = New System.Windows.Forms.Button()
            Me.groupBox1 = New System.Windows.Forms.GroupBox()
            Me.groupBox2 = New System.Windows.Forms.GroupBox()
            Me.cmdRecord = New System.Windows.Forms.Button()
            Me.txtRecording = New System.Windows.Forms.Label()
            Me.groupBox3 = New System.Windows.Forms.GroupBox()
            Me.groupBox5 = New System.Windows.Forms.GroupBox()
            Me.groupBox4 = New System.Windows.Forms.GroupBox()
            Me.label8 = New System.Windows.Forms.Label()
            Me.label7 = New System.Windows.Forms.Label()
            Me.txtCurrentFPS = New System.Windows.Forms.Label()
            Me.label10 = New System.Windows.Forms.Label()
            Me.txtVDropped = New System.Windows.Forms.Label()
            Me.txtVProcessed = New System.Windows.Forms.Label()
            Me.txtVCallbacks = New System.Windows.Forms.Label()
            Me.txtADropped = New System.Windows.Forms.Label()
            Me.txtAProcessed = New System.Windows.Forms.Label()
            Me.txtACallbacks = New System.Windows.Forms.Label()
            Me.label6 = New System.Windows.Forms.Label()
            Me.label5 = New System.Windows.Forms.Label()
            Me.label4 = New System.Windows.Forms.Label()
            Me.txtAverageFPS = New System.Windows.Forms.Label()
            Me.label1 = New System.Windows.Forms.Label()
            Me.txtNumNotDropped = New System.Windows.Forms.Label()
            Me.txtNumDropped = New System.Windows.Forms.Label()
            Me.labelNotDropped = New System.Windows.Forms.Label()
            Me.labelDropped = New System.Windows.Forms.Label()
            Me.txtRecTime = New System.Windows.Forms.Label()
            Me.label3 = New System.Windows.Forms.Label()
            Me.previewBox = New System.Windows.Forms.PictureBox()
            Me.comboPresets = New System.Windows.Forms.ComboBox()
            Me.label2 = New System.Windows.Forms.Label()
            Me.btnChooseOutput = New System.Windows.Forms.Button()
            Me.txtOutput = New System.Windows.Forms.TextBox()
            Me.label9 = New System.Windows.Forms.Label()
            Me.groupBox1.SuspendLayout()
            Me.groupBox2.SuspendLayout()
            Me.groupBox3.SuspendLayout()
            CType(Me.previewBox, System.ComponentModel.ISupportInitialize).BeginInit()
            Me.SuspendLayout()
            '
            'listAudioDev
            '
            Me.listAudioDev.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
            Me.listAudioDev.FormattingEnabled = True
            Me.listAudioDev.Location = New System.Drawing.Point(17, 17)
            Me.listAudioDev.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.listAudioDev.Name = "listAudioDev"
            Me.listAudioDev.Size = New System.Drawing.Size(197, 21)
            Me.listAudioDev.TabIndex = 0
            '
            'cmdAudioDevProp
            '
            Me.cmdAudioDevProp.Location = New System.Drawing.Point(17, 41)
            Me.cmdAudioDevProp.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.cmdAudioDevProp.Name = "cmdAudioDevProp"
            Me.cmdAudioDevProp.Size = New System.Drawing.Size(81, 24)
            Me.cmdAudioDevProp.TabIndex = 1
            Me.cmdAudioDevProp.Text = "Device..."
            Me.cmdAudioDevProp.UseVisualStyleBackColor = True
            '
            'listVideoDev
            '
            Me.listVideoDev.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
            Me.listVideoDev.FormattingEnabled = True
            Me.listVideoDev.Location = New System.Drawing.Point(17, 17)
            Me.listVideoDev.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.listVideoDev.Name = "listVideoDev"
            Me.listVideoDev.Size = New System.Drawing.Size(197, 21)
            Me.listVideoDev.TabIndex = 3
            '
            'cmdVideoDevProp
            '
            Me.cmdVideoDevProp.Location = New System.Drawing.Point(17, 41)
            Me.cmdVideoDevProp.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.cmdVideoDevProp.Name = "cmdVideoDevProp"
            Me.cmdVideoDevProp.Size = New System.Drawing.Size(81, 24)
            Me.cmdVideoDevProp.TabIndex = 4
            Me.cmdVideoDevProp.Text = "Device..."
            Me.cmdVideoDevProp.UseVisualStyleBackColor = True
            '
            'cmdVideoCaptureProp
            '
            Me.cmdVideoCaptureProp.Location = New System.Drawing.Point(110, 41)
            Me.cmdVideoCaptureProp.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.cmdVideoCaptureProp.Name = "cmdVideoCaptureProp"
            Me.cmdVideoCaptureProp.Size = New System.Drawing.Size(81, 24)
            Me.cmdVideoCaptureProp.TabIndex = 5
            Me.cmdVideoCaptureProp.Text = "Capture..."
            Me.cmdVideoCaptureProp.UseVisualStyleBackColor = True
            '
            'groupBox1
            '
            Me.groupBox1.Controls.Add(Me.listAudioDev)
            Me.groupBox1.Controls.Add(Me.cmdAudioDevProp)
            Me.groupBox1.Location = New System.Drawing.Point(10, 5)
            Me.groupBox1.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.groupBox1.Name = "groupBox1"
            Me.groupBox1.Padding = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.groupBox1.Size = New System.Drawing.Size(224, 76)
            Me.groupBox1.TabIndex = 6
            Me.groupBox1.TabStop = False
            Me.groupBox1.Text = "Audio Devices"
            '
            'groupBox2
            '
            Me.groupBox2.Controls.Add(Me.listVideoDev)
            Me.groupBox2.Controls.Add(Me.cmdVideoDevProp)
            Me.groupBox2.Controls.Add(Me.cmdVideoCaptureProp)
            Me.groupBox2.Location = New System.Drawing.Point(10, 83)
            Me.groupBox2.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.groupBox2.Name = "groupBox2"
            Me.groupBox2.Padding = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.groupBox2.Size = New System.Drawing.Size(224, 72)
            Me.groupBox2.TabIndex = 7
            Me.groupBox2.TabStop = False
            Me.groupBox2.Text = "Video Devices"
            '
            'cmdRecord
            '
            Me.cmdRecord.Location = New System.Drawing.Point(400, 327)
            Me.cmdRecord.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.cmdRecord.Name = "cmdRecord"
            Me.cmdRecord.Size = New System.Drawing.Size(104, 32)
            Me.cmdRecord.TabIndex = 8
            Me.cmdRecord.Text = "Record"
            Me.cmdRecord.UseVisualStyleBackColor = True
            '
            'txtRecording
            '
            Me.txtRecording.AutoSize = True
            Me.txtRecording.ForeColor = System.Drawing.Color.Red
            Me.txtRecording.Location = New System.Drawing.Point(421, 309)
            Me.txtRecording.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtRecording.Name = "txtRecording"
            Me.txtRecording.Size = New System.Drawing.Size(65, 13)
            Me.txtRecording.TabIndex = 9
            Me.txtRecording.Text = "Recording..."
            '
            'groupBox3
            '
            Me.groupBox3.Controls.Add(Me.groupBox5)
            Me.groupBox3.Controls.Add(Me.groupBox4)
            Me.groupBox3.Controls.Add(Me.label8)
            Me.groupBox3.Controls.Add(Me.label7)
            Me.groupBox3.Controls.Add(Me.txtCurrentFPS)
            Me.groupBox3.Controls.Add(Me.label10)
            Me.groupBox3.Controls.Add(Me.txtVDropped)
            Me.groupBox3.Controls.Add(Me.txtVProcessed)
            Me.groupBox3.Controls.Add(Me.txtVCallbacks)
            Me.groupBox3.Controls.Add(Me.txtADropped)
            Me.groupBox3.Controls.Add(Me.txtAProcessed)
            Me.groupBox3.Controls.Add(Me.txtACallbacks)
            Me.groupBox3.Controls.Add(Me.label6)
            Me.groupBox3.Controls.Add(Me.label5)
            Me.groupBox3.Controls.Add(Me.label4)
            Me.groupBox3.Controls.Add(Me.txtAverageFPS)
            Me.groupBox3.Controls.Add(Me.label1)
            Me.groupBox3.Controls.Add(Me.txtNumNotDropped)
            Me.groupBox3.Controls.Add(Me.txtNumDropped)
            Me.groupBox3.Controls.Add(Me.labelNotDropped)
            Me.groupBox3.Controls.Add(Me.labelDropped)
            Me.groupBox3.Controls.Add(Me.txtRecTime)
            Me.groupBox3.Controls.Add(Me.label3)
            Me.groupBox3.Location = New System.Drawing.Point(10, 270)
            Me.groupBox3.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.groupBox3.Name = "groupBox3"
            Me.groupBox3.Padding = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.groupBox3.Size = New System.Drawing.Size(343, 103)
            Me.groupBox3.TabIndex = 12
            Me.groupBox3.TabStop = False
            Me.groupBox3.Text = "Statistics"
            '
            'groupBox5
            '
            Me.groupBox5.Location = New System.Drawing.Point(131, 39)
            Me.groupBox5.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.groupBox5.Name = "groupBox5"
            Me.groupBox5.Padding = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.groupBox5.Size = New System.Drawing.Size(2, 57)
            Me.groupBox5.TabIndex = 26
            Me.groupBox5.TabStop = False
            '
            'groupBox4
            '
            Me.groupBox4.Location = New System.Drawing.Point(253, 40)
            Me.groupBox4.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.groupBox4.Name = "groupBox4"
            Me.groupBox4.Padding = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.groupBox4.Size = New System.Drawing.Size(2, 57)
            Me.groupBox4.TabIndex = 26
            Me.groupBox4.TabStop = False
            '
            'label8
            '
            Me.label8.AutoSize = True
            Me.label8.Location = New System.Drawing.Point(275, 40)
            Me.label8.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.label8.Name = "label8"
            Me.label8.Size = New System.Drawing.Size(34, 13)
            Me.label8.TabIndex = 25
            Me.label8.Text = "Video"
            '
            'label7
            '
            Me.label7.AutoSize = True
            Me.label7.Location = New System.Drawing.Point(198, 40)
            Me.label7.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.label7.Name = "label7"
            Me.label7.Size = New System.Drawing.Size(34, 13)
            Me.label7.TabIndex = 24
            Me.label7.Text = "Audio"
            '
            'txtCurrentFPS
            '
            Me.txtCurrentFPS.Location = New System.Drawing.Point(85, 81)
            Me.txtCurrentFPS.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtCurrentFPS.Name = "txtCurrentFPS"
            Me.txtCurrentFPS.Size = New System.Drawing.Size(42, 13)
            Me.txtCurrentFPS.TabIndex = 21
            Me.txtCurrentFPS.Text = "[curfps]"
            Me.txtCurrentFPS.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'label10
            '
            Me.label10.AutoSize = True
            Me.label10.Location = New System.Drawing.Point(8, 81)
            Me.label10.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.label10.Name = "label10"
            Me.label10.Size = New System.Drawing.Size(61, 13)
            Me.label10.TabIndex = 20
            Me.label10.Text = "Current fps:"
            '
            'txtVDropped
            '
            Me.txtVDropped.Location = New System.Drawing.Point(298, 81)
            Me.txtVDropped.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtVDropped.Name = "txtVDropped"
            Me.txtVDropped.Size = New System.Drawing.Size(40, 13)
            Me.txtVDropped.TabIndex = 19
            Me.txtVDropped.Text = "[vdrop]"
            Me.txtVDropped.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'txtVProcessed
            '
            Me.txtVProcessed.Location = New System.Drawing.Point(298, 67)
            Me.txtVProcessed.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtVProcessed.Name = "txtVProcessed"
            Me.txtVProcessed.Size = New System.Drawing.Size(40, 13)
            Me.txtVProcessed.TabIndex = 18
            Me.txtVProcessed.Text = "[vproc]"
            Me.txtVProcessed.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'txtVCallbacks
            '
            Me.txtVCallbacks.Location = New System.Drawing.Point(307, 53)
            Me.txtVCallbacks.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtVCallbacks.Name = "txtVCallbacks"
            Me.txtVCallbacks.Size = New System.Drawing.Size(31, 13)
            Me.txtVCallbacks.TabIndex = 17
            Me.txtVCallbacks.Text = "[vcb]"
            Me.txtVCallbacks.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'txtADropped
            '
            Me.txtADropped.Location = New System.Drawing.Point(211, 81)
            Me.txtADropped.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtADropped.Name = "txtADropped"
            Me.txtADropped.Size = New System.Drawing.Size(40, 13)
            Me.txtADropped.TabIndex = 16
            Me.txtADropped.Text = "[adrop]"
            Me.txtADropped.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'txtAProcessed
            '
            Me.txtAProcessed.Location = New System.Drawing.Point(211, 67)
            Me.txtAProcessed.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtAProcessed.Name = "txtAProcessed"
            Me.txtAProcessed.Size = New System.Drawing.Size(40, 13)
            Me.txtAProcessed.TabIndex = 15
            Me.txtAProcessed.Text = "[aproc]"
            Me.txtAProcessed.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'txtACallbacks
            '
            Me.txtACallbacks.Location = New System.Drawing.Point(220, 53)
            Me.txtACallbacks.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtACallbacks.Name = "txtACallbacks"
            Me.txtACallbacks.Size = New System.Drawing.Size(31, 13)
            Me.txtACallbacks.TabIndex = 14
            Me.txtACallbacks.Text = "[acb]"
            Me.txtACallbacks.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'label6
            '
            Me.label6.AutoSize = True
            Me.label6.Location = New System.Drawing.Point(145, 80)
            Me.label6.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.label6.Name = "label6"
            Me.label6.Size = New System.Drawing.Size(49, 13)
            Me.label6.TabIndex = 10
            Me.label6.Text = "dropped:"
            '
            'label5
            '
            Me.label5.AutoSize = True
            Me.label5.Location = New System.Drawing.Point(135, 67)
            Me.label5.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.label5.Name = "label5"
            Me.label5.Size = New System.Drawing.Size(59, 13)
            Me.label5.TabIndex = 9
            Me.label5.Text = "processed:"
            '
            'label4
            '
            Me.label4.AutoSize = True
            Me.label4.Location = New System.Drawing.Point(139, 53)
            Me.label4.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.label4.Name = "label4"
            Me.label4.Size = New System.Drawing.Size(55, 13)
            Me.label4.TabIndex = 8
            Me.label4.Text = "callbacks:"
            '
            'txtAverageFPS
            '
            Me.txtAverageFPS.Location = New System.Drawing.Point(82, 66)
            Me.txtAverageFPS.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtAverageFPS.Name = "txtAverageFPS"
            Me.txtAverageFPS.Size = New System.Drawing.Size(45, 13)
            Me.txtAverageFPS.TabIndex = 7
            Me.txtAverageFPS.Text = "[avgfps]"
            Me.txtAverageFPS.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'label1
            '
            Me.label1.AutoSize = True
            Me.label1.Location = New System.Drawing.Point(2, 66)
            Me.label1.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.label1.Name = "label1"
            Me.label1.Size = New System.Drawing.Size(67, 13)
            Me.label1.TabIndex = 6
            Me.label1.Text = "Average fps:"
            '
            'txtNumNotDropped
            '
            Me.txtNumNotDropped.Location = New System.Drawing.Point(76, 52)
            Me.txtNumNotDropped.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtNumNotDropped.Name = "txtNumNotDropped"
            Me.txtNumNotDropped.Size = New System.Drawing.Size(51, 13)
            Me.txtNumNotDropped.TabIndex = 5
            Me.txtNumNotDropped.Text = "[dsvproc]"
            Me.txtNumNotDropped.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'txtNumDropped
            '
            Me.txtNumDropped.Location = New System.Drawing.Point(76, 37)
            Me.txtNumDropped.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtNumDropped.Name = "txtNumDropped"
            Me.txtNumDropped.Size = New System.Drawing.Size(51, 13)
            Me.txtNumDropped.TabIndex = 4
            Me.txtNumDropped.Text = "[dsvdrop]"
            Me.txtNumDropped.TextAlign = System.Drawing.ContentAlignment.TopRight
            '
            'labelNotDropped
            '
            Me.labelNotDropped.AutoSize = True
            Me.labelNotDropped.Location = New System.Drawing.Point(9, 52)
            Me.labelNotDropped.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.labelNotDropped.Name = "labelNotDropped"
            Me.labelNotDropped.Size = New System.Drawing.Size(60, 13)
            Me.labelNotDropped.TabIndex = 3
            Me.labelNotDropped.Text = "Processed:"
            '
            'labelDropped
            '
            Me.labelDropped.AutoSize = True
            Me.labelDropped.Location = New System.Drawing.Point(18, 37)
            Me.labelDropped.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.labelDropped.Name = "labelDropped"
            Me.labelDropped.Size = New System.Drawing.Size(51, 13)
            Me.labelDropped.TabIndex = 2
            Me.labelDropped.Text = "Dropped:"
            '
            'txtRecTime
            '
            Me.txtRecTime.AutoSize = True
            Me.txtRecTime.Location = New System.Drawing.Point(76, 19)
            Me.txtRecTime.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.txtRecTime.Name = "txtRecTime"
            Me.txtRecTime.Size = New System.Drawing.Size(43, 13)
            Me.txtRecTime.TabIndex = 1
            Me.txtRecTime.Text = "0:00:00"
            '
            'label3
            '
            Me.label3.AutoSize = True
            Me.label3.Location = New System.Drawing.Point(8, 19)
            Me.label3.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.label3.Name = "label3"
            Me.label3.Size = New System.Drawing.Size(59, 13)
            Me.label3.TabIndex = 0
            Me.label3.Text = "Rec. Time:"
            '
            'previewBox
            '
            Me.previewBox.BackColor = System.Drawing.SystemColors.ButtonShadow
            Me.previewBox.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
            Me.previewBox.Location = New System.Drawing.Point(248, 11)
            Me.previewBox.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.previewBox.Name = "previewBox"
            Me.previewBox.Size = New System.Drawing.Size(192, 144)
            Me.previewBox.TabIndex = 15
            Me.previewBox.TabStop = False
            '
            'comboPresets
            '
            Me.comboPresets.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
            Me.comboPresets.FormattingEnabled = True
            Me.comboPresets.Location = New System.Drawing.Point(101, 179)
            Me.comboPresets.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.comboPresets.Name = "comboPresets"
            Me.comboPresets.Size = New System.Drawing.Size(339, 21)
            Me.comboPresets.TabIndex = 17
            '
            'label2
            '
            Me.label2.AutoSize = True
            Me.label2.Location = New System.Drawing.Point(17, 181)
            Me.label2.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.label2.Name = "label2"
            Me.label2.Size = New System.Drawing.Size(74, 13)
            Me.label2.TabIndex = 16
            Me.label2.Text = "Output preset:"
            '
            'btnChooseOutput
            '
            Me.btnChooseOutput.Location = New System.Drawing.Point(447, 218)
            Me.btnChooseOutput.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.btnChooseOutput.Name = "btnChooseOutput"
            Me.btnChooseOutput.Size = New System.Drawing.Size(57, 20)
            Me.btnChooseOutput.TabIndex = 20
            Me.btnChooseOutput.Text = "..."
            Me.btnChooseOutput.UseVisualStyleBackColor = True
            '
            'txtOutput
            '
            Me.txtOutput.Location = New System.Drawing.Point(101, 218)
            Me.txtOutput.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.txtOutput.Name = "txtOutput"
            Me.txtOutput.ReadOnly = True
            Me.txtOutput.Size = New System.Drawing.Size(339, 20)
            Me.txtOutput.TabIndex = 19
            Me.txtOutput.TabStop = False
            '
            'label9
            '
            Me.label9.AutoSize = True
            Me.label9.Location = New System.Drawing.Point(17, 218)
            Me.label9.Margin = New System.Windows.Forms.Padding(2, 0, 2, 0)
            Me.label9.Name = "label9"
            Me.label9.Size = New System.Drawing.Size(58, 13)
            Me.label9.TabIndex = 18
            Me.label9.Text = "Output file:"
            '
            'CaptureDSForm
            '
            Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
            Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
            Me.ClientSize = New System.Drawing.Size(531, 397)
            Me.Controls.Add(Me.btnChooseOutput)
            Me.Controls.Add(Me.txtOutput)
            Me.Controls.Add(Me.label9)
            Me.Controls.Add(Me.comboPresets)
            Me.Controls.Add(Me.label2)
            Me.Controls.Add(Me.previewBox)
            Me.Controls.Add(Me.groupBox3)
            Me.Controls.Add(Me.txtRecording)
            Me.Controls.Add(Me.cmdRecord)
            Me.Controls.Add(Me.groupBox2)
            Me.Controls.Add(Me.groupBox1)
            Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle
            Me.Margin = New System.Windows.Forms.Padding(2, 2, 2, 2)
            Me.MaximizeBox = False
            Me.Name = "CaptureDSForm"
            Me.Text = "CaptureDS (VB.NET)"
            Me.groupBox1.ResumeLayout(False)
            Me.groupBox2.ResumeLayout(False)
            Me.groupBox3.ResumeLayout(False)
            Me.groupBox3.PerformLayout()
            CType(Me.previewBox, System.ComponentModel.ISupportInitialize).EndInit()
            Me.ResumeLayout(False)
            Me.PerformLayout()

        End Sub

		#End Region

		Private WithEvents listAudioDev As System.Windows.Forms.ComboBox
		Private WithEvents cmdAudioDevProp As System.Windows.Forms.Button
		Private WithEvents listVideoDev As System.Windows.Forms.ComboBox
		Private WithEvents cmdVideoDevProp As System.Windows.Forms.Button
		Private WithEvents cmdVideoCaptureProp As System.Windows.Forms.Button
		Private groupBox1 As System.Windows.Forms.GroupBox
		Private groupBox2 As System.Windows.Forms.GroupBox
		Private WithEvents cmdRecord As System.Windows.Forms.Button
		Private txtRecording As System.Windows.Forms.Label
		Private groupBox3 As System.Windows.Forms.GroupBox
		Private label3 As System.Windows.Forms.Label
		Private txtRecTime As System.Windows.Forms.Label
		Private labelNotDropped As System.Windows.Forms.Label
		Private labelDropped As System.Windows.Forms.Label
		Private txtNumNotDropped As System.Windows.Forms.Label
		Private txtNumDropped As System.Windows.Forms.Label
		Private txtAverageFPS As System.Windows.Forms.Label
		Private label1 As System.Windows.Forms.Label
		Private label4 As System.Windows.Forms.Label
		Private label5 As System.Windows.Forms.Label
		Private txtADropped As System.Windows.Forms.Label
		Private txtAProcessed As System.Windows.Forms.Label
		Private txtACallbacks As System.Windows.Forms.Label
		Private label6 As System.Windows.Forms.Label
		Private txtVDropped As System.Windows.Forms.Label
		Private txtVProcessed As System.Windows.Forms.Label
		Private txtVCallbacks As System.Windows.Forms.Label
		Private txtCurrentFPS As System.Windows.Forms.Label
		Private label10 As System.Windows.Forms.Label
		Private groupBox4 As System.Windows.Forms.GroupBox
		Private label8 As System.Windows.Forms.Label
		Private label7 As System.Windows.Forms.Label
		Private previewBox As System.Windows.Forms.PictureBox
		Private groupBox5 As System.Windows.Forms.GroupBox
		Private WithEvents comboPresets As System.Windows.Forms.ComboBox
		Private label2 As System.Windows.Forms.Label
		Private WithEvents btnChooseOutput As System.Windows.Forms.Button
		Private txtOutput As System.Windows.Forms.TextBox
		Private label9 As System.Windows.Forms.Label

	End Class
End Namespace

