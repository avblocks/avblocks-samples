/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/


#include "stdafx.h"
#include "AvbUtil.h"
#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo;
using namespace std;

void savePicture(primo::codecs::MetaPicture* pic, const wstring& baseFilename)
{
	wstring filename(baseFilename);

	if (0 == strcmp( pic->mimeType(), primo::codecs::MimeType::Jpeg))
	{
		filename += L".jpg";
	}
	else if (0 == strcmp( pic->mimeType(), primo::codecs::MimeType::Png))
	{
		filename += L".png";
	}
	else if (0 == strcmp( pic->mimeType(), primo::codecs::MimeType::Gif))
	{
		filename += L".gif";
	}
	else if (0 == strcmp( pic->mimeType(), primo::codecs::MimeType::Tiff))
	{
		filename += L".tiff";
	}
	else
	{
		// unexpected picture mime type
		return;
	}

	ofstream out(filename.c_str(), ios::out | ios::binary | ios::trunc);

	if (out)
	{
		out.write((const char*)pic->data(), pic->dataSize());
	}
}

void savePictures(primo::codecs::Metadata* meta, const wchar_t* inputFile)
{
	if (!meta || meta->pictures()->count() == 0)
		return;

	wstring baseFilename(inputFile);
	baseFilename += L".pic";

	MetaPictureList* piclist = meta->pictures();
	for (int i=0; i < piclist->count(); ++i)
	{
		wostringstream picname;
		picname << baseFilename << (i+1);
		savePicture(piclist->at(i), picname.str());
	}
}

void printMetaAttributes(Metadata* meta)
{
	stdout_utf16 mode;	

	wcout << L"Metadata\r\n--------" << endl;

	MetaAttributeList* attlist = meta->attributes();
	wcout << attlist->count() << L" attributes:" << endl;

	for (int i=0; i < attlist->count(); ++i)
	{
		MetaAttribute* attrib = attlist->at(i);
		wcout << setw(15) << left << attrib->name() << L": " << attrib->value() << endl;
	}
	wcout << endl;

	MetaPictureList* piclist = meta->pictures();
	wcout << piclist->count() << L" pictures:" << endl;
	
	for (int i=0; i < piclist->count(); ++i)
	{
		MetaPicture* pic = piclist->at(i);
		wcout << L"#" << (i+1) << L" mime: " << pic->mimeType() << L"; size: " << pic->dataSize();

		wcout << L"; type: " << getMetaPictureTypeName(pic->pictureType()) << endl;
		wcout << L"description: " << pic->description() << endl;
	}
	wcout << endl;
}

void printMetadata(MediaInfo* info, const wchar_t* inputFile)
{
    Metadata* meta = NULL;
    if(info->outputs()->count() > 0)
	    meta = info->outputs()->at(0)->metadata();

	if (!meta)
	{
		wcout << L"Could not find any metadata." << endl;
		return;
	}

	printMetaAttributes(meta);
	savePictures(meta, inputFile);
}

void printError(const primo::error::ErrorInfo* e)
{
	if (primo::error::ErrorFacility::Success == e->facility())
	{
		wcout << L"Success";
	}
	else
	{
		if (e->message())
		{
			wcout << e->message() << L", ";
		}

		wcout << L"facility:" << e->facility() << L", error:" << e->code();
	}

	wcout << endl;
}

bool metaInfo(Options& opt)
{
	auto info = primo::make_ref(Library::createMediaInfo());

	info->inputs()->at(0)->setFile(opt.avfile.c_str());

	if(info->open())
	{	
		printMetadata(info.get(), opt.avfile.c_str());
		return true;
	}
	else
	{
		printError(info->error());
		return false;
	}
}

int wmain(int argc, wchar_t* argv[])
{
	Options opt;

	switch(prepareOptions( opt, argc, argv))
	{
		case Command: return 0;
		case Error: return 1;
	}

	Library::initialize();

	//Library::setLicense("<license-string>");

	bool metaInfoResult = metaInfo(opt);

	Library::shutdown();

	return metaInfoResult ? 0 : 1;
}
