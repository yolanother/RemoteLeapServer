#ifndef HAND_TRANSFORM_H
#define HAND_TRANSFORM_H

#include "BaseData.hpp"
#include "../utilities/bitpackdata/bitpackdatabuffer.hpp"
#include "Leap.h"

#include <math.h>
#include <cmath>
#include <iostream>

namespace DoubTech {
    namespace RemoteLeap {

        enum PositionBuckets {
            BucketMin = 0,
            Bucket_8 = 0,
            Bucket_10,
            Bucket_13,
            Bucket_16,
            Bucket_19,
            Bucket_20,
            Bucket_24,
            Bucket_32,
            BucketMax
        };

        enum Precision {
            TinyPrecision,
            LowPrecision,
            MediumPrecision,
            HighPrecision,
            PrecisionMax
        };

        struct HandDataFormat {
            const size_t flags = 8;
            const size_t activeHands = 2;
            const size_t precision = 2;
            const size_t rootPositionBucket = 3;
            const size_t boneBucketSize = 3;
            const size_t boneDirectionSize = 3;
            const size_t rotation = 32;
        };

        enum Hands {
            LeftHand = 0x1,
            RightHand = 0x2
        };

        class HandTransform : public BaseData {
            private:
                DoubTech::Utils::BitpackDataBuffer buffer;


                const uint PositionBucketSizes[BucketMax] {
                    8,
                    10,
                    13,
                    16,
                    19,
                    20,
                    24,
                    32
                };

                const uint PrecisionValues[PrecisionMax] {
                    100,
                    1000,
                    10000,
                    100000
                };

                Precision precision = Precision::LowPrecision;

                inline long ftol(float value) {
                    return PrecisionValues[precision] * value;
                }

                inline long maxv(const Leap::Vector& vector, long max) {
                    max = std::max(max, abs((long) vector.x));
                    max = std::max(max, abs((long) vector.y));
                    max = std::max(max, abs((long) vector.z));
                    return max;
                }
                
                inline long maxv(const Leap::Vector& source, const Leap::Vector& dest) {
                    long max = abs(ftol(dest.x) - ftol(source.x));
                    max = std::max(abs(ftol(dest.y) - ftol(source.y)), max);
                    max = std::max(abs(ftol(dest.z) - ftol(source.z)), max);
                    return max;
                }

                inline void encodeHand(const Leap::Hand& hand, int boneCoordSize, int boneDirectionSize) {
                    for(Leap::Finger finger : hand.fingers()) {
                        encodeFinger(hand, finger, boneCoordSize, boneDirectionSize);
                    }
                }

                inline void encodeFinger(const Leap::Hand& hand, const Leap::Finger &finger, int boneCoordSize, int boneDirectionSize) {
                    std::cout << std::endl;
                    for(int b = 0; b < 4; ++b) {
                        Leap::Bone bone = finger.bone(static_cast<Leap::Bone::Type>(b));
                        Leap::Vector joint = bone.nextJoint() - (b > 0 ? bone.prevJoint() : hand.palmPosition());
                        
                        encodeCoordinate(joint.x, boneCoordSize, true);
                        encodeCoordinate(joint.y, boneCoordSize, true);
                        encodeCoordinate(joint.z, boneCoordSize, true);

                        encodeDirection(bone.direction(), boneDirectionSize);
                    } std::cout << std::endl;
                }

                inline void encodeDirection(Leap::Vector direction, int boneDirectionSize) {
                    direction = direction * Leap::RAD_TO_DEG * PrecisionValues[precision];
                    std::cout << (long) direction.x << ',' << (long) direction.y << ',' << (long) direction.z << std::endl;
                    encodeCoordinate(direction.x, boneDirectionSize, true);
                    encodeCoordinate(direction.y, boneDirectionSize, true);
                    encodeCoordinate(direction.z, boneDirectionSize, true);
                }

                inline void encodeCoordinate(const float &coord, int boneCoordSize, bool debug) {
                    bool isNegative = coord > 0;
                    long position = abs(ftol(coord));
                    boneCoordSize = PositionBucketSizes[boneCoordSize];
                    if(debug) std::cout << " " << position / PrecisionValues[precision] << " (" << (int) (log2(position) + 1) << "/" << boneCoordSize << ") " << std::bitset<19>(position) << "  ";
                    buffer.addData(1, &isNegative);
                    buffer.addData(boneCoordSize, &position);
                }

                inline PositionBuckets getBucketSize(long max) {
                    int container = max > 0 ? log2(max) + 1 : 0;
                    PositionBuckets coordBucketSize = BucketMax;
                    for(int i = BucketMin; i < BucketMax; i++) {
                        if(container < PositionBucketSizes[i]) {
                            coordBucketSize = static_cast<PositionBuckets>(i);
                            break;
                        }
                    }
                    return coordBucketSize;
                }

                void debugHeader();
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
                    writer.send(std::make_shared<DoubTech::Sockets::DataReference>(
                        reinterpret_cast<const char*>(buffer.data()),
                        buffer.size()
                    ));
                }
        };
    }
}

#endif