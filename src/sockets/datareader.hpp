#ifndef DATA_READER_H
#define DATA_READER_H

namespace DoubTech {
    namespace Sockets {
        class DataReader {
            public:
                virtual void onReceive(std::shared_ptr<Data> data) = 0;
        };
    }
}

#endif
