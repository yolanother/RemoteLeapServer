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
    
    Vector leftPalm;
    Vector rightPalm;
    Vector leftPalmDirection;
    Vector rightPalmDirection;

    int handIndex = 0;
    int leftHandIndex = -1;
    int rightHandIndex = -1;

    long max = 0;
    long maxDirection = 0;
    for(auto hand : frame.hands()) {
        if(hand.isLeft()) {
            leftHandIndex = handIndex++;
            hands |= LeftHand;
            leftPalm = hand.palmPosition();
            leftPalmDirection = hand.direction() * RAD_TO_DEG * PrecisionValues[precision];
            maxv(leftPalmDirection, maxDirection);
        } else {
            rightHandIndex = handIndex++;
            hands |= RightHand;
            rightPalm = hand.palmPosition();
            rightPalmDirection = hand.direction() * RAD_TO_DEG * PrecisionValues[precision];
            maxv(rightPalmDirection, maxDirection);
        }
        max = std::max(max, maxv(hand.palmPosition(), hand.wristPosition()));
        for(Finger finger : hand.fingers()) {
            for(int b = 0; b < 4; ++b) {
                Bone bone = finger.bone(static_cast<Bone::Type>(b));
                Vector prev = b > 0 ? bone.prevJoint() : hand.palmPosition();
                max = std::max(max, maxv(prev, bone.nextJoint()));
                Leap::Vector direction = bone.direction() * Leap::RAD_TO_DEG * PrecisionValues[precision];
                maxDirection = maxv(direction, maxDirection);
            }
        }
    }

    int boneCoordSize = getBucketSize(max);
    int boneDirectionSize = getBucketSize(maxDirection);
    int palmCoordSize = getBucketSize(PrecisionValues[precision] * 500) + 1;

    buffer.reset();
    buffer.addData(format.activeHands, &hands);
    buffer.addData(format.precision, &precision);
    buffer.addData(format.rootPositionBucket, &palmCoordSize);
    buffer.addData(format.boneBucketSize, &boneCoordSize);
    buffer.addData(format.boneDirectionSize, &boneDirectionSize);
    if((hands & LeftHand) > 0) {
        encodeCoordinate(leftPalm.x, palmCoordSize, true);
        encodeCoordinate(leftPalm.y, palmCoordSize, true);
        encodeCoordinate(leftPalm.z, palmCoordSize, true);
        encodeCoordinate(leftPalmDirection.x, boneDirectionSize, true);
        encodeCoordinate(leftPalmDirection.y, boneDirectionSize, true);
        encodeCoordinate(leftPalmDirection.z, boneDirectionSize, true);
        encodeHand(frame.hands()[leftHandIndex], boneCoordSize, boneDirectionSize);
    }
    if((hands & RightHand) > 0) {
        encodeCoordinate(rightPalm.x, palmCoordSize, true);
        encodeCoordinate(rightPalm.y, palmCoordSize, true);
        encodeCoordinate(rightPalm.z, palmCoordSize, true);
        encodeCoordinate(rightPalmDirection.x, boneDirectionSize, true);
        encodeCoordinate(rightPalmDirection.y, boneDirectionSize, true);
        encodeCoordinate(rightPalmDirection.z, boneDirectionSize, true);
        encodeHand(frame.hands()[rightHandIndex], boneCoordSize, boneDirectionSize);
    }
    
    std::cout << std::endl;
    logd("Active Hands: ", (int) hands, ", Precision: ", (int) precision, " palmCoordSize: ", (int) palmCoordSize, ", boneCoordSize: ", (int) boneCoordSize);
    
    logd("Data capacity: ", buffer.size());
    debugHeader();
}

void HandTransform::debugHeader() {
    std::cout << "Header: ";
    for(int i = 0; i < 4; i++) {
        uint8_t data = buffer.data()[i];
        std::bitset<8> x(data);
        std::cout << x << " ";
    }
    std::cout << std::endl;
}