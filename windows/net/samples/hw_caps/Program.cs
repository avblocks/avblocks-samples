/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.IO;
using PrimoSoftware.AVBlocks;

namespace HwCapsSample
{
    class Program
    {
        static int Main(string[] args)
        {
            Library.Initialize();

            // Set license information. To run AVBlocks in demo mode, comment the next line out
            // Library.SetLicense("<license-string>");

            PrintHardwareList();
   
            Library.Shutdown();

            return 0;
        }

        static void PrintHardwareList()
        {
            Hardware hw = new Hardware();

            hw.Refresh();

            foreach(var device in hw.Devices)
            {
                Console.WriteLine("{0,-15}{1}", "Vendor", device.Vendor);
                Console.WriteLine("{0,-15}{1}", "ID", device.ID);
                Console.WriteLine("{0,-15}{1}", "Device name", device.Name);
                
                foreach(var codec in device.Codecs)
                {
                    Console.WriteLine("{0,-15}{1}", "Engine", codec.Engine);
                    Console.WriteLine("{0,-15}{1}", "Type", codec.Type);
                    Console.WriteLine("{0,-15}{1}", "API", codec.Api);
                    Console.WriteLine("{0,-15}{1}", "Codec name", codec.Name);
                }

                Console.WriteLine();
            }
        }
    }
}
