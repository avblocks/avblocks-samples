## enc_aac_adts_pull

The enc_aac_adts_pull sample shows how to encode WAV file to AAC file in Audio Data Transport Stream (ADTS) format with Transcoder::pull.

### Command Line

~~~ shell
enc_aac_adts_pull -i <wav file> -o <aac file>
enc_aac_adts_pull --input <wav file> --output <aac file>
~~~

###	Examples

The following example encode input file `../assets/aud/equinox-48KHz.wav` into output file `equinox-48KHz.adts.aac`:

~~~ shell
./enc_aac_adts_pull --input ../assets/aud/equinox-48KHz.wav --output equinox-48KHz.adts.aac
~~~

and using default options:
~~~ shell
./enc_aac_adts_pull
~~~
***
~~~ shell
mac:~/AVBlocksSDK.macosx/lib$ ./enc_aac_adts_pull --input ../assets/aud/equinox-48KHz.wav --output equinox-48KHz.adts.aac
Input file: ../../assets/aud/equinox-48KHz.wav
Output file: equinox-48KHz.adts.aac
Transcoder pull: End of stream, facility:9, error:9, hint:

mac:~/AVBlocksSDK.macosx/lib$
~~~ 