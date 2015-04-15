#include <stdio.h>
#include <stdlib.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <AL/efx.h>


static const char *voice_audio_file = "../data/wav/voice-example.wav";

ALCdevice *g_device = NULL;
ALCcontext *g_context = NULL;

static void close_al()
{
    printf("Destroying the context and closing the device...\n");
    alcDestroyContext(g_context);
    alcCloseDevice(g_device);
}
static void setup_listener ()
{
    printf("Setting up the AL Listener\n");
    ALfloat velocity[3] = {-2.0, -2.0, -1.0};
    ALfloat orientation[6] = {0.0, 0.0, -1.0, 0.0, 1.0, 1.0};
    ALfloat position[3] = {1.0, -1.0, -2.0};

    alListenerfv(AL_POSITION, position);
    alListenerfv(AL_VELOCITY, velocity);
    alListenerfv(AL_ORIENTATION, orientation);
}

static void gen_source(ALuint source)
{
    }

static void play_audio (ALuint source)
{
    printf("Playing the AL source\n");
    ALuint source_state;
    alSourcePlay(source);
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    while (source_state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    }
}

int main(int argc, char *argv[])
{
    ALuint buffer, source, filter;

    // Initializing the AL library, opening the default device and create an AL context.
    alGetError();
    g_device = alcOpenDevice(NULL);
    ALCenum error = alGetError();
    if (error != AL_NO_ERROR && alGetString(error) != NULL) {
        printf("Failed to open default device: %s\n", alGetString(error));
        alcCloseDevice(g_device);
        return EXIT_FAILURE;
    }

    error = AL_NO_ERROR;
    alGetError();
    g_context = alcCreateContext(g_device, NULL);
    error = alGetError();
    if (error != AL_NO_ERROR && alGetString(error) != NULL) {
        printf("Failed to create context\n");
        close_al();
        return EXIT_FAILURE;
    }
    ALboolean result = alcMakeContextCurrent(g_context);
    if (result == AL_FALSE) {
        printf("Failed to make the context current\n");
        close_al();
        return EXIT_FAILURE;
    }

    // Initialing the alut library
    alutGetError();
    alutInit(&argc, argv);
    error = alutGetError();
    if (error != ALUT_ERROR_NO_ERROR) {
        printf("Failed to initialize ALUT. %s\n", alutGetErrorString(error));
        close_al();
        alcMakeContextCurrent(NULL);
        return EXIT_FAILURE;
    }

    // creating buffer from  file
    alutGetError();
    buffer = alutCreateBufferFromFile(voice_audio_file);
    error = alutGetError();
    if (error != ALUT_ERROR_NO_ERROR || buffer == AL_NONE) {
        printf("Failed to create buffer from file.%s\n", alutGetErrorString(error));
        close_al();
        alcMakeContextCurrent(NULL);
        return EXIT_FAILURE;
    }

    setup_listener();

    alGetError();

    printf("Generating the AL Source\n");
    ALfloat position[3] = {2.0, 2.0, 1.0};
    ALfloat velocity[3] = {3.0, 3.0, 3.0};
    ALfloat direction[3] = {1.0, 2.0, -1.0};

    alGenSources(1, &source);
    alSourcef(source, AL_GAIN, 1.0);
    alSourcefv(source, AL_POSITION, position);
    alSourcefv(source, AL_VELOCITY, velocity);
    alSourcefv(source, AL_DIRECTION, direction);

    alSourcei(source, AL_LOOPING, AL_FALSE);
    alSourcef(source, AL_PITCH, 1.0);
    alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);

    error = alGetError();
    if (error != AL_NO_ERROR && alGetString(error) != NULL) {
        printf("Failed to gen source\n");
        close_al();
        alcMakeContextCurrent(NULL);
        return EXIT_FAILURE;
    }

    // generating the filters
    alGenFilters(1, &filter);
    if (alIsFilter(filter) == AL_TRUE) {
        printf("Is a valid filter setting the filter type to low pass...\n");
        alFilteri(filter, AL_FILTER_TYPE, AL_FILTER_LOWPASS);
        ALfloat lowgain[2] = {0.13, 0.3};
        ALfloat highgain [2] = {0.3, 0.1};
        alFilterfv(filter, AL_LOWPASS_GAIN, lowgain);
        alFilterfv(filter, AL_LOWPASS_GAINHF, highgain);
    }

    // assign the filter with the source as a dry filter or direct filter
    alSourcei(source, AL_DIRECT_FILTER, filter);

    // attaching the buffer to the current source
    alSourcei(source, AL_BUFFER, buffer);

    play_audio(source);
    close_al();
    return EXIT_SUCCESS;
}
