## hw_caps

The hw_caps sample shows how to enumerate the available hardware encoding devices.  

### Command Line

	hw_caps.x64.exe 

###	Examples

The following command prints the available hardware encoding devices.
	
	.\hw_caps.x64.exe 

***

	Windows PowerShell
	Copyright (C) 2015 Microsoft Corporation. All rights reserved.
	
	PS AVBlocksSDK\lib>.\hw_caps.x64.exe 
	Vendor         Intel
	ID             0
	Device name    Intel GPU
	Engine         QuickSyncVideo
	Type           H264Encoder
	API            IntelMedia
	Codec name     QSV H.264 Encoder
	
	Vendor         Nvidia
	ID             0
	Device name    GeForce GTX 960
	Engine         Nvenc
	Type           H264Encoder
	API            Nvenc
	Codec name     NVENC H.264 Encoder
	Engine         Nvenc
	Type           H265Encoder
	API            Nvenc
	Codec name     NVENC HEVC Encoder
	
	Vendor         Nvidia
	ID             1
	Device name    Quadro K600
	Engine         Nvenc
	Type           H264Encoder
	API            Nvenc
	Codec name     NVENC H.264 Encoder

	PS AVBlocksSDK\lib>