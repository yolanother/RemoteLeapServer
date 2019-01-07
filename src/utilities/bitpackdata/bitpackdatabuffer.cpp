#include "bitpackdatabuffer.hpp"
#include "../StringUtils.hpp"
#include "../logging.hpp"

#include <algorithm>

#define TAG "BitPackDataBuffer"

using namespace DoubTech::Utils;

#define bitmask(nbits)  ((1<<(nbits))-1) /* a mask of nbits 1's */
#define getbits(x,bit1,nbits) (((x)>>(bit1)) & (bitmask(nbits)))
#define setbits(x,bit1,nbits,val)   /* x:bit1...bit1+nbits-1 = val */ \
        if ( (nbits)>0 && (bit1)>=0 ) { /* check input */ \
          (x) &=      (~((bitmask((nbits))) << (bit1))); /*set field=0's*/ \
          (x) |= (((val)&(bitmask((nbits)))) << (bit1)); /*set field=val*/ \
          } else                        /* let user supply final ; */

void BitpackDataBuffer::setCapacity(uint32_t bitCapacity) {
    bitSize = bitCapacity;
    uint32_t bufSize = std::ceil(bitCapacity / 8.0f);
    if(byteBuffer.size() < bufSize) {
        byteBuffer.resize(bufSize, 0);
    } else {
        // TODO: Fix cases where buffer is partially bigger and not full buff gets reset
        // by resize.
        for(int i = this->bufSize + 1; i < bufSize; i++) {
            byteBuffer[i] = 0;
        }
    }
    this->bufSize = bufSize;
}

void BitpackDataBuffer::bitCopyData(const uint8_t* src, uint8_t *dst, uint32_t srcBitIndex, uint32_t dstBitIndex, size_t bitCount, bool clearTargetBytes) {
    size_t stored = 0;
    src += (int) (srcBitIndex / 8.0f);
    dst += (int) (dstBitIndex / 8.0f);
    srcBitIndex = srcBitIndex % 8;
    dstBitIndex = dstBitIndex % 8;

    if(clearTargetBytes) *dst = 0;

    while(stored < bitCount) {
        uint8_t srcRemain = 8 - srcBitIndex;
        uint8_t dstRemain = 8 - dstBitIndex;
        uint8_t toStore = std::min<int>(bitCount - stored, std::min(srcRemain, dstRemain));

        setbits(*dst, dstBitIndex, toStore, getbits(*src, srcBitIndex, toStore));
        srcBitIndex += toStore;
        dstBitIndex += toStore;
        stored += toStore;

        if(stored < bitCount) {
            if(dstBitIndex == 8) {
                dst++;
                if(clearTargetBytes) *dst = 0;
                dstBitIndex = 0;
            }

            if(srcBitIndex == 8) {
                src++;
                srcBitIndex = 0;
            }
        }
    }
}

void BitpackDataBuffer::assertDataSize(uint32_t bitIndex, size_t bitSize) {
    int byteSize = std::ceil((bitIndex + bitSize) / 8.0f);
    if(byteSize > byteBuffer.size()) {
        throwDomainError("Data will not fit in CurrentBitpackDataBuffer: ", byteSize, " > ", byteBuffer.size());
    }
}