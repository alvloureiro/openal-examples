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
    alcMakeContextCurrent(NULL);
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
    alListenerf(AL_METERS_PER_UNIT, 0.3f);
    if (alGetError() != AL_NO_ERROR) {
        printf("failed to set distance units\n");
    }
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
    ALint context_attributes[4] = {0};
    ALCint aux_send = 0; //auxiliary sends per source
    ALuint aux_effect_slots[4] = {0};
    ALuint effects[2] = {0};
    ALuint loop;

    // Initializing the AL library, opening the default device and create an AL context.
    alGetError();
    g_device = alcOpenDevice(NULL);
    ALCenum error = alGetError();
    if (error != AL_NO_ERROR && alGetString(error) != NULL) {
        printf("Failed to open default device: %s\n", alGetString(error));
        alcCloseDevice(g_device);
        return EXIT_FAILURE;
    }

    // checking if the effects extension is available
    if (alcIsExtensionPresent(g_device, "ALC_EXT_EFX") == AL_FALSE) {
        printf("Effects extension is not available.\n");
        alcCloseDevice(g_device);
        return EXIT_FAILURE;
    }

    error = AL_NO_ERROR;
    alGetError();
    // we will request for 4 aux sends
    context_attributes[0] = ALC_MAX_AUXILIARY_SENDS;
    context_attributes[1] = 2;
    g_context = alcCreateContext(g_device, context_attributes);
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

    // getting the number of aux sends available
    alcGetIntegerv(g_device, ALC_MAX_AUXILIARY_SENDS, 1, &aux_send);
    printf("Max Auxiliary sends per source is : %d\n", aux_send);

    // as we asked for 4 aux sends we will create 4 aux slots
    alGetError();
    for (loop = 0; loop < 2; loop++) {
        alGenAuxiliaryEffectSlots(1, &aux_effect_slots[loop]);
        if (alGetError() != AL_NO_ERROR) {
            printf("Failed to create the auxiliary effect slots...\n");
            break;
        }
    }

    printf("%d aux effects slots were created\n", loop);

    // creating 2 effects
    alGetError();
    for (loop = 0; loop < 2; loop++) {
        alGenEffects(1, &effects[loop]);
        if (alGetError() != AL_NO_ERROR) {
            printf("Failed to create effects\n");
            break;
        }
    }

    printf("%d effects objects created..\n", loop);
    // setting the effects properties
    alGetError();
    if (alIsEffect(effects[0]) == AL_TRUE) {
        alEffecti(effects[0], AL_EFFECT_TYPE, AL_EFFECT_REVERB);
        if (alGetError() != AL_NO_ERROR) {
            printf("Reverb effect is not supported\n");
            return EXIT_FAILURE;
        } else {
            // setting the decai time if there is no error
            printf("setting the decay time to 5.0f\n");
            alEffectf(effects[0], AL_REVERB_DECAY_TIME, 5.0f);
        }
    }
    // setting the second effect to flanger type
    // A flanger is an effect that alters the sound signal
    // introducing a ciclicaly varying phase shift into the
    // signal copy.
    alGetError();
    if (alIsEffect(effects[1]) == AL_TRUE) {
        alEffecti(effects[1], AL_EFFECT_TYPE, AL_EFFECT_FLANGER);
        if (alGetError() != AL_NO_ERROR) {
            printf("effect is not supported %s\n", alGetString(alGetError()));
            //TODO release the al device and al context
            //return EXIT_FAILURE;
        }
        alEffecti(effects[1], AL_FLANGER_PHASE, 180);
    }

    // attaching the effects to the aux slots
    // as we have 4 effects slots we will attach 2 slots for each effect
    // that was created above
    alGetError();
    alAuxiliaryEffectSloti(aux_effect_slots[0], AL_EFFECTSLOT_EFFECT, effects[0]);
    if (alGetError() != AL_NO_ERROR) {
        printf("Could not attach effect to the slot\n");
        return EXIT_FAILURE;
    }
    alGetError();
    alAuxiliaryEffectSloti(aux_effect_slots[1], AL_EFFECTSLOT_EFFECT, effects[1]);
    if (alGetError() != AL_NO_ERROR) {
        printf("Could not attach the second effect to the slot\n");
        return EXIT_FAILURE;
    }

    // Initialing the alut library
    alutGetError();
    alutInit(&argc, argv);
    error = alutGetError();
    if (error != ALUT_ERROR_NO_ERROR) {
        printf("Failed to initialize ALUT. %s\n", alutGetErrorString(error));
        close_al();
        return EXIT_FAILURE;
    }

    // creating buffer from  file
    alutGetError();
    buffer = alutCreateBufferFromFile(voice_audio_file);
    error = alutGetError();
    if (error != ALUT_ERROR_NO_ERROR || buffer == AL_NONE) {
        printf("Failed to create buffer from file.%s\n", alutGetErrorString(error));
        close_al();
        return EXIT_FAILURE;
    }
    // creating and setting the listener properties
    setup_listener();

    // creating the source
    alGetError();
    printf("Generating the AL Source\n");
    ALfloat position[3] = {2.0, 2.0, 2.0};
    ALfloat velocity[3] = {2.0, 2.0, 2.0};
    ALfloat direction[3] = {2.0, 2.0, 2.0};

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
        return EXIT_FAILURE;
    }

    // setting the aux sends and aux effects slots properly
    alSource3i(source, AL_AUXILIARY_SEND_FILTER, aux_effect_slots[0], 0, AL_FILTER_NULL);
    alSource3i(source, AL_AUXILIARY_SEND_FILTER, aux_effect_slots[1], 1, AL_FILTER_NULL);

    alGetError();
    alSourcef(source, AL_CONE_OUTER_GAINHF, 0.5f);
    if (alGetError() != AL_NO_ERROR) {
        printf("Failed to set cone outside gain filter\n");
    }

    // attaching the buffer to the current source
    alSourcei(source, AL_BUFFER, buffer);

    play_audio(source);
    alDeleteAuxiliaryEffectSlots(1, &aux_effect_slots[0]);
    alDeleteAuxiliaryEffectSlots(1, &aux_effect_slots[1]);
    close_al();
    return EXIT_SUCCESS;
}
