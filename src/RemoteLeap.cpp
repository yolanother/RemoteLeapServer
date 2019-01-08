#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>

#include "Leap.h"
#include "sockets/tcpserver.hpp"
#include "sockets/udpserver.hpp"
#include "utilities/logging.hpp"
#include "leapdata/HandTransform.hpp"

#include "utilities/bitpackdata/bitpackdatabuffer.hpp"
using namespace Leap;
using namespace DoubTech::Sockets;
using namespace DoubTech::Utils;
using namespace DoubTech::RemoteLeap;

#define TAG "RemoteLeap"

class RemoteLeapServer : public Listener
{
    public:
        virtual void onInit(const Controller &);
        virtual void onConnect(const Controller &);
        virtual void onDisconnect(const Controller &);
        virtual void onExit(const Controller &);
        virtual void onFrame(const Controller &);
        virtual void onFocusGained(const Controller &);
        virtual void onFocusLost(const Controller &);
        virtual void onDeviceChange(const Controller &);
        virtual void onServiceConnect(const Controller &);
        virtual void onServiceDisconnect(const Controller &);

        void createServer(int port, bool udp) {
            if(udp) {
                logd("Starting UDP server...");
                server = std::make_shared<UdpServer>(port);
            } else {
                logd("Starting TCP server on port ", port, "...");
                server = std::make_shared<TcpServer>(port);
            }
            server->start();
        }

        void setLoggingEnabled(bool enabled) {
            loggingEnabled = enabled;
        }

        virtual ~RemoteLeapServer() {
            if(server) server->stop();
        }

    private:
        HandTransform transform;
        std::shared_ptr<BaseServer> server;
        bool loggingEnabled;
};

const std::string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const std::string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const std::string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

void RemoteLeapServer::onInit(const Controller &controller)
{
    logd("Leap Service Initialized");
}

void RemoteLeapServer::onConnect(const Controller &controller)
{
    logd("Leap service Connected");
    controller.enableGesture(Gesture::TYPE_CIRCLE);
    controller.enableGesture(Gesture::TYPE_KEY_TAP);
    controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
    controller.enableGesture(Gesture::TYPE_SWIPE);
}

void RemoteLeapServer::onDisconnect(const Controller &controller)
{
    // Note: not dispatched when running in a debugger.
    logd("Leap service Disconnected");
}

void RemoteLeapServer::onExit(const Controller &controller)
{
    logd("Exited");
}

void loghand(const Frame &frame)
{
    for (Hand hand : frame.hands())
    {
        std::cout << hand.palmPosition() << "," << hand.wristPosition() - hand.palmPosition() << std::endl;

        // Get fingers
        for (Finger finger : hand.fingers())
        {
            for (int b = 0; b < 4; ++b)
            {
                Bone::Type boneType = static_cast<Bone::Type>(b);
                Bone bone = finger.bone(boneType);
                std::cout << std::string(3, ' ') << (hand.isLeft() ? 'L' : 'R')
                          << std::string(3, ' ') << finger.type()
                          << std::string(3, ' ') << boneType
                          << " diff: " << bone.nextJoint() - bone.prevJoint()
                          << " start: " << bone.prevJoint() - hand.palmPosition()
                          << ", end: " << bone.nextJoint() - hand.palmPosition()
                          << ", direction: " << bone.direction() * RAD_TO_DEG << std::endl;
            }
        }
    }
}

void RemoteLeapServer::onFrame(const Controller &controller)
{

    // Get the most recent frame and report some basic information
    const Frame frame = controller.frame();
    if(loggingEnabled) loghand(frame);

    if (frame.hands().count() > 0)
    {
        transform.encode(frame);
        transform.writeData(*server);

        if(loggingEnabled) {
            std::cout << std::endl
                    << std::endl;
        }
    }
}

void RemoteLeapServer::onFocusGained(const Controller &controller)
{
    std::cout << "Focus Gained" << std::endl;
}

void RemoteLeapServer::onFocusLost(const Controller &controller)
{
    std::cout << "Focus Lost" << std::endl;
}

void RemoteLeapServer::onDeviceChange(const Controller &controller)
{
    std::cout << "Device Changed" << std::endl;
    const DeviceList devices = controller.devices();

    for (int i = 0; i < devices.count(); ++i)
    {
        std::cout << "id: " << devices[i].toString() << std::endl;
        std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
    }
}

void RemoteLeapServer::onServiceConnect(const Controller &controller)
{
    logd("Leap Service Connected");
}

void RemoteLeapServer::onServiceDisconnect(const Controller &controller)
{
    logd("Leap Service Disconnected");
}

int main(int argc, char **argv)
{
    // Create a sample listener and controller
    RemoteLeapServer listener;
    Controller controller;
    int port = 4444;
    bool udp = true;

    // Have the sample listener receive events from the controller
    controller.addListener(listener);

    if (argc > 1 && strcmp(argv[1], "--bg") == 0)
    {
        controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);
    }

    int c;
    while ((c = getopt(argc, argv, "tvbp:")) != -1)
    {
        switch (c)
        {
        case 'v':
            listener.setLoggingEnabled(true);
            break;
        case 'b':
            controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);
            break;
        case 't':
            udp = false;
            break;
        case 'p':
            if (optarg)
                port = std::atoi(optarg);
            break;
        }
    }

    listener.createServer(port, udp);

    // Keep this process running until Enter is pressed
    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    // Remove the sample listener when done
    controller.removeListener(listener);

    return 0;
}
