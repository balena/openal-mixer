/*
 * ALx
 * Windows Implementation
 *
 * Written by Dominic Mazzoni for PortMixer
 * Updated and extended for OpenAL by Guilherme Balena Versiani
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

#ifndef __MINGW32__
#define _CRT_SECURE_NO_DEPRECATE // get rid of sprintf security warnings on VS2005
#define _CRT_NONSTDC_NO_DEPRECATE
#pragma comment(lib, "winmm.lib")
#endif

#define ALX_BUILD_LIBRARY

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <al.h>
#include <alx.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Mmsystem.h>

#pragma warning (disable: 4290)

///////////////////////////////////////////////////////
// ALMix Related helper functions

struct ALXctrl
{
    ALXchar    *name;
    DWORD       lineID;
    DWORD       controlID;

    ALXctrl() : name(0), lineID(0), controlID(0) {}
    ~ALXctrl() { free(name); }
};

namespace alx {

enum ComponentType_Type {
    DstUndefined   = MIXERLINE_COMPONENTTYPE_DST_UNDEFINED,
    DstDigital     = MIXERLINE_COMPONENTTYPE_DST_DIGITAL,
    DstLine        = MIXERLINE_COMPONENTTYPE_DST_LINE,
    DstMonitor     = MIXERLINE_COMPONENTTYPE_DST_MONITOR,
    DstSpeakers    = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS,
    DstHeadphones  = MIXERLINE_COMPONENTTYPE_DST_HEADPHONES,
    DstTelephone   = MIXERLINE_COMPONENTTYPE_DST_TELEPHONE,
    DstWaveIn      = MIXERLINE_COMPONENTTYPE_DST_WAVEIN,
    DstVoiceIn     = MIXERLINE_COMPONENTTYPE_DST_VOICEIN,
    SrcUndefined   = MIXERLINE_COMPONENTTYPE_SRC_UNDEFINED,
    SrcDigital     = MIXERLINE_COMPONENTTYPE_SRC_DIGITAL,
    SrcLine        = MIXERLINE_COMPONENTTYPE_SRC_LINE,
    SrcMicrophone  = MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE,
    SrcSynthesizer = MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER,
    SrcCompactDisc = MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC,
    SrcTelephone   = MIXERLINE_COMPONENTTYPE_SRC_TELEPHONE,
    SrcPcSpeaker   = MIXERLINE_COMPONENTTYPE_SRC_PCSPEAKER,
    SrcWaveOut     = MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT,
    SrcAuxiliary   = MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY,
    SrcAnalog      = MIXERLINE_COMPONENTTYPE_SRC_ANALOG
};

enum ControlType_Type {
    Custom         = MIXERCONTROL_CONTROLTYPE_CUSTOM,
    BooleanMeter   = MIXERCONTROL_CONTROLTYPE_BOOLEANMETER,
    SignedMeter    = MIXERCONTROL_CONTROLTYPE_SIGNEDMETER,
    PeakMeter      = MIXERCONTROL_CONTROLTYPE_PEAKMETER,
    UnsignedMeter  = MIXERCONTROL_CONTROLTYPE_UNSIGNEDMETER,
    Boolean        = MIXERCONTROL_CONTROLTYPE_BOOLEAN,
    OnOff          = MIXERCONTROL_CONTROLTYPE_ONOFF,
    Mute           = MIXERCONTROL_CONTROLTYPE_MUTE,
    Mono           = MIXERCONTROL_CONTROLTYPE_MONO,
    Loudness       = MIXERCONTROL_CONTROLTYPE_LOUDNESS,
    StereoEnh      = MIXERCONTROL_CONTROLTYPE_STEREOENH,
    BassBoost      = MIXERCONTROL_CONTROLTYPE_BASS_BOOST,
    Button         = MIXERCONTROL_CONTROLTYPE_BUTTON,
    Decibels       = MIXERCONTROL_CONTROLTYPE_DECIBELS,
    Signed         = MIXERCONTROL_CONTROLTYPE_SIGNED,
    Unsigned       = MIXERCONTROL_CONTROLTYPE_UNSIGNED,
    Percent        = MIXERCONTROL_CONTROLTYPE_PERCENT,
    Slider         = MIXERCONTROL_CONTROLTYPE_SLIDER,
    Pan            = MIXERCONTROL_CONTROLTYPE_PAN,
    QsoundPan      = MIXERCONTROL_CONTROLTYPE_QSOUNDPAN,
    Fader          = MIXERCONTROL_CONTROLTYPE_FADER,
    Volume         = MIXERCONTROL_CONTROLTYPE_VOLUME,
    Bass           = MIXERCONTROL_CONTROLTYPE_BASS,
    Treble         = MIXERCONTROL_CONTROLTYPE_TREBLE,
    Equalizer      = MIXERCONTROL_CONTROLTYPE_EQUALIZER,
    SingleSelect   = MIXERCONTROL_CONTROLTYPE_SINGLESELECT,
    Mux            = MIXERCONTROL_CONTROLTYPE_MUX,
    MultipleSelect = MIXERCONTROL_CONTROLTYPE_MULTIPLESELECT,
    Mixer          = MIXERCONTROL_CONTROLTYPE_MIXER,
    Microtime      = MIXERCONTROL_CONTROLTYPE_MICROTIME,
    Millitime      = MIXERCONTROL_CONTROLTYPE_MILLITIME
};

struct ComponentType
{
public:
    explicit ComponentType(ComponentType_Type componentType)
        : _componentType(componentType)
    {}
    DWORD operator()() const {
        return MIXER_GETLINEINFOF_COMPONENTTYPE;
    }
    void operator()(MIXERLINE &line) const {
        line.dwComponentType = _componentType;
    }

private:
    DWORD _componentType;
};

struct Source
{
public:
    explicit Source(DWORD dwDestination, DWORD dwSource)
        : _destination(dwDestination), _source(dwSource)
    {}
    DWORD operator()() const {
        return MIXER_GETLINEINFOF_SOURCE;
    }
    void operator()(MIXERLINE &line) const {
        line.dwDestination = _destination;
        line.dwSource = _source;
    }

private:
    DWORD _destination;
    DWORD _source;
};

struct Destination
{
public:
    explicit Destination(DWORD dwSource)
        : _source(dwSource)
    {}
    DWORD operator()() const {
        return MIXER_GETLINEINFOF_DESTINATION;
    }
    void operator()(MIXERLINE &line) const {
        line.dwSource = _source;
    }

private:
    DWORD _source;
};

struct ControlType
{
public:
    explicit ControlType(ControlType_Type controlType)
        : _controlType(controlType)
    {}
    DWORD operator()() const {
        return MIXER_GETLINECONTROLSF_ONEBYTYPE;
    }
    void operator()(MIXERLINECONTROLS &ctrls) const {
        ctrls.dwControlType = _controlType;
    }

private:
    DWORD _controlType;
};

template<typename T>
MMRESULT getLineInfo(HMIXEROBJ hMixer, T &what, MIXERLINE &line)
{
    memset(&line, 0, sizeof(line));
    line.cbStruct = sizeof(MIXERLINE);
    what(line);

    return mixerGetLineInfo(hMixer, &line, MIXER_OBJECTF_HMIXER|what());
}

template<typename T>
MMRESULT getLineControl(HMIXEROBJ hMixer, DWORD dwLineID, T &what, MIXERCONTROL &control)
{
    MIXERLINECONTROLS controls;

    memset(&controls, 0, sizeof(controls));
    controls.cbStruct  = sizeof(MIXERLINECONTROLS);
    controls.dwLineID  = dwLineID;
    controls.cControls = 1;
    controls.cbmxctrl  = sizeof(MIXERCONTROL);
    controls.pamxctrl  = &control;
    what(controls);

    memset(&control, 0, sizeof(control));
    control.cbStruct = sizeof(MIXERCONTROL);

    return mixerGetLineControls(hMixer, &controls, MIXER_OBJECTF_HMIXER|what());
}

template<typename T>
DWORD getLineControlID(HMIXEROBJ hMixer, DWORD dwLineID, T &what)
{
    MMRESULT res;
    MIXERCONTROL control;

    res = getLineControl(hMixer, dwLineID, what, control);
    if (res != MMSYSERR_NOERROR)
        return -1;

    return control.dwControlID;
}

template<typename T, typename U>
MMRESULT findControl(HMIXEROBJ hMixer, T &whatLine, U &whatControl, MIXERCONTROL &control)
{
    MMRESULT res;
    MIXERLINE line;

    res = getLineInfo(hMixer, whatLine, line);
    if (res != MMSYSERR_NOERROR)
        return res;

    res = getLineControl(hMixer, line.dwLineID, whatControl, control);
    if (res != MMSYSERR_NOERROR)
        return res;

    return MMSYSERR_NOERROR;
}

template<typename T, typename U>
DWORD findControl(HMIXEROBJ hMixer, T &whatLine, U &whatControl)
{
    MMRESULT res;
    MIXERCONTROL control;

    res = findControl(hMixer, whatLine, whatControl, control);
    if (res != MMSYSERR_NOERROR)
        return -1;

    return control.dwControlID;
}

template<typename T, typename U>
DWORD findControl(HMIXEROBJ hMixer, T &whatLine1, T &whatLine2, U &whatControl)
{
    MMRESULT res;
    MIXERCONTROL control;

    res = findControl(hMixer, whatLine1, whatControl, control);
    if (res == MMSYSERR_NOERROR)
        return control.dwControlID;

    res = findControl(hMixer, whatLine2, whatControl, control);
    if (res == MMSYSERR_NOERROR)
        return control.dwControlID;

    return -1;
}

template<typename T, typename U>
UINT getControls(HMIXEROBJ hMixer, T &whatLine, U &whatControl, ALXctrl **pctrls)
{
    MMRESULT res;
    MIXERLINE line;
    ALXctrl *ctrls = 0;
    UINT num;
    UINT s;

    do {
        res = getLineInfo(hMixer, whatLine, line);
        if (res != MMSYSERR_NOERROR)
            break;

        num = (UINT) line.cConnections;

        ctrls = new ALXctrl[num];
        if (ctrls == NULL)
            break;

        for (s = 0; s < num; s++) {
            res = getLineInfo(hMixer, Source(line.dwDestination, s), line);
            if (res != MMSYSERR_NOERROR)
                break;

            ctrls[s].lineID    = line.dwLineID;
            ctrls[s].name      = strdup(line.szName);
            ctrls[s].controlID = getLineControlID(hMixer, line.dwLineID, whatControl);
        }

        if (s != num)
            break;

        *pctrls = ctrls;
        return num;

    } while (FALSE);

    delete [] ctrls;

    return 0;
}

template<typename T, typename U>
UINT getControls(HMIXEROBJ hMixer, T &whatLine1, T &whatLine2, U &whatControl, ALXctrl **pctrls)
{
    UINT num;

    num = getControls(hMixer, whatLine1, whatControl, pctrls);
    if (num > 0)
        return num;

    num = getControls(hMixer, whatLine2, whatControl, pctrls);
    if (num > 0)
        return num;

    return 0;
}

class Control
{
public:
    Control(HMIXEROBJ hMixer, DWORD dwControlID)
        : _hMixer(hMixer)
    {
        memset(&_details, 0, sizeof(_details));
        _details.cbStruct = sizeof(MIXERCONTROLDETAILS);
        _details.dwControlID = dwControlID;
        _details.cChannels = 1; /* all channels */
        _details.cMultipleItems = 0;
    }

    ALXfloat getVolume() {
        MMRESULT result;
        MIXERCONTROLDETAILS_UNSIGNED value;

        result = getValue(value);
        if (result != MMSYSERR_NOERROR)
            return -1.0;

        return (ALXfloat)(value.dwValue / 65535.0);
    }

    MMRESULT setVolume(ALXfloat volume) {
        if (volume < 0.0f || volume > 1.0f)
            return MMSYSERR_INVALPARAM;

        MIXERCONTROLDETAILS_UNSIGNED value = { 0 };
        value.dwValue = (unsigned short)(volume * 65535.0);

        return setValue(value);
    }

    ALXboolean disabled() {
        MMRESULT result;
        MIXERCONTROLDETAILS_BOOLEAN value;

        result = getValue(value);
        if (result != MMSYSERR_NOERROR)
            return ALX_TRUE;

        return value.fValue ? ALX_TRUE : ALX_FALSE;
    }

    MMRESULT disable(ALXboolean flag) {
        MIXERCONTROLDETAILS_BOOLEAN value = { 0 };
        value.fValue = flag ? TRUE : FALSE;

        return setValue(value);
    }

