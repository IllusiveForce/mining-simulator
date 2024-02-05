/*
  mining simulator
  programmed by doyoung kim
  code here may be cognitohazardous
*/
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include <fstream>
#include <stdlib.h>
#include <algorithm>

#define depth 1200
#define testicle int

//i dont really need to see these functions so i made them all one line
char* convertIntegerToChar(testicle N) { testicle m = N;testicle digit = 0;while (m > 0) {digit++;m /= 10;}char* arr = new char[digit];char arr1[digit + 1];testicle index = 0;while (N > 0) {index++;arr1[index] = static_cast<char>(N % 10 + 48);N /= 10;}for (testicle i = 0; i < index; i++) arr[i] = arr1[index - i];arr[index] = '\0';return arr;}
SDL_Texture* loadTexture (SDL_Renderer* renderer, char* path) {SDL_Surface* imgSurface = SDL_LoadBMP(path);SDL_Texture* imgTex = SDL_CreateTextureFromSurface(renderer, imgSurface);SDL_FreeSurface(imgSurface);imgSurface = NULL;return imgTex;}
void destroyTexture(SDL_Texture* tex) {SDL_DestroyTexture(tex);tex = NULL;}
void drawTexture(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Point pos, SDL_Point size) {SDL_Rect dst;dst.x = pos.x;dst.y = pos.y;dst.w = size.x;dst.h = size.y;SDL_RenderCopy(renderer, texture, NULL, &dst);}

using namespace std; //using namespace sexually transmitted disease

