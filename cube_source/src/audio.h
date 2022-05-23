// Additional functions for the audio part of the game by Andrea Guastadisegni

enum MonsterName
{
    OGRE,
    RHINO,
    RAT,
    SLITH,
    BAUUL,
    HELLPIG,
    KNIGHT,
    GOBLIN
};

/* char* gunsName[] = {"fist", "shotgun", "chaingun", "rocket", "rifle"}; */


void snd_setswitch (char* s_group, char* s_name, dynent* g_object);

char* getWeaponName(int gunNumber);