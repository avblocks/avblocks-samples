## enc_aac_adif_file

The enc_aac_adif_file sample shows how to encode WAV file to AAC file in Audio Data Transport Stream (ADTS) format.

### Command Line

~~~ shell
enc_aac_adif_file -i <wav file> -o <aac file>
enc_aac_adif_file --input <wav file> --output <aac file>
~~~

###	Examples

The following example encode input file `../assets/aud/equinox-48KHz.wav` into output file `equinox-48KHz.adif.aac`:

~~~ shell
./enc_aac_adif_file --input ../assets/aud/equinox-48KHz.wav --output equinox-48KHz.adif.aac
~~~

and using default options:
~~~ shell
./enc_aac_adif_file
~~~
***
~~~ shell
mac:~/AVBlocksSDK.macosx/lib$ ./enc_aac_adif_file --input ../assets/aud/equinox-48KHz.wav --output equinox-48KHz.adif.aac
Input file: ../../assets/aud/equinox-48KHz.wav
Output file: equinox-48KHz.adif.aac

mac:~/AVBlocksSDK.macosx/lib$
~~~ 