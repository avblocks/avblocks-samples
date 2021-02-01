## read_metadata_any_file

The read_metadata_any_file sample shows how to use the AVBlocks MediaInfo and Metadata APIs to extract metadata information from a media file.   

### Command Line

	.\read_metadata_any_file.x64.exe <file>
 
###	Examples

The following example extracts the metadata from the `Hydrate-Kenny_Beltrey.ogg` song:
	
	.\read_metadata_any_file.x64.exe ..\assets\aud\Hydrate-Kenny_Beltrey.ogg

and using default option:
	
	.\read_metadata_any_file.x64.exe

***

	Windows PowerShell
	Copyright (C) 2012 Microsoft Corporation. All rights reserved.
	
	PS AVBlocks.NET\lib> .\read_metadata_any_file.x64.exe ..\assets\aud\Hydrate-Kenny_Beltrey.ogg
	file: ..\assets\aud\Hydrate-Kenny_Beltrey.ogg
	
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
	
	PS AVBlocks.NET\lib>