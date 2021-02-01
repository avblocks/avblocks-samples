## read_metadata_any_file

The read_metadata_any_file sample shows how to use the AVBlocks MediaInfo and Metadata APIs to extract metadata information from a media file.   

### Command Line

	./read_metadata_any_file --input <file>
 
###	Examples

The following example extracts the metadata from the `Hydrate-Kenny_Beltrey.ogg` song:
	
	./read_metadata_any_file --input ../assets/aud/Hydrate-Kenny_Beltrey.ogg
	
and using default option:

	./read_metadata_any_file
	
***

	mac:~/AVBlocksSDK.macosx/lib$ ./read_metadata_any_file --input ../assets/aud/Hydrate-Kenny_Beltrey.ogg
	Metadata
	--------
	6 attributes:
	Title          : Hydrate - Kenny Beltrey
	LeadArtist     : Kenny Beltrey
	Album          : Favorite Things
	Date           : 2002
	Comment        : http://www.kahvi.org
	TrackNum       : 2
	
	0 pictures:
	
	mac:~/AVBlocksSDK.macosx/lib$ 
