/*
 * physim.hpp
 *
 *  Created on: Jun 30, 2013
 *      Author: Reuben
 */
#include "SDL/SDL.h"
#include "SDL/SDL_Image.h"
#include "SDL/SDL_ttf.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <ctime>
#include <vector>
#include <headers/vect.hpp>

using namespace std;

SDL_Rect scrdim;
SDL_Surface* scr;
short bpp;

int random(int a,int b)
{
	int d=b-a;
	return a+rand()%d;
}
long double random(long double a,long double b)
{
	long double d=b-a;
	if(d<0)
		d=-d;

	if(log(d)>7)
		return a+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
//(else
		return a+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));
}
vect random(vect a,vect b)
{
	vect c;

	long double d;
	d=b.x-a.x;
			if(d<0)
				d=-d;

			if(log(d)>7)
				c.x=a.x+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
			else
				c.x=a.x+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));
	d=b.y-a.y;
			if(d<0)
				d=-d;

			if(log(d)>7)
				c.y=a.y+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
			else
				c.y=a.y+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));
	d=b.z-a.z;
			if(d<0)
				d=-d;

			if(log(d)>7)
				c.z=a.z+(long double)(rand()%(int)(d/pow(10,log(d)-7)))*pow(10,log(d)-7);
			else
				c.z=a.z+(long double)(rand()%(int)(d*pow(10,7-log(d))))/pow(10,7-log(d));

	return c;
}

char* currentdatetime()
{
	time_t     now = time(0);
	struct tm  tstruct;
	char	buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
	return buf;
}

class DEBUG
{
	int num;
	int max;
	char debug_name[128];
	char description[1000][256];
	char source[1000][128];
	char time[1000][48];
	ofstream log;
public:
	DEBUG(char user_name[128]=NULL)
	{
		if(user_name==NULL)
		{
			strcpy(debug_name,"DEBUG");
		}
		else
			strcpy(debug_name,user_name);
		strcat(debug_name,".txt");
		log.open(debug_name,ios::app);
		log<<"\n"<<currentdatetime()<<"	---------New Execution---------	("<<debug_name<<")\n";
		num=0;
		max=1000;
		for(int i=0;i<max;i++)
		{
			strcpy(description[i],"<empty>");
			strcpy(source[i],"<empty>");
			strcpy(time[i],"<empty>");
		}
	}
	void rename_debugger(char* user_name,char* mode="default")
	{
		strcat(user_name,".txt");
		log.close();
		if(rename(debug_name,user_name)!=-1)
		{
			log.open(user_name,ios::app);
			log<<"\n"<<currentdatetime()<<"	log renamed from "<<debug_name<<" to "<<user_name<<'\n';
			strcpy(debug_name,user_name);
		}
		else if(strcmp(mode,"overwrite")==0)
		{
			remove(user_name);
			if(rename(debug_name,user_name)!=-1)
			{
				log.open(user_name,ios::app);
				log<<"\n"<<currentdatetime()<<"	log renamed from "<<debug_name<<" to "<<user_name<<'\n';
				strcpy(debug_name,user_name);
			}
			else
			{
				log.open(debug_name,ios::app);
				found("DEBUG.rename_debugger()","error renaming log (make sure the file doesn't already exist!)");
			}
		}
		else if(strcmp(mode,"switch")==0)
		{
			remove("deleteme.tmp");
			rename(user_name,"deleteme.tmp");
			rename(debug_name,user_name);
			rename("deleteme.tmp",debug_name);
			log.open(user_name,ios::app);
			log<<"\n"<<currentdatetime()<<"	log renamed from "<<debug_name<<" to "<<user_name<<'\n';
			strcpy(debug_name,user_name);
		}
		else
		{
			log.open(debug_name,ios::app);
			found("DEBUG.rename_debugger()","error renaming log (make sure the file doesn't already exist!)");
		}
	}
	int found(const char user_source[100],const char user_description[256])
	{
		for(int i=0;num<max;i++)
		{
			if(strcmp(description[i],"<empty>")==0)
			{
				strcpy(description[i],user_description);
				strcpy(source[i],user_source);
				strcpy(time[i],currentdatetime());
				num++;
			}
			return num;
		}
		return num;
	}
	int resolved(const char user_source[100],const char user_description[256])
	{
		for(int i=0;i<max;i++)
		{
			if(strcmp(user_description,description[i])==0&&strcmp(user_source,source[i])==0)
			{
				strcpy(source[i],"<empty>");
				strcpy(description[i],"<empty>");
				strcpy(time[i],"<empty>");
				num--;
				return num;
			}
		}
		return num;
	}
	~DEBUG()
	{
		int sno=0;
		for(int i=0;i<max;i++)
		{
			if(strcmp(description[i],"<empty>"))
			{
				log<<'\n'<<++sno<<".\t"<<time[i]<<"\nSource:\t\t"<<source[i]<<"\nDescription:\t"<<description[i]<<'\n';
			}
		}
		log<<"\n"<<currentdatetime()<<"	__________Termination__________\n";
		log.close();
	}
}debugger((char*)"physim.h");

