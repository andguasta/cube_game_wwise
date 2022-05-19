// worldio.cpp: loading & saving of maps and savegames

#include "cube.h"

void backup(char *name, char *backupname)
{
    remove(backupname);
    rename(name, backupname);
};

string cgzname, bakname, pcfname, mcfname, bnkname; 

void setnames(char *name)
{
    string pakname, mapname;
    char *slash = strpbrk(name, "/\\");
    if(slash)
    {
        strn0cpy(pakname, name, slash-name+1);
        strcpy_s(mapname, slash+1);
		sprintf_s(bnkname)("%s.bnk", pakname);
    }
    else
    {
        strcpy_s(pakname, "base");
        strcpy_s(mapname, name);
		sprintf_s(bnkname)("");
    };
	sprintf_s(cgzname)(I_PATH "packages" S_PATH "%s" S_PATH "%s.cgz",      pakname, mapname);
    sprintf_s(bakname)(I_PATH "packages" S_PATH "%s" S_PATH "%s_%d.BAK",   pakname, mapname, lastmillis);
    sprintf_s(pcfname)(I_PATH "packages" S_PATH "%s" S_PATH "package.cfg", pakname);
    sprintf_s(mcfname)(I_PATH "packages" S_PATH "%s" S_PATH "%s.cfg",      pakname, mapname);

    path(cgzname);
    path(bakname);
};

// the optimize routines below are here to reduce the detrimental effects of messy mapping by
// setting certain properties (vdeltas and textures) to neighbouring values wherever there is no
// visible difference. This allows the mipmapper to generate more efficient mips.
// the reason it is done on save is to reduce the amount spend in the mipmapper (as that is done
// in realtime).

inline bool nhf(sqr *s) { return s->type!=FHF && s->type!=CHF; };

void voptimize()        // reset vdeltas on non-hf cubes
{
    loop(x, ssize) loop(y, ssize)
    {
        sqr *s = S(x, y);
        if(x && y) { if(nhf(s) && nhf(S(x-1, y)) && nhf(S(x-1, y-1)) && nhf(S(x, y-1))) s->vdelta = 0; }
        else s->vdelta = 0;
    };
};

void topt(sqr *s, bool &wf, bool &uf, int &wt, int &ut)
{
    sqr *o[4];
    o[0] = SWS(s,0,-1,ssize);
    o[1] = SWS(s,0,1,ssize);
    o[2] = SWS(s,1,0,ssize);
    o[3] = SWS(s,-1,0,ssize);
    wf = true;
    uf = true;
    if(SOLID(s))
    {
        loopi(4) if(!SOLID(o[i]))
        {
            wf = false;
            wt = s->wtex;
            ut = s->utex;
            return;
        };
    }
    else
    {
        loopi(4) if(!SOLID(o[i]))
        {
            if(o[i]->floor<s->floor) { wt = s->wtex; wf = false; };
            if(o[i]->ceil>s->ceil)   { ut = s->utex; uf = false; };
        };
    };
};

void toptimize() // FIXME: only does 2x2, make atleast for 4x4 also
{
    bool wf[4], uf[4];
    sqr *s[4];
    for(int x = 2; x<ssize-4; x += 2) for(int y = 2; y<ssize-4; y += 2)
    {
        s[0] = S(x,y);
        int wt = s[0]->wtex, ut = s[0]->utex;
        topt(s[0], wf[0], uf[0], wt, ut);
        topt(s[1] = SWS(s[0],0,1,ssize), wf[1], uf[1], wt, ut);
        topt(s[2] = SWS(s[0],1,1,ssize), wf[2], uf[2], wt, ut);
        topt(s[3] = SWS(s[0],1,0,ssize), wf[3], uf[3], wt, ut);
        loopi(4)
        {
            if(wf[i]) s[i]->wtex = wt;
            if(uf[i]) s[i]->utex = ut;
        };
    };
};

// these two are used by getmap/sendmap.. transfers compressed maps directly 

void writemap(char *mname, int msize, uchar *mdata)
{
    setnames(mname);
    backup(cgzname, bakname);
    FILE *f = fopen(cgzname, "wb");
    if(!f) { conoutf("could not write map to %s", cgzname); return; };
    fwrite(mdata, 1, msize, f);
    fclose(f);
    conoutf("wrote map %s as file %s", mname, cgzname);
}

