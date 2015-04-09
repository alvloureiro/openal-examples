#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *audio_file = "sample1.wav";
static const char *voice_audio_file = "../data/wav/voice-example.wav";
static const int capture_audio_freq = 44100;
static const int capture_audio_size = 1024;

ALbyte buffer[22050];
ALint sample;
ALCdevice *g_device = NULL;
ALCcontext *g_context = NULL;
ALCenum g_error = AL_NO_ERROR;
const ALchar *g_string_error = NULL;

typedef enum {
    HIGH_PITCH = 0,
    LOW_PITCH,
    NORMAL_PITCH
} PITCH_EFFECT;

static ALboolean check_al_error()
{
    g_error = alGetError();
    if (g_error != AL_NO_ERROR && alGetString(g_error) != NULL) {
        g_string_error = alGetString(g_error);
        printf("AL error: %s\n", g_string_error);
        g_error = AL_NO_ERROR;
        g_string_error = NULL;
        return AL_TRUE;
    }
    return AL_FALSE;
}

static ALboolean check_alut_error()
{
    g_error = alutGetError();
    if (g_error != ALUT_ERROR_NO_ERROR) {
        g_string_error = alutGetErrorString(g_error);
        printf("ALUT error: %s\n", g_string_error);
        g_error = ALUT_ERROR_NO_ERROR;
        g_string_error = NULL;
        return AL_TRUE;;
    }
    return AL_FALSE;
}

static void initialize_audio_library ()
{
    alGetError();
    if (g_device == NULL) {
        // getting the default device
        g_device = alcOpenDevice(NULL);
        check_al_error();
    }
    if (g_device != NULL) {
        g_context = alcCreateContext(g_device, NULL);
        if (g_context != NULL) {
            if (alcMakeContextCurrent(g_context) == AL_FALSE) {
                printf("Failed to make context current\n");
                check_al_error();
                return;
            }
        }
    }
}

static void close_audio_library ()
{
    if (g_context != NULL) {
        // release the context before destroy it
        alcMakeContextCurrent(NULL);
        alcDestroyContext(g_context);
    }
    if (g_device != NULL) {
        // close the current device
        alcCloseDevice(g_device);
    }
}

static void print_available_devices ()
{
    const ALCchar *available_devices = NULL;
    const ALCchar *next = NULL;
    size_t length = 0;

    available_devices = alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER);
    next = available_devices + 1;
    //XXX do it using strtok
    printf("Available capture devices\n");
    while (available_devices && *available_devices != '\0'
            && next && *next != '\0') {
        printf("Device: %s\n", available_devices);
        length = strlen(available_devices);
        available_devices += (length + 1);
        next += (length + 2);
    }
}

static void start_record_audio ()
{
    print_available_devices();
    ALCdevice *capture_device = alcCaptureOpenDevice(NULL, capture_audio_freq, AL_FORMAT_STEREO16, capture_audio_size);
    ALboolean got_error = check_al_error();
    if (got_error) {
        printf("Error when try to open a capture device\n");
        alcCaptureCloseDevice(capture_device);
        return;
    }
    alcCaptureStart(capture_device);
    int capturing = 1;
    while (capturing) {
        alcGetIntegerv(capture_device, ALC_CAPTURE_SAMPLES, (ALsizei) sizeof(ALint), &sample);
    }
    do {
        printf("sample size: %d\n", sample);
    } while (sample < 40960);//22050); // 4096 = 1024 * 4 comes from AL_FORMAT_STEREO16

    alcCaptureSamples(capture_device, (ALCvoid *)buffer, sample);
    alcCaptureStop(capture_device);
    alcCaptureCloseDevice(capture_device);
}

static void stop_record_audio ()
{
    return;
}

static void play_audio (ALuint source)
{
    ALuint source_state;
    alSourcePlay(source);
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    while (source_state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    }
}

static void do_pitch_effect (PITCH_EFFECT effect, ALuint source)
{
    switch (effect) {
        case NORMAL_PITCH:
            alSourcef(source, AL_PITCH, 1.0);
        break;
        case LOW_PITCH:
            alSourcef(source, AL_PITCH, 0.6);
        break;
        case HIGH_PITCH:
            alSourcef(source, AL_PITCH, 2.0);
        break;
        default:
        break;
    }
    play_audio(source);
}

static void setup_listener ()
{
    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    alListener3f(AL_ORIENTATION, 0, 0, -1);
}

int main (int argc, char** argv)
{
    ALuint buffer;
    ALuint source;

    initialize_audio_library();

    alutInit(&argc, argv);
    if (check_alut_error()) {
        close_audio_library();
        alutExit();
        return EXIT_FAILURE;
    }

    /*buffer = alutCreateBufferFromFile(audio_file);*/
    buffer = alutCreateBufferFromFile(voice_audio_file);
    if (check_alut_error() || buffer == AL_NONE) {
        close_audio_library();
        alutExit();
        return EXIT_FAILURE;
    }

    setup_listener();

    ALfloat position[3] = {1.0, 1.0, 1.0};
    ALfloat velocity[3] = {1.0, 1.0, 1.0};
    alGenSources(1, &source);
    alSourcef(source, AL_GAIN, 1.0);
    alSourcefv(source, AL_POSITION, position);
    alSourcefv(source, AL_VELOCITY, velocity);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    alSourcei(source, AL_BUFFER, buffer);
    do_pitch_effect(NORMAL_PITCH, source);
    play_audio(source);
    do_pitch_effect(HIGH_PITCH, source);
    play_audio(source);
    do_pitch_effect(LOW_PITCH, source);
    play_audio(source);
    close_audio_library();
    alutExit();
    return EXIT_SUCCESS;
}
