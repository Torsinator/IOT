#pragma once

namespace Security
{
    void Setup();
    void Pair();
    void DebounceCallback();
    void ButtonCallback();
    void SecurityThread();
    void SetPairing(bool value);
}