class timer
{
	unsigned long int start_time,cummulative_ticks;
	bool current_state;//0 means stopped, 1 means running
public:
	timer()
	{
		reset();
	}
	void reset()
	{
		cummulative_ticks=0;
		current_state=0;
	}
	void start()
	{
		if(current_state==0)//if it is not running already
		{
			start_time=SDL_GetTicks();
			current_state=1;
		}
	}
	void stop()
	{
		cummulative_ticks+=SDL_GetTicks()-start_time;
		current_state=0;
	}
	unsigned long int elapse()
	{
		if(current_state==0)
			return cummulative_ticks;
		else
			return cummulative_ticks + (SDL_GetTicks()-start_time);
	}
};

class framer : protected timer
{
	unsigned long int count;
	double minfps,maxfps,current_fps;
	double minfreq,maxfreq,currentfreq;
public:
	framer(double user_minfps=10,double user_maxfps=32)
	{
		minfps=user_minfps;
		maxfps=user_maxfps;
		current_fps=(minfps+maxfps)/2;

		minfreq=1000/maxfps;
		maxfreq=1000/minfps;
		currentfreq=1000/current_fps;
		count=0;
	}
	void updatefpslimits(double user_minfps=10,double user_maxfps=30)
	{
		minfps=user_minfps;
		maxfps=user_maxfps;
		current_fps=(minfps+maxfps)/2;

		minfreq=1000/maxfps;
		maxfreq=1000/minfps;
		currentfreq=1000/current_fps;
	}
	void newframe()
	{
	}
	void endframe()
	{
		currentfreq=elapse();
		current_fps=1000/currentfreq;
		reset();
		start();
		count++;
	}
	unsigned long int currentframe()
	{
		return count;
	}
	double currentfrequency()
	{
		return currentfreq;
	}
	double currentfps()
	{
		return current_fps;
	}
	double remainingfreetime()
	{
		if(elapse()<minfreq)
			return double(minfreq)-double(elapse());
		else return 0;
	}
	double elapsed()
	{
		return (double)elapse();
	}
	double deltatime()
	{
		if(currentfreq>maxfreq)
			return maxfreq;
		else if(currentfreq<minfreq)
			return minfreq;
		else
			return currentfreq;
	}
};

SDL_Surface* loadimage(string filename)
{
	SDL_Surface* tex=IMG_Load(filename.c_str());
	if(tex!=NULL)
	{
		tex=SDL_DisplayFormat(tex);
		if(tex!=NULL)
		{
			SDL_SetColorKey(tex,SDL_SRCCOLORKEY,SDL_MapRGB(tex->format,0,0xFF,0xFF));
			if(tex==NULL)
				debugger.found("loadimage()","SDL_SetColorKey() failed");
		}
		else
			debugger.found("loadimage()","SDL_DisplayFormat() failed");
	}
		else
			debugger.found("loadimage()","IMG_Load() failed");
	return tex;
}

void applysurface(SDL_Surface* image,vect pos=(vect){0,0,0} )
{
	if(scr==NULL)
		debugger.found("applysurface()","::scr is pointing to NULL");
	else
	{
		SDL_Rect temp={pos.x,pos.y};
		SDL_BlitSurface(image,NULL,scr,&temp);
	}
}

class PARTICLE
{
	SDL_Surface* tex;
	vect pos,dim,vel,acc;
	bool just_collided;
public:
	void general_construction()
	{
		just_collided=0;
	}
	PARTICLE(vect position,vect dimension, SDL_Surface* user_texture)
	{
		general_construction();
		pos=position;
		dim=dimension;
		tex=user_texture;
		if(tex==NULL)
			debugger.found("PARTICLE()","loadimage() failed");
	}
	PARTICLE(SDL_Surface* user_texture)
	{
		general_construction();
		tex=user_texture;
		if(tex==NULL)
			debugger.found("PARTICLE()","loadimage() failed");
		dim.x=tex->w;
		dim.y=tex->h;
		vect from(0,0,0);
		vect to(scr->w-dim.x,scr->h-dim.y,0);
		pos=random(from,to);
	}
	void trial(SDL_Event eve)
	{

		//If a key was pressed
		if( eve.type == SDL_KEYDOWN )
		{
			//vel.x++;
			//Set the proper message surface
			switch( (int)eve.key.keysym.sym )
			{
				//case SDLK_UP: ge; break;
				//case SDLK_DOWN: message = downMessage; break;
				case SDLK_LEFT: vel.x-=4;break;
				case SDLK_RIGHT: vel.x+=4; break;
			}
			//pos.x+=vel.x;
		}
	}
	vect addvel(vect b)
	{
		vel.add(b);
		return vel;
	}
	vect addacc(vect b)
	{
		acc.add(b);
		return acc;
	}

