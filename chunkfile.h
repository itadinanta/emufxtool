#ifndef _CHUNKFILE_H
#define _CHUNKFILE_H
#include "config.h"
#include <stdio.h>

class chunkStream {
      public:
	virtual unsigned long read(void *, unsigned long size) = 0;
	virtual unsigned long write(const void *, unsigned long size) = 0;
	virtual unsigned long pos() = 0;
	virtual unsigned long eof() = 0;
	virtual void seek(unsigned long where) = 0;
	virtual void skip(unsigned long amount) = 0;
	virtual unsigned long is_open() = 0;
};

class chunkStreamFILE:public chunkStream {
	FILE *F;
      public:
	  chunkStreamFILE(const char *filename, const char *mode) {
		F = fopen(filename, mode);
	} ~chunkStreamFILE() {
		if (F)
			fclose(F);
	}
	virtual unsigned long is_open() {
		return F != 0;
	}
	virtual unsigned long read(void *ptr, unsigned long size) {
		return fread(ptr, sizeof(char), size, F);
	}
	virtual unsigned long write(const void *ptr, unsigned long size) {
		return fwrite(ptr, sizeof(char), size, F);
	}
	virtual unsigned long pos() {
		return ftell(F);
	}
	virtual unsigned long eof() {
		return feof(F);
	}
	virtual void seek(unsigned long where) {
		fseek(F, where, SEEK_SET);
	}
	virtual void skip(unsigned long amount) {
		fseek(F, amount, SEEK_CUR);
	}
};

class chunkArchive {
	chunkStream *F;
	unsigned long status_bits;
	unsigned short current_chunk_id;
	unsigned long current_chunk_start;
	unsigned long current_chunk_end;
	enum status_bits { bTAG, bCHUNK };
      public:
	enum atoms { CHUNK_BYTE, CHUNK_SHORT, CHUNK_LONG, CHUNK_STRING,
			CHUNK_BINARY };
	static const unsigned short EOC = 0xffff;
	  chunkArchive(chunkStream * f);
	 ~chunkArchive();
	void detach();
	unsigned long readBlock(void *, unsigned long size);
	char *readBinary(unsigned long *size = 0);
	unsigned long readLong();
	unsigned short readShort();
	unsigned char readByte();
	char *readString(unsigned long *size = 0);
	unsigned long readAny(unsigned char *type = 0, unsigned long *size = 0);
	void tag_next();
	unsigned long writeBlock(const char *, unsigned long size);
	void writeLong(unsigned long);
	void writeByte(unsigned char);
	void writeShort(unsigned short);
	void writeString(const char *);
	void writeBinary(const char *, unsigned long size);
	void newStream(unsigned long magic);
	int validateStream(unsigned long magic);
	void closeStream();
	void newChunk(unsigned short id);
	void closeChunk();

	unsigned short nextChunk();
	void skipChunk();
	unsigned long beginChunk();
	void ignore();
	void endChunk();
	static void freeBlock(void *);
      public:
	void do_tagging(enum atoms TAG);
};

#endif
