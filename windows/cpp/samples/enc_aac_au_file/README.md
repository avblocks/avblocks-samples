## enc_aac_au_file

The enc_aac_au_file sample shows how to encode WAV file to AAC file in raw data format.

### Command Line

~~~ shell
enc_aac_au_file -i <wav file> -o <aac file>
enc_aac_au_file --input <wav file> --output <aac file>
~~~

###	Examples

The following example encode input file `..\assets\aud\equinox-48KHz.wav` into output file `equinox-48KHz.au.aac`:

~~~ shell
.\enc_aac_au_file.exe --input ..\assets\aud\equinox-48KHz.wav --output equinox-48KHz.au.aac
~~~

and using default options:
~~~ shell
.\enc_aac_au_file.exe
~~~
***
~~~ shell
Windows PowerShell
Copyright (C) 2015 Microsoft Corporation. All rights reserved.

PS AVBlocksSDK\lib> .\enc_aac_au_file.exe --input ..\assets\aud\equinox-48KHz.wav --output equinox-48KHz.au.aac
Input file: ..\assets\aud\equinox-48KHz.wav
Output file: equinox-48KHz.au.aac

PS AVBlocksSDK\lib>
~~~ 
