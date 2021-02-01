using System;
using System.Collections.Generic;
using System.Windows.Forms;
using PrimoSoftware.AVBlocks;

namespace CaptureDS
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // Call EnableVisualStyles before initializing PrimoSoftware libraries
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            Library.Initialize();

            // Set license information. To run AVBlocks in demo mode, comment the next line out
            // Library.SetLicense("<license-string>");

            // allow AMD MFT
            Library.Config.Hardware.AmdMft = true;

            Application.Run(new CaptureDSForm());

            Library.Shutdown();
        }
    }
}
