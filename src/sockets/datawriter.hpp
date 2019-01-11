#ifndef DATA_WRITER_H
#define DATA_WRITER_H

#include <memory>

namespace DoubTech {
    namespace Sockets {

        class IData {
            public:
                virtual const char* data() = 0;
                virtual size_t size() = 0;
        };

        class DataReference : public IData {
            private:
                const char* buffer;
                size_t len;
            public:
                virtual const char* data() override { return buffer; }
                virtual size_t size() override { return len; }
                DataReference(const char* buffer, size_t len) : buffer(buffer), len(len) {}
        };

        class Data : public IData {
            protected:
                std::vector<char> buffer;
            public:
                void reserve(size_t size) { buffer.reserve(size); }
                virtual const char* data() override { return buffer.data(); }
                virtual size_t size() override { return buffer.size(); }
                void push_back(std::vector<char> buffer) {
                    this->buffer.insert( this->buffer.end(), buffer.begin(), buffer.end() );
                }
                void push_back(const char* buffer, size_t len) {
                    size_t offset = this->buffer.size();
                    this->buffer.reserve(this->buffer.size() + len);
                    memcpy(this->buffer.data() + offset, buffer, len);
                }
                Data() {}
                Data(const char* data, size_t len) {
                    push_back(data, len);
                }
        };

        class DataWriter {
            public:
                virtual void send(std::shared_ptr<IData> data) = 0;
        };
    }
}

#endif
