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

    int handIndex = 0;
    int leftHandIndex = -1;
    int rightHandIndex = -1;

    long max = 0;
    for(auto hand : frame.hands()) {
        if(hand.isLeft()) {
            leftHandIndex = handIndex++;
            hands |= LeftHand;
            leftPalm = hand.palmPosition();
        } else {
            rightHandIndex = handIndex++;
            hands |= RightHand;
            rightPalm = hand.palmPosition();
        }
        max = std::max(max, maxv(hand.palmPosition(), hand.wristPosition()));
        for(Finger finger : hand.fingers()) {
            for(int b = 0; b < 4; ++b) {
                Bone bone = finger.bone(static_cast<Bone::Type>(b));
                Vector prev = b > 0 ? bone.prevJoint() : hand.palmPosition();
                max = std::max(max, maxv(prev, bone.nextJoint()));
            }
        }
    }

    int boneCoordSize = getBucketSize(max);
    int palmCoordSize = getBucketSize(PrecisionValues[precision] * 500) + 1;

    buffer.reset();
    buffer.addData(format.activeHands, &hands);
    buffer.addData(format.precision, &precision);
    buffer.addData(format.rootPositionBucket, &palmCoordSize);
    buffer.addData(format.boneBucketSize, &boneCoordSize);
    if((hands & LeftHand) > 0) {
        encodeCoordinate(leftPalm.x, palmCoordSize, true);
        encodeCoordinate(leftPalm.y, palmCoordSize, true);
        encodeCoordinate(leftPalm.z, palmCoordSize, true);
        encodeHand(frame.hands()[leftHandIndex], boneCoordSize);
    }
    if((hands & RightHand) > 0) {
        encodeCoordinate(rightPalm.x, palmCoordSize, true);
        encodeCoordinate(rightPalm.y, palmCoordSize, true);
        encodeCoordinate(rightPalm.z, palmCoordSize, true);
        encodeHand(frame.hands()[rightHandIndex], boneCoordSize);
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