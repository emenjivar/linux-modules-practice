// Global structures
struct song {
    char *name;
    char *author;
};

// Prototypes of public functions
void load_songs(void);
struct song get_random_track(char *mood);
