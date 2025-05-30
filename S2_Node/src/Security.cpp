#include "Security.h"
#include "Constants.h"
#include "Bluetooth.h"
#include "SevenSeg.h"

extern os_queue_t pairing_queue;

namespace Security
{
    std::atomic_bool debouncing = false;
    std::atomic_bool button_press = false;
    std::atomic<uint8_t> position = 0;
    std::atomic<char> ss_number = 0;
    uint8_t fixedPasskey[6] = {0};
    std::atomic_bool pairing;
    Timer flash_timer(300, []
                      { SevenSeg::DotOff(); }, true);
    Timer debounce_timer(BTN_DEBOUNCE_MS, DebounceCallback, true);

    void Setup()
    {
        Timer debounce_timer(BTN_DEBOUNCE_MS, DebounceCallback, true);
        attachInterrupt(PIN_BTN, ButtonCallback, FALLING);
        new Thread("Security_Thread", SecurityThread);
        SevenSeg::Setup();
    }

    void SetPairing(bool value)
    {
        pairing = value;
    }

    void DebounceCallback(void)
    {
        debouncing = false;
    }

    char CalculateNumber(uint16_t pot_value)
    {
        double factor = 4095 / 9;
        uint8_t int_val = (uint8_t)(pot_value / factor);
        char val = '0' + int_val;
        return val;
    }

    // Callback for button press
    void ButtonCallback()
    {
        button_press = true;
    }

    void SecurityThread()
    {
        while (true)
        {
            PairingStatus queue_value;
            if (os_queue_take(pairing_queue, &queue_value, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
            {
                if (queue_value == PairingStatus::OFF)
                {
                    SevenSeg::Off();
                }
                else if (queue_value == PairingStatus::PAIRING)
                {
                    Log.info("Starting Pairing");
                    pairing = true;
                    while (pairing)
                    {
                        if (button_press)
                        {
                            Log.info("Got button press");
                            button_press = false;
                            if (debouncing || !pairing)
                            {
                                continue;
                            }
                            else
                            {
                                fixedPasskey[position] = ss_number;
                                position++;
                                SevenSeg::ShowDot();
                                flash_timer.start();
                            }
                            if (position >= 6)
                            {
                                Log.info("Sending data");
                                Bluetooth::SetPairingPasskey(fixedPasskey);
                                pairing = false;
                                break;
                            }
                            debouncing = true;
                            debounce_timer.start();
                        }
                        uint32_t pot_value = analogRead(DJ_KNOB);
                        ss_number = CalculateNumber((uint16_t)pot_value);
                        SevenSeg::ShowNumber(ss_number.load());
                        delay(100);
                    }
                    position = 0;
                    Log.info("Turning 7 seg off");
                    SevenSeg::Off();
                    Log.info("Pairing done");
                }
            }
        }
    }
}