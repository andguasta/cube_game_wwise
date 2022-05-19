// protos for ALL external functions in cube... 

// initialization
extern bool Cube_Init();
extern bool Cube_Tick();
extern void Cube_Term();
extern void Cube_Arg( char * arg );

// command
extern int variable(const char *name, int min, int cur, int max, int *storage, void (*fun)(), bool persist);
extern void setvar(char *name, int i);
extern int getvar(char *name);
extern bool identexists(char *name);
extern bool addcommand(const char *name, void (*fun)(), int narg);
extern int execute(char *p, bool down = true);
extern void exec(char *cfgfile);
extern bool execfile(char *cfgfile);
extern void resetcomplete();
extern void complete(char *s);
extern void alias(char *name, char *action);
extern char *getalias(char *name);
extern void writecfg();
extern void itoa(char *s, int i);
extern dynent *basicmonster(int type, int yaw, int state, int trigger, int move);
// console
extern void charpress( char ch );
extern bool keypress(int code, bool isdown); // return true if processed
extern void renderconsole();
extern void conoutf(const char *s, ...);
extern char *getcurcommand();
extern void writebinds(FILE *f);

// menus
extern bool rendermenu();
extern void newmenu(char *name);
extern void sortmenu(int start, int num);
extern void menuset(int menu);
extern void menumanual(int m, int n, char *text);
extern bool menukey(int code, bool isdown);
extern void showmenu(char *name);

// serverbrowser
extern void addserver(char *servername);
extern char *getservername(int n);
extern void writeservercfg();

// rendergl
extern void gl_init(int w, int h);
extern void cleangl();
extern void gl_drawframe(int w, int h, float curfps);
extern bool installtex(int tnum, char *texname, int &xs, int &ys, bool clamp = false);
extern void mipstats(int a, int b, int c);
extern void vertf(float v1, float v2, float v3, sqr *ls, float t1, float t2);
extern void addstrip(int tex, int start, int n);
extern int lookuptexture(int tex, int &xs, int &ys);
extern int lookupmaterial( int tex );
extern void purgetextures();

// rendercubes
extern void resetcubes();
extern void render_flat(int tex, int x, int y, int size, int h, sqr *l1, sqr *l2, sqr *l3, sqr *l4, bool isceil);
extern void render_flatdelta(int wtex, int x, int y, int size, float h1, float h2, float h3, float h4, sqr *l1, sqr *l2, sqr *l3, sqr *l4, bool isceil);
extern void render_square(int wtex, float floor1, float floor2, float ceil1, float ceil2, int x1, int y1, int x2, int y2, int size, sqr *l1, sqr *l2, bool topleft);
extern void render_tris(int x, int y, int size, bool topleft, sqr *h1, sqr *h2, sqr *s, sqr *t, sqr *u, sqr *v);
extern void addwaterquad(int x, int y, int size);
extern int renderwater(float hf);
extern void finishstrips();
extern void setarraypointers();

// client
extern void connects(char *servername);
extern void sendpackettoserv(void *packet);
extern void changemapserv(char *name, int mode);
extern bool multiplayer();
extern void updatepos(dynent *d);

extern void disconnect(int onlyclean = 0, int async = 0);
extern bool netmapstart();
extern bool allowedittoggle();
extern void toserver(char *text);
extern void localservertoclient(uchar *buf, int len);
extern void writeclientinfo(FILE *f);
extern void addmsg(int rel, int num, int type, ...);
extern void c2sinfo(dynent *d);
extern int getclientnum();
extern void neterr(char *s);
extern void gets2c();
extern void initclientnet();

// clientgame
extern void mousemove(int dx, int dy); 
extern void updateworld(float millis);
extern void startmap(char *name);
extern void initclient();
extern void spawnplayer(dynent *d);
extern void selfdamage(int damage, int actor, dynent *act);
extern dynent *newdynent( int in_state, char * in_name = NULL );
extern char *getclientmap();
extern void zapdynent(dynent *&d);
extern dynent *getclient(int cn);
extern void timeupdate(int timeremain);
extern void resetmovement(dynent *d);
extern void fixplayer1range();

extern const char *modestr(int n);
extern void changemap(char *name);

// clientextras
extern void renderclients();
extern void renderclient(dynent *d, bool team, char *mdlname, bool hellpig, float scale);
void showscores(bool on);
extern void renderscores();

// world
extern void setupworld(int factor);
extern void empty_world(int factor, bool force);
extern void remip(block &b, int level = 0);
extern void remipmore(block &b, int level = 0);
extern int closestent();
extern int findentity(int type, int index = 0);
extern void trigger(int tag, int type, bool savegame);
extern void resettagareas();
extern void settagareas();
extern entity *newentity(int x, int y, int z, char *what, int v1, int v2, int v3, int v4);

// worldlight
extern void calclight();
extern void dodynlight(vec &vold, vec &v, int reach, int strength, dynent *owner);
extern void cleardlights();
extern block *blockcopy(block &b);
extern void blockpaste(block &b);

// worldrender
extern void render_world(float vx, float vy, float vh, int yaw, int pitch, float widef, int w, int h);

// worldocull
extern void computeraytable(float vx, float vy);
extern int isoccluded(float vx, float vy, float cx, float cy, float csize);

// main
extern void fatal(const char *s, const char *o = "");
extern void *alloc(int s);

// rendertext
extern void draw_text(char *str, int left, int top, int gl_num);
extern void draw_textf(char *fstr, int left, int top, int gl_num, ...);
extern int text_width(char *str);
extern void draw_envbox(int t, int fogdist);

