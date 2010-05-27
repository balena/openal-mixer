/*
 * Simple OpenAL mixer testing
 *
 * Copyright (c) 2002-2009
 *
 * Written by Guilherme Balena Versiani
 *
 * OpenAL mixer is intended to work side-by-side with OpenAL.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <al.h>
#include <alc.h>

#if HAVE_ALEXT_H
#include <alext.h>
#endif

#include <alx.h>
#include <string.h>

int stack_ptr = 0;
char stack[1024];

template<typename T>
void push(const T &value)
{
    if (stack_ptr + sizeof(T) > sizeof(stack))
        return;
    *reinterpret_cast<T*>(stack + stack_ptr) = value;
    stack_ptr += sizeof(T);
}

template<typename T>
void pop(T &value)
{
    if (stack_ptr - sizeof(T) < 0) {
        value = 0;
        return;
    }

    value = *reinterpret_cast<T*>(stack + stack_ptr - sizeof(T));
    stack_ptr -= sizeof(T);
}

static void testInputDevice(const ALXchar *deviceName)
{
    ALXfloat value;
    const ALXchar *inputName;

    ALCdevice *device = alcCaptureOpenDevice(deviceName, 8000, AL_FORMAT_MONO16, 1000);
    ALXdevice *mixer = alxMapCaptureDevice(device);

    ALXboolean disabled;
    ALXint i, numInputs = alxGetInteger(mixer, ALX_INPUT_SOURCE_SPECIFIER);

    printf("---- Input device: %s\n", deviceName);
    printf("Available sources:\n");
    for (i = 0; i < numInputs; ++i) {
        inputName = alxGetIndexedString(mixer, ALX_INPUT_SOURCE_SPECIFIER, i);
        disabled = alxGetIndexedBoolean(mixer, ALX_INPUT_SOURCE, i);
        printf("+ %s%s\n", inputName,
            i == disabled ? " (disabled)" : "");
    }

    value = alxGetFloat(mixer, ALX_INPUT_VOLUME); push(value); 
    printf("\nInput Volume: %f\n", value);

    printf("Setting input volume to 0.8:\n");
    alxSetFloat(mixer, ALX_INPUT_VOLUME, 0.8f);
    value = alxGetFloat(mixer, ALX_INPUT_VOLUME);
    printf("Input Volume: %f\n\n", value);

    pop(value); alxSetFloat(mixer, ALX_INPUT_VOLUME, value);

    alxCloseDevice(mixer);
}

static void testInputDevices()
{
    printf("==== Testing input devices ====\n");

    const ALXchar *defaultDevice = alcGetString(NULL, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER);
    printf("---- Default input device: %s\n", defaultDevice);

    const ALCchar* pDeviceNames = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);

    if (pDeviceNames && strlen(pDeviceNames) > 0) {
        while (pDeviceNames && *pDeviceNames) {
            testInputDevice(pDeviceNames);
            pDeviceNames += strlen(pDeviceNames) + 1;
        }
    }
}

static void testOutputDevice(const ALXchar *deviceName)
{
    ALXfloat value;
    ALXboolean disabled;
    const ALXchar *outputName;

    ALCdevice *device = alcOpenDevice(deviceName);
    ALXdevice *mixer = alxMapDevice(device);

    ALXint i, numOutputs = alxGetInteger(mixer, ALX_OUTPUT_VOLUME_SPECIFIER);
    ALXboolean pcmExists = alxGetBoolean(mixer, ALX_PCM_OUTPUT);

    printf("---- Output device: %s\n", deviceName);
    printf("Available destinations:\n");
    for (i = 0; i < numOutputs; ++i) {
        outputName = alxGetIndexedString(mixer, ALX_OUTPUT_VOLUME_SPECIFIER, i);
        value = alxGetIndexedFloat(mixer, ALX_OUTPUT_VOLUME, i); push(value);
        disabled = alxGetIndexedBoolean(mixer, ALX_OUTPUT_VOLUME, i); push(disabled);

        printf("+ %s: %f%s\n", outputName, value,
            disabled ? " disabled" : "");
    }

    value = alxGetFloat(mixer, ALX_MASTER_VOLUME); push(value);
    disabled = alxGetBoolean(mixer, ALX_MASTER_VOLUME); push(disabled);

    printf("\nMaster Volume: %f%s\n", value,
        disabled ? " disabled" : "");

    if (pcmExists) {
        value = alxGetFloat(mixer, ALX_PCM_OUTPUT_VOLUME); push(value);
        disabled = alxGetBoolean(mixer, ALX_PCM_OUTPUT_VOLUME); push(disabled);
        printf("PCM Output Volume: %f%s\n", value,
            disabled ? " disabled" : "");
    }

    printf("\n---- Trying to set all values to 0.8, disabling device 0 and master volume:\n");
    alxSetIndexedBoolean(mixer, ALX_OUTPUT_VOLUME, 0, ALX_TRUE);

    printf("Available destinations:\n");
    for (i = 0; i < numOutputs; ++i) {
        outputName = alxGetIndexedString(mixer, ALX_OUTPUT_VOLUME_SPECIFIER, i);

        alxSetIndexedFloat(mixer, ALX_OUTPUT_VOLUME, i, 0.8f);
        value = alxGetIndexedFloat(mixer, ALX_OUTPUT_VOLUME, i);
        disabled = alxGetIndexedBoolean(mixer, ALX_OUTPUT_VOLUME, i);

        printf("+ %s: %f%s\n", outputName, value,
            disabled ? " disabled" : "");
    }

    alxSetFloat(mixer, ALX_MASTER_VOLUME, 0.8f);
    alxSetBoolean(mixer, ALX_MASTER_VOLUME, ALX_TRUE);

    value = alxGetFloat(mixer, ALX_MASTER_VOLUME);
    disabled = alxGetBoolean(mixer, ALX_MASTER_VOLUME);

    printf("\nMaster Volume: %f%s\n", value,
        disabled ? " disabled" : "");

    if (pcmExists) {
        alxSetFloat(mixer, ALX_PCM_OUTPUT_VOLUME, 0.8f);
        alxSetBoolean(mixer, ALX_PCM_OUTPUT_VOLUME, ALX_TRUE);

        value = alxGetFloat(mixer, ALX_PCM_OUTPUT_VOLUME);
        disabled = alxGetBoolean(mixer, ALX_PCM_OUTPUT_VOLUME);

        printf("PCM Output Volume: %f%s\n", value,
            disabled ? " disabled" : "");
    }

    printf("\n--- Restoring original values\n");

    if (pcmExists) {
        pop(disabled); alxSetBoolean(mixer, ALX_PCM_OUTPUT_VOLUME, disabled);
        pop(value); alxSetFloat(mixer, ALX_PCM_OUTPUT_VOLUME, value);
    }
    pop(disabled); alxSetBoolean(mixer, ALX_MASTER_VOLUME, disabled);
    pop(value); alxSetFloat(mixer, ALX_MASTER_VOLUME, value);
    for (i = numOutputs-1; i >= 0; --i) {
        pop(disabled); alxSetIndexedBoolean(mixer, ALX_OUTPUT_VOLUME, i, disabled);
        pop(value); alxSetIndexedFloat(mixer, ALX_OUTPUT_VOLUME, i, value);
    }

    alxCloseDevice(mixer);
}

static void testOutputDevices()
{
    printf("==== Testing output devices ====\n");

    const ALXchar *defaultDevice = alcGetString(NULL, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
    printf("---- Default output device: %s\n", defaultDevice);

    const ALCchar* pDeviceNames = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);

    if (pDeviceNames && strlen(pDeviceNames) > 0) {
        while (pDeviceNames && *pDeviceNames) {
            testOutputDevice(pDeviceNames);
            pDeviceNames += strlen(pDeviceNames) + 1;
        }
    }
}

int main()
{
    testInputDevices();
    testOutputDevices();
    getchar();
    return 0;
}
