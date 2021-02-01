## enc_aac_au_pull

The enc_aac_au_pull sample shows how to encode WAV file to AAC file in raw data format with `Transcoder.Pull`.

### Command Line

~~~ shell
enc_aac_au_pull -i <wav file> -o <aac file>
enc_aac_au_pull --input <wav file> --output <aac file>
~~~

###	Examples

The following example encode input file `..\assets\aud\equinox-48Khz.wav` into output file `equinox-48Khz.au.aac`:

~~~ shell
.\enc_aac_au_pull.x64.exe --input ..\assets\aud\equinox-48Khz.wav --output equinox-48Khz.au.aac
~~~

and using default options:
~~~ shell
.\enc_aac_au_pull.x64.exe
~~~
***
~~~ shell
Windows PowerShell
Copyright (C) 2015 Microsoft Corporation. All rights reserved.

PS AVBlocksSDK\lib> .\enc_aac_au_pull.x64.exe --input ..\assets\aud\equinox-48Khz.wav --output equinox-48Khz.au.aac
Input file: ..\assets\aud\equinox-48Khz.wav
Output file: equinox-48Khz.au.aac
Transcoder pull: End of stream, facility:Codec code:9 hint:

PS AVBlocksSDK\lib>
~~~ 