private:
    HMIXEROBJ                    _hMixer;
    MIXERCONTROLDETAILS          _details;

    template<typename T>
    MMRESULT getValue(T &value) {
        _details.cbDetails = sizeof(value);
        _details.paDetails = &value;
        memset(&value, 0, sizeof(value));

        return mixerGetControlDetails(
            _hMixer, &_details,
            MIXER_OBJECTF_HMIXER|MIXER_GETCONTROLDETAILSF_VALUE);
    }

    template<typename T>
    MMRESULT setValue(T &value) {
        _details.cbDetails = sizeof(value);
        _details.paDetails = &value;

        return mixerSetControlDetails(
            _hMixer, &_details,
            MIXER_OBJECTF_HMIXER|MIXER_GETCONTROLDETAILSF_VALUE);
    }
};

class ListTextDetails
{
public:
    ListTextDetails(MIXERCONTROLDETAILS_LISTTEXT *list, DWORD size)
        : _list(list), _size(size)
    {}
    DWORD getFlag() const { return MIXER_GETCONTROLDETAILSF_LISTTEXT; }
    void operator ()(MIXERCONTROLDETAILS &details) const {
        details.cChannels = 1;
        details.cMultipleItems = _size;
        details.cbDetails = sizeof(MIXERCONTROLDETAILS_LISTTEXT);
        details.paDetails = _list;
    }

private:
    MIXERCONTROLDETAILS_LISTTEXT *_list;
    DWORD _size;
};