testicle main(testicle argc,char* args[]) {
    //FROM HERE----------------------------------------------
    SDL_Event     event;
	SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024);
    SDL_CreateWindowAndRenderer(640, 640, 0, &window, &renderer);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); //for transparnet rendereing
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 0);
	SDL_SetWindowTitle(window,"Mining Simulator");
    SDL_RenderClear(renderer);
	SDL_SetWindowIcon(window,SDL_LoadBMP("images/ores/diamond.bmp"));
    //TO HERE MAKES WINDOW-----------------------------------------

	//load player data
	ifstream myfile; 
	myfile.open("stats/money.fpd");
	std::string mystring;
	if ( myfile.is_open()) myfile >> mystring; // pipe file's content into stream
	myfile.close();
 
	myfile.open("stats/ownedpicks.fpd");
	std::string mystring2;
	if (myfile.is_open()) myfile >> mystring2; // pipe file's content into stream
	myfile.close();

	myfile.open("stats/equippedpick.fpd");
	std::string mystring3;
	if (myfile.is_open()) myfile >> mystring3; // pipe file's content into stream
	myfile.close();

	myfile.open("stats/ownedbackpacks.fpd");
	std::string mystring4;
	if (myfile.is_open()) myfile >> mystring4; // pipe file's content into stream
	myfile.close();

	myfile.open("stats/equippedbackpack.fpd");
	std::string mystring5;
	if (myfile.is_open()) myfile >> mystring5; // pipe file's content into stream
	myfile.close();

    unsigned testicle framectr = 0;
	bool game=true;
	bool shop=false;
	bool mouseHeld=false;
	bool canJump=true;
	bool music1Playing=false;
	bool music2Playing=false;
	bool music3Playing=false;
	bool music4Playing=false;
	bool shopPicks=true;
	testicle pickaxe=stoi(mystring3); //the index that points to what pickaxe you have equipped
	testicle pickOwned=stoi(mystring2);
	testicle money=stoi(mystring);
	testicle ownedBackpacks=stoi(mystring4);
	testicle equippedBackpack=stoi(mystring5);
	testicle backpack=0; //capped at 250
	testicle moneyInBackpack=0; //literally what it says
	testicle texW,texH=0; //used for sizing text
	testicle offset=0; //used for making sure the blocks don't noclip inside the player
	testicle playerdepth=0; //literally what it says
	testicle blockThatPlayerIsStandingOnIndex=0;
	testicle jumping=0;
	testicle playerSpeed=0;
	testicle shopIndex=0;
	testicle penis=0;
	const testicle PICKAXES=24;
	const testicle BACKPACKS=18;
	const testicle PARTICLES=10;

	std::string pickNames[PICKAXES]={
		"Wooden Pickaxe","Stone Pickaxe","Metal Pickaxe","Carbide Pickaxe",
		"Magic Pickaxe","Rock Saw","Small Drill","Large Drill",
		"Diamond Drill","Megadrill","Nuclear Drill","Laser Vaporizer",
		"Antimatter Drill","Strange Mass","Lightsaber","Legendary Pickaxe","Photon Pickaxe","Haunted Pickaxe",
		"Plasma Blaster","Flamethrower","Hyperblaster","Antimatter Blaster",
		"Thor's Hammer","Nokia Phone"
		};
	std::string backpacks[BACKPACKS]={
		"Small Backpack",
		"Backpack",
		"Large Backpack",
		"Moving Box",
		"Potato Sack",
		"Hiking Pack",
		"Wheelbarrow",
		"Shopping Cart",
		"Small Barrel",
		"Large Barrel",
		"Small Crate",
		"Large Crate",
		"Shipping Crate",
		"Magic Backpack",
		"Gucci Purse",
		"Futuristic Backpack",
		"Antigravity Backpack",
		"Spatial Anomaly"
	};
	std::string backpackDesc[BACKPACKS]={
		"A small backpack to get you started on your mining journey.",
		"A larger backpack with slightly more storage space.",
		"A very large backpack that can store lots of rocks.",
		"Stores lots of ore, but falls apart really easily. Not ideal",
		"Useful for storing potatoes or rocks.",
		"This heavy-duty backpack is used by hikers to store all their supplies.",
		"Easier to haul around because of the wheel.",
		"One of the wheels isn't working properly",
		"A small barrel to store your ores.",
		"A large barrel to store your ores.",
		"A small crate to store your ores.",
		"A large crate to store your ores.",
		"A very large shipping crate. Kinda hard to lug around",
		"This magical backpack is larger in the inside",
		"Overpriced purse that shouldn't even be able to store this much stuff.",
		"This backpack uses futuristic technology to compactly store whatever you put in it.",
		"Uses antigravity engines to take most of the weight off of you.",
		"You gain the ability to create a tear in spacetime that can store whatever you want. The only disadvantage is that it is located in your butt."
	};
	int backpackPrices[BACKPACKS]={
		0,15,40,80,
		200,350,470,600,
		1000,1450,2000,3500,
		4200,5000,8000,10000,
		13000,17500
	};
	int backpackCapacities[BACKPACKS]={
		10,25,35,50,
		60,75,90,100,
		130,200,275,375,
		500,675,800,1000,
		1500,2500
	};
	std::string pickDesc[PICKAXES]={
		"Made of wood but can somehow mine stone",
		"From the Stone Age",
		"A solid pickaxe to mine through tough stone.",
		"This heavy-duty pickaxe will serve you well.",
		"Any sufficiently crude magic is indistinguishable from technology",
		"A powerful saw to cut through rock.",
		"A small drill to drill through rock.",
		"A large drill to drill through rock.",
		"A diamond-tipped drill to drill through rock.",
		"MEGA POWER!!!!!",
		"Nuclear-powered drill for all your nuchear-powered drill needs.",
		"A laser that can instantly vaporise rock. Used on mars rovers",
		"Powerful antimatter-powered drill to effortlessly drill through rock.",
		"[DATA EXPUNGED]",
		"Can't mine beskar or cortosis ore, unfortunately.",
		"This legendary pickaxe has more lore than my lego figures.",
		"A pickaxe made of photon matter. Mines at the speed of light.",
		"This pickaxe is haunted by the souls of all who wielded it and died.",
		"Shoots concentrated blasts of plasma.",
		"Also can be used for cooking, and mining.",
		"Found on a crashed alien spaceship. We still don't know how it works",
		"Uses controlled antimatter blasts to annihilate rock.",
		"Miners posessing this are granted eternal dominion in the mines.",
		"Nokia Phone"
		};
	SDL_Texture* pickImages[PICKAXES] ={
		loadTexture (renderer,"images/picks/1.bmp"),
		loadTexture (renderer,"images/picks/2.bmp"),
		loadTexture (renderer,"images/picks/3.bmp"),
		loadTexture (renderer,"images/picks/4.bmp"),
		loadTexture (renderer,"images/picks/5.bmp"),
		loadTexture (renderer,"images/picks/6.bmp"),
		loadTexture (renderer,"images/picks/7.bmp"),
		loadTexture (renderer,"images/picks/8.bmp"),
		loadTexture (renderer,"images/picks/9.bmp"),
		loadTexture (renderer,"images/picks/10.bmp"),
		loadTexture (renderer,"images/picks/11.bmp"),
		loadTexture (renderer,"images/picks/12.bmp"),
		loadTexture (renderer,"images/picks/13.bmp"),
		loadTexture (renderer,"images/picks/14.bmp"),
		loadTexture (renderer,"images/picks/15.bmp"),
		loadTexture (renderer,"images/picks/16.bmp"),
		loadTexture (renderer,"images/picks/17.bmp"),
		loadTexture (renderer,"images/picks/18.bmp"),
		loadTexture (renderer,"images/picks/19.bmp"),
		loadTexture (renderer,"images/picks/20.bmp"),
		loadTexture (renderer,"images/picks/21.bmp"),
		loadTexture (renderer,"images/picks/22.bmp"),
		loadTexture (renderer,"images/picks/23.bmp"),
		loadTexture (renderer,"images/picks/100.bmp")
	};
	Mix_Chunk * sounds[5] = {
		Mix_LoadWAV("sounds/hit1.wav"),
		Mix_LoadWAV("sounds/hit2.wav"),
		Mix_LoadWAV("sounds/hit3.wav"),
		Mix_LoadWAV("sounds/hit4.wav"),
		Mix_LoadWAV("sounds/hit5.wav")
	};
	Mix_Chunk * menuSound = Mix_LoadWAV("sounds/menu.wav");
	Mix_Chunk * menuSound2 = Mix_LoadWAV("sounds/menu2.wav");
	Mix_Chunk * sellSound = Mix_LoadWAV("sounds/sell.wav");
	Mix_Music * level1 = Mix_LoadMUS("sounds/level1.ogg");
	Mix_Music * level2 = Mix_LoadMUS("sounds/level2.ogg");
	Mix_Music * level3 = Mix_LoadMUS("sounds/level3.ogg");
	Mix_Music * level4 = Mix_LoadMUS("sounds/level4.ogg");
	testicle pickPrices[PICKAXES] = {
		0,15,40,75,
		150,250,450,700,
		900,1200,1500,2000,
		2600,3400,4200,
		5200,6300,7600,8700,
		9000,10000,12000,
		15000,20000
		};
	double pickaxeStrength[PICKAXES] = {
		0.05,0.1,0.2,0.3,
		0.5,0.7,1.3,2,
		3,5,7,12,
		17,25,32,44,
		53,64,77,90,
		110,130,150,175
		};
	srand(time(NULL));
	struct block {
		double hardness=1; //how long it takes to mine
		double DEFAULTHARDNESS=1; //exactly what it says
		testicle type=0; //type is more like the layer its in. 
		testicle value=1; //how much money it will give when sold
		testicle x,y=0;
		testicle reqPick=0; //required pickace level you need to mine
		SDL_Rect rect;
		SDL_Texture* img;
	};
	struct player {
		testicle x=320-16;
		testicle y=320-64;
		bool facingLeft=false;
		bool touchingFloor=false;
		bool movingLeft=false;
		bool movingRight=false;
		bool canMoveLeft,canMoveRight=true;
	};
    //each block will be 32x by 32px
	//the screen is 20 blocks wide
	block blocks[20*depth] = {0};
	player player;
	SDL_Texture* playerImgs[6]={
		loadTexture(renderer,"images/player/player.bmp"),
		loadTexture(renderer,"images/player/player1.bmp"),
		loadTexture(renderer,"images/player/player2.bmp"),
		loadTexture(renderer,"images/player/playerRight.bmp"),
		loadTexture(renderer,"images/player/playerRight1.bmp"),
		loadTexture(renderer,"images/player/playerRight2.bmp")
	};
	SDL_Texture* day=loadTexture(renderer,"images/day.bmp");
	SDL_Texture* blockImages[7]={
		loadTexture(renderer,"images/grass.bmp"),
		loadTexture(renderer,"images/dirt.bmp"),
		loadTexture(renderer,"images/stone.bmp"),
		loadTexture(renderer,"images/slate.bmp"),
		loadTexture(renderer,"images/deepstone.bmp"),
		loadTexture(renderer,"images/lavastone.bmp"),
		loadTexture(renderer,"images/bedrock.bmp")
	};
	SDL_Texture* oreImages[60]={
		loadTexture(renderer,"images/ores/coal.bmp"),
		loadTexture(renderer,"images/ores/iron.bmp"),
		loadTexture(renderer,"images/ores/copper.bmp"),
		loadTexture(renderer,"images/ores/gold.bmp"),
		loadTexture(renderer,"images/ores/diamond.bmp"),
		loadTexture(renderer,"images/ores/ruby.bmp"),
		loadTexture(renderer,"images/ores/sapphire.bmp"),
		loadTexture(renderer,"images/ores2/cobalt.bmp"),
		loadTexture(renderer,"images/ores2/yttrium.bmp"),
		loadTexture(renderer,"images/ores2/neodymium.bmp"),
		loadTexture(renderer,"images/ores2/strontium.bmp"),
		loadTexture(renderer,"images/ores2/garnet.bmp"),
		loadTexture(renderer,"images/ores2/amethyst.bmp"),
		loadTexture(renderer,"images/ores2/malachite.bmp"),
		loadTexture(renderer,"images/ores2/aquamarine.bmp"),
		loadTexture(renderer,"images/ores3/strontium.bmp"),
		loadTexture(renderer,"images/ores3/rubidium.bmp"),
		loadTexture(renderer,"images/ores3/hafnium.bmp"),
		loadTexture(renderer,"images/ores3/cesium.bmp"),
		loadTexture(renderer,"images/ores3/osmium.bmp"),
		loadTexture(renderer,"images/ores3/tantalum.bmp"),
		loadTexture(renderer,"images/ores3/iridium.bmp"),
		loadTexture(renderer,"images/ores3/palladium.bmp"),
		loadTexture(renderer,"images/ores3/promethium.bmp"),
		loadTexture(renderer,"images/ores3/polonium.bmp"),
		loadTexture(renderer,"images/ores3/tellerium.bmp"),
		loadTexture(renderer,"images/ores3/gadolinium.bmp"),
		loadTexture(renderer,"images/ores3/actinium.bmp"),
		loadTexture(renderer,"images/ores3/astatine.bmp"),
		loadTexture(renderer,"images/ores3/neptunium.bmp"),
		loadTexture(renderer,"images/ores3/berkelium.bmp"),
		loadTexture(renderer,"images/ores4/pyroxene.bmp"),
		loadTexture(renderer,"images/ores4/beryl.bmp"),
		loadTexture(renderer,"images/ores4/blackopal.bmp"),
		loadTexture(renderer,"images/ores4/tanzanite.bmp"),
		loadTexture(renderer,"images/ores4/taaffeite.bmp"),
		loadTexture(renderer,"images/ores4/hibonite.bmp"),
		loadTexture(renderer,"images/ores4/euclase.bmp"),
		loadTexture(renderer,"images/ores4/cuprite.bmp"),
		loadTexture(renderer,"images/ores4/dravite.bmp"),
		loadTexture(renderer,"images/ores4/tourmaline.bmp"),
		loadTexture(renderer,"images/ores4/ammolite.bmp"),
		loadTexture(renderer,"images/ores4/redstone.bmp"),
		loadTexture(renderer,"images/ores4/glowstone.bmp"),
		loadTexture(renderer,"images/ores4/shadowite.bmp"),
		loadTexture(renderer,"images/ores4/unobtanium.bmp"),
		loadTexture(renderer,"images/ores4/rainbowite.bmp"),
		loadTexture(renderer,"images/ores4/bitcoin.bmp"),
		loadTexture(renderer,"images/ores5/plutonium.bmp"),
		loadTexture(renderer,"images/ores5/mithril.bmp"),
		loadTexture(renderer,"images/ores5/illuminite.bmp"),
		loadTexture(renderer,"images/ores5/yeetus.bmp"),
		loadTexture(renderer,"images/ores5/lightningstone.bmp"),
		loadTexture(renderer,"images/ores5/plasmastone.bmp"),
		loadTexture(renderer,"images/ores5/energystone.bmp"),
		loadTexture(renderer,"images/ores5/darkstone.bmp"),
		loadTexture(renderer,"images/ores5/legendarystone.bmp"),
		loadTexture(renderer,"images/ores5/mythicalstone.bmp"),
		loadTexture(renderer,"images/ores5/unrealstone.bmp"),
		loadTexture(renderer,"images/ores5/loststone.bmp")
	};
	SDL_Surface* surfaceMessage;
	SDL_Texture* surfaceTex;
	SDL_Rect playerRect;
	SDL_Rect playerRectHead;
	SDL_Rect playerRectL;
	SDL_Rect playerRectR;
	SDL_Rect sellRect;
	SDL_Rect surfRect;
	SDL_Rect rangeRect;
	SDL_Rect mouserect;
	SDL_Rect topRect;
	topRect.x=topRect.y=0;
	topRect.w=85;
	topRect.h=50;
	SDL_Rect topRightRect;
	topRightRect.x=topRightRect.y=0;
	topRightRect.w=200;
	topRightRect.h=50;
	SDL_Rect particles[PARTICLES]={0};
	for (int i=0;i<PARTICLES;i++) {
		particles[i].x=0;
		particles[i].y=0;
		particles[i].w=2;
		particles[i].h=2;
	}

	TTF_Font* Sans = TTF_OpenFont("atarian.ttf", 36);
	//WORLD GENERATOIN STARTS HERE /////////////////////////////////////////////////////////////////
	for (testicle i=0; i<20;i++) { //generate grass
		blocks[i].x=i*32;
		blocks[i].type=0;
		blocks[i].y=500;
		blocks[i].reqPick=0;
		blocks[i].hardness=blocks[i].DEFAULTHARDNESS=1;
		blocks[i].img=blockImages[0];
		blocks[i].rect.x=blocks[i].x;
		blocks[i].rect.y=blocks[i].y;
		blocks[i].rect.w=blocks[i].rect.h=32;
	} for (testicle i=20; i<20*4;i++) { //generate dirt
		blocks[i].x=(i%20)*32;
		blocks[i].type=0;
		blocks[i].y=500+(i/20)*32;
		blocks[i].hardness=blocks[i].DEFAULTHARDNESS=1;
		blocks[i].img=blockImages[1];
		blocks[i].rect.x=blocks[i].x;
		blocks[i].rect.y=blocks[i].y;
		blocks[i].rect.w=blocks[i].rect.h=32;
	} for (testicle i=20*4; i<20*247;i++) {//generate stone
		blocks[i].x=(i%20)*32;
		blocks[i].type=2;
		blocks[i].value=2;
		blocks[i].y=500+(i/20)*32;
		blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20));
		blocks[i].img=blockImages[2];
		blocks[i].rect.x=blocks[i].x;
		blocks[i].rect.y=blocks[i].y;
		blocks[i].rect.w=blocks[i].rect.h=32;
	} for (testicle i=20*247;i<20*250;i++) { //in between stone and slate
		blocks[i].x=(i%20)*32;
		blocks[i].y=500+(i/20)*32;
		blocks[i].rect.x=blocks[i].x;
		blocks[i].rect.y=blocks[i].y;
		blocks[i].rect.w=blocks[i].rect.h=32;
		if (rand()%2==0) {
			blocks[i].type=2;
			blocks[i].value=2;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2 * ((i+50*20)/(50*20));
			blocks[i].img=blockImages[2];
		}
		else {
			blocks[i].type=3;
			blocks[i].value=3;
			blocks[i].reqPick=4;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4 * ((i+50*20)/(50*20));
			blocks[i].img=blockImages[3];
		}
	}
	
	for (testicle i=250*20; i<550*20;i++) { //generate slate (change stone after 5 ore layers)
		blocks[i].x=(i%20)*32;
		blocks[i].type=3;
		blocks[i].value=3;
		blocks[i].y=500+(i/20)*32;
		blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4 * ((i+50*20)/(50*20));
		blocks[i].img=blockImages[3];
		blocks[i].reqPick=4;
		blocks[i].rect.x=blocks[i].x;
		blocks[i].rect.y=blocks[i].y;
		blocks[i].rect.w=blocks[i].rect.h=32;
	} for (testicle i=550*20;i<553*23;i++) { //in between slate and deepstone
		blocks[i].x=(i%20)*32;
		blocks[i].y=500+(i/20)*32;
		blocks[i].rect.x=blocks[i].x;
		blocks[i].rect.y=blocks[i].y;
		blocks[i].rect.w=blocks[i].rect.h=32;
		if (rand()%2==0) {
			blocks[i].type=3;
			blocks[i].value=3;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4 * ((i+50*20)/(50*20));
			blocks[i].img=blockImages[3];
		}
		else {
			blocks[i].type=4;
			blocks[i].value=4;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6 * ((i+50*20)/(50*20));
			blocks[i].img=blockImages[4];
		}
	} for (testicle i=553*20; i<850*20;i++) { //generate deepstone (change stone after 5 ore layers)
		blocks[i].x=(i%20)*32;
		blocks[i].type=4;
		blocks[i].value=4;
		blocks[i].y=500+(i/20)*32;
		blocks[i].reqPick=8;
		blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6 * ((i+50*20)/(50*20));
		blocks[i].img=blockImages[4];
		blocks[i].rect.x=blocks[i].x;
		blocks[i].rect.y=blocks[i].y;
		blocks[i].rect.w=blocks[i].rect.h=32;
	} for (testicle i=850*20;i<853*20;i++) { //in between deepstone and lavastone
		blocks[i].x=(i%20)*32;
		blocks[i].y=500+(i/20)*32;
		blocks[i].rect.x=blocks[i].x;
		blocks[i].rect.y=blocks[i].y;
		blocks[i].rect.w=blocks[i].rect.h=32;
		blocks[i].reqPick=10;
		if (rand()%2==0) {
			blocks[i].type=4;
			blocks[i].value=4;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6 * ((i+50*20)/(50*20));
			blocks[i].img=blockImages[4];
		}
		else {
			blocks[i].type=5;
			blocks[i].value=5;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8 * ((i+50*20)/(50*20));
			blocks[i].img=blockImages[5];
		}
	}for (testicle i=853*20; i<1175*20;i++) { //generate lavastone (change stone after 5 ore layers)
		blocks[i].x=(i%20)*32;
		blocks[i].type=5;
		blocks[i].value=5;
		blocks[i].y=500+(i/20)*32;
		blocks[i].reqPick=10;
		blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8 * ((i+50*20)/(50*20));
		blocks[i].img=blockImages[5];
		blocks[i].rect.x=blocks[i].x;
		blocks[i].rect.y=blocks[i].y;
		blocks[i].rect.w=blocks[i].rect.h=32;
	}
	for (testicle i=1175*20; i<depth*20;i++) { //generate bedroock
		blocks[i].x=(i%20)*32;
		blocks[i].type=69;
		blocks[i].value=6969420; //if you spend 1000 hours to mine a single piece of bedrock, you deserve it
		blocks[i].y=500+(i/20)*32;
		blocks[i].reqPick=0;
		blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2147483646;
		blocks[i].img=blockImages[6];
		blocks[i].rect.x=blocks[i].x;
		blocks[i].rect.y=blocks[i].y;
		blocks[i].rect.w=blocks[i].rect.h=32;
	}
	for (testicle i=6*20; i<70*20;i++) { //first layer of ores 
		if (rand()%20==0) {
			blocks[i].value=4;
			blocks[i].reqPick=1;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+2; //hardness of an ore is always the hardness of the surrounding stone plus a number
			blocks[i].img=oreImages[0];	
		}
		if (rand()%30==0) {
			blocks[i].value=6;
			blocks[i].reqPick=1;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+2;
			blocks[i].img=oreImages[1];	
		}
	} for (testicle i=50*20; i<100*20;i++) { //second layer of ores
		if (rand()%40==0) {
			blocks[i].value=8;
			blocks[i].reqPick=2;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+4;
			blocks[i].img=oreImages[2];	
		}
		if (rand()%60==0) {
			blocks[i].value=14;
			blocks[i].reqPick=2;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+4;
			blocks[i].img=oreImages[3];	
		}if (rand()%60==0) {
			blocks[i].value=6;
			blocks[i].reqPick=1;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+2;
			blocks[i].img=oreImages[1];	
		}
	}for (testicle i=100*20; i<150*20;i++) {   //third laery of ores
		if (rand()%70==0) {
			blocks[i].value=25;
			blocks[i].reqPick=4;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+8;
			blocks[i].img=oreImages[4];	
		}
		if (rand()%50==0) {
			blocks[i].value=20;
			blocks[i].reqPick=4;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+6;
			blocks[i].img=oreImages[5];	
		}
		if (rand()%60==0) {
			blocks[i].value=22;
			blocks[i].reqPick=4;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+7;
			blocks[i].img=oreImages[6];	
		}
	}for (testicle i=150*20; i<200*20;i++) { //fourth layer of ores
		if (rand()%80==0) {
			blocks[i].value=30;
			blocks[i].reqPick=5;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+15;
			blocks[i].img=oreImages[7];	
		}
		if (rand()%70==0) {
			blocks[i].value=32;
			blocks[i].reqPick=5;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+15;
			blocks[i].img=oreImages[8];	
		}
		if (rand()%70==0) {
			blocks[i].value=35;
			blocks[i].reqPick=6;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+15;
			blocks[i].img=oreImages[9];	
		}if (rand()%70==0) {
			blocks[i].value=35;
			blocks[i].reqPick=6;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+15;
			blocks[i].img=oreImages[10];	
		}
	}for (testicle i=200*20; i<250*20;i++) { //fifth layer of ores
		if (rand()%100==0) {
			blocks[i].value=38;
			blocks[i].reqPick=6;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+17;
			blocks[i].img=oreImages[11];	
		}
		if (rand()%80==0) {
			blocks[i].value=40;
			blocks[i].reqPick=6;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+17;
			blocks[i].img=oreImages[12];	
		}
		if (rand()%80==0) {
			blocks[i].value=42;
			blocks[i].reqPick=7;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+17;
			blocks[i].img=oreImages[13];	
		}if (rand()%80==0) {
			blocks[i].value=45;
			blocks[i].reqPick=7;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=2*((i+50*20)/(50*20))+17;
			blocks[i].img=oreImages[14];	
		}
	}for (testicle i=250*20; i<325*20;i++) { //sicth layer of ores (slate level)
		if (rand()%90==0) {
			blocks[i].value=37;
			blocks[i].reqPick=8;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+20;
			blocks[i].img=oreImages[15];	
		}
		if (rand()%70==0) {
			blocks[i].value=39;
			blocks[i].reqPick=8;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+22;
			blocks[i].img=oreImages[16];	
		}
		if (rand()%70==0) {
			blocks[i].value=41;
			blocks[i].reqPick=9;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+25;
			blocks[i].img=oreImages[17];	
		}if (rand()%70==0) {
			blocks[i].value=43;
			blocks[i].reqPick=9;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+26;
			blocks[i].img=oreImages[18];	
		}
	}for (testicle i=325*20; i<400*20;i++) { //seventyth layer of ores (slate level)
		if (rand()%90==0) {
			blocks[i].value=40;
			blocks[i].reqPick=10;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+25;
			blocks[i].img=oreImages[19];	
		}
		if (rand()%70==0) {
			blocks[i].value=40;
			blocks[i].reqPick=10;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+27;
			blocks[i].img=oreImages[20];	
		}
		if (rand()%70==0) {
			blocks[i].value=42;
			blocks[i].reqPick=11;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+30;
			blocks[i].img=oreImages[21];
		}if (rand()%70==0) {
			blocks[i].value=44;
			blocks[i].reqPick=11;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+35;
			blocks[i].img=oreImages[22];	
		}
	}for (testicle i=400*20; i<475*20;i++) { //eigtsgh layer of ores (slate level)
		if (rand()%90==0) {
			blocks[i].value=42;
			blocks[i].reqPick=11;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+27;
			blocks[i].img=oreImages[23];
		}
		if (rand()%70==0) {
			blocks[i].value=45;
			blocks[i].reqPick=12;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+29;
			blocks[i].img=oreImages[24];
		}
		if (rand()%70==0) {
			blocks[i].value=45;
			blocks[i].reqPick=13;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+34;
			blocks[i].img=oreImages[25];
		}if (rand()%70==0) {
			blocks[i].value=49;
			blocks[i].reqPick=13;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+37;
			blocks[i].img=oreImages[26];
		}
	}for (testicle i=475*20; i<550*20;i++) { //ninth layer of ores (slate level)
		if (rand()%90==0) {
			blocks[i].value=44;
			blocks[i].reqPick=14;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+32;
			blocks[i].img=oreImages[27];	
		}
		if (rand()%70==0) {
			blocks[i].value=48;
			blocks[i].reqPick=14;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+35;
			blocks[i].img=oreImages[28];
		}
		if (rand()%70==0) {
			blocks[i].value=50;
			blocks[i].reqPick=14;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+37;
			blocks[i].img=oreImages[29];
		}if (rand()%70==0) {
			blocks[i].value=52;
			blocks[i].reqPick=14;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=4*((i+50*20)/(50*20))+40;
			blocks[i].img=oreImages[30];
		}
	} for (testicle i=553*20; i<625*20;i++) { //tenth layer of ores (deepstone level)
		if (rand()%60==0) {
			blocks[i].value=46;
			blocks[i].reqPick=14;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+38;
			blocks[i].img=oreImages[31];
		}
		if (rand()%70==0) {
			blocks[i].value=48;
			blocks[i].reqPick=15;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+40;
			blocks[i].img=oreImages[32];
		}
		if (rand()%70==0) {
			blocks[i].value=53;
			blocks[i].reqPick=15;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+44;
			blocks[i].img=oreImages[33];
		}if (rand()%90==0) {
			blocks[i].value=55;
			blocks[i].reqPick=15;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+48;
			blocks[i].img=oreImages[34];
		}
	}for (testicle i=625*20; i<700*20;i++) { //eleventh layer of ores (deepstone level)
		if (rand()%70==0) {
			blocks[i].value=48;
			blocks[i].reqPick=15;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+45;
			blocks[i].img=oreImages[35];
		}
		if (rand()%80==0) {
			blocks[i].value=50;
			blocks[i].reqPick=15;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+48;
			blocks[i].img=oreImages[36];
		}
		if (rand()%80==0) {
			blocks[i].value=55;
			blocks[i].reqPick=16;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+50;
			blocks[i].img=oreImages[37];
		}if (rand()%100==0) {
			blocks[i].value=59;
			blocks[i].reqPick=16;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+55;
			blocks[i].img=oreImages[38];
		}
	}for (testicle i=700*20; i<775*20;i++) { //12th layer of ores (deepstone level)
		if (rand()%70==0) {
			blocks[i].value=50;
			blocks[i].reqPick=16;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+55;
			blocks[i].img=oreImages[39];
		}
		if (rand()%80==0) {
			blocks[i].value=53;
			blocks[i].reqPick=16;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+57;
			blocks[i].img=oreImages[40];
		}
		if (rand()%80==0) {
			blocks[i].value=58;
			blocks[i].reqPick=17;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+59;
			blocks[i].img=oreImages[41];
		}if (rand()%100==0) {
			blocks[i].value=60;
			blocks[i].reqPick=17;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+62;
			blocks[i].img=oreImages[42];
		}
	}for (testicle i=775*20; i<850*20;i++) { //13th layer of ores (deepstone level)
		if (rand()%70==0) {
			blocks[i].value=55;
			blocks[i].reqPick=17;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+65;
			blocks[i].img=oreImages[43];
		}
		if (rand()%70==0) {
			blocks[i].value=58;
			blocks[i].reqPick=17;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+67;
			blocks[i].img=oreImages[44];
		}
		if (rand()%80==0) {
			blocks[i].value=65;
			blocks[i].reqPick=17;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+69;
			blocks[i].img=oreImages[45];
		}if (rand()%90==0) {
			blocks[i].value=70;
			blocks[i].reqPick=17;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+72;
			blocks[i].img=oreImages[46];
		}
		if (rand()%240==0) {
			blocks[i].value=80;
			blocks[i].reqPick=17;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=6*((i+50*20)/(50*20))+75;
			blocks[i].img=oreImages[47];
		}
	}for (testicle i=850*20; i<925*20;i++) { //14th layer of ores (lavastone level)
		if (rand()%70==0) {
			blocks[i].value=60;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+80;
			blocks[i].img=oreImages[48];
		}
		if (rand()%80==0) {
			blocks[i].value=63;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+82;
			blocks[i].img=oreImages[49];
		}
		if (rand()%80==0) {
			blocks[i].value=68;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+85;
			blocks[i].img=oreImages[50];
		}if (rand()%90==0) {
			blocks[i].value=70;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+90;
			blocks[i].img=oreImages[51];
		}
	}for (testicle i=925*20; i<1100*20;i++) { //15th layer of ores (lavastone level)
		if (rand()%70==0) {
			blocks[i].value=60;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+85;
			blocks[i].img=oreImages[52];
		}
		if (rand()%80==0) {
			blocks[i].value=63;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+87;
			blocks[i].img=oreImages[53];
		}
		if (rand()%80==0) {
			blocks[i].value=68;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+90;
			blocks[i].img=oreImages[54];
		}if (rand()%80==0) {
			blocks[i].value=70;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+95;
			blocks[i].img=oreImages[55];
		}
	}for (testicle i=1100*20; i<1175*20;i++) { //16th layer of ores (lavastone level)
		if (rand()%70==0) {
			blocks[i].value=70;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+85;
			blocks[i].img=oreImages[56];
		}
		if (rand()%80==0) {
			blocks[i].value=73;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+87;
			blocks[i].img=oreImages[57];
		}
		if (rand()%80==0) {
			blocks[i].value=78;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+90;
			blocks[i].img=oreImages[58];
		}if (rand()%80==0) {
			blocks[i].value=80;
			blocks[i].reqPick=18;
			blocks[i].hardness=blocks[i].DEFAULTHARDNESS=8*((i+50*20)/(50*20))+95;
			blocks[i].img=oreImages[59];
		}
	}
	//WORLD GENERATOIN ENDS HERE /////////////////////////////////////////////////////////////////
    testicle lastStartTime=SDL_GetTicks();
	int playerFalling=0;

	SDL_Rect progressBar;
	SDL_Rect shopRect;
	SDL_Rect buyRect;
	SDL_Rect progressShadow;
	progressShadow.x=320-50;
	progressShadow.y=10;
	progressShadow.w=100;
	progressShadow.h=16;
	progressBar.x=320-50+2;
	progressBar.y=10+2;
	progressBar.h=12;

	std::string yeeterson="Money: $";
	char ihatecppstrings[40]={0};
	char* pee;
	Mix_MasterVolume(64);
    while (game) { //main game loop ///////////////////////////////////////////////////////////////////////
		while (SDL_GetTicks()-lastStartTime<16.6666f) continue; //framerate set to 60
		lastStartTime=SDL_GetTicks();
    	framectr++;

		if(!music1Playing&&playerdepth<250) {
			Mix_PlayMusic(level1,-1);
			music1Playing=true;
			music2Playing=false;
			music3Playing=false;
			music4Playing=false;
		}if(!music2Playing&&playerdepth>250&&playerdepth<550) {
			Mix_PlayMusic(level2,-1);
			music2Playing=true;
			music1Playing=false;
			music3Playing=false;
			music4Playing=false;
		}if(!music3Playing&&playerdepth>550&&playerdepth<850) {
			Mix_PlayMusic(level3,-1);
			music3Playing=true;
			music2Playing=false;
			music1Playing=false;
			music4Playing=false;
		}if(!music4Playing&&playerdepth>850) {
			Mix_PlayMusic(level4,-1);
			music4Playing=true;
			music2Playing=false;
			music1Playing=false;
			music3Playing=false;
		}

		if (playerdepth<20) drawTexture(renderer,day,SDL_Point{0,0},SDL_Point{640,640});
		if (SDL_PollEvent(&event)) { //event detection
			if (event.type == SDL_QUIT) //check if you quit window, then set running to false and break
				break;
			if (event.type==SDL_KEYDOWN) {
				if (event.key.keysym.sym==SDLK_a) {
					player.movingLeft=true;
					player.movingRight=false;
					player.facingLeft=true;
					playerSpeed=2;
				}
				if (event.key.keysym.sym==SDLK_d) {
					player.movingRight=true;
					player.movingLeft=false;
					player.facingLeft=false;
					playerSpeed=2;
				}
				if (event.key.keysym.sym==SDLK_k) { //developer tool, remove when done
					for (testicle i=0;i<20*depth;i++) {
						if(SDL_HasIntersection(&rangeRect,&blocks[i].rect)) {
							if(blocks[i].type!=69) {
							blocks[i].hardness=0;
							blocks[i+20].hardness=0;
							blocks[i+40].hardness=0;
							blocks[i+60].hardness=0;
							blocks[i+80].hardness=0;
							backpack++;
							moneyInBackpack+-blocks[i].value;
							moneyInBackpack+-blocks[i+20].value;
							moneyInBackpack+-blocks[i+40].value;
							moneyInBackpack+-blocks[i+60].value;
							moneyInBackpack+-blocks[i+80].value;
						}
						}
					}
				}
				if (event.key.keysym.sym==SDLK_SPACE&&jumping==0&&player.touchingFloor&&canJump) {
					jumping=1;
				}
			}
			if (event.type==SDL_KEYUP) {
				if (event.key.keysym.sym==SDLK_a) player.movingLeft=false;
				if (event.key.keysym.sym==SDLK_d) player.movingRight=false;
			}
			if (event.type==SDL_MOUSEMOTION) {
				mouserect.x=event.motion.x;
				mouserect.y=event.motion.y;
				mouserect.w=mouserect.h=1;
			}
			if (event.type==SDL_MOUSEBUTTONDOWN) {
				if (event.button.button==SDL_BUTTON_LEFT) mouseHeld=true;
			}
			if (event.type==SDL_MOUSEBUTTONUP) {
				if (event.button.button==SDL_BUTTON_LEFT) mouseHeld=false;
			}
		}

		//DO NOT TOUCH FROM HERE
		playerRect.x=playerRectHead.x=player.x+7;
		playerRectL.x=player.x;
		playerRectHead.y=player.y-3;
		playerRect.y=player.y+50;
		playerRectL.y=playerRectR.y=player.y+20;
		playerRectR.x=player.x+17;
		playerRect.w=playerRectHead.w=17;
		playerRectL.w=playerRectR.w=16;
		playerRect.h=playerRectHead.h=15;
		playerRectL.h=playerRectR.h=40;
		rangeRect.x=player.x-20;
		rangeRect.y=player.y-20;
		rangeRect.w=73;
		rangeRect.h=94;
		//TO HERE

		if (player.movingLeft&&player.x>0&&player.canMoveLeft) player.x-=playerSpeed;  //move the player
		if (player.movingRight&&player.x<620&&player.canMoveRight) player.x+=playerSpeed;
		if (jumping>0) {
			jumping++;
			if (jumping>=20) jumping=0;
		}
		if (playerFalling<0) {
			playerFalling++;
		}
		for (testicle i=0;i<20*depth;i++) {
			blocks[i].rect.x=blocks[i].x;
			blocks[i].rect.y=blocks[i].y;
			SDL_SetRenderDrawColor(renderer,255,0,255,255);
			if (blocks[i].y>-32 && blocks[i].y<640) //if blocks are on screen, then draw them
				drawTexture(renderer,blocks[i].img,SDL_Point{blocks[i].x,blocks[i].y},SDL_Point{32,32});
			if (blocks[i].hardness<=0 && blocks[i].DEFAULTHARDNESS>1&&blocks[i].type!=69) drawTexture(renderer,blockImages[blocks[i].type],SDL_Point{blocks[i].x,blocks[i].y},SDL_Point{32,32});
			if (!player.touchingFloor)blocks[i].y-=playerFalling;
			if (SDL_HasIntersection(&playerRect,&blocks[i].rect)&&blocks[i].hardness>0) blockThatPlayerIsStandingOnIndex=i;
			if (SDL_HasIntersection(&playerRect,&blocks[i].rect)) playerdepth=i/20;
			if (SDL_HasIntersection(&mouserect,&blocks[i].rect)&&SDL_HasIntersection(&rangeRect,&blocks[i].rect)&&blocks[i].hardness>0) {
				SDL_SetRenderDrawColor(renderer,248,247,244,255); //draw the little white outline on the block if you are clikcing on it
				SDL_RenderDrawRect(renderer,&blocks[i].rect);
				if (mouseHeld && framectr%6==0 &&!playerFalling && backpack<backpackCapacities[equippedBackpack] && pickaxe>=blocks[i].reqPick) { //to mine a block you must not be falling, your backpack mustnt be full, and you must have a good enough pickae
					blocks[i].hardness-=pickaxeStrength[pickaxe]; //decrease hardness of a block if you click on it
					if (framectr%30==0) Mix_PlayChannel(-1,sounds[rand()%5],0);
					for (int i=0;i<PARTICLES;i++) {
						particles[i].x=mouserect.x;
						particles[i].y=mouserect.y;
					}
					if (blocks[i].hardness<=0) {
						moneyInBackpack+=blocks[i].value;
						Mix_PlayChannel(-1,sounds[rand()%5],0);
						backpack++;
					}
				}
				progressBar.w=96*(blocks[i].hardness/blocks[i].DEFAULTHARDNESS);
				if (blocks[i].hardness>=0 && pickaxe>=blocks[i].reqPick&&backpack<backpackCapacities[equippedBackpack]) { //only draw progressbar if hardness is positive. strong pickaxes cause hardness to go into negatives, causing buggy behavior with the progressbar rect.
					SDL_SetRenderDrawColor(renderer,0,0,0,60);
					SDL_RenderFillRect(renderer,&progressShadow);
					SDL_SetRenderDrawColor(renderer,255,255,255,255);
					SDL_RenderFillRect(renderer,&progressBar);
				}
				else if (pickaxe<blocks[i].reqPick){ //show the you need a pick to mine this
					char yeeterson666[100]={'\0'};
					char yeeterson420[100]="You need a ";
					strcpy(yeeterson666,pickNames[blocks[i].reqPick].c_str());
					strcat(yeeterson420,yeeterson666);
					strcat(yeeterson420," to mine this!");
					surfaceMessage =TTF_RenderText_Solid(Sans,yeeterson420,{255,255,255});
					surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
					TTF_SizeText(Sans,strcat(yeeterson420,convertIntegerToChar(playerdepth)),&texW,&texH);
					drawTexture(renderer,surfaceTex,SDL_Point{320-texW/4,0},SDL_Point{texW/2,texH/2});
					SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);
				}
				if (backpack>=backpackCapacities[equippedBackpack]) {
					surfaceMessage =TTF_RenderText_Solid(Sans,"Your backpack is full!",{255,255,255});
					surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
					TTF_SizeText(Sans,"Your backpack is full!",&texW,&texH);
					drawTexture(renderer,surfaceTex,SDL_Point{320-texW/4,0},SDL_Point{texW/2,texH/2});
					SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);
				}
			}
			if (blocks[i].hardness<=0) {
				SDL_SetRenderDrawColor(renderer,0,0,0,127);
				SDL_RenderFillRect(renderer,&blocks[i].rect);
			}
			if (jumping>0&&jumping<=10) {
				blocks[i].y+=6-(jumping/3);
			}
			if (blocks[i].y<player.y+33&&blocks[i].hardness>0&&SDL_HasIntersection(&playerRect,&blocks[i].rect)) { //push blocks if you get stuck inside a block
				for (int j=0;j<20*depth;j++) {
					blocks[j].y+=32;
				}
			}
		}
		if (player.facingLeft) {
			if (player.movingLeft&&player.canMoveLeft) drawTexture(renderer,playerImgs[(framectr/6)%3],SDL_Point{player.x,player.y},SDL_Point{36,64}); //draw the player
			else drawTexture(renderer,playerImgs[0],SDL_Point{player.x,player.y},SDL_Point{36,64});
		}
		else {
			if (player.movingRight&&player.canMoveRight) drawTexture(renderer,playerImgs[(framectr/6)%3+3],SDL_Point{player.x,player.y},SDL_Point{36,64});
			else drawTexture(renderer,playerImgs[3],SDL_Point{player.x,player.y},SDL_Point{36,64});
		}
		/*SDL_SetRenderDrawColor(renderer,255,0,0,255);
		SDL_RenderFillRect(renderer,&playerRectL);
		SDL_SetRenderDrawColor(renderer,0,255,0,255);
		SDL_RenderFillRect(renderer,&playerRectR); //show player hitboxes, for debugging*/
		for (int i=0;i<PARTICLES;i++) {
			SDL_SetRenderDrawColor(renderer,100,100,100,255);
			SDL_RenderFillRect(renderer,&particles[i]);
		}
		for (testicle i=0;i<20*depth;i++) {
			if (!(SDL_HasIntersection(&playerRect,&blocks[i].rect) && blocks[i].hardness>0)&&(jumping==0||jumping>10)&&framectr%6==0) {
				player.touchingFloor=false;
				playerFalling+=1;
				break;
			}
		}
		for (testicle i=0;i<20*depth;i++) {
			if (SDL_HasIntersection(&playerRect,&blocks[i].rect) && blocks[i].hardness>0) {
				player.touchingFloor=true;
				playerFalling=0;
				break;
			}
		}
		for (testicle i=0;i<20*depth;i++) {
			if (SDL_HasIntersection(&playerRectHead,&blocks[i].rect) && blocks[i].hardness>0) {
				canJump=false;
				break;
			}else canJump=true;
		}
		for (testicle i=0;i<20*depth;i++) {
			if (SDL_HasIntersection(&playerRectL,&blocks[i].rect)&&blocks[i].hardness>0) {
				player.canMoveLeft=false;
				break;
			}
			else player.canMoveLeft=true;
		}
		for (testicle i=0;i<20*depth;i++) {
			if (SDL_HasIntersection(&playerRectR,&blocks[i].rect)&&blocks[i].hardness>0) {
				player.canMoveRight=false;
				break;
			}
			else player.canMoveRight=true;
		}
		//do not touch from here
		if (blocks[blockThatPlayerIsStandingOnIndex].y%32!=0&&!playerFalling&&jumping==0) {
			offset=blocks[blockThatPlayerIsStandingOnIndex].y%32;
			for (testicle i=0;i<20*depth;i++) {
				blocks[i].y+=(32-offset);
			}
		}
		//to here
		if(backpack>0)topRect.h=35;
		else topRect.h=50;
		SDL_SetRenderDrawColor(renderer,0,0,0,60); //these are the two somewhat transparent rects that are below the text to make it look cool
		SDL_RenderFillRect(renderer,&topRect);
		SDL_RenderFillRect(renderer,&topRightRect);
		yeeterson="Money: $";
		yeeterson.append(std::to_string(money));
		strcpy(ihatecppstrings,yeeterson.c_str());
		surfaceMessage =TTF_RenderText_Solid(Sans,ihatecppstrings,{255,255,255});
		surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
		TTF_SizeText(Sans,ihatecppstrings,&texW,&texH);
		drawTexture(renderer,surfaceTex,SDL_Point{635-texW/2,0},SDL_Point{texW/2,texH/2});
		SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);
		penis=630-texW/2; //just some evil math that detects if the x-position of the "Money" text or the "Backpack" text is lower, i need this to properly display the transparent rects
		
		yeeterson="Backpack: ";
		ihatecppstrings[0]='\0';
		strcat(ihatecppstrings,yeeterson.c_str());
		strcat(ihatecppstrings,convertIntegerToChar(backpack));
		strcat(ihatecppstrings,"/");
		strcat(ihatecppstrings,convertIntegerToChar(backpackCapacities[equippedBackpack]));
		surfaceMessage=TTF_RenderText_Solid(Sans, ihatecppstrings,{255,255,255});
		surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
		TTF_SizeText(Sans,ihatecppstrings,&texW,&texH);
		drawTexture(renderer,surfaceTex,SDL_Point{635-texW/2,15},SDL_Point{texW/2,texH/2});
		SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);
		topRightRect.x=min(630-texW/2,penis); //just some evil math that detects if the x-position of the "Money" text or the "Backpack" text is lower, i need this to properly display the transparent rects
		
		surfaceMessage =TTF_RenderText_Solid(Sans, "Sell Ores",{255,255,255});
		surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
		TTF_SizeText(Sans,"Sell Ores",&texW,&texH);
		drawTexture(renderer,surfaceTex,SDL_Point{635-texW/2,30},SDL_Point{texW/2,texH/2});
		SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);
		sellRect.w=texW/2;
		sellRect.h=texH/2;
		sellRect.x=650-texW/2;
		sellRect.y=30;
		if (mouseHeld&&SDL_HasIntersection(&mouserect,&sellRect) && backpack>0) {
			Mix_PlayChannel(-1,sellSound,0);
			money+=moneyInBackpack;
			moneyInBackpack=backpack=0;
			playerFalling=0;
			for (testicle i=0; i<20*depth;i++) {
				blocks[i].x=(i%20)*32;
				blocks[i].y=500+(i/20)*32;
			}playerdepth=0;
			mouseHeld=false;
		}
		if (mouseHeld&&SDL_HasIntersection(&mouserect,&surfRect) && playerdepth>0) {
			Mix_PlayChannel(-1,menuSound2,0);
			playerFalling=0;
			for (testicle i=0; i<20*depth;i++) {
				blocks[i].x=(i%20)*32;
				blocks[i].y=500+(i/20)*32;
			}playerdepth=0;
			mouseHeld=false;
		}

		if (mouseHeld&&SDL_HasIntersection(&mouserect,&shopRect)) {  //when you click shop button
			shop=true;
			player.movingLeft=player.movingRight=false; //just needed to get rid of buggy behavior when you move and click shop with move held donw
			Mix_PlayChannel(-1,menuSound2,0);
			while (shop) {
				if (SDL_PollEvent(&event)) { //event detection
					if (event.type == SDL_QUIT) { //check if you quit window, then set running to false and break
						shop=false;
						game=false;
						break;
					}
					if (event.type==SDL_MOUSEMOTION) {
						mouserect.x=event.motion.x;
						mouserect.y=event.motion.y;
						mouserect.w=mouserect.h=1;
					}
					if (event.type==SDL_MOUSEBUTTONDOWN) {
						if (event.button.button==SDL_BUTTON_LEFT) mouseHeld=true;
					}
					if (event.type==SDL_MOUSEBUTTONUP) {
						if (event.button.button==SDL_BUTTON_LEFT) mouseHeld=false;
					}
					if (event.type==SDL_MOUSEWHEEL) {
						if(event.wheel.y < 0) {
							if (shopPicks&&shopIndex<PICKAXES-1)shopIndex++;
							if (!shopPicks&&shopIndex<BACKPACKS-1)shopIndex++;
						}
						if(event.wheel.y > 0 && shopIndex>0) shopIndex--;
					}
				}
				for (testicle i=0;i<640/32;i++) { //fill bakground
					for (testicle j=0;j<640/32;j++) {
						drawTexture(renderer,blockImages[3],SDL_Point{i*32,j*32},SDL_Point{32,32});
					}
				}
				surfaceMessage =TTF_RenderText_Solid(Sans, "Back",{255,255,255});
				surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
				TTF_SizeText(Sans,"Back",&texW,&texH);
				drawTexture(renderer,surfaceTex,SDL_Point{0,0},SDL_Point{texW/2,texH/2});
				SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);

				surfaceMessage =TTF_RenderText_Solid(Sans, "Pickaxes",{255,255,255});
				surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
				TTF_SizeText(Sans,"Pickaxes",&texW,&texH);
				drawTexture(renderer,surfaceTex,SDL_Point{100,0},SDL_Point{texW/2,texH/2});
				SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);
				SDL_Rect pickaxesRect;
				pickaxesRect.x=100;
				pickaxesRect.y=0;
				pickaxesRect.w=texW/2;
				pickaxesRect.h=texH/2;

				surfaceMessage =TTF_RenderText_Solid(Sans, "Backpacks",{255,255,255});
				surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
				TTF_SizeText(Sans,"Backpacks",&texW,&texH);
				drawTexture(renderer,surfaceTex,SDL_Point{200,0},SDL_Point{texW/2,texH/2});
				SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);
				SDL_Rect backpacksRect;
				backpacksRect.x=200;
				backpacksRect.y=0;
				backpacksRect.w=texW/2;
				backpacksRect.h=texH/2;

				shopRect.w=texW/2;
				shopRect.h=texH/2;
				shopRect.x=5;
				shopRect.y=5;
				if (mouseHeld&&SDL_HasIntersection(&mouserect,&shopRect)) {
					Mix_PlayChannel(-1,menuSound2,0);
					shop=false;
					mouseHeld=false;
				}
				if (mouseHeld&&SDL_HasIntersection(&mouserect,&pickaxesRect)) {
					Mix_PlayChannel(-1,menuSound2,0);
					shopPicks=true;
					mouseHeld=false;
					shopIndex=0;
				}
				if (mouseHeld&&SDL_HasIntersection(&mouserect,&backpacksRect)) {
					Mix_PlayChannel(-1,menuSound2,0);
					shopPicks=false;
					mouseHeld=false;
					shopIndex=0;
				}
				if (shopPicks) {
					pee = &pickNames[shopIndex][0];
					surfaceMessage =TTF_RenderText_Solid(Sans, pee,{255,255,255});
					surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
					TTF_SizeText(Sans,pee,&texW,&texH);
					drawTexture(renderer,surfaceTex,SDL_Point{320-texW/2,150},SDL_Point{texW,texH});
					SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);

					pee = &pickDesc[shopIndex][0];
					surfaceMessage =TTF_RenderText_Solid(Sans, pee,{255,255,255});
					surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
					TTF_SizeText(Sans,pee,&texW,&texH);
					drawTexture(renderer,surfaceTex,SDL_Point{320-texW/4,190},SDL_Point{texW/2,texH/2});
					SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);
					
					ihatecppstrings[0]='\0';
					strcat(ihatecppstrings,"Cost: $");
					strcat(ihatecppstrings,convertIntegerToChar(pickPrices[shopIndex]));
					surfaceMessage =TTF_RenderText_Solid(Sans, ihatecppstrings,{255,255,255});
					surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
					TTF_SizeText(Sans,ihatecppstrings,&texW,&texH);
					drawTexture(renderer,surfaceTex,SDL_Point{320-texW/4,450},SDL_Point{texW/2,texH/2});
					SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);

					if (shopIndex==pickOwned+1){
						surfaceMessage =TTF_RenderText_Solid(Sans, "Buy",{255,255,255});
						TTF_SizeText(Sans,"Buy",&texW,&texH);
					}else if (shopIndex<=pickOwned&&shopIndex!=pickaxe){
						surfaceMessage =TTF_RenderText_Solid(Sans, "Equip",{255,255,255});
						TTF_SizeText(Sans,"Equip",&texW,&texH);
					}else if (shopIndex!=pickaxe) {
						surfaceMessage =TTF_RenderText_Solid(Sans, "Locked",{255,255,255});
						TTF_SizeText(Sans,"Locked",&texW,&texH);
					}if (shopIndex==pickaxe){
						surfaceMessage =TTF_RenderText_Solid(Sans, "Equipped",{255,255,255});
						TTF_SizeText(Sans,"Equipped",&texW,&texH);
					}
					surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
					drawTexture(renderer,surfaceTex,SDL_Point{320-texW/4,470},SDL_Point{texW/2,texH/2});
					SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);
					buyRect.x=320-texW/4;
					buyRect.y=470;
					buyRect.w=texW/2;
					buyRect.h=texH/2;
					if (mouseHeld&&SDL_HasIntersection(&mouserect,&buyRect)) { //if you clikc the buy button
						if (shopIndex==pickOwned+1&&money>=pickPrices[shopIndex]) { //if the button says buy
							pickOwned=shopIndex;
							pickaxe=pickOwned;
							money-=pickPrices[shopIndex];
							Mix_PlayChannel(-1,menuSound,0);
						}else if (shopIndex<=pickOwned&&shopIndex!=pickaxe){ //if the button says equip
							pickaxe=shopIndex;
							Mix_PlayChannel(-1,menuSound,0);
						} //we ignore it if it says equipped or locked
					}
					if (shopIndex==12||shopIndex==19||shopIndex==20||shopIndex==21) drawTexture(renderer,pickImages[shopIndex],SDL_Point{320-150,250},SDL_Point{300,150}); //draw the pickaxe image, some W  I  D  E  picakxe images require different scaling so the if-else
					else drawTexture(renderer,pickImages[shopIndex],SDL_Point{320-75,250},SDL_Point{150,150});
					if (shopIndex==11||shopIndex==18||shopIndex==19||shopIndex==20) {
					if (shopIndex<PICKAXES-1) drawTexture(renderer,pickImages[shopIndex+1],SDL_Point{480,300},SDL_Point{75,37});
					}
					else {
						if (shopIndex<PICKAXES-1) drawTexture(renderer,pickImages[shopIndex+1],SDL_Point{480,300},SDL_Point{37,37});
					}
					if (shopIndex==13||shopIndex==20||shopIndex==21||shopIndex==22) {
					if (shopIndex>0) drawTexture(renderer,pickImages[shopIndex-1],SDL_Point{80,300},SDL_Point{75,37});
					}
					else {
						if (shopIndex>0) drawTexture(renderer,pickImages[shopIndex-1],SDL_Point{80,300},SDL_Point{37,37});
					}
				}
				//yeetus
				else {
					pee = &backpacks[shopIndex][0];
					surfaceMessage =TTF_RenderText_Solid(Sans, pee,{255,255,255});
					surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
					TTF_SizeText(Sans,pee,&texW,&texH);
					drawTexture(renderer,surfaceTex,SDL_Point{320-texW/2,150},SDL_Point{texW,texH});
					SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);

					pee = &backpackDesc[shopIndex][0];
					surfaceMessage =TTF_RenderText_Solid(Sans, pee,{255,255,255});
					surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
					TTF_SizeText(Sans,pee,&texW,&texH);
					drawTexture(renderer,surfaceTex,SDL_Point{320-texW/4,190},SDL_Point{texW/2,texH/2});
					SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);
					
					ihatecppstrings[0]='\0';
					strcat(ihatecppstrings,"Cost: $");
					strcat(ihatecppstrings,convertIntegerToChar(backpackPrices[shopIndex]));
					surfaceMessage =TTF_RenderText_Solid(Sans, ihatecppstrings,{255,255,255});
					surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
					TTF_SizeText(Sans,ihatecppstrings,&texW,&texH);
					drawTexture(renderer,surfaceTex,SDL_Point{320-texW/4,450},SDL_Point{texW/2,texH/2});
					SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);

					if (shopIndex==ownedBackpacks+1){
						surfaceMessage =TTF_RenderText_Solid(Sans, "Buy",{255,255,255});
						TTF_SizeText(Sans,"Buy",&texW,&texH);
					}else if (shopIndex<=ownedBackpacks&&shopIndex!=equippedBackpack){
						surfaceMessage =TTF_RenderText_Solid(Sans, "Equip",{255,255,255});
						TTF_SizeText(Sans,"Equip",&texW,&texH);
					}else if (shopIndex!=equippedBackpack) {
						surfaceMessage =TTF_RenderText_Solid(Sans, "Locked",{255,255,255});
						TTF_SizeText(Sans,"Locked",&texW,&texH);
					}if (shopIndex==equippedBackpack){
						surfaceMessage =TTF_RenderText_Solid(Sans, "Equipped",{255,255,255});
						TTF_SizeText(Sans,"Equipped",&texW,&texH);
					}
					surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
					drawTexture(renderer,surfaceTex,SDL_Point{320-texW/4,470},SDL_Point{texW/2,texH/2});
					SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);
					buyRect.x=320-texW/4;
					buyRect.y=470;
					buyRect.w=texW/2;
					buyRect.h=texH/2;
					if (mouseHeld&&SDL_HasIntersection(&mouserect,&buyRect)) { //if you clikc the buy button
						if (shopIndex==ownedBackpacks+1&&money>=backpackPrices[shopIndex]) { //if the button says buy
							ownedBackpacks=shopIndex;
							equippedBackpack=ownedBackpacks;
							money-=backpackPrices[shopIndex];
							Mix_PlayChannel(-1,menuSound,0);
						}else if (shopIndex<=ownedBackpacks&&shopIndex!=equippedBackpack){ //if the button says equip
							equippedBackpack=shopIndex;
							Mix_PlayChannel(-1,menuSound,0);
						} //we ignore it if it says equipped or locked
					}
					/*if (shopIndex==12||shopIndex==19||shopIndex==20||shopIndex==21) drawTexture(renderer,pickImages[shopIndex],SDL_Point{320-150,250},SDL_Point{300,150}); //draw the pickaxe image, some W  I  D  E  picakxe images require different scaling so the if-else
					else drawTexture(renderer,pickImages[shopIndex],SDL_Point{320-75,250},SDL_Point{150,150});
					if (shopIndex==11||shopIndex==18||shopIndex==19||shopIndex==20) {
					if (shopIndex<PICKAXES-1) drawTexture(renderer,pickImages[shopIndex+1],SDL_Point{480,300},SDL_Point{75,37});
					}
					else {
						if (shopIndex<PICKAXES-1) drawTexture(renderer,pickImages[shopIndex+1],SDL_Point{480,300},SDL_Point{37,37});
					}
					if (shopIndex==13||shopIndex==20||shopIndex==21||shopIndex==22) {
						if (shopIndex>0) drawTexture(renderer,pickImages[shopIndex-1],SDL_Point{80,300},SDL_Point{75,37});
						}
						else {
							if (shopIndex>0) drawTexture(renderer,pickImages[shopIndex-1],SDL_Point{80,300},SDL_Point{37,37});
						}*/
				}
				yeeterson="Money: $";
				yeeterson.append(std::to_string(money));
				strcpy(ihatecppstrings,yeeterson.c_str());
				surfaceMessage =TTF_RenderText_Solid(Sans,ihatecppstrings,{255,255,255});
				surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
				TTF_SizeText(Sans,ihatecppstrings,&texW,&texH);
				drawTexture(renderer,surfaceTex,SDL_Point{630-texW/2,0},SDL_Point{texW/2,texH/2});
				SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);

				SDL_RenderPresent(renderer);
			}
			mouseHeld=false;
		}

		ihatecppstrings[0]='\0';
		strcat(ihatecppstrings,"Depth: ");
		strcat(ihatecppstrings,convertIntegerToChar(playerdepth));
		strcat(ihatecppstrings,"m");
		surfaceMessage =TTF_RenderText_Solid(Sans, ihatecppstrings,{255,255,255});
		surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
		TTF_SizeText(Sans,ihatecppstrings,&texW,&texH);
		drawTexture(renderer,surfaceTex,SDL_Point{5,0},SDL_Point{texW/2,texH/2});
		SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);

		surfaceMessage =TTF_RenderText_Solid(Sans, "To Surface",{255,255,255});
		surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
		TTF_SizeText(Sans,"To Surface",&texW,&texH);
		drawTexture(renderer,surfaceTex,SDL_Point{5,15},SDL_Point{texW/2,texH/2});
		SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);

		surfRect.w=texW/2;
		surfRect.h=texH/2;
		surfRect.x=5;
		surfRect.y=15;

		if (playerdepth<=0&&backpack<=0) {
			surfaceMessage =TTF_RenderText_Solid(Sans, "Shop",{255,255,255});
			surfaceTex=SDL_CreateTextureFromSurface(renderer,surfaceMessage);
			TTF_SizeText(Sans,"Shop",&texW,&texH);
			drawTexture(renderer,surfaceTex,SDL_Point{5,30},SDL_Point{texW/2,texH/2});
			SDL_FreeSurface(surfaceMessage);SDL_DestroyTexture(surfaceTex);

			shopRect.w=texW/2;
			shopRect.h=texH/2;
			shopRect.x=5;
			shopRect.y=30;
		}
		else shopRect.x=1000;
		SDL_RenderPresent(renderer);
	}
	//This will run when game loop exits
	ofstream my_file;
	my_file.open("stats/money.fpd", ios::in|ios_base::trunc);
	if (my_file.is_open()) my_file<<money;
	my_file.close();
	
	my_file.open("stats/ownedpicks.fpd", ios::in|ios_base::trunc);
	if (my_file.is_open()) my_file<<pickOwned;
	my_file.close();

	my_file.open("stats/equippedpick.fpd", ios::in|ios_base::trunc);
	if (my_file.is_open()) my_file<<pickaxe;
	my_file.close();

	my_file.open("stats/ownedbackpacks.fpd", ios::in|ios_base::trunc);
	if (my_file.is_open()) my_file<<ownedBackpacks;
	my_file.close();

	my_file.open("stats/equippedbackpack.fpd", ios::in|ios_base::trunc);
	if (my_file.is_open()) my_file<<equippedBackpack;
	my_file.close();
	for (testicle i=0;i<PICKAXES;i++)destroyTexture(pickImages[i]);
	for (testicle i=0;i<sizeof(blockImages)/sizeof(blockImages[0]);i++)destroyTexture(blockImages[i]);
	for (testicle i=0;i<sizeof(oreImages)/sizeof(oreImages[0]);i++)destroyTexture(oreImages[i]);
	for(testicle i=0;i<5;i++) Mix_FreeChunk(sounds[i]);
	Mix_FreeChunk(menuSound);
	Mix_FreeChunk(menuSound2);
	Mix_FreeChunk(sellSound);
	Mix_FreeMusic(level1);
	Mix_FreeMusic(level2);
	Mix_FreeMusic(level3);
	Mix_FreeMusic(level4);
	TTF_CloseFont(Sans);
 	SDL_DestroyWindow(window);
 	SDL_DestroyRenderer(renderer);
	TTF_Quit();
 	SDL_Quit();
    return 0;
}