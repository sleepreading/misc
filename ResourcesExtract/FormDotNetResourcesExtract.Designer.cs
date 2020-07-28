namespace NirSoft.DotNetResourcesExtract
{
    partial class FormDotNetResourcesExtract
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
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.textBoxSource = new System.Windows.Forms.TextBox();
            this.textBoxDest = new System.Windows.Forms.TextBox();
            this.buttonStart = new System.Windows.Forms.Button();
            this.buttonExit = new System.Windows.Forms.Button();
            this.checkBoxSubFolders = new System.Windows.Forms.CheckBox();
            this.buttonStop = new System.Windows.Forms.Button();
            this.labelCurrentFilename = new System.Windows.Forms.Label();
            this.folderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.buttonBrowseSource = new System.Windows.Forms.Button();
            this.buttonBrowseDest = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 16);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(89, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Source Wildcard:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(11, 51);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(95, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Destination Folder:";
            // 
            // textBoxSource
            // 
            this.textBoxSource.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest;
            this.textBoxSource.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.FileSystem;
            this.textBoxSource.Location = new System.Drawing.Point(118, 13);
            this.textBoxSource.Name = "textBoxSource";
            this.textBoxSource.Size = new System.Drawing.Size(368, 20);
            this.textBoxSource.TabIndex = 1;
            // 
            // textBoxDest
            // 
            this.textBoxDest.AutoCompleteMode = System.Windows.Forms.AutoCompleteMode.Suggest;
            this.textBoxDest.AutoCompleteSource = System.Windows.Forms.AutoCompleteSource.FileSystemDirectories;
            this.textBoxDest.Location = new System.Drawing.Point(118, 49);
            this.textBoxDest.Name = "textBoxDest";
            this.textBoxDest.Size = new System.Drawing.Size(368, 20);
            this.textBoxDest.TabIndex = 3;
            // 
            // buttonStart
            // 
            this.buttonStart.Location = new System.Drawing.Point(175, 146);
            this.buttonStart.Name = "buttonStart";
            this.buttonStart.Size = new System.Drawing.Size(110, 26);
            this.buttonStart.TabIndex = 6;
            this.buttonStart.Text = "&Start";
            this.buttonStart.UseVisualStyleBackColor = true;
            this.buttonStart.Click += new System.EventHandler(this.buttonStart_Click);
            // 
            // buttonExit
            // 
            this.buttonExit.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.buttonExit.Location = new System.Drawing.Point(421, 146);
            this.buttonExit.Name = "buttonExit";
            this.buttonExit.Size = new System.Drawing.Size(110, 26);
            this.buttonExit.TabIndex = 8;
            this.buttonExit.Text = "E&xit";
            this.buttonExit.UseVisualStyleBackColor = true;
            this.buttonExit.Click += new System.EventHandler(this.buttonExit_Click);
            // 
            // checkBoxSubFolders
            // 
            this.checkBoxSubFolders.AutoSize = true;
            this.checkBoxSubFolders.Location = new System.Drawing.Point(12, 92);
            this.checkBoxSubFolders.Name = "checkBoxSubFolders";
            this.checkBoxSubFolders.Size = new System.Drawing.Size(102, 17);
            this.checkBoxSubFolders.TabIndex = 5;
            this.checkBoxSubFolders.Text = "Scan subfolders";
            this.checkBoxSubFolders.UseVisualStyleBackColor = true;
            // 
            // buttonStop
            // 
            this.buttonStop.Location = new System.Drawing.Point(298, 146);
            this.buttonStop.Name = "buttonStop";
            this.buttonStop.Size = new System.Drawing.Size(110, 26);
            this.buttonStop.TabIndex = 7;
            this.buttonStop.Text = "S&top";
            this.buttonStop.UseVisualStyleBackColor = true;
            this.buttonStop.Click += new System.EventHandler(this.buttonStop_Click);
            // 
            // labelCurrentFilename
            // 
            this.labelCurrentFilename.AutoSize = true;
            this.labelCurrentFilename.Location = new System.Drawing.Point(9, 115);
            this.labelCurrentFilename.Name = "labelCurrentFilename";
            this.labelCurrentFilename.Size = new System.Drawing.Size(0, 13);
            this.labelCurrentFilename.TabIndex = 8;
            // 
            // openFileDialog
            // 
            this.openFileDialog.DefaultExt = "dll";
            this.openFileDialog.FileName = "openFileDialog1";
            this.openFileDialog.Filter = ".NET Framework EXE/DLL files|*.dll;*.exe";
            // 
            // buttonBrowseSource
            // 
            this.buttonBrowseSource.Location = new System.Drawing.Point(493, 12);
            this.buttonBrowseSource.Name = "buttonBrowseSource";
            this.buttonBrowseSource.Size = new System.Drawing.Size(38, 21);
            this.buttonBrowseSource.TabIndex = 2;
            this.buttonBrowseSource.Text = "...";
            this.buttonBrowseSource.UseVisualStyleBackColor = true;
            this.buttonBrowseSource.Click += new System.EventHandler(this.buttonBrowseSource_Click);
            // 
            // buttonBrowseDest
            // 
            this.buttonBrowseDest.Location = new System.Drawing.Point(493, 49);
            this.buttonBrowseDest.Name = "buttonBrowseDest";
            this.buttonBrowseDest.Size = new System.Drawing.Size(38, 21);
            this.buttonBrowseDest.TabIndex = 4;
            this.buttonBrowseDest.Text = "...";
            this.buttonBrowseDest.UseVisualStyleBackColor = true;
            this.buttonBrowseDest.Click += new System.EventHandler(this.buttonBrowseDest_Click);
            // 
            // FormDotNetResourcesExtract
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.buttonExit;
            this.ClientSize = new System.Drawing.Size(553, 182);
            this.Controls.Add(this.buttonBrowseDest);
            this.Controls.Add(this.buttonBrowseSource);
            this.Controls.Add(this.labelCurrentFilename);
            this.Controls.Add(this.buttonStop);
            this.Controls.Add(this.checkBoxSubFolders);
            this.Controls.Add(this.buttonExit);
            this.Controls.Add(this.buttonStart);
            this.Controls.Add(this.textBoxDest);
            this.Controls.Add(this.textBoxSource);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.Name = "FormDotNetResourcesExtract";
            this.Text = "DotNetResourcesExtract";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormDotNetResourcesExtract_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBoxSource;
        private System.Windows.Forms.TextBox textBoxDest;
        private System.Windows.Forms.Button buttonStart;
        private System.Windows.Forms.Button buttonExit;
        private System.Windows.Forms.CheckBox checkBoxSubFolders;
        private System.Windows.Forms.Button buttonStop;
        private System.Windows.Forms.Label labelCurrentFilename;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog;
        private System.Windows.Forms.OpenFileDialog openFileDialog;
        private System.Windows.Forms.Button buttonBrowseSource;
        private System.Windows.Forms.Button buttonBrowseDest;
    }
}

