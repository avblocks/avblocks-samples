Imports Microsoft.VisualBasic
Imports System
Namespace CaptureDS
	Partial Public Class VideoCapturePropertiesForm
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
			Me.comboBoxFormats = New System.Windows.Forms.ComboBox()
			Me.buttonOK = New System.Windows.Forms.Button()
			Me.buttonCancel = New System.Windows.Forms.Button()
			Me.label1 = New System.Windows.Forms.Label()
			Me.label2 = New System.Windows.Forms.Label()
			Me.comboBoxFrameRate = New System.Windows.Forms.ComboBox()
			Me.label3 = New System.Windows.Forms.Label()
			Me.SuspendLayout()
			' 
			' comboBoxFormats
			' 
			Me.comboBoxFormats.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
			Me.comboBoxFormats.FormattingEnabled = True
			Me.comboBoxFormats.Location = New System.Drawing.Point(90, 47)
			Me.comboBoxFormats.Name = "comboBoxFormats"
			Me.comboBoxFormats.Size = New System.Drawing.Size(342, 21)
			Me.comboBoxFormats.TabIndex = 0
			' 
			' buttonOK
			' 
			Me.buttonOK.Location = New System.Drawing.Point(246, 149)
			Me.buttonOK.Name = "buttonOK"
			Me.buttonOK.Size = New System.Drawing.Size(75, 23)
			Me.buttonOK.TabIndex = 1
			Me.buttonOK.Text = "OK"
			Me.buttonOK.UseVisualStyleBackColor = True
'			Me.buttonOK.Click += New System.EventHandler(Me.buttonOK_Click);
			' 
			' buttonCancel
			' 
			Me.buttonCancel.Location = New System.Drawing.Point(354, 149)
			Me.buttonCancel.Name = "buttonCancel"
			Me.buttonCancel.Size = New System.Drawing.Size(75, 23)
			Me.buttonCancel.TabIndex = 2
			Me.buttonCancel.Text = "Cancel"
			Me.buttonCancel.UseVisualStyleBackColor = True
'			Me.buttonCancel.Click += New System.EventHandler(Me.buttonCancel_Click);
			' 
			' label1
			' 
			Me.label1.AutoSize = True
			Me.label1.Location = New System.Drawing.Point(13, 52)
			Me.label1.Name = "label1"
			Me.label1.Size = New System.Drawing.Size(69, 13)
			Me.label1.TabIndex = 3
			Me.label1.Text = "Video format:"
			' 
			' label2
			' 
			Me.label2.AutoSize = True
			Me.label2.Location = New System.Drawing.Point(13, 91)
			Me.label2.Name = "label2"
			Me.label2.Size = New System.Drawing.Size(60, 13)
			Me.label2.TabIndex = 5
			Me.label2.Text = "Frame rate:"
			' 
			' comboBoxFPS
			' 
			Me.comboBoxFrameRate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
			Me.comboBoxFrameRate.FormattingEnabled = True
			Me.comboBoxFrameRate.Location = New System.Drawing.Point(90, 86)
			Me.comboBoxFrameRate.Name = "comboBoxFPS"
			Me.comboBoxFrameRate.Size = New System.Drawing.Size(64, 21)
			Me.comboBoxFrameRate.TabIndex = 4
			' 
			' label3
			' 
			Me.label3.AutoSize = True
			Me.label3.Location = New System.Drawing.Point(161, 91)
			Me.label3.Name = "label3"
			Me.label3.Size = New System.Drawing.Size(27, 13)
			Me.label3.TabIndex = 6
			Me.label3.Text = "FPS"
			' 
			' VideoCapturePropertiesForm
			' 
			Me.AutoScaleDimensions = New System.Drawing.SizeF(6F, 13F)
			Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
			Me.ClientSize = New System.Drawing.Size(447, 192)
			Me.Controls.Add(Me.label3)
			Me.Controls.Add(Me.label2)
			Me.Controls.Add(Me.comboBoxFrameRate)
			Me.Controls.Add(Me.label1)
			Me.Controls.Add(Me.buttonCancel)
			Me.Controls.Add(Me.buttonOK)
			Me.Controls.Add(Me.comboBoxFormats)
			Me.MaximizeBox = False
			Me.MinimizeBox = False
			Me.Name = "VideoCapturePropertiesForm"
			Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent
			Me.Text = "Properties"
'			Me.Load += New System.EventHandler(Me.VideoCapturePropertiesForm_Load);
			Me.ResumeLayout(False)
			Me.PerformLayout()

		End Sub

		#End Region

		Private comboBoxFormats As System.Windows.Forms.ComboBox
		Private WithEvents buttonOK As System.Windows.Forms.Button
		Private WithEvents buttonCancel As System.Windows.Forms.Button
		Private label1 As System.Windows.Forms.Label
		Private label2 As System.Windows.Forms.Label
		Private comboBoxFrameRate As System.Windows.Forms.ComboBox
		Private label3 As System.Windows.Forms.Label
	End Class
End Namespace