// clientextras.cpp: stuff that didn't fit in client.cpp or clientgame.cpp :)

#include "cube.h"

// render players & monsters
// very messy ad-hoc handling of animation frames, should be made more configurable

//              D    D    D    D'   D    D    D    D'   A   A'  P   P'  I   I' R,  R'  E    L    J   J'
int frame[] = { 178, 184, 190, 137, 183, 189, 197, 164, 46, 51, 54, 32, 0,  0, 40, 1,  162, 162, 67, 168 };
int range[] = { 6,   6,   8,   28,  1,   1,   1,   1,   8,  19, 4,  18, 40, 1, 6,  15, 1,   1,   1,  1   };

void renderclient(dynent *d, bool team, char *mdlname, bool hellpig, float scale)
{
    int n = 3;
    float speed = 100.0f;
    float mz = d->o.z-d->eyeheight+1.55f*scale;
    int basetime = -((AkIntPtr)d&0xFFF);
    if(d->state==CS_DEAD)
    {
        int r;
        if(hellpig) { n = 2; r = range[3]; } else { n = (AkIntPtr)d%3; r = range[n]; };
        basetime = (int) d->lastaction;
        int t = (int) ( lastmillis-d->lastaction );
        if(t<0 || t>20000) return;
        if(t>(r-1)*100) { n += 4; if(t>(r+10)*100) { t -= (r+10)*100; mz -= t*t/10000000000.0f*t; }; };
        if(mz<-1000) return;
    }
    else if(d->state==CS_EDITING)					n = 16; 
    else if(d->state==CS_LAGGED)					n = 17; 
    else if(d->GetMonsterState()==M_ATTACKING)		n = 8;  
	else if (d->GetMonsterState() == M_PAIN)		n = 10;
    else if((!d->move && !d->strafe) || !d->moving)	n = 12; 
    else if(!d->onfloor && d->timeinair>100)		n = 18; 
    else
	{ 
		n = 14; 
		speed = 1200/d->maxspeed*scale; 
		if(hellpig) speed = 300/d->maxspeed;  
	}; 
    if(hellpig) { n++; scale *= 32; mz -= 1.9f; };
    rendermodel(mdlname, frame[n], range[n], 0, 1.5f, d->o.x, mz, d->o.y, d->yaw+90, d->pitch/2, team, scale, speed, 0, (float)basetime, d, hellpig);
};

extern int democlientnum;

void renderclients()
{
	// local player
	// we don't actually render its model, so we need to handle the footsteps separately
	if ( ( player1->move || player1->strafe ) 
		&& ( player1->onfloor || player1->timeinair<=100 ) )
	{
		// see above table
		int frame = 40, range = 6;

		int basetime = -((AkIntPtr)player1&0xFFF);
		float speed = 1200/player1->maxspeed; 
		int time = (int)lastmillis-basetime;
		int fr1 = (int)(time/speed);
		fr1 = fr1%range+frame;
		int fr2 = fr1+1;
		if(fr2>=frame+range) fr2 = frame;

		if ( player1->lastanimframe != fr2 )
		{
			if ( fr2 == 41 // left foot
				|| fr2 == 44 ) // right foot )
			{
				monsterfootstep( player1, fr2 == 44 );
			}

			player1->lastanimframe = fr2;
		}
	}

	// network players

    loopv(players) 
	{
	    dynent *d;
		if((d = players[i]) && (!demoplayback || i!=democlientnum)) 
			renderclient(d, isteam(player1->team, d->team), "monster/ogro", false, 1.0f);
	}
};

// creation of scoreboard pseudo-menu

bool scoreson = false;

void showscores(bool on)
{
    scoreson = on;
	menuset(((int)on)-1);
};

struct sline { string s; };
vector<sline> scorelines;

void renderscore(dynent *d)
{
    sprintf_sd(lag)("%d", d->plag);
    sprintf_sd(name) ("(%s)", d->name); 
    sprintf_s(scorelines.add().s)("%d\t%s\t%d\t%s\t%s", d->frags, d->state==CS_LAGGED ? "LAG" : lag, d->ping, d->team, d->state==CS_DEAD ? name : d->name);
	menumanual(0, scorelines.length()-1, scorelines.last().s); 
};

const int maxteams = 4;
char *teamname[maxteams];
int teamscore[maxteams], teamsused;
string teamscores;
int timeremain = 0;

void addteamscore(dynent *d)
{
    if(!d) return;
    loopi(teamsused) if(strcmp(teamname[i], d->team)==0) { teamscore[i] += d->frags; return; };
    if(teamsused==maxteams) return;
    teamname[teamsused] = d->team;
    teamscore[teamsused++] = d->frags;
};

void renderscores()
{
    if(!scoreson) return;
    scorelines.setsize(0);
    if(!demoplayback) renderscore(player1);
    loopv(players) if(players[i]) renderscore(players[i]);
	sortmenu(0, scorelines.length());
    if(m_teammode)
    {
        teamsused = 0;
        loopv(players) addteamscore(players[i]);
        if(!demoplayback) addteamscore(player1);
        teamscores[0] = 0;
        loopj(teamsused)
        {
            sprintf_sd(sc)("[ %s: %d ]", teamname[j], teamscore[j]);
            strcat_s(teamscores, sc);
        };
        menumanual(0, scorelines.length(), "");
        menumanual(0, scorelines.length()+1, teamscores);
    };
};

// sendmap/getmap commands, should be replaced by more intuitive map downloading

void sendmap(char *mapname)
{
    if(*mapname) save_world(mapname);
    changemap(mapname);
    mapname = getclientmap();
    int mapsize;
    uchar *mapdata = readmap(mapname, &mapsize); 
    if(!mapdata) return;
    ENetPacket *packet = enet_packet_create(NULL, MAXTRANS + mapsize, ENET_PACKET_FLAG_RELIABLE);
    uchar *start = packet->data;
    uchar *p = start+2;
    putint(p, SV_SENDMAP);
    sendstring(mapname, p);
    putint(p, mapsize);
    if(65535 - (p - start) < mapsize)
    {
        conoutf("map %s is too large to send", mapname);
        free(mapdata);
        enet_packet_destroy(packet);
        return;
    };
    memcpy(p, mapdata, mapsize);
    p += mapsize;
    free(mapdata); 
    *(ushort *)start = ENET_HOST_TO_NET_16(p-start);
    enet_packet_resize(packet, p-start);
    sendpackettoserv(packet);
    conoutf("sending map %s to server...", mapname);
    sprintf_sd(msg)("[map %s uploaded to server, \"getmap\" to receive it]", mapname);
    toserver(msg);
}

void getmap()
{
    ENetPacket *packet = enet_packet_create(NULL, MAXTRANS, ENET_PACKET_FLAG_RELIABLE);
    uchar *start = packet->data;
    uchar *p = start+2;
    putint(p, SV_RECVMAP);
    *(ushort *)start = ENET_HOST_TO_NET_16(p-start);
    enet_packet_resize(packet, p-start);
    sendpackettoserv(packet);
    conoutf("requesting map from server...");
}

COMMAND(sendmap, ARG_1STR);
COMMAND(getmap, ARG_NONE);

