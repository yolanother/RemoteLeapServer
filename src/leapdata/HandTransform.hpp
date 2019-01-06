#ifndef HAND_TRANSFORM_H
#define HAND_TRANSFORM_H

#include "BaseData.hpp"
#include "../utilities/bitpackdata/bitpackdatabuffer.hpp"
#include "Leap.h"

#include <math.h>
#include <cmath>

namespace DoubTech {
    namespace RemoteLeap {

        enum PositionBuckets {
            TinyBucket,
            SmallBucket,
            MediumBucket,
            LargeBucket,
            HugeBucket
        };

        enum Precision {
            TinyPrecision,
            LowPrecision,
            MediumPrecision,
            HighPrecision
        };

        struct HandDataFormat {
            const size_t flags = 8;
            const size_t activeHands = 2;
            const size_t precision = 2;
            const size_t positionBucket = 2;
            const size_t coordBucket = 2;
            const size_t rotation = 32;
        };

        enum Hands {
            LeftHand = 0x1,
            RightHand = 0x2
        };

        class HandTransform : public BaseData {
            private:
                DoubTech::Utils::BitpackDataBuffer buffer;


                const uint PositionBucketSizes[5] {
                    13,
                    16,
                    19,
                    20,
                    32
                };

                const uint PrecisionValues[4] {
                    100,
                    1000,
                    10000,
                    100000
                };

                Precision precision = Precision::LowPrecision;
                PositionBuckets positionBucket = PositionBuckets::MediumBucket;

                inline long ftol(float value) {
                    return PrecisionValues[precision] * value;
                }

                inline void addCoords(std::vector<uint32_t> &coords, Leap::Vector position) {
                    coords.push_back(ftol(position.x));
                    coords.push_back(ftol(position.y));
                    coords.push_back(ftol(position.z));
                }
                
                inline long maxv(const Leap::Vector& source, const Leap::Vector& dest) {
                    long max = abs(ftol(dest.x) - ftol(source.x));
                    max = std::max(abs(ftol(dest.y) - ftol(source.y)), max);
                    max = std::max(abs(ftol(dest.z) - ftol(source.z)), max);
                    return max;
                }
            protected:
                virtual const uint8_t* onGetDataBuffer() override {
                    return buffer.data();
                }
                virtual DataTypes onGetDataType() override {
                    return TypeHandTransform;
                }
            public:
                void encode(Leap::Frame frame);
                void setPrecision(Precision precision) { this->precision = precision; }
                virtual void writeData(DoubTech::Sockets::DataWriter& writer) override {
                    writer.send(DoubTech::Sockets::Data{
                        reinterpret_cast<const char*>(buffer.data()),
                        buffer.size()
                    });
                }
        };
    }
}

#endif