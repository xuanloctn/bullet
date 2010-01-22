/*
bParse
Copyright (c) 2006-2010 Erwin Coumans  http://gamekit.googlecode.com

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it freely,
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include "btBulletFile.h"
#include "bDefines.h"
#include "bDNA.h"
#include <string.h>


// 32 && 64 bit versions
extern unsigned char sBulletDNAstr[];
extern int sBulletDNAlen;

//not yetto. extern unsigned char DNAstr64[];
//not yetto. extern int DNAlen64;


using namespace bParse;

btBulletFile::btBulletFile()
:bFile("", "BULLET ")
{
	mMemoryDNA = new bDNA();
	mMemoryDNA->init((char*)sBulletDNAstr,sBulletDNAlen);
}


btBulletFile::btBulletFile(const char* fileName)
:bFile(fileName, "BULLET ")
{
}



btBulletFile::btBulletFile(char *memoryBuffer, int len)
:bFile(memoryBuffer,len, "BULLET ")
{

}


btBulletFile::~btBulletFile()
{

}



// ----------------------------------------------------- //
void btBulletFile::parseData()
{
	printf ("Building datablocks");
	printf ("Chunk size = %d",CHUNK_HEADER_LEN);
	printf ("File chunk size = %d",ChunkUtils::getOffset(mFlags));

	const bool swap = (mFlags&FD_ENDIAN_SWAP)!=0;
	

	mDataStart = 12;

	char *dataPtr = mFileBuffer+mDataStart;

	bChunkInd dataChunk;
	dataChunk.code = 0;


	//dataPtr += ChunkUtils::getNextBlock(&dataChunk, dataPtr, mFlags);
	int seek = ChunkUtils::getNextBlock(&dataChunk, dataPtr, mFlags);
	//dataPtr += ChunkUtils::getOffset(mFlags);
	char *dataPtrHead = 0;

	while (dataChunk.code != DNA1)
	{
		



		// one behind
		if (dataChunk.code == SDNA) break;
		//if (dataChunk.code == DNA1) break;

		// same as (BHEAD+DATA dependancy)
		dataPtrHead = dataPtr+ChunkUtils::getOffset(mFlags);
		char *id = readStruct(dataPtrHead, dataChunk);

		// lookup maps
		if (id)
		{
			mLibPointers.insert(dataChunk.oldPtr, (bStructHandle*)id);

			m_chunks.push_back(dataChunk);
			// block it
			//bListBasePtr *listID = mMain->getListBasePtr(dataChunk.code);
			//if (listID)
			//	listID->push_back((bStructHandle*)id);
		}

		if (dataChunk.code == BT_RIGIDBODY_CODE)
		{
			m_rigidBodies.push_back((bStructHandle*) id);
		}
		
		if (dataChunk.code == BT_COLLISIONOBJECT_CODE)
		{
			m_collisionObjects.push_back((bStructHandle*) id);
		}

		if (dataChunk.code == BT_SHAPE_CODE)
		{
			m_collisionShapes.push_back((bStructHandle*) id);
		}

//		if (dataChunk.code == GLOB)
//		{
//			m_glob = (bStructHandle*) id;
//		}

		// next please!
		dataPtr += seek;

		seek =  ChunkUtils::getNextBlock(&dataChunk, dataPtr, mFlags);
		if (seek < 0)
			break;
	}

}

void	btBulletFile::addDataBlock(char* dataBlock)
{
	//mMain->addDatablock(dataBlock);
}




void	btBulletFile::writeDNA(FILE* fp)
{

	bChunkInd dataChunk;
	dataChunk.code = DNA1;
	dataChunk.dna_nr = 0;
	dataChunk.nr = 1;
	
	if (VOID_IS_8)
	{
		//dataChunk.len = DNAlen64;
		//dataChunk.oldPtr = DNAstr64;
		//fwrite(&dataChunk,sizeof(bChunkInd),1,fp);
		//fwrite(DNAstr64, DNAlen64,1,fp);
	}
	else
	{
		dataChunk.len = sBulletDNAlen;
		dataChunk.oldPtr = sBulletDNAstr;
		fwrite(&dataChunk,sizeof(bChunkInd),1,fp);
		fwrite(sBulletDNAstr, sBulletDNAlen,1,fp);
	}
}


void	btBulletFile::parse(bool verboseDumpAllTypes)
{
	if (VOID_IS_8)
	{
		exit(0);
		//parseInternal(verboseDumpAllTypes,(char*)DNAstr64,DNAlen64);
	}
	else
	{
		parseInternal(verboseDumpAllTypes,(char*)sBulletDNAstr,sBulletDNAlen);
	}
}

// experimental
int		btBulletFile::write(const char* fileName, bool fixupPointers)
{
	FILE *fp = fopen(fileName, "wb");
	if (fp)
	{
		char header[SIZEOFBLENDERHEADER] ;
		memcpy(header, m_headerString, 7);
		int endian= 1;
		endian= ((char*)&endian)[0];

		if (endian)
		{
			header[7] = '_';
		} else
		{
			header[7] = '-';
		}
		if (VOID_IS_8)
		{
			header[8]='V';
		} else
		{
			header[8]='v';
		}

		header[9] = '2';
		header[10] = '7';
		header[11] = '5';
		
		fwrite(header,SIZEOFBLENDERHEADER,1,fp);

		writeChunks(fp, fixupPointers);

		writeDNA(fp);

		fclose(fp);
		
	} else
	{
		printf("Error: cannot open file %s for writing\n",fileName);
		return 0;
	}
	return 1;
}



void	btBulletFile::addStruct(const	char* structType,void* data, int len, void* oldPtr, int code)
{
	
	bParse::bChunkInd dataChunk;
	dataChunk.code = code;
	dataChunk.nr = 1;
	dataChunk.len = len;
	dataChunk.dna_nr = mMemoryDNA->getReverseType(structType);
	dataChunk.oldPtr = oldPtr;

	///Perform structure size validation
	short* structInfo= mMemoryDNA->getStruct(dataChunk.dna_nr);
	int elemBytes = mMemoryDNA->getLength(structInfo[0]);
//	int elemBytes = mMemoryDNA->getElementSize(structInfo[0],structInfo[1]);
	assert(len==elemBytes);

	mLibPointers.insert(dataChunk.oldPtr, (bStructHandle*)data);
	m_chunks.push_back(dataChunk);
}