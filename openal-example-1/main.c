#include <AL/al.h>
#include <AL/alut.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *audio_file = "../data/wave/audio.wav";

int main (int argc, char** argv)
{
    ALenum error;
    ALuint buffer;
    ALuint source, source_state;

    alutGetError();

    alutInit(&argc, argv);
    error = alutGetError();
    if (error != ALUT_ERROR_NO_ERROR) {
        printf("Error when try initialize alut context: %s", alutGetErrorString(error));
        return EXIT_FAILURE;
    }

    alutGetError();
    buffer = alutCreateBufferFromFile(audio_file);
    if (buffer == AL_NONE) {
        error = alutGetError();
        printf("Error when try to load audio file: %s", alutGetErrorString(error));
        alutExit();
        return EXIT_FAILURE;
    }

    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    alListener3f(AL_ORIENTATION, 0, 0, -1);

    ALfloat position[3] = {1.0, 1.0, 1.0};
    ALfloat velocity[3] = {1.0, 1.0, 1.0};
    alGenSources(1, &source);
    alSourcef(source, AL_PITCH, 1.0);
    alSourcef(source, AL_GAIN, 1.0);
    alSourcefv(source, AL_POSITION, position);
    alSourcefv(source, AL_VELOCITY, velocity);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    alSourcei(source, AL_BUFFER, buffer);
    alSourcePlay(source);
    alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    while (source_state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
    }

    return EXIT_SUCCESS;
}
