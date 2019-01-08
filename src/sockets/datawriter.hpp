#ifndef DATA_WRITER_H
#define DATA_WRITER_H

#include <memory>

namespace DoubTech {
    namespace Sockets {

        class Data {
            private:
                const char* m_data;
                size_t m_length;
            public:
                Data(const char* data, size_t length) :
                    m_data(data), m_length(length) {}
                virtual const char* data() { return m_data; }
                virtual size_t length() { return m_length; }
        };

        class DataWriter {
            public:
                virtual void send(Data data) = 0;
        };
    }
}

#endif
