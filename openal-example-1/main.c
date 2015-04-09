#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *audio_file = "../data/wav/sample1.wav";
static const char *voice_audio_file = "../../data/wav/voice-example.wav";

ALbyte buffer[22050];
ALint sample;
ALCdevice *g_device = NULL;
ALCcontext *g_context = NULL;
ALCenum g_error = AL_NO_ERROR;
const ALchar *g_string_error = NULL;

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

static void play_audio (ALuint source)
{
    ALuint source_state;
    alSourcePlay(source);
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    while (source_state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    }
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
    play_audio(source);
    close_audio_library();
    alutExit();
    return EXIT_SUCCESS;
}
