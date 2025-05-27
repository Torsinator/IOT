#include "SevenSeg.h"
#include "Enumerations.h"
#include "Constants.h"
#include "Structs.h"

namespace SevenSeg
{
    os_queue_t seven_seg_queue;
    void Setup()
    {
        Off();
    }

    void Off()
    {
        digitalWrite(SSEG_A, LOW);
        digitalWrite(SSEG_B, LOW);
        digitalWrite(SSEG_C, LOW);
        digitalWrite(SSEG_D, LOW);
        digitalWrite(SSEG_E, LOW);
        digitalWrite(SSEG_F, LOW);
        digitalWrite(SSEG_G, LOW);
        digitalWrite(SSEG_DOT, LOW);
    }

    void SetSegA(char number)
    {
        switch (number)
        {
        case '0':
        case '2':
        case '3':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            digitalWrite(SSEG_A, HIGH);
            break;

        default:
            digitalWrite(SSEG_A, LOW);
            break;
        }
    }

    void SetSegB(char number)
    {
        switch (number)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '7':
        case '8':
        case '9':
            digitalWrite(SSEG_B, HIGH);
            break;

        default:
            digitalWrite(SSEG_B, LOW);
            break;
        }
    }

    void SetSegC(char number)
    {
        if (number == '2')
        {
            digitalWrite(SSEG_C, LOW);
        }
        else
        {
            digitalWrite(SSEG_C, HIGH);
        }
    }

    void SetSegD(char number)
    {
        switch (number)
        {
        case '1':
        case '4':
        case '7':
            digitalWrite(SSEG_D, LOW);
            break;

        default:
            digitalWrite(SSEG_D, HIGH);
            break;
        }
    }

    void SetSegE(char number)
    {
        switch (number)
        {
        case '1':
        case '3':
        case '4':
        case '5':
        case '7':
        case '9':
            digitalWrite(SSEG_E, LOW);
            break;

        default:
            digitalWrite(SSEG_E, HIGH);
            break;
        }
    }

    void SetSegF(char number)
    {
        switch (number)
        {
        case '1':
        case '2':
        case '3':
        case '7':
            digitalWrite(SSEG_F, LOW);
            break;

        default:
            digitalWrite(SSEG_F, HIGH);
            break;
        }
    }

    void SetSegG(char number)
    {
        switch (number)
        {
        case '0':
        case '1':
        case '7':
            digitalWrite(SSEG_G, LOW);
            break;

        default:
            digitalWrite(SSEG_G, HIGH);
            break;
        }
    }

    void ShowNumber(char number)
    {
        SetSegA(number);
        SetSegB(number);
        SetSegC(number);
        SetSegD(number);
        SetSegE(number);
        SetSegF(number);
        SetSegG(number);
    }

    void ShowDot()
    {
        digitalWrite(SSEG_DOT, HIGH);
    }

    void DotOff()
    {
        digitalWrite(SSEG_DOT, LOW);
    }
}