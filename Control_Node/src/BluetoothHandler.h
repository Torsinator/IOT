#pragma once

#include <string>
#include <atomic>

#include "Particle.h"

class BluetoothHandler final
{
public:
    BluetoothHandler(char *uuid, os_queue_t &message_queue);
    void StartThread();
    void Shutdown();

private:
    void GetData();
    void Connect();
    void Disconnect();

    const char *m_uuid;
    BlePeerDevice m_device;
    os_queue_t &m_message_queue;
    std::atomic_bool m_shutdown = false;
};