// ELEC4740 A1
// Tors Webster c3376513

#pragma once

namespace Fan
{
    void RunFanThread(void);
    void Setup();
    void FanEdgeCallback(void);
    void FanCountTimerCallback();
} // namespace Fan
