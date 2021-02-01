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

namespace AudioConverter
{
    public static class ConverterConfig
    {
        public static string Name = "AudioConverter";
        public static string InputFileFilter = "Audio files (*.wma,*.wav,*.aac,*.m4a,*.mp3,*.mp2,*.ogg,*.oga,*.ogm)|*.wma;*.wav;*.aac;*.m4a;*.mp3;*.mp2;*.ogg;*.oga;*.ogm|Video files (*.mp4,*.mpg,*.mpeg,*.avi,*.wmv,*.mts,*.ts,*.m4v,*.webm,*.dat,*.mpe,*.mpeg4)|*.mp4;*.mpg;*.mpeg;*.avi;*.wmv;*.mts;*.ts;*.m4v;*.webm;*.dat;*.mpe;*.mpeg4|";
        public static bool   ProduceAudio = true;
        public static bool   ProduceVideo = false;
    }
}
