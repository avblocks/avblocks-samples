## enc_aac_au_pull

The enc_aac_au_pull sample shows how to encode WAV file to AAC file in raw data format with `Transcoder::pull`.

### Command Line

~~~ shell
enc_aac_au_pull -i <wav file> -o <aac file>
enc_aac_au_pull --input <wav file> --output <aac file>
~~~

###	Examples

The following example encode input file `../assets/aud/equinox-48KHz.wav` into output file `equinox-48KHz.au.aac`:

~~~ shell
./enc_aac_au_pull --input ../assets/aud/equinox-48KHz.wav --output equinox-48KHz.au.aac
~~~

and using default options:
~~~ shell
./enc_aac_au_pull
~~~
***
~~~ shell
mac:~/AVBlocksSDK.macosx/lib$ ./enc_aac_au_pull --input ../assets/aud/equinox-48KHz.wav --output equinox-48KHz.au.aac
Input file: ../../assets/aud/equinox-48KHz.wav
Output file: equinox-48KHz.au.aac
Transcoder pull: End of stream, facility:9, error:9, hint:

mac:~/AVBlocksSDK.macosx/lib$
~~~ 