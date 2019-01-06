#include "HandTransform.hpp"

#include "../utilities/logging.hpp"

#include "Leap.h"

#include <cmath>
#include <math.h>

using namespace DoubTech::RemoteLeap;
using namespace DoubTech::Utils;
using namespace Leap;

#define TAG "HandTransform"

/**
 * Data format:
 * activeHands (2 bits): flag (1: left, 2: right)
 * positionBucket
 */


void HandTransform::encode(Frame frame) {
    HandDataFormat format;

    uint8_t hands = 0;
    
    std::vector<uint32_t> coords;
    Vector leftPalm;
    Vector rightPalm;

    long max = 0;
    for(auto hand : frame.hands()) {
        if(hand.isLeft()) {
            hands |= LeftHand;
            leftPalm = hand.palmPosition();
        } else {
            hands |= RightHand;
            rightPalm = hand.palmPosition();
        }
        max = std::max(max, maxv(hand.palmPosition(), hand.wristPosition()));
        addCoords(coords, hand.wristPosition() - hand.palmPosition());
        for(Finger finger : hand.fingers()) {
            for(int b = 0; b < 4; ++b) {
                Bone bone = finger.bone(static_cast<Bone::Type>(b));
                Vector prev = b > 0 ? bone.prevJoint() : hand.palmPosition();
                max = std::max(max, maxv(prev, bone.nextJoint()));
                addCoords(coords, bone.nextJoint() - prev);
            }
        }
    }

    logd("Hand max vector: ", max);
    int container = max > 0 ? log2(max) + 1 : 0;
    logd("Bit container size: ", container);

    PositionBuckets coordBucketSize = HugeBucket;
    for(int i = TinyBucket; i < HugeBucket; i++) {
        if(container < PositionBucketSizes[i]) {
            coordBucketSize = static_cast<PositionBuckets>(i);
            break;
        }
    }

    int boneCoordSize = PositionBucketSizes[positionBucket];
    int palmCoordSize = log2(PrecisionValues[precision] * 500) + 1;

    buffer.reset();
    buffer.addData(format.activeHands, &hands);
    buffer.addData(format.precision, &precision);
    buffer.addData(format.coordBucket, &coordBucketSize);
    buffer.addData(format.positionBucket, &positionBucket);
    if((hands & LeftHand) > 0) {
        long coord = ftol(leftPalm.x);
        buffer.addData(palmCoordSize, &coord);
        coord = ftol(leftPalm.y);
        buffer.addData(palmCoordSize, &coord);
        coord = ftol(leftPalm.z);
        buffer.addData(palmCoordSize, &coord);
    }
    if((hands & RightHand) > 0) {
        long coord = ftol(rightPalm.x);
        buffer.addData(palmCoordSize, &coord);
        coord = ftol(rightPalm.y);
        buffer.addData(palmCoordSize, &coord);
        coord = ftol(rightPalm.z);
    }
    for(auto coord : coords) {
        buffer.addData(boneCoordSize, &coord);
    }
    logd("Data capacity: ", buffer.size());
}