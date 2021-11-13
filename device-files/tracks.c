#include <linux/random.h>
#include <linux/string.h>

#define ROW 4
#define COL 6
#define SAD 0
#define LONELY 1
#define HAPPY 2
#define WEIRD 3

// Variables
static char *mood_matrix[ROW][COL][2] = {
    // sad
    {
        {"Where is my love?", "Cat power"},
        {"Space song", "Beach house"},
        {"Come sweet death", "The end of evangelion"},
        {"Jueves", "La oreja de van gogh"},
        {"Alma mia", "Silvio Rodriquez"},
        {"Fireworks", "Stina Nordenstam"}
    },
    // lonely
    {
        {"Don't give up", "Peter Gabriel"},
        {"Thank you for hearing me", "Sinead o'connor"},
        {"Dimentions", "Her OST"},
        {"The moon song", "Karen O"},
        {"Asleep", "The smiths"},
        {"Eclipse de luna en el cielo", "Natalia lafourcade"}
    },
    // happy
    {
        {"Over the rainbow", "Judy garland"},
        {"Come on eileen", "Dexis midnight runners"},
        {"Sea of love", "Cat power"},
        {"I'll try anything once", "The strokes"},
        {"Starry, starry night", "Don McLean"},
        {"For lovers", "Lamp"}
    },
    // weird
    {
        {"Creep", "Radiohead"},
        {"Weird fishes", "Radiohead"},
        {"13 angels standing guard 'round the side of your bed", "Silver Mt Zion"},
        {"Little dark age", "MGMT"},
        {"Pneumonia", "Bjork"},
        {"Mercy street", "Peter gabriel"}
    }
};


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

int get_random_track_index(void) {
    int random_bytes, index;

    get_random_bytes(&random_bytes, sizeof(random_bytes));

    // Get a random number between 0 and 5
    index = random_bytes % 6;

    // Calculate absolute value of index
    if(index < 0) {
        index *= -1;
    }

    return index;
}

char **get_random_track(char *mood) {
    int mood_index, track_index;

    mood_index = get_mood_index(mood);
    track_index = get_random_track_index();
    return mood_matrix[mood_index][track_index];
}