	void integrate(double deltatime_1000)
	{
		long double deltatime=deltatime_1000/1000.0;
		if(deltatime==0)
			debugger.found("integrate()","deltatimevalue=0");
		vect u=vel;
		vel.add(multiply(acc,deltatime));	//v=u+at
		pos.add(multiply(u,deltatime));		//s=s0+ut
		pos.add(multiply(acc,0.5*deltatime*deltatime));	//s=s0+a*t^2
	}
	int globalcollision(double deltatime_1000)
	{
		long double deltatime=deltatime_1000/1000.0;
		if(pos.y+dim.y+(vel.y+acc.y*deltatime)*deltatime>scrdim.y+scrdim.h)
		{
			long double frac=(vel.y*deltatime+0.5*acc.y*deltatime*deltatime);
			if(frac<0)
				frac=-frac;
			for(int i=0;i<frac;i++)
			{
				integrate(1000.0*deltatime/frac);
				if(pos.y+dim.y>scrdim.y+scrdim.h)
				{
					if(vel.y>0)
					{
						vel.y=-vel.y;
					}
					applysurface(tex,pos);
				}
			}
			return just_collided=1;
		}
		else if(pos.x+dim.x+(vel.x+acc.x*deltatime)*deltatime>scrdim.x+scrdim.w)
		{
			long double frac=(vel.x*deltatime+0.5*acc.x*deltatime*deltatime);
			for(int i=0;i<frac;i++)
			{
				integrate(1000.0*deltatime/frac);
				if(pos.x+dim.x>scrdim.x+scrdim.w)
				{
					if(vel.x>0)
					{
						vel.x=-vel.x;
					}
					applysurface(tex,pos);
				}
			}
			return just_collided=1;
		}
		else if(pos.y+(vel.y+acc.y*deltatime)*deltatime<scrdim.y)
		{
			long double frac=(vel.y*deltatime+0.5*acc.y*deltatime*deltatime);
			if(frac<0)
				frac=-frac;
			for(int i=0;i<frac;i++)
			{
				integrate(1000.0*deltatime/frac);
				if(pos.y<scrdim.y)
				{
					if(vel.y<0)
					{
						vel.y=-vel.y;
					}
					applysurface(tex,pos);
				}
			}
			return 1;
		}
		else if(pos.x+(vel.x+acc.x*deltatime)*deltatime<scrdim.x)
		{
			long double frac=-(vel.x*deltatime+0.5*acc.x*deltatime*deltatime);
			if(frac<0)
				frac=-frac;
			for(int i=0;i<frac;i++)
			{
				integrate(1000.0*deltatime/frac);
				if(pos.x<scrdim.x)
				{
					if(vel.x<0)
					{
						vel.x=-vel.x;
					}
					applysurface(tex,pos);
				}
			}
			return just_collided=1;
		}
		return just_collided=0;
	}
	bool justcollided()
	{
		return just_collided;
	}

	void display(SDL_Surface* screen)
	{
		applysurface(tex,pos);
	}

	~PARTICLE()
	{
		SDL_FreeSurface(tex);
	}
};


class PHYSIM
{
	SDL_Surface* scr;
	SDL_Rect scrdim;
	PARTICLE* handler;
	int N;
	void frametermination()
	{
		SDL_Delay(frametimer.remainingfreetime());
		frametimer.endframe();
	}
	void general_genparticle()
	{
		particle.push_back(handler);
		N++;
	}
public:
	vector<PARTICLE*> particle;
	int bpp;
	DEBUG* error;
	timer runtime;
	framer frametimer;
	bool ended;
	PHYSIM(SDL_Rect user_dim,int user_bpp=32)
	{
		runtime.start();
		error=new DEBUG((char*)"psm");
		handler=NULL;
		N=0;
		if(SDL_Init(SDL_INIT_EVERYTHING)==-1)
			error->found((char*)"PHYSIM()",(char*)"SDL_Init() failed");
		srand(SDL_GetTicks());
		bpp=user_bpp;
		::scrdim=scrdim=user_dim;
		::scr=scr=SDL_SetVideoMode(scrdim.w,scrdim.h,bpp,SDL_SWSURFACE|SDL_RESIZABLE);
		frametimer.currentfrequency();
		ended=false;
	}

	PARTICLE* genparticle(SDL_Surface* user_texture)
	{
		handler=new PARTICLE(user_texture);
		general_genparticle();
		return handler;
	}
	PARTICLE* genparticle(vect position,vect dimension, SDL_Surface* user_texture)
	{
		handler=new PARTICLE(position,dimension,user_texture);
		general_genparticle();
		return handler;
	}
	void initiateframe()
	{
		frametimer.newframe();
	}
	void terminateframe(SDL_Color user_color)
	{
		SDL_Flip(scr);
		SDL_FillRect(scr,&scr->clip_rect,SDL_MapRGB(scr->format,user_color.r,user_color.g,user_color.b));
		frametermination();
	}
	void terminateframe(SDL_Surface* user_background)
	{
		SDL_Flip(scr);
		applysurface(user_background);
		frametermination();
	}
	~PHYSIM()
	{
		ofstream allo("allocation log.txt");
		allo<<particle.size()-1<<" particles to delete:"<<"\n";
		allo.close();
		for(int i=particle.size()-1;i>=0;i--)
		{
			handler=particle[i];
			allo.open("allocation log.txt",ios::app);
			allo<<"deleting	"<<handler<<"\n";
			allo.close();
			delete handler;
		}
		SDL_FreeSurface(scr);
		SDL_Quit();
		delete error;
	}
};
