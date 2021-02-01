/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using PrimoSoftware.AVBlocks;

namespace AudioConverter
{
    public partial class ConverterForm : Form
    {
        private bool m_working;
        private bool m_stopRequested;
        
        private string m_inputFile;
        private string m_outputFile;
        private string m_outputPreset;

        private string m_errorMessage;

        public ConverterForm()
        {
            InitializeComponent();

            this.Text = "AVBlocks SDK for .NET - " + ConverterConfig.Name;
        }

        private void ConverterForm_Load(object sender, EventArgs e)
        {
            for (int i=0; i < AvbTranscoder.Presets.Length; ++i)
            {
                PresetDescriptor preset =  AvbTranscoder.Presets[i];

                if ((preset.AudioOnly && ConverterConfig.ProduceAudio) ||
                    (!preset.AudioOnly && ConverterConfig.ProduceVideo))
                {
                    comboPresets.Items.Add(preset);
                }
            }

            comboPresets.SelectedIndex = 0;
            UpdateControls();
        }

        private void btnChooseInput_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Multiselect = false;
            
            dlg.Filter = ConverterConfig.InputFileFilter + "All files (*.*)|*.*";

            if (txtInput.Text.Length > 0)
            {
                dlg.FileName = System.IO.Path.GetFileName(txtInput.Text);
                dlg.InitialDirectory = System.IO.Path.GetDirectoryName(txtInput.Text);
            }

            if (DialogResult.OK != dlg.ShowDialog())
                return;
            
            txtInput.Text = dlg.FileName;

        }

        private void btnChooseOutput_Click(object sender, EventArgs e)
        {
            PresetDescriptor preset = comboPresets.SelectedItem as PresetDescriptor;

            SaveFileDialog dlg = new SaveFileDialog();

            string filter = string.Empty;

            if (!string.IsNullOrEmpty(preset.FileExtension))
            {
                dlg.DefaultExt = preset.FileExtension;

                filter = string.Format("(*.{0})|*.{0}|", preset.FileExtension);
            }

            filter += "All files (*.*)|*.*";
            dlg.Filter = filter;
            dlg.OverwritePrompt = true;

            if (txtOutput.Text.Length > 0)
            {
                dlg.FileName = System.IO.Path.GetFileName(txtOutput.Text);
                dlg.InitialDirectory = System.IO.Path.GetDirectoryName(txtOutput.Text);
            }

            if (DialogResult.OK != dlg.ShowDialog())
                return;

            txtOutput.Text = dlg.FileName;
        }

        private void comboPresets_SelectedIndexChanged(object sender, EventArgs e)
        {
            PresetDescriptor preset = comboPresets.SelectedItem as PresetDescriptor;

            if (string.IsNullOrEmpty(preset.FileExtension) || txtOutput.Text.Length == 0)
                return;

	        string newExt = "." + preset.FileExtension;
            string oldExt = System.IO.Path.GetExtension(txtOutput.Text);

            if (oldExt == newExt)
                return;

            string newFile = System.IO.Path.ChangeExtension(txtOutput.Text, newExt);

            if (System.IO.File.Exists(newFile))
            {
                string prompt = string.Format("{0} already exists. Do you want to replace the file?", newFile);
                if (DialogResult.Yes != MessageBox.Show(prompt, "Warning",MessageBoxButtons.YesNo,MessageBoxIcon.Warning))
                    return;
            }
	
	        txtOutput.Text = newFile;
        }

        private bool ValidateInputData()
        {
            if (txtInput.Text.Length == 0)
            {
                MessageBox.Show("Please select input file.");
                return false;
            }

            if (txtOutput.Text.Length == 0)
            {
                MessageBox.Show("Please select output file.");
                return false;
            }

            return true;
        }

        private void UpdateControls()
        {
            btnChooseInput.Enabled = !m_working;
            btnChooseOutput.Enabled = !m_working;
            comboPresets.Enabled = !m_working;
            btnStart.Enabled = !m_working;
            btnStop.Enabled = (m_working && !m_stopRequested);
            
	        if (m_stopRequested)
            {
                status.Text = "Stopping...";
            }
	        else
            {
                if (m_working)
                {
                    status.Text = "Working";
                }
                else
                {
                    status.Text = "Ready";
                }
            }
        }

        private void btnStart_Click(object sender, EventArgs e)
        {
            if (m_working)
                return;

            if (!ValidateInputData())
                return;

            PresetDescriptor preset = comboPresets.SelectedItem as PresetDescriptor;
            m_outputPreset = preset.Name;
            m_inputFile = txtInput.Text;
            m_outputFile = txtOutput.Text;
            m_stopRequested = false;
            m_working = true;

            UpdateControls();

            try
            {
                System.IO.File.Delete(txtOutput.Text);
            }
            catch { }

            Thread convertThread = new Thread(this.Convert);
            convertThread.Start();
        }

        private void btnStop_Click(object sender, EventArgs e)
        {
            if (m_working)
            {
                this.m_stopRequested = true;
	            UpdateControls();
            }
        }

        // This method runs in a worker thread
        private void Convert()
        {
            m_errorMessage = AvbTranscoder.ConvertFileWithPreset(
                m_inputFile, m_outputFile, m_outputPreset,this.OnContinue, this.OnProgress, this.OnStatus
                );

            BeginInvoke(new MethodInvoker(OnConvertFinished));
        }


        private void OnConvertFinished()
        {
            status.Text = "Finished";
	        
	        if (m_errorMessage == null)
	        {
		        MessageBox.Show("Conversion successful.", "", MessageBoxButtons.OK, MessageBoxIcon.Information);
	        }
	        else
	        {
                MessageBox.Show(m_errorMessage, "Conversion failed", MessageBoxButtons.OK, MessageBoxIcon.Error);
	        }

            progress.Value = 0;
	
	        m_stopRequested = false;
            m_working = false;

	        UpdateControls();
        }

        private delegate void UpdateProgressDelegate(int percent);
        private void UpdateProgress(int percent)
        {
            this.progress.Value = percent;
        }

        private void OnProgress(object sender, TranscoderProgressEventArgs e)
        {
            if (e.TotalTime > 0)
            {
                int percent = (int)(100 * e.CurrentTime / e.TotalTime);
                percent = Math.Max(0, percent);
                percent = Math.Min(100, percent);

                this.BeginInvoke(new UpdateProgressDelegate(UpdateProgress), percent);
            }
        }

        private void OnStatus(object sender, TranscoderStatusEventArgs e)
        {
            if (e.Status == TranscoderStatus.Completed && !m_stopRequested)
            {
                this.BeginInvoke(new UpdateProgressDelegate(UpdateProgress),100);
            }
        }

        private void OnContinue(object sender, TranscoderContinueEventArgs e)
        {
            e.Continue = !m_stopRequested;
        }

    }
}
