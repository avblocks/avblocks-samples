## enc_aac_adts_file

The enc_aac_adts_file sample shows how to encode WAV file to AAC file in Audio Data Transport Stream (ADTS) format.

### Command Line

~~~ shell
enc_aac_adts_file -i <wav file> -o <aac file>
enc_aac_adts_file --input <wav file> --output <aac file>
~~~

###	Examples

The following example encode input file `../assets/aud/equinox-48KHz.wav` into output file `equinox-48KHz.adts.aac`:

~~~ shell
./enc_aac_adts_file --input ../assets/aud/equinox-48KHz.wav --output equinox-48KHz.adts.aac
~~~

and using default options:
~~~ shell
./enc_aac_adts_file
~~~
***
~~~ shell
mac:~/AVBlocksSDK.macosx/lib$ ./enc_aac_adts_file --input ../assets/aud/equinox-48KHz.wav --output equinox-48KHz.adts.aac
Input file: ../../assets/aud/equinox-48KHz.wav
Output file: equinox-48KHz.adts.aac

mac:~/AVBlocksSDK.macosx/lib$
~~~ 