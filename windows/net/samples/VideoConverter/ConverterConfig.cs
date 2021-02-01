/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.Collections.Generic;
using System.Text;

namespace VideoConverter
{
    public static class ConverterConfig
    {
        public static string Name = "VideoConverter";
        public static string InputFileFilter = "Video files (*.mp4,*.mpg,*.mpeg,*.mod,*.avi,*.wmv,*.mts,*.m2t,*.ts,*.tod,*.m2v,*.m4v,*.webm,*.dat,*.mpe,*.mpeg4,*.ogm)|*.mp4;*.mpg;*.mpeg;*.mod;*.avi;*.wmv;*.mts;*.m2t;*.ts;*.tod;*.m2v;*.m4v;*.webm;*.dat;*.mpe;*.mpeg4;*.ogm|";
        public static bool   ProduceAudio = false;
        public static bool   ProduceVideo = true;
    }
}
