#include <chunkfile.h>
#include <stdlib.h>
#include <stdexcept>

#define EXCEP(msg) {throw std::runtime_error(msg);}
#define VALIDF if (!F || !F->is_open()) EXCEP("file not open");

#define TEST_BIT(r,bit) ((r) & (1L<<(bit)))
#define SET_BIT(r,bit) ((r) |= (1L<<(bit)))
#define CLEAR_BIT(r,bit) ((r) &=~ (1L<<(bit)))

#define CHNK_MAGIC 0x4b4e4843L

chunkArchive::chunkArchive(chunkStream * f)
{
	F = f;
	current_chunk_start = 0;
	current_chunk_end = 0;
	current_chunk_id = 0;
	status_bits = 0;
}

chunkArchive::~chunkArchive()
{
	if (F)
		delete F;
}

void
  chunkArchive::freeBlock(void *c)
{
	free(c);
}

void chunkArchive::detach()
{
	F = 0;
}

unsigned long chunkArchive::readBlock(void *data, unsigned long size)
{
	VALIDF;
	unsigned long nread = F->read(data, size);

	if (nread != size) {
		EXCEP("trying to read beyond end of stream");
	}
	return nread;
}

char *chunkArchive::readBinary(unsigned long *size)
{
	unsigned long BlockSize = readLong();
	if (size)
		*size = BlockSize;
	char *chunk = (char *) malloc(BlockSize);
	if (!chunk)
		EXCEP("out of memory");
	readBlock((void *) chunk, BlockSize);
	return chunk;
}

unsigned long chunkArchive::readLong()
{
	unsigned char b[4];
	readBlock(b, 4);
	return (unsigned long) b[0] + ((unsigned long) b[1] << 8) +
		((unsigned long) b[2] << 16) + ((unsigned long) b[3] << 24);
}

unsigned short chunkArchive::readShort()
{
	unsigned char b[2];
	readBlock(b, 2);
	return (unsigned short) b[0] + ((unsigned short) b[1] << 8);
}

unsigned char chunkArchive::readByte()
{
	unsigned char c;
	readBlock(&c, 1);
	return c;
}

char *chunkArchive::readString(unsigned long *size)
{
	return readBinary(size);
}

unsigned long chunkArchive::readAny(unsigned char *type, unsigned long *size)
{
	unsigned char id = readByte();
	if (type)
		*type = id;
	switch ((atoms) (id)) {
	case CHUNK_BYTE:
		if (size)
			*size = 1;
		return readByte();
		break;
	case CHUNK_SHORT:
		if (size)
			*size = 2;
		return readShort();
		break;
	case CHUNK_LONG:
		if (size)
			*size = 4;
		return readLong();
		break;
	case CHUNK_STRING:
		return (unsigned long) readString(size);
		break;
	case CHUNK_BINARY:
		return (unsigned long) readBinary(size);
		break;
	default:
		if (size)
			*size = 0;
		return 0;
	}
}

unsigned long chunkArchive::writeBlock(const char *data, unsigned long size)
{
	VALIDF;
	return F->write(data, size);
}

void chunkArchive::writeBinary(const char *data, unsigned long size)
{
	do_tagging(CHUNK_BINARY);
	writeLong(size);
	writeBlock(data, size);
}

void chunkArchive::writeByte(unsigned char c)
{
	do_tagging(CHUNK_BYTE);
	writeBlock((const char *) &c, 1);
}

void chunkArchive::writeLong(unsigned long l)
{
	do_tagging(CHUNK_LONG);
	unsigned char b[4];
	b[0] = (l & 0xffL);
	l >>= 8;
	b[1] = (l & 0xffL);
	l >>= 8;
	b[2] = (l & 0xffL);
	l >>= 8;
	b[3] = (l & 0xffL);
	writeBlock((const char *) b, 4);
}

void chunkArchive::writeShort(unsigned short l)
{
	do_tagging(CHUNK_SHORT);
	unsigned char b[2];
	b[0] = (l & 0xff);
	l >>= 8;
	b[1] = (l & 0xff);
	writeBlock((const char *) b, 2);
}

void chunkArchive::writeString(const char *s)
{
	do_tagging(CHUNK_STRING);
	unsigned long size = strlen(s) + 1;
	writeLong(size);
	writeBlock((const char *) s, size);
}

void chunkArchive::newStream(unsigned long magic)
{
	VALIDF;
	F->seek(0);
	writeLong(CHNK_MAGIC);	// CHNK
	writeLong(magic);
}

int chunkArchive::validateStream(unsigned long magic)
{
	VALIDF;
	F->seek(0);
	return (readLong() == CHNK_MAGIC) && (readLong() == magic);
}

void chunkArchive::closeStream()
{
	newChunk(EOC);		// Last Chunk
	closeChunk();
}

void chunkArchive::newChunk(unsigned short id)
{
	if (TEST_BIT(status_bits, bCHUNK))
		closeChunk();
	current_chunk_id = id;
	writeShort(id);
	writeLong(0);
	current_chunk_start = F->pos();
	SET_BIT(status_bits, bCHUNK);
}

void chunkArchive::do_tagging(enum atoms TAG)
{
	if (!TEST_BIT(status_bits, bTAG))
		return;
	CLEAR_BIT(status_bits, bTAG);
	writeBlock((const char *) &TAG, 1);
}

void chunkArchive::tag_next()
{
	SET_BIT(status_bits, bTAG);
}

void chunkArchive::closeChunk()
{
	VALIDF;
	current_chunk_end = F->pos();
	F->seek(current_chunk_start - sizeof(unsigned long));
	writeLong(current_chunk_end - current_chunk_start);
	F->seek(current_chunk_end);
	CLEAR_BIT(status_bits, bCHUNK);
	current_chunk_id = 0;
}

unsigned short chunkArchive::nextChunk()
{
	return readShort();
}

void chunkArchive::skipChunk()
{
	VALIDF;
	F->skip(readLong());
}

unsigned long chunkArchive::beginChunk()
{
	VALIDF;
	unsigned long len = readLong();
	SET_BIT(status_bits, bCHUNK);
	current_chunk_start = F->pos();
	current_chunk_end = current_chunk_start + len;
	return len;
}

void chunkArchive::endChunk()
{
	VALIDF;
	if (TEST_BIT(status_bits, bCHUNK)) {
		F->seek(current_chunk_end);
		CLEAR_BIT(status_bits, bCHUNK);
	}
}

void chunkArchive::ignore()
{
	VALIDF;
	if (TEST_BIT(status_bits, bCHUNK)) {
		F->seek(current_chunk_end);
		CLEAR_BIT(status_bits, bCHUNK);
	}
}