class BooleanDetails
{
public:
    BooleanDetails(MIXERCONTROLDETAILS_BOOLEAN *list, DWORD size)
        : _list(list), _size(size)
    {}
    DWORD getFlag() const { return MIXER_GETCONTROLDETAILSF_VALUE; }
    DWORD setFlag() const { return MIXER_SETCONTROLDETAILSF_VALUE; }
    void operator ()(MIXERCONTROLDETAILS &details) const {
        details.cChannels = 1;
        details.cMultipleItems = _size;
        details.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
        details.paDetails = _list;
    }

private:
    MIXERCONTROLDETAILS_BOOLEAN *_list;
    DWORD _size;
};

template<typename T>
MMRESULT getControlDetails(HMIXEROBJ hMixer, DWORD dwControlID, T &what)
{
    MIXERCONTROLDETAILS details;

    memset(&details, 0, sizeof(details));
    details.cbStruct = sizeof(MIXERCONTROLDETAILS);
    what(details);

    return mixerGetControlDetails(hMixer,
        &details, MIXER_OBJECTF_HMIXER|what.getFlag());
}

template<typename T>
MMRESULT setControlDetails(HMIXEROBJ hMixer, DWORD dwControlID, T &what)
{
    MIXERCONTROLDETAILS details;

    memset(&details, 0, sizeof(details));
    details.cbStruct = sizeof(MIXERCONTROLDETAILS);
    what(details);

    return mixerGetControlDetails(hMixer,
        &details, MIXER_OBJECTF_HMIXER|what.setFlag());
}

} // namespace alx

//////////////////////////////////////////////////////////////////////////////

struct ALXdevice_struct
{
    HMIXEROBJ   hmx;
    int         numInputs;
    int         numOutputs;
    ALXctrl    *src;
    ALXctrl    *srcBoolean;
    ALXctrl    *dst;
    ALXctrl    *dstBoolean;

    HWAVEIN     hWaveIn;
    HWAVEOUT    hWaveOut;

    bool        inputMux;
    DWORD       muxID;
    DWORD       speakerID;
    DWORD       speakerID_boolean;
    DWORD       waveID;
    DWORD       waveID_boolean;

    ALXchar    *szDeviceName;

    ALXdevice_struct()
        : hmx(0), numInputs(0), numOutputs(0),
          src(0), srcBoolean(0), dst(0), dstBoolean(0),
          hWaveIn(0), hWaveOut(0), muxID(-1), speakerID(-1),
          speakerID_boolean(-1), waveID(-1), waveID_boolean(-1),
          szDeviceName(0), inputMux(false)
    {}

