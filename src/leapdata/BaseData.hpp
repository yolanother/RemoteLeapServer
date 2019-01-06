#ifndef BASE_DATA_H
#define BASE_DATA_H

#include <vector>
#include <memory>
#include <cmath>

#include "../sockets/datawriter.hpp"

namespace DoubTech {
    namespace RemoteLeap {
        enum DataTypes {
            TypeUnknown,
            TypeHandTransform,
            TypeGestureData
        };

        class BaseData {
            protected:
                virtual const uint8_t* onGetDataBuffer() = 0;
                virtual DataTypes onGetDataType() = 0;
            public:
                virtual void writeData(DoubTech::Sockets::DataWriter& writer) = 0;

        };
    }
}

#endif