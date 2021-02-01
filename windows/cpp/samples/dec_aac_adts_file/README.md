## dec_aac_adts_file

Decode AAC file in Audio Data Transport Stream (ADTS) format and save output to WAV file.

### Command Line

~~~ shell
dec_aac_adts_file -i <aac file> -o <wav file>
dec_aac_adts_file --input <aac file> --output <wav file>
~~~

###	Examples

The following example encode input file `..\assets\aud\Hydrate-Kenny_Beltrey.adts.aac` into output file `Hydrate-Kenny_Beltrey.wav`:

~~~ shell
.\dec_aac_adts_file.exe --input ..\assets\aud\Hydrate-Kenny_Beltrey.adts.aac --output Hydrate-Kenny_Beltrey.wav
~~~

and using default options:

``` posh
.\dec_aac_adts_file.exe

Using default options:
 --input ..\assets\aud\Hydrate-Kenny_Beltrey.adts.aac
 --output Hydrate-Kenny_Beltrey.wav
``` 
