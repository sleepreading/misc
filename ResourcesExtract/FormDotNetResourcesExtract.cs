using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace NirSoft.DotNetResourcesExtract
{
    public partial class FormDotNetResourcesExtract : Form
    {
        private ResourcesExtractorConfig config = new ResourcesExtractorConfig();
        private ResourcesExtractor resourcesExtractor;
        private bool inExtractProcess = false;

        public FormDotNetResourcesExtract()
        {
            InitializeComponent();
        }

        private void buttonExit_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            FormToConfig();

            inExtractProcess = true;
            EnableDisable();
            
            resourcesExtractor = new ResourcesExtractor();

            resourcesExtractor.OnException += new OnExceptionEventHandler(resourcesExtractor_OnException);
            resourcesExtractor.OnExtractFilename += new OnExtractFilenameEventHandler(resourcesExtractor_OnExtractFilename);
            resourcesExtractor.OnExtractFinish += new OnExtractFinishEventHandler(resourcesExtractor_OnExtractFinish);
            resourcesExtractor.Extract(config);

        }

        private void resourcesExtractor_OnExtractFinish()
        {
            Invoke(new OnExtractFinishEventHandler(resourcesExtractor_OnExtractFinishInvoke), null);
        }

        private void resourcesExtractor_OnExtractFinishInvoke()
        {
            inExtractProcess = false;
            labelCurrentFilename.Text = string.Empty;
            EnableDisable();
        }

        private void resourcesExtractor_OnExtractFilename(string filename)
        {
            Invoke(new OnExtractFilenameEventHandler(resourcesExtractor_OnExtractFilenameInvoke), new object[] { filename });
        }

        private void resourcesExtractor_OnExtractFilenameInvoke(string filename)
        {
            labelCurrentFilename.Text = filename;
        }


        private void resourcesExtractor_OnException(Exception exception)
        {
            Invoke(new OnExceptionEventHandler(resourcesExtractor_OnExceptionInvoke), new object[] { exception });
        }

        private void resourcesExtractor_OnExceptionInvoke(Exception exception)
        {
            MessageBox.Show(this, exception.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error); 
        }

        private void FormToConfig()
        {
            config.SourceWildcard = textBoxSource.Text;
            config.DestFolder = textBoxDest.Text;
            config.ScanSubFolders = checkBoxSubFolders.Checked;
        }

        private void buttonStop_Click(object sender, EventArgs e)
        {
            resourcesExtractor.StopProcess();
        }


        private void EnableDisable()
        {
            textBoxSource.Enabled = !inExtractProcess;
            textBoxDest.Enabled = !inExtractProcess;
            checkBoxSubFolders.Enabled = !inExtractProcess;
            buttonStart.Enabled = !inExtractProcess;
            buttonExit.Enabled = !inExtractProcess;
            buttonStop.Enabled = inExtractProcess;

        }

        private void FormDotNetResourcesExtract_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (inExtractProcess)
                e.Cancel = true;
        }

        private void buttonBrowseSource_Click(object sender, EventArgs e)
        {
            openFileDialog.FileName = textBoxSource.Text;

            if (openFileDialog.ShowDialog(this) == DialogResult.OK)
            {
                textBoxSource.Text = openFileDialog.FileName;
            }
        }

        private void buttonBrowseDest_Click(object sender, EventArgs e)
        {
            folderBrowserDialog.SelectedPath = textBoxDest.Text;

            if (folderBrowserDialog.ShowDialog(this) == DialogResult.OK)
            {
                textBoxDest.Text = folderBrowserDialog.SelectedPath;

            }
        }

    }

}