    ~ALXdevice_struct() {
        if (hWaveIn)
            waveInClose(hWaveIn);
        if (hWaveOut)
            waveOutClose(hWaveOut);
        if (hmx)
            mixerClose((HMIXER) hmx);
        delete [] src;
        delete [] srcBoolean;
        delete [] dst;
        delete [] dstBoolean;
        free(szDeviceName);
    }

    ALXfloat getMasterVolume() {
        return alx::Control(hmx, speakerID).getVolume();
    }

    void setMasterVolume(ALXfloat level) {
        (void) alx::Control(hmx, speakerID).setVolume(level);
    }

    ALXfloat getPCMOutputVolume() {
        return alx::Control(hmx, waveID).getVolume();
    }

    void setPCMOutputVolume(ALXfloat level) {
        (void) alx::Control(hmx, waveID).setVolume(level);
    }

    bool hasPCMOutputVolume() {
        return waveID != -1;
    }

    int getNumOutputVolumes() {
        return numOutputs;
    }

    const char *getOutputVolumeName(int i) {
        if (i >= 0 && i < numOutputs)
            return dst[i].name;
        return NULL;
    }

    ALXfloat getOutputVolume(int i) {
        if (i >= 0 && i < numOutputs)
            return alx::Control(hmx, dst[i].controlID).getVolume();
        return -1.0;
    }

    void setOutputVolume(int i, ALXfloat level) {
        if (i >= 0 && i < numOutputs)
            (void) alx::Control(hmx, dst[i].controlID).setVolume(level);
    }

    ALXfloat getInputVolume() {
        int i;

        if (hmx) {
            if (inputMux) {
                i = getCurrentInputSource();
                if (i >= 0 && i < numInputs)
                    return alx::Control(hmx, src[i].controlID).getVolume();
            }
            else {
                return alx::Control(hmx, muxID).getVolume();
            }
        }

        return -1.0;
    }

    void setInputVolume(ALXfloat level) {
        int i;

        if (hmx) {
            if (inputMux) {
                i = getCurrentInputSource();
                if (i >= 0 && i < numInputs)
                    (void) alx::Control(hmx, src[i].controlID).setVolume(level);
            }
            else {
                (void) alx::Control(hmx, muxID).setVolume(level);
            }
        }
    }

    ALXboolean isDisabledOutputVolume(int i) {
        if (i < numOutputs)
            return alx::Control(hmx, dstBoolean[i].controlID).disabled();
        return ALX_TRUE;
    }

    ALXboolean isDisabledInputVolume(int i) {
        if (i < numInputs) {
            if (inputMux) {
                return i == getCurrentInputSource() ? ALX_FALSE : ALX_TRUE;
            }
            else {
                return alx::Control(hmx, srcBoolean[i].controlID).disabled()
                    ? ALX_FALSE : ALX_TRUE;
            }
        }
        return ALX_TRUE;
    }

    void disableOutputVolume(int i, ALXboolean flag) {
        if (i >= 0 && i < numOutputs)
            (void) alx::Control(hmx, dstBoolean[i].controlID).disable(flag);
    }

    ALXboolean isDisabledMasterVolume() {
        return alx::Control(hmx, speakerID_boolean).disabled();
    }

    void disableMasterVolume(ALXboolean flag) {
        (void) alx::Control(hmx, speakerID_boolean).disable(flag);
    }

    ALXboolean isDisabledPCMOutputVolume() {
        return alx::Control(hmx, waveID_boolean).disabled();
    }

    void disablePCMOutputVolume(ALXboolean flag) {
        (void) alx::Control(hmx, waveID_boolean).disable(flag);
    }

    int getNumInputSources() {
        return numInputs;
    }

    const char *getInputSourceName(int i) {
        if (i >= 0 && i < numInputs)
            return src[i].name;
        return NULL;
    }

    int getCurrentInputSource() {
        int j;
        int i = 0;
        MMRESULT res;
        MIXERCONTROLDETAILS_LISTTEXT *list = 0;
        MIXERCONTROLDETAILS_BOOLEAN *flags = 0;

        if (numInputs <= 0)
            return -1;

        list = new MIXERCONTROLDETAILS_LISTTEXT[numInputs];
        flags = new MIXERCONTROLDETAILS_BOOLEAN[numInputs];

        res = alx::getControlDetails(hmx, muxID,
            alx::ListTextDetails(list, numInputs));

        if (res == MMSYSERR_NOERROR) {

            res = alx::getControlDetails(hmx, muxID,
                alx::BooleanDetails(flags, numInputs));

            if (res == MMSYSERR_NOERROR) {

                for (j = 0; j < numInputs; j++) {
                    if (flags[j].fValue) {
                        i = j;
                        break;
                    }
                }

                for (j = 0; j < numInputs; j++) {
                    if (src[j].lineID == list[i].dwParam1) {
                        i = j;
                        break;
                    }
                }
            }
        }

        delete list;
        delete flags;

        return i;
    }

    void setCurrentInputSource(int i) {
        int j;
        MMRESULT res;
        MIXERCONTROLDETAILS_LISTTEXT *list = 0;
        MIXERCONTROLDETAILS_BOOLEAN *flags = 0;

        if (numInputs <= 0)
            return;

        list = new MIXERCONTROLDETAILS_LISTTEXT[numInputs];
        flags = new MIXERCONTROLDETAILS_BOOLEAN[numInputs];

        res = alx::getControlDetails(hmx, muxID,
            alx::ListTextDetails(list, numInputs));

        if (res == MMSYSERR_NOERROR) {

            for (j = 0; j < numInputs; j++) {
                if (list[j].dwParam1 == src[i].lineID)
                    flags[j].fValue = TRUE;
                else
                    flags[j].fValue = FALSE;
            }

            (void) alx::setControlDetails(hmx, muxID,
                alx::BooleanDetails(flags, numInputs));
        }

        delete list;
        delete flags;
    }
};

