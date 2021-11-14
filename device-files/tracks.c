#include <linux/kernel.h>
#include <linux/random.h>
#include <linux/string.h>
#include <linux/slab.h>
#include "tracks.h"

#define SAD 0
#define LONELY 1
#define HAPPY 2
#define WEIRD 3
#define NUMBER_SOUNDTRACKS 4
#define NUMBER_TRACKS 6

// Local structures
struct playlist {
    char *mood;
    struct song tracks[NUMBER_TRACKS];
};

static struct playlist complete_soundtrack[NUMBER_SOUNDTRACKS];

struct song init_song(char *name, char *author) {
    struct song my_song = {
        .name = name,
        .author = author
    };
    return my_song;
}

void load_songs(void) {
    struct playlist sad_playlist = {
        .mood = "sad",
        .tracks = {
            init_song("Where is my love?", "Cat power"),
            init_song("Space song", "Beach house"),
            init_song("Come sweet death", "The end of evangelion"),
            init_song("Jueves", "La oreja de van gogh"),
            init_song("Alma mia", "Silvio Rodriguez"),
            init_song("Fireworks", "Stina Nordenstam")
        }
    };

    struct playlist lonely_playlist = {
        .mood = "lonely",
        .tracks = {
            init_song("Don't give up", "Peter Gabriel"),
            init_song("Thank you for hearing me", "Sinead o'connor"),
            init_song("Dimentions", "Her OST"),
            init_song("The moon song", "Karen O"),
            init_song("Asleep", "The smiths"),
            init_song("Eclipse de luna en el cielo", "Natalia lafourcade")
        }
    };

    struct playlist happy_playlist = {
        .mood = "happy",
        .tracks = {
            init_song("Over the raindow", "Jury Garland"),
            init_song("Come on eileen", "Dexis midnight runners"),
            init_song("Sea of love", "Cat power"),
            init_song("I'll try anything once", "The strokes"),
            init_song("Starry, starry night", "Don McLean"),
            init_song("For lovers", "Lamp")
        }
    };

    struct playlist weird_playlist = {
        .mood = "weird",
        .tracks = {
            init_song("Creep", "Radiohead"),
            init_song("Weird fishes", "Radiohead"),
            init_song("13 angels standing guard 'round the side of your bed", "Silver Mt Zion"),
            init_song("Little dark age", "MGMT"),
            init_song("Pneumonia", "Bjork"),
            init_song("Mercy street", "Peter Gabriel")
        }
    };

    complete_soundtrack[0] = sad_playlist;
    complete_soundtrack[1] = lonely_playlist;
    complete_soundtrack[2] = happy_playlist;
    complete_soundtrack[3] = weird_playlist;
}

/**
* Generate a mood index according to
* mood parameter passed when module is mounted
*/
int get_mood_index(char *mood) {
    int index;

    if(strcmp(mood, "sad") == 0) {
        index = SAD;
    } else if(strcmp(mood, "lonely") == 0) {
        index = LONELY;
    } else if(strcmp(mood, "happy") == 0) {
        index = HAPPY;
    } else if(strcmp(mood, "weird") == 0) {
        index = WEIRD;
    } else {
        index = SAD; // Default
    }

    return index;
}

/**
* Generate a random index from tracks
* For playlist.tracks between 0 and 5
*/
int get_random_track_index(void) {
    int random_bytes, index;

    get_random_bytes(&random_bytes, sizeof(random_bytes));

    // Get a random number between 0 and 5
    index = random_bytes % NUMBER_TRACKS;

    // Calculate absolute value of index
    if(index < 0) {
        index *= -1;
    }

    return index;
}

/**
* Get a random track from selected mood
*/
struct song get_random_track(char *mood) {
    int mood_index, track_index;

    mood_index = get_mood_index(mood);
    track_index = get_random_track_index();
    return complete_soundtrack[mood_index].tracks[track_index];
}
