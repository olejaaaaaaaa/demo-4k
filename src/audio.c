#include <windows.h>
#include <mmsystem.h>

#define SAMPLE_RATE    44100
#define DEMO_LENGTH    30
#define MUSIC_LENGTH   (DEMO_LENGTH + 5)

#define SOUND_DURATION 4
#define N_HARMONICS    3
#define NUM_VOICES     4

static float music[SAMPLE_RATE * MUSIC_LENGTH * 2];
static float instruments[NUM_VOICES][SOUND_DURATION * SAMPLE_RATE];

static const float BASE_FREQ[7] = { 16.35f, 18.35f, 20.60f, 21.83f, 25.50f, 27.50f, 30.87f };
#define NOTE_C 0
#define NOTE_D 1
#define NOTE_A 5
#define NOTE_H 6

static float NoteFreq(int note, int octave) {
    return BASE_FREQ[note] * (float)(1 << octave);
}

static unsigned int rngState = 0x1234567;
static unsigned int NextRandom(void) {
    rngState ^= rngState << 13;
    rngState ^= rngState >> 17;
    rngState ^= rngState << 5;
    return rngState;
}

static void BuildInstruments(float freqs[NUM_VOICES]) {
    int v, h, i;
    for (v = 0; v < NUM_VOICES; v++) {
        float *instrument = instruments[v];
        for (i = 0; i < SOUND_DURATION * SAMPLE_RATE; i++) instrument[i] = 0.0f;

        for (h = 1; h <= N_HARMONICS; h++) {
            float harmonicFreq = freqs[v] * ((float)(h + 1) / (float)h);
            float position = 0.0f;
            float step = harmonicFreq / SAMPLE_RATE;

            for (i = 0; i < SOUND_DURATION * SAMPLE_RATE; i++) {
                float val;
                if (position > 0.5f) position -= 1.0f;
                val = (position < 0 ? -position : position) * 4.0f - 1.0f;
                instrument[i] += val / (50.0f * (float)h);
                position += step;
            }
        }
    }
}

static void ComposeNote(int second, int voice) {
    const float *instrument = instruments[voice];
    int n = SOUND_DURATION * SAMPLE_RATE;
    int i;
    float x = 0.0f;

    for (i = 0; i < n; i++) {
        int idx = second * SAMPLE_RATE + i;
        float filter;
        if (idx >= SAMPLE_RATE * MUSIC_LENGTH) break;

        filter = (x - 2.0f) * (x - 2.0f) * x;
        music[idx] += instrument[i] * filter;

        x += (1.0f / SAMPLE_RATE) / 4.0f;
    }
}

void PlayMusic(void) {
    float freqs[NUM_VOICES];
    int s, v;
    WAVEFORMATEX wfx = { 0 };
    WAVEHDR waveHeader = { 0 };
    HWAVEOUT hWaveOut;

    freqs[0] = NoteFreq(NOTE_A, 3);
    freqs[1] = NoteFreq(NOTE_H, 3);
    freqs[2] = NoteFreq(NOTE_C, 3);
    freqs[3] = NoteFreq(NOTE_D, 3);

    BuildInstruments(freqs);

    for (s = 0; s < DEMO_LENGTH; s++) {
        for (v = 0; v < NUM_VOICES; v++) {
            if ((NextRandom() & 0xFFFFFFF) < 0x3000000) {
                ComposeNote(s, v);
            }
        }
    }

    wfx.wFormatTag = 3;
    wfx.nChannels = 1;
    wfx.nSamplesPerSec = SAMPLE_RATE;
    wfx.wBitsPerSample = 32;
    wfx.nBlockAlign = 4;
    wfx.nAvgBytesPerSec = SAMPLE_RATE * 4;

    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

    waveHeader.lpData = (LPSTR)music;
    waveHeader.dwBufferLength = sizeof(music);

    waveOutPrepareHeader(hWaveOut, &waveHeader, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &waveHeader, sizeof(WAVEHDR));
}