namespace alx {

///////////////////////////////////////////////////////
// STRING and EXTENSIONS

typedef struct ALXextension_struct
{
    ALCchar       *extName;
    ALvoid        *address;
} ALXextension;

typedef struct ALXfunction_struct
{
    ALCchar       *funcName;
    ALvoid        *address;
} ALXfunction;

static ALXextension Extensions[] = {
    { NULL,                           (ALvoid *) NULL                     } };

static ALXfunction  Functions[] = {
    { "alxOpenDevice",                (ALvoid *) alxOpenDevice            },
    { "alxOpenCaptureDevice",         (ALvoid *) alxOpenCaptureDevice     },
    { "alxMapDevice",                 (ALvoid *) alxMapDevice             },
    { "alxMapCaptureDevice",          (ALvoid *) alxMapCaptureDevice      },
    { "alxCloseDevice",               (ALvoid *) alxCloseDevice           },
    { "alxGetFloat",                  (ALvoid *) alxGetFloat              },
    { "alxSetFloat",                  (ALvoid *) alxSetFloat              },
    { "alxGetBoolean",                (ALvoid *) alxGetBoolean            },
    { "alxSetBoolean",                (ALvoid *) alxSetBoolean            },
    { "alxGetString",                 (ALvoid *) alxGetString             },
    { "alxGetInteger",                (ALvoid *) alxGetInteger            },
    { "alxSetInteger",                (ALvoid *) alxSetInteger            },
    { "alxGetIndexedString",          (ALvoid *) alxGetIndexedString      },
    { "alxGetIndexedFloat",           (ALvoid *) alxGetIndexedFloat       },
    { "alxGetIndexedBoolean",         (ALvoid *) alxGetIndexedBoolean     },
    { "alxSetIndexedFloat",           (ALvoid *) alxSetIndexedFloat       },
    { "alxSetIndexedBoolean",         (ALvoid *) alxSetIndexedBoolean     },
    { "alxGetError",                  (ALvoid *) alxGetError              },
    { NULL,                           (ALvoid *) NULL                     } };

///////////////////////////////////////////////////////
// Global Variables

// Mixer Error
ALXenum LastError = ALX_NO_ERROR;

// Device strings
ALXchar DeviceList[2048] = { 0 };
ALXchar CaptureDeviceList[2048] = { 0 };

ALXint MajorVersion = 1;
ALXint MinorVersion = 1;

///////////////////////////////////////////////////////

ALXboolean match_device_name(const char *name1, const char *name2)
{
    int name1len, name2len, prefix;
    struct {
        const char *s;
        int len;
    } g, l;

    name1len = strlen(name1);
    name2len = strlen(name2);
    prefix = name1len - name2len;

    if (prefix >= 0) {
        g.s = name1;
        g.len = name1len;
        l.s = name2;
        l.len = name2len;
    }
    else {
        g.s = name2;
        g.len = name2len;
        l.s = name1;
        l.len = name1len;
        prefix = -prefix;
    }

    while (prefix >= 0) {
        if (!strncmp(g.s + prefix, l.s, l.len))
            return ALX_TRUE;
        --prefix;
    }

    return ALX_FALSE;
}

/*
    alx::setError

    Store latest ALC Error
*/
void setError(ALXenum errorCode)
{
    LastError = errorCode;
}

} // namespace alx

///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
// ALMix Functions calls

#define ALXAPI
#define ALXAPIENTRY