// editing
extern void cursorupdate();
extern void toggleedit();
extern void editdrag(bool isdown);
extern void setvdeltaxy(int delta, block &sel);
extern void editequalisexy(bool isfloor, block &sel);
extern void edittypexy(int type, block &sel);
extern void edittexxy(int type, int t, block &sel);
extern void editheightxy(bool isfloor, int amount, block &sel);
extern bool noteditmode();
extern void pruneundos(int maxremain = 0);

// renderextras
extern void line(int x1, int y1, float z1, int x2, int y2, float z2);
extern void box(block &b, float z1, float z2, float z3, float z4);
extern void dot(int x, int y, float z);
extern void linestyle(float width, int r, int g, int b);
extern void newsphere(vec &o, float max, int type);
extern void renderspheres(float time);
extern void gl_drawhud(int w, int h, int curfps, int nquads, int curvert, bool underwater);
extern void readdepth(int w, int h);
extern void blendbox(int x1, int y1, int x2, int y2, bool border);
extern void damageblend(int n);

// renderparticles
extern void setorient(vec &r, vec &u);
extern void particle_splash(int type, int num, int fade, vec &p);
extern void particle_trail(int type, int fade, vec &from, vec &to);
extern void render_particles(float time);

// worldio
extern void save_world(char *fname);
extern void load_world(char *mname);
extern void writemap(char *mname, int msize, uchar *mdata);
extern uchar *readmap(char *mname, int *msize);
extern void loadgamerest();
extern void incomingdemodata(uchar *buf, int len, bool extras = false);
extern void demoplaybackstep();
extern void stop();
extern void stopifrecording();
extern void demodamage(int damage, vec &o);
extern void demoblend(int damage);

// physics
extern void moveplayer(dynent *pl, int moveres, bool local);
extern bool collide(dynent *d, bool spawn, float drop, float rise);
extern void entinmap(dynent *d);
extern void setentphysics(int mml, int mmr);
extern void physicsframe();

// sound

// initialize sound engine
extern void snd_init();

// terminate sound engine
extern void snd_term();

// entity registration as game object in audio engine
extern void snd_registent( baseent * ent, char * in_name = NULL );
extern void snd_unregistent( baseent * ent );

// generic ptr registration as game object in audio engine
extern void snd_registptr( void * ent, char * in_name = NULL );
extern void snd_unregistptr( void * ent );

// post a client event (attached to player, replicated to network peers)
extern void snd_clientevent(int event);

// set a wwise state
extern void snd_state(int stategroup, int state);

// set a wwise game parameter (at global scope)
extern void snd_gameparam(int param, float value);

// post an event attached to the specified location in world
extern void snd_event(int event, vec *loc = 0);
extern void snd_event(char * name, vec *loc = 0);

// post an event attached to the specified entity
extern void snd_event(int event, void * ent );
extern void snd_event(char * name, void * ent );

// set the position of a non-dynamic entity
extern void snd_setpos( void * ent, float x, float y, float z );

// stop all sounds playing on entity
extern void snd_stopall( void * ent );

// update sound engine (called from main loop, once per frame)
extern void snd_update();

// post a message in the wwise capture list
extern void snd_message( char * szMessage );

// query material id from name
extern int  snd_getmaterialid( char * name );

// set current material id of entity
extern void snd_setmaterial( void * ent, int materialid );

// set current side of foot
extern void snd_setfoot( void * ent, bool right );

// load map-specific bank file. assumes map loading code sets bnkname to the correct file path.
extern void snd_loadmapbank();

// unload map-specific bank file
extern void snd_unloadmapbank();


// rendermd2
extern void rendermodel(char *mdl, int frame, int range, int tex, float rad, float x, float y, float z, float yaw, float pitch, bool teammate, float scale, float speed, int snap = 0, float basetime = 0, dynent * ent = NULL, bool hellpig = false);
extern mapmodelinfo *getmminfo(int i);

// server
extern void initserver(bool dedicated, int uprate, char *sdesc, char *ip, char *master, char *passwd, int maxcl);
extern void cleanupserver();
extern void localconnect();
extern void localdisconnect();
extern void localclienttoserver(struct _ENetPacket *);
extern void serverslice(int seconds, unsigned int timeout);
extern int getint(uchar *&p);
extern void sendstring(char *t, uchar *&p);
extern char msgsizelookup(int msg);
extern void serverms(int mode, int numplayers, int minremain, char *smapname, int seconds, bool isfull);
extern void servermsinit(const char *master, char *sdesc, bool listen);
extern void sendmaps(int n, string mapname, int mapsize, uchar *mapdata);

extern void putint(uchar *&p, int n);
extern void startintermission();
extern void restoreserverstate(vector<entity> &ents);
extern uchar *retrieveservers(uchar *buf, int buflen);
extern ENetPacket *recvmap(int n);

// weapon
extern void selectgun(int a = -1, int b = -1, int c =-1);
extern void shoot(dynent *d, vec &to);
extern void shootv(int gun, vec &from, vec &to, dynent *d = 0, bool local = false);
extern void createrays(vec &from, vec &to);
extern void moveprojectiles(float time);
extern void projreset();
extern char *playerincrosshair();
extern int reloadtime(int gun);

// monster
extern void monsterclear();
extern void restoremonsterstate();
extern void monsterthink();
extern void monsterrender();
extern void monsterfootstep( dynent * m, bool right );
extern dvector &getmonsters();
extern void monsterpain(dynent *m, int damage, dynent *d);
extern void endsp(bool allkilled);

// entities
extern void renderents();
extern void putitems(uchar *&p);
extern void checkquad(float time);
extern void checkitems();
extern void realpickup(int n, dynent *d);
extern void renderentities();
extern void resetspawns();
extern void setspawn(uint i, bool on);
extern void teleport(int n, dynent *d);
extern void baseammo(int gun);

// rndmap
extern void perlinarea(block &b, int scale, int seed, int psize);

