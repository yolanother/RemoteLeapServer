#ifndef BITPACK_DATA_BUFFER_H
#define BITPACK_DATA_BUFFER_H

#include <vector>
#include <memory>
#include <cmath>
#include <bitset>

namespace DoubTech {
    namespace Utils {
        class BitpackDataBuffer {
            private:
                std::vector<uint8_t> byteBuffer;
                size_t bufSize = 0;
                uint32_t bitSize = 0;

                void assertDataSize(uint32_t bitIndex, size_t bitSize);
                void bitCopyData(const uint8_t* src, uint8_t *dst, uint32_t srcBitIndex, uint32_t dstBitIndex, size_t bitCount, bool clearTargetBytes);
            public:
                void setCapacity(uint32_t bitCapacity);

                const uint8_t* data() noexcept {
                    return byteBuffer.data();
                }

                size_t size() {
                    return bufSize;
                }

                void reset() {
                    bitSize = 0;
                }

                template<typename T>
                void addData(size_t bitSize, const T* data) {
                    uint32_t bitIndex = this->bitSize;
                    setCapacity(bitIndex + bitSize);
                    setData(bitIndex, bitSize, data);
                }

                template<typename T>
                void setData(uint32_t bitIndex, size_t bitSize, const T* data) {
                    assertDataSize(bitIndex, bitSize);
                    bitCopyData(reinterpret_cast<const uint8_t *> (data), &byteBuffer[0], 0, bitIndex, bitSize, false);
                }

                template<typename T>
                T getData(uint32_t bitIndex, size_t bitSize) {
                    T data; 
                    bitCopyData(byteBuffer.data(), (uint8_t *) &data, bitIndex, 0, bitSize, true);
                    return data;
                }

                template<typename T>
                std::shared_ptr<T> getDataPointer(uint32_t bitIndex, size_t bitSize) {
                    std::shared_ptr<T> data = std::make_shared<T>;
                    bitCopyData(byteBuffer.data(), (uint8_t *) data.get(), bitIndex, 0, bitSize, true);
                    return data;
                }
        };
    }
}

#endif