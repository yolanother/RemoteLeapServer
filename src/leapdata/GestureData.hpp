#ifndef GESTURE_DATA_H
#define GESTURE_DATA_H

namespace DoubTech {
    namespace RemoteLeap {
        enum Gesture {
            Unknown,
            Circle,
            Swipe,
            Tap
        }

        struct GestureData {
            unsigned char gestureType : 2;
            unsigned int positionX : 18;
            unsigned int positionY : 18;
            unsigned int positionZ : 18;
            unsigned int rotationX : 18;
            unsigned int rotationY : 18;
            unsigned int rotationZ : 18;
        }
    }
}



#endif