uchar *readmap(char *mname, int *msize)
{
    setnames(mname);
    uchar *mdata = (uchar *)loadfile(cgzname, msize);
    if(!mdata) { conoutf("could not read map %s", cgzname); return NULL; };
    return mdata;
}

// save map as .cgz file. uses 2 layers of compression: first does simple run-length
// encoding and leaves out data for certain kinds of cubes, then zlib removes the
// last bits of redundancy. Both passes contribute greatly to the miniscule map sizes.

void save_world(char *mname)
{
    resettagareas();    // wouldn't be able to reproduce tagged areas otherwise
    voptimize();
    toptimize();
    if(!*mname) mname = getclientmap();
    setnames(mname);
    backup(cgzname, bakname);
    gzFile f = gzopen(cgzname, "wb9");
    if(!f) { conoutf("could not write map to %s", cgzname); return; };
    hdr.version = MAPVERSION;
    hdr.numents = 0;
    loopv(ents) if(ents[i].type!=NOTUSED) hdr.numents++;
    header tmp = hdr;
    endianswap(&tmp.version, sizeof(int), 4);
    endianswap(&tmp.waterlevel, sizeof(int), 16);
    gzwrite(f, &tmp, sizeof(header));
    loopv(ents) 
    {
        if(ents[i].type!=NOTUSED) 
        {
            entity tmp = ents[i];
            endianswap(&tmp, sizeof(short), 4);
            gzwrite(f, &tmp, sizeof(persistent_entity));
        };
    };
    sqr *t = NULL;
    int sc = 0;
    #define spurge while(sc) { gzputc(f, 255); if(sc>255) { gzputc(f, 255); sc -= 255; } else { gzputc(f, sc); sc = 0; } };
    loopk(cubicsize)
    {
        sqr *s = &world[k];
        #define c(f) (s->f==t->f)
        // 4 types of blocks, to compress a bit:
        // 255 (2): same as previous block + count
        // 254 (3): same as previous, except light // deprecated
        // SOLID (5)
        // anything else (9)

        if(SOLID(s))
        {
            if(t && c(type) && c(wtex) && c(vdelta))
            {
                sc++;
            }
            else
            {
                spurge;
                gzputc(f, s->type);
                gzputc(f, s->wtex);
                gzputc(f, s->vdelta);
                gzputc(f, s->env);
            };
        }
        else
        {
            if(t && c(type) && c(floor) && c(ceil) && c(ctex) && c(ftex) && c(utex) && c(wtex) && c(vdelta) && c(tag))
            {
                sc++;
            }
            else
            {
                spurge;
                gzputc(f, s->type);
                gzputc(f, s->floor);
                gzputc(f, s->ceil);
                gzputc(f, s->wtex);
                gzputc(f, s->ftex);
                gzputc(f, s->ctex);
                gzputc(f, s->vdelta);
                gzputc(f, s->utex);
                gzputc(f, s->tag);
                gzputc(f, s->env);
            };
        };
        t = s;
    };
    spurge;
    gzclose(f);
    conoutf("wrote map file %s", cgzname);
    settagareas();
};

