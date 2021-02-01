## enc_aac_adif_file

The enc_aac_adif_file sample shows how to encode WAV file to AAC file in Audio Data Transport Stream (ADTS) format.

### Command Line

~~~ shell
enc_aac_adif_file -i <wav file> -o <aac file>
enc_aac_adif_file --input <wav file> --output <aac file>
~~~

###	Examples

The following example encode input file `..\assets\aud\equinox-48Khz.wav` into output file `equinox-48Khz.adif.aac`:

~~~ shell
.\enc_aac_adif_file.x64.exe --input ..\assets\aud\equinox-48Khz.wav --output equinox-48Khz.adif.aac
~~~

and using default options:
~~~ shell
.\enc_aac_adif_file.x64.exe
~~~
***
~~~ shell
Windows PowerShell
Copyright (C) 2015 Microsoft Corporation. All rights reserved.

PS AVBlocksSDK\lib> .\enc_aac_adif_file.x64.exe --input ..\assets\aud\equinox-48Khz.wav --output equinox-48Khz.adif.aac
Input file: ..\assets\aud\equinox-48Khz.wav
Output file: equinox-48Khz.adif.aac

PS AVBlocksSDK\lib>
~~~ 
