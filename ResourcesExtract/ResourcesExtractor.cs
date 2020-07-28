using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Reflection;
using System.Threading;

namespace NirSoft.DotNetResourcesExtract
{
    
    public class ResourcesExtractorConfig
    {
        private string sourceWildcard;
        private string destFolder;
        private bool scanSubFolders;

        public ResourcesExtractorConfig()
        {
            sourceWildcard = string.Empty;
            destFolder = string.Empty;
            scanSubFolders = false;
        }

        public bool ScanSubFolders
        {
            get
            {
                return scanSubFolders;
            }
            set
            {
                scanSubFolders = value;
            }
        }

        public string SourceWildcard
        {
            get
            {
                return sourceWildcard;
            }
            set
            {
                sourceWildcard = value;
            }
        }

        public string DestFolder
        {
            get
            {
                return destFolder;
            }
            set
            {
                destFolder = value;
            }
        }

    }

    public delegate void OnExceptionEventHandler(Exception exception);
    public delegate void OnExtractFilenameEventHandler(string filename);
    public delegate void OnExtractFinishEventHandler();

    public class ResourcesExtractor
    {
        private bool stopProcess = false;
        private Thread extractThread;

        public event OnExceptionEventHandler OnException;
        public event OnExtractFilenameEventHandler OnExtractFilename;
        public event OnExtractFinishEventHandler OnExtractFinish;
 
        public void StopProcess()
        {
            stopProcess = true;
        }

        public void Extract(ResourcesExtractorConfig config)
        {
            extractThread = new Thread(new ParameterizedThreadStart(ExtractThread));
            extractThread.Start(config);
        }

        public void ExtractThread(object data)
        {
            ResourcesExtractorConfig config = (ResourcesExtractorConfig)data;

            stopProcess = false;

            try
            {
                string wildcardOnly = Path.GetFileName(config.SourceWildcard);

                string path = config.SourceWildcard.Substring(0, config.SourceWildcard.LastIndexOf('\\'));

                Extract(path, wildcardOnly, config);


                
            }
            catch (Exception exception)
            {
                if (OnException != null)
                    OnException(exception);
            }

            OnExtractFinish();
            
        }

        private void Extract(string path, string wildcard, ResourcesExtractorConfig config)
        {

            DirectoryInfo dirInfo = new DirectoryInfo(path);

            FileInfo[] files = dirInfo.GetFiles(wildcard);

            foreach (FileInfo file in files)
            {
                if (stopProcess)
                    return;

                ExtractFilename(file.FullName, config);    
            }

            if (config.ScanSubFolders)
            {
                DirectoryInfo[] dirs = dirInfo.GetDirectories();

                foreach (DirectoryInfo subdirInfo in dirs)
                {
                    if (stopProcess)
                        return;

                    Extract(subdirInfo.FullName, wildcard, config);
                }
                
            }

        }

        private void ExtractFilename(string filename, ResourcesExtractorConfig config)
        {
            if (OnExtractFilename != null)
                OnExtractFilename(filename);

            Assembly assembly = null;

            try
            {
                assembly = Assembly.LoadFile(filename);
            }
            catch
            {

            }

            if (assembly != null)
            {
                string[] names = assembly.GetManifestResourceNames();

                foreach (string name in names)
                {
                    int bufferLen = 4096;
                    byte[] buffer = new byte[bufferLen];
                    Stream stream = assembly.GetManifestResourceStream(name);

                    string destFilename = Path.Combine(config.DestFolder, name);
                    String directory = Path.GetDirectoryName(destFilename);
                    Directory.CreateDirectory(directory);

                    FileStream fileStream = new FileStream(destFilename, FileMode.Create, FileAccess.ReadWrite);


                    while (true)
                    {
                        int read = stream.Read(buffer, 0, bufferLen);

                        if (read > 0)
                        {
                            fileStream.Write(buffer, 0, read);
                        }
                        else
                            break;

                    }

                    fileStream.Close();
                    stream.Close();


                }

            }
        }

    }


}