extern "C" {

ALXAPI ALXdevice * ALXAPIENTRY alxOpenDevice(const ALXchar *devicename)
{
    HMIXER hmx = NULL;
    ALXdevice *pMixer = NULL;
    UINT i, numDevs;
    HWAVEOUT hWaveOut = NULL;
    WAVEFORMATEX OutputType;
    MMRESULT mmres;
    const ALCchar *mixerDevice;

    memset(&OutputType,0,sizeof(WAVEFORMATEX));
    OutputType.wFormatTag=WAVE_FORMAT_PCM;
    OutputType.nChannels=1;
    OutputType.wBitsPerSample=16;
    OutputType.nBlockAlign=OutputType.nChannels*OutputType.wBitsPerSample/8;
    OutputType.nSamplesPerSec=8000;
    OutputType.nAvgBytesPerSec=OutputType.nSamplesPerSec*OutputType.nBlockAlign;
    OutputType.cbSize=0;

    if ((devicename)) {
        mixerDevice = alxGetString(NULL, ALX_DEVICE_SPECIFIER);

        i = 0;
        while (*mixerDevice) {
            if (!strcmp(devicename, mixerDevice))
                break;
            mixerDevice += strlen(mixerDevice) + 1;
            ++i;
        }

        if (*mixerDevice) {
            numDevs = waveOutGetNumDevs();
            if (i < numDevs) {
                waveOutOpen(&hWaveOut, i, &OutputType, 0, 0, CALLBACK_NULL);
                mmres = mixerOpen((LPHMIXER)&hmx,(UINT)hWaveOut,0,0,MIXER_OBJECTF_HWAVEOUT);
            }

            if (mmres==MMSYSERR_NOERROR) {
                pMixer = new ALXdevice;
                if ((pMixer))
                {
                    pMixer->hmx = reinterpret_cast<HMIXEROBJ&>(hmx);
                    pMixer->hWaveOut = hWaveOut;
                    pMixer->szDeviceName = strdup(mixerDevice);

                    pMixer->speakerID = alx::findControl(pMixer->hmx,
                        alx::ComponentType(alx::DstSpeakers),
                        alx::ComponentType(alx::DstHeadphones),
                        alx::ControlType(alx::Volume));
                    if (pMixer->speakerID != -1) {
                        pMixer->speakerID_boolean = alx::findControl(pMixer->hmx,
                            alx::ComponentType(alx::DstSpeakers),
                            alx::ComponentType(alx::DstHeadphones),
                            alx::ControlType(alx::Mute));
                    }
                    pMixer->numOutputs = alx::getControls(pMixer->hmx,
                        alx::ComponentType(alx::DstSpeakers),
                        alx::ComponentType(alx::DstHeadphones),
                        alx::ControlType(alx::Volume),
                        &pMixer->dst);
                    if (pMixer->numOutputs > 0) {
                        (void) alx::getControls(pMixer->hmx,
                            alx::ComponentType(alx::DstSpeakers),
                            alx::ComponentType(alx::DstHeadphones),
                            alx::ControlType(alx::Mute),
                            &pMixer->dstBoolean);
                    }
                    pMixer->waveID = alx::findControl(pMixer->hmx,
                        alx::ComponentType(alx::SrcWaveOut),
                        alx::ControlType(alx::Volume));
                    if (pMixer->waveID != -1) {
                        pMixer->waveID_boolean = alx::findControl(pMixer->hmx,
                            alx::ComponentType(alx::SrcWaveOut),
                            alx::ControlType(alx::Mute));
                    }
                }
                else {
                    mixerClose(hmx);
                    alx::setError(ALX_OUT_OF_MEMORY);
                }
            }
            else {
                alx::setError(ALX_INVALID_DEVICE);
            }
        }
        else {
            alx::setError(ALX_INVALID_DEVICE);
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }

    return pMixer;
}


ALXAPI ALXdevice * ALXAPIENTRY alxOpenCaptureDevice(const ALXchar *devicename)
{
    HMIXER hmx = NULL;
    ALXdevice *pMixer = NULL;
    UINT i, numDevs;
    HWAVEIN hWaveIn = NULL;
    WAVEFORMATEX InputType;
    MMRESULT mmres;
    const ALCchar *mixerDevice;

    memset(&InputType,0,sizeof(WAVEFORMATEX));
    InputType.wFormatTag=WAVE_FORMAT_PCM;
    InputType.nChannels=1;
    InputType.wBitsPerSample=16;
    InputType.nBlockAlign=InputType.nChannels*InputType.wBitsPerSample/8;
    InputType.nSamplesPerSec=8000;
    InputType.nAvgBytesPerSec=InputType.nSamplesPerSec*InputType.nBlockAlign;
    InputType.cbSize=0;

    if ((devicename)) {
        mixerDevice = alxGetString(NULL, ALX_CAPTURE_DEVICE_SPECIFIER);

        i = 0;
        while (*mixerDevice) {
            if (!strcmp(devicename, mixerDevice))
                break;
            mixerDevice += strlen(mixerDevice) + 1;
            ++i;
        }

        if (*mixerDevice) {
            numDevs = waveInGetNumDevs();
            if (i < numDevs) {
                waveInOpen(&hWaveIn, i, &InputType, 0, 0, CALLBACK_NULL);
                mmres = mixerOpen((LPHMIXER)&hmx,(UINT)hWaveIn,0,0,MIXER_OBJECTF_HWAVEIN);
            }

            if (mmres==MMSYSERR_NOERROR) {
                pMixer = new ALXdevice;
                if ((pMixer))
                {
                    pMixer->hmx = reinterpret_cast<HMIXEROBJ&>(hmx);
                    pMixer->hWaveIn = hWaveIn;
                    pMixer->szDeviceName = strdup(mixerDevice);

                    struct {
                        alx::ComponentType_Type component;
                        alx::ControlType_Type   control;
                    } tries[] = {
                        { alx::DstWaveIn, alx::Mux   },
                        { alx::DstWaveIn, alx::Mixer },
                    };

                    for (i = 0; i < sizeof(tries) / sizeof(tries[0]); ++i) {
                        pMixer->muxID = alx::findControl(pMixer->hmx,
                            alx::ComponentType(tries[i].component),
                            alx::ControlType(tries[i].control));
                        if (pMixer->muxID != -1) {
                            pMixer->inputMux = true;
                            break;
                        }
                    }

                    if (pMixer->muxID == -1) {
                        pMixer->inputMux = false;
                        pMixer->muxID = alx::findControl(pMixer->hmx,
                            alx::ComponentType(alx::DstWaveIn),
                            alx::ControlType(alx::Volume));
                    }

                    pMixer->numInputs = alx::getControls(pMixer->hmx,
                        alx::ComponentType(alx::DstWaveIn),
                        alx::ControlType(alx::Volume),
                        &pMixer->src);
                    if (pMixer->numInputs > 0) {
                        (void) alx::getControls(pMixer->hmx,
                            alx::ComponentType(alx::DstWaveIn),
                            alx::ControlType(alx::Mute),
                            &pMixer->srcBoolean);
                    }
                }
                else {
                    mixerClose(hmx);
                    alx::setError(ALX_OUT_OF_MEMORY);
                }
            }
            else {
                alx::setError(ALX_INVALID_DEVICE);
            }
        }
        else {
            alx::setError(ALX_INVALID_DEVICE);
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }

    return pMixer;
}


ALXAPI ALXdevice * ALXAPIENTRY alxMapDevice(ALCdevice *pDevice)
{
    ALXdevice *pMixer = NULL;
    const ALCchar *deviceName;
    const ALCchar *mixerDevice;

    if ((pDevice)) {
        deviceName = alcGetString(pDevice, ALC_DEVICE_SPECIFIER);
        mixerDevice = alxGetString(NULL, ALX_DEVICE_SPECIFIER);

        while (*mixerDevice) {
            if (alx::match_device_name(deviceName, mixerDevice))
                break;
            mixerDevice += strlen(mixerDevice) + 1;
        }

        if (*mixerDevice) {
            pMixer = alxOpenDevice(mixerDevice);
        }
        else {
            // Open default output mixer, so we can map
            // 'Generic Software' and 'Generic Hardware'
            pMixer = alxOpenDevice(alx::DeviceList);
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }

    return pMixer;
}


ALXAPI ALXdevice * ALXAPIENTRY alxMapCaptureDevice(ALCdevice *pDevice)
{
    ALXdevice *pMixer = NULL;
    const ALCchar *deviceName;
    const ALCchar *mixerDevice;

    if ((pDevice)) {
        deviceName = alcGetString(pDevice, ALC_CAPTURE_DEVICE_SPECIFIER);
        mixerDevice = alxGetString(NULL, ALX_CAPTURE_DEVICE_SPECIFIER);

        while (*mixerDevice) {
            if (alx::match_device_name(deviceName, mixerDevice))
                break;
            mixerDevice += strlen(mixerDevice) + 1;
        }

        if (*mixerDevice) {
            pMixer = alxOpenCaptureDevice(mixerDevice);
        }
        else {
            // Open default output mixer, so we can map
            // 'Generic Software' and 'Generic Hardware'
            pMixer = alxOpenCaptureDevice(alx::CaptureDeviceList);
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }

    return pMixer;
}


ALXAPI void ALXAPIENTRY alxCloseDevice(ALXdevice *pMixer)
{
    delete pMixer;
}


ALXAPI ALXfloat ALXAPIENTRY alxGetFloat(ALXdevice *pMixer, ALXenum param)
{
    ALXfloat value = -1.0;

    if (pMixer)
    {
        switch (param)
        {
        case ALX_MASTER_VOLUME:
            value = pMixer->getMasterVolume();
            break;

        case ALX_PCM_OUTPUT_VOLUME:
            value = pMixer->getPCMOutputVolume();
            break;

        case ALX_INPUT_VOLUME:
            value = pMixer->getInputVolume();
            break;

        default:
            alx::setError(ALX_INVALID_ENUM);
            break;
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }

    return value;
}


ALXAPI void ALXAPIENTRY alxSetFloat(ALXdevice *pMixer, ALXenum param, ALXfloat value)
{
    if (pMixer)
    {
        switch (param)
        {
        case ALX_MASTER_VOLUME:
            pMixer->setMasterVolume(value);
            break;

        case ALX_PCM_OUTPUT_VOLUME:
            pMixer->setPCMOutputVolume(value);
            break;

        case ALX_INPUT_VOLUME:
            pMixer->setInputVolume(value);
            break;

        default:
            alx::setError(ALX_INVALID_ENUM);
            break;
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }
}


ALXAPI ALXboolean ALXAPIENTRY alxGetBoolean(ALXdevice *pMixer, ALXenum param)
{
    ALXboolean value = ALX_FALSE;

    if (pMixer)
    {
        switch (param)
        {
        case ALX_PCM_OUTPUT:
            value = pMixer->hasPCMOutputVolume();
            break;

        case ALX_MASTER_VOLUME:
            value = pMixer->isDisabledMasterVolume();
            break;

        case ALX_PCM_OUTPUT_VOLUME:
            value = pMixer->isDisabledPCMOutputVolume();
            break;

        default:
            alx::setError(ALX_INVALID_ENUM);
            break;
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }

    return value;
}


ALXAPI void ALXAPIENTRY alxSetBoolean(ALXdevice *pMixer, ALXenum param, ALXboolean value)
{
    if (pMixer)
    {
        switch (param)
        {
        case ALX_MASTER_VOLUME:
            pMixer->disableMasterVolume(value);
            break;

        case ALX_PCM_OUTPUT_VOLUME:
            pMixer->disablePCMOutputVolume(value);
            break;

        default:
            alx::setError(ALX_INVALID_ENUM);
            break;
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }
}


ALXAPI const ALXchar * ALXAPIENTRY alxGetString(ALXdevice *pMixer, ALXenum param)
{
    UINT i, numDevs;
    const ALXchar *value = NULL;
    ALXchar *szDeviceList;
    WAVEINCAPS WaveInCaps;
    WAVEOUTCAPS WaveOutCaps;

    switch (param)
    {
    case ALX_DEVICE_SPECIFIER:
        if (pMixer)
        {
            if (pMixer->hWaveOut)
                value = pMixer->szDeviceName;
            else
                alx::setError(ALX_INVALID_DEVICE);
        }
        else
        {
            szDeviceList = alx::DeviceList;

            numDevs = waveOutGetNumDevs();
            for (i = 0; i < numDevs; ++i)
            {
                if (waveOutGetDevCaps(i, &WaveOutCaps, sizeof(WaveOutCaps)) == MMSYSERR_NOERROR)
                {
                    sprintf(szDeviceList, "%s", WaveOutCaps.szPname);
                    szDeviceList += strlen(szDeviceList) + 1;
                }
            }

            szDeviceList[0] = '\0';
            value = alx::DeviceList;
        }
        break;

    case ALX_CAPTURE_DEVICE_SPECIFIER:
        if (pMixer)
        {
            if (pMixer->hWaveIn)
                value = pMixer->szDeviceName;
            else
                alx::setError(ALX_INVALID_DEVICE);
        }
        else
        {
            szDeviceList = alx::CaptureDeviceList;

            numDevs = waveInGetNumDevs();
            for (i = 0; i < numDevs; ++i)
            {
                if (waveInGetDevCaps(i, &WaveInCaps, sizeof(WaveInCaps)) == MMSYSERR_NOERROR)
                {
                    sprintf(szDeviceList, "%s", WaveInCaps.szPname);
                    szDeviceList += strlen(szDeviceList) + 1;
                }
            }

            szDeviceList[0] = '\0';
            value = alx::CaptureDeviceList;
        }
        break;

    default:
        alx::setError(ALX_INVALID_ENUM);
        break;
    }

    return value;
}


ALXAPI ALXint ALXAPIENTRY alxGetInteger(ALXdevice *pMixer, ALXenum param)
{
    ALXint value = -1;

    if (pMixer) {
        switch (param)
        {
        case ALX_OUTPUT_VOLUME_SPECIFIER:
            value = pMixer->getNumOutputVolumes();
            break;
 
        case ALX_INPUT_SOURCE_SPECIFIER:
            value = pMixer->getNumInputSources();
            break;
 
        default:
            alx::setError(ALX_INVALID_ENUM);
            break;
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }

    return value;
}


ALXAPI void ALXAPIENTRY alxSetInteger(ALXdevice *pMixer, ALXenum param, ALXint value)
{
    if (pMixer) {
        switch (param)
        {
        case ALX_INPUT_SOURCE:
            pMixer->setCurrentInputSource(value);
            break;
 
        default:
            alx::setError(ALX_INVALID_ENUM);
            break;
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }
}


ALXAPI const ALXchar * ALXAPIENTRY alxGetIndexedString(ALXdevice *pMixer, ALXenum param, ALXint index)
{
    const ALXchar *value = NULL;

    if (pMixer) {
        switch (param)
        {
        case ALX_OUTPUT_VOLUME_SPECIFIER:
            value = pMixer->getOutputVolumeName(index);
            break;
 
        case ALX_INPUT_SOURCE_SPECIFIER:
            value = pMixer->getInputSourceName(index);
            break;
 
        default:
            alx::setError(ALX_INVALID_ENUM);
            break;
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }

    return value;
}


ALXAPI ALXfloat ALX_APIENTRY alxGetIndexedFloat(ALXdevice *pMixer, ALXenum param, ALXint index)
{
    ALXfloat value = -1.0f;

    if (pMixer) {
        switch (param)
        {
        case ALX_OUTPUT_VOLUME:
            value = pMixer->getOutputVolume(index);
            break;
 
        default:
            alx::setError(ALX_INVALID_ENUM);
            break;
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }

    return value;
}


ALXAPI ALXboolean ALXAPIENTRY alxGetIndexedBoolean(ALXdevice *pMixer, ALXenum param, ALXint index)
{
    ALXboolean value = ALX_FALSE;

    if (pMixer) {
        switch (param)
        {
        case ALX_OUTPUT_VOLUME:
            value = pMixer->isDisabledOutputVolume(index);
            break;

        case ALX_INPUT_SOURCE:
            value = pMixer->isDisabledInputVolume(index);
            break;

        default:
            alx::setError(ALX_INVALID_ENUM);
            break;
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }

    return value;
}


ALXAPI void ALXAPIENTRY alxSetIndexedFloat(ALXdevice *pMixer, ALXenum param, ALXint index, ALXfloat value)
{
    if (pMixer) {
        switch (param)
        {
        case ALX_OUTPUT_VOLUME:
            pMixer->setOutputVolume(index, value);
            break;

        default:
            alx::setError(ALX_INVALID_ENUM);
            break;
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }
}


ALXAPI void ALXAPIENTRY alxSetIndexedBoolean(ALXdevice *pMixer, ALXenum param, ALXint index, ALXboolean value)
{
    if (pMixer) {
        switch (param)
        {
        case ALX_OUTPUT_VOLUME:
            pMixer->disableOutputVolume(index, value);
            break;

        default:
            alx::setError(ALX_INVALID_ENUM);
            break;
        }
    }
    else {
        alx::setError(ALX_INVALID_DEVICE);
    }
}


/*
    alxGetProcAddress

    Retrieves the function address for a particular extension function
*/
ALXAPI void * ALXAPIENTRY alxGetProcAddress(ALXdevice *device, const ALXchar *funcName)
{
    int i = 0;
    void *pFunction = NULL;

    if (funcName)
    {
        while ((alx::Functions[i].funcName)&&(strcmp(alx::Functions[i].funcName,funcName)))
            i++;
        pFunction = alx::Functions[i].address;
    }
    else
    {
        alx::setError(ALX_INVALID_VALUE);
    }

    return pFunction;
}


/*
    alxGetError

    Return last ALMix generated error code
*/
ALXAPI ALXenum ALXAPIENTRY alxGetError(ALXdevice *pMixer)
{
    ALXenum errorCode;

    errorCode = alx::LastError;
    alx::LastError = ALX_NO_ERROR;
    return errorCode;
}

} // extern "C"

/* Modeline for vim: set tw=79 et ts=4: */

