#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <thread>
#include <cstdio>
#include <termios.h>
#include <unistd.h>

namespace Audio
{
    unsigned Frequency = 48000;
    Uint16   Format = AUDIO_S16SYS;
    unsigned short Channels = 2;
    unsigned ChunkSize = 4096;
};

void
CountTime(bool *Paused, bool *Quit)
{
    unsigned Time = 0;
    
    for (;;) {
        unsigned Min = ((Time/60)%60);
        unsigned Sec = (Time%60);
        
        if (*Paused == true) continue;
        if (*Quit == true) {
            std::cout << std::endl;
            break;
        }
        
        std::cout << "\33[s" << std::flush;
        printf(" %02d:%02d", Min, Sec);
        std::cout << std::flush;
        SDL_Delay(1000);
        std::cout << "\33[u" << std::flush;
        Time++;
    }
}

char
GetCh() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
            perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
            perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
            perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
            perror ("tcsetattr ~ICANON");
    return (buf);
}

int
main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [filename]\n";
        exit(1);
    }
    
    std::string Filename = argv[1];
    
    SDL_Init(SDL_INIT_AUDIO);
    Mix_Init(MIX_INIT_MP3);

    Mix_OpenAudio(Audio::Frequency, Audio::Format, Audio::Channels, Audio::ChunkSize);
    Mix_Music *Audio = Mix_LoadMUS(Filename.c_str());
    bool Paused = false;
    bool Quit = false;
    
    if (!Audio) {
        std::cerr << "Error: file " << Filename << " doesn't exist\n";
        exit(1);
    }
    
    Mix_PlayMusic(Audio, 1);
    std::thread Timer(CountTime, &Paused, &Quit);

    std::cout << "File " << Filename << " at " << Audio::Frequency << " kHz (" << Audio::Channels
              << " channels) (playing)" << std::flush;

    while (Quit!=true) {
        char Key = GetCh();
        
        switch (Key)
        {
            case 'p':
                if (Paused) {
                    Mix_ResumeMusic();
                    Paused = false;
                } else {
                    Mix_PauseMusic();
                    Paused = true;
                }
                
                break;

            case 'q':
                Quit = true;
                break;
                
            default:
                break;
        }
    }
    
    Timer.join();
        
    Mix_FreeMusic(Audio);
    Mix_Quit();
    return 0;
}
