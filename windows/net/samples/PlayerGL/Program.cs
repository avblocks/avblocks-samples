/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

using System;
using System.IO;
using System.Windows.Forms;
using PrimoSoftware.AVBlocks;


namespace PlayerGLSample
{
    class Program
    {
         [STAThread] 
        static int Main(string[] args)
        {
            var opt = new Options();

            if (!opt.Prepare(args))
                return opt.Error ? (int)ExitCodes.OptionsError : (int)ExitCodes.Success;

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            PrimoSoftware.AVBlocks.Library.Initialize();

            // Set license information. To run AVBlocks in demo mode, comment the next line out
            // Library.SetLicense("<license-string>");

            using (Player player = new Player())
            {
                if (player.Open(opt.InputFile))
	            {
		            player.EventLoop();
	            }
	            else
	            {
                    string msg = string.Format("Cannot open the input file: {0}\r\n", opt.InputFile);
                    MessageBox.Show(msg, "PlayerGL");
	            }
            }

            PrimoSoftware.AVBlocks.Library.Shutdown();

            return (int)ExitCodes.Success;
        }

         enum ExitCodes : int
         {
             Success = 0,
             OptionsError = 1,
             PlayerError = 2,
         }
    }
}