void load_world(char *mname)        // still supports all map formats that have existed since the earliest cube betas!
{
    stopifrecording();
    cleardlights();
    pruneundos();
	snd_unloadmapbank();
    setnames(mname);
	snd_loadmapbank();
	purgetextures();

	gzFile f = gzopen(cgzname, "rb9");
	
    if(!f) { conoutf("could not read map %s", cgzname); return; };
    gzread(f, &hdr, sizeof(header)-sizeof(int)*16);
    endianswap(&hdr.version, sizeof(int), 4);
    if(strncmp(hdr.head, "CUBE", 4)!=0) fatal("while reading map: header malformatted");
    if(hdr.version>MAPVERSION) fatal("this map requires a newer version of cube");
    if(sfactor<SMALLEST_FACTOR || sfactor>LARGEST_FACTOR) fatal("illegal map size");
    if(hdr.version>=4)
    {
        gzread(f, &hdr.waterlevel, sizeof(int)*16);
        endianswap(&hdr.waterlevel, sizeof(int), 16);
    }
    else
    {
        hdr.waterlevel = -100000;
    };
	loopv(ents)
    {
        entity &e = ents[i];
		if(e.type==TELEPORT)
		{
			snd_stopall( (void *) i );
			snd_unregistptr( (void *) i );
		}
	}
	ents.setsize(0);
    loopi(hdr.numents)
    {
        entity &e = ents.add();
        gzread(f, &e, sizeof(persistent_entity));
        endianswap(&e, sizeof(short), 4);
        e.spawned = false;
        if(e.type==LIGHT)
        {
            if(!e.attr2) e.attr2 = 255;  // needed for MAPVERSION<=2
            if(e.attr1>32) e.attr1 = 32; // 12_03 and below
        }
    };
    free(world);
    setupworld(hdr.sfactor);
	char texuse[256];
	loopi(256) texuse[i] = 0;
    sqr *t = NULL;
    loopk(cubicsize)
    {
        sqr *s = &world[k];
        int type = gzgetc(f);
        switch(type)
        {
            case 255:  
            {
                int n = gzgetc(f);
                for(int i = 0; i<n; i++, k++) memcpy(&world[k], t, sizeof(sqr));
                k--;
                break;
            };
            case 254: // only in MAPVERSION<=2
            {
                memcpy(s, t, sizeof(sqr));
                s->r = s->g = s->b = gzgetc(f);
                gzgetc(f);
                break;
            };
            case SOLID:
            {
                s->type = SOLID;
                s->wtex = gzgetc(f);
                s->vdelta = gzgetc(f);
                if(hdr.version<=2) { gzgetc(f); gzgetc(f); };
                s->ftex = DEFAULT_FLOOR;
                s->ctex = DEFAULT_CEIL;
                s->utex = s->wtex;
                s->tag = 0;
                s->floor = 0;
                s->ceil = 16;
                s->env = (hdr.version>=6) ? gzgetc(f) : 0;
                break;
            };
            default:
            {
                if(type<0 || type>=MAXTYPE)
                {
                    sprintf_sd(t)("%d @ %d", type, k);
                    fatal("while reading map: type out of range: ", t);
                };
                s->type = type;
                s->floor = gzgetc(f);
                s->ceil = gzgetc(f);
                if(s->floor>=s->ceil) s->floor = s->ceil-1;  // for pre 12_13
                s->wtex = gzgetc(f);
                s->ftex = gzgetc(f);
                s->ctex = gzgetc(f);
                if(hdr.version<=2) { gzgetc(f); gzgetc(f); };
                s->vdelta = gzgetc(f);
                s->utex = (hdr.version>=2) ? gzgetc(f) : s->wtex;
                s->tag = (hdr.version>=5) ? gzgetc(f) : 0;
                s->type = type;
                s->env = (hdr.version>=6) ? gzgetc(f) : 0;
            };
        };
        s->defer = 0;
        t = s;
        texuse[s->wtex] = 1;
        if(!SOLID(s)) texuse[s->utex] = texuse[s->ftex] = texuse[s->ctex] = 1;
    };
    gzclose(f);
    calclight();
    settagareas();
    int xs, ys;
    loopi(256) if(texuse) lookuptexture(i, xs, ys);

	loopv(ents)
    {
        entity &e = ents[i];
		if(e.type==TELEPORT)
		{
			char name[256];
			sprintf( name, "Teleport (Entity #%i)", i );

			snd_registptr( (void *) i, name );
			snd_setpos( (void *) i, e.x, e.y, S(e.x, e.y)->floor );
			snd_event( AK::EVENTS::EMITTER_TELEPORT, (void *) i );
		}
	}

    conoutf("read map %s (%d milliseconds)", cgzname, Cube_GetTicks()-lastmillis);
	conoutf("%s", hdr.maptitle);
    startmap(mname);
    loopl(256)
    {
        sprintf_sd(aliasname)("level_trigger_%d", l);     // can this be done smarter?
        if(identexists(aliasname)) alias(aliasname, "");
    };

	execfile(I_PATH "data" S_PATH "default_map_settings.cfg");
    execfile(pcfname);
    execfile(mcfname);
};

COMMANDN(savemap, save_world, ARG_1STR);

