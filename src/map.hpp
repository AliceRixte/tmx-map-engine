//fonctionnement :
//on charge une map à prtir d'un fichier
//on envoie le rectangle dans lequel la map doit s'afficher.
//possibilité de centrer le scrolling sur une position précise (setPosCam). Cett position est relative à la map.
//on récupère la position initiale du personnage principale avec getPosInit.
//on peut contrôler le mouvement des personnages avec calcMov, qui fonctionne avec une boucle qui ne s'arrête qu'une fois le mouvement terminé
//à n'importe quel moment, on peut récupérer les propriétés des tile adjacents ainsi que celles dont le coin est en contact avec un des coins du personnage.

//fonctions à risque (bug possibles)
//getBorders
//calcMov

#ifndef MAP
#define MAP


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <utility>
#include <string>
#include <iostream>
#include <sstream>
#include <tmx.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <cmath>


#include <cassert>




#define MAP_UP 1
#define MAP_LEFT 2
#define MAP_DOWN 4
#define MAP_RIGHT 8
#define MAP_CORNER_UL 16
#define MAP_CORNER_DL 32
#define MAP_CORNER_DR 64
#define MAP_CORNER_UR 128
#define MAP_UP_DOWN 5
#define MAP_LEFT_RIGHT 10

#define MAP_EVENT_NUM 32 //nombre total d'évènements

enum event_map {MAP_DEFAULT = 0, MAP_COLLISION = 8, MAP_LIMIT = 16, MAP_PATH = 24};//liste des évènements possibles
enum Direction {UP = 0,LEFT = 1,DOWN = 2,RIGHT = 3, CORNER_UL = 4, CORNER_DL = 5, CORNER_DR = 6, CORNER_UR = 7};

#define TOLERANCE_FRONTIERE 0.001

SDL_Texture* loadTextureImg(SDL_Renderer* renderer, const char* fichier);
bool isPosInRect(const int x, const int y, const SDL_Rect rect);
bool isPosInRect(const SDL_Rect pos, const SDL_Rect rect);
void printRect(const SDL_Rect rect);
std::string intToString(int n);


typedef struct frontiers
{
    int x1,x2,y1,y2;
    bool isNeighbor[4];
} frontiers;

typedef std::vector< tmx_property* >*  border;
typedef tmx_property** corner;



std::vector<SDL_Texture*> tilesetToVect(SDL_Renderer* renderer, SDL_Texture* tileset,int tileW, int tileH, int nbTile);
float mod_float(float,int);

class Map
{
public :
    Map(SDL_Renderer* , char* fichier, SDL_Rect posMap);//constructeur
    ~Map();                                           //Destructeur

    ///MAPSHOW.CPP
    void getDimMap(int* w, int* h);
    void getPosInit(float* x, float* y); //récupère la position initiale renseignée dans la map
    void setPosCam(float x,float y); //change la position du scrolling, le mieux est de donner la position du personnage relativement à la map.
    void setRectMap(SDL_Rect posMap);//change la position et les dimensions du rectangle dans lequel est affichée la map.
    SDL_Rect posMapToPosRend(SDL_Rect pos); //convertit une position dans la map en position d'affichage sur le renderer
    SDL_Rect getRectMap();//récupère la position et les dimensions du rectangle dans lequel est affichée la map.*
    void showMap();//affiche la map à la position de scrolling

    ///MAPMOV.CPP
    void getDimTile(int* w, int* h);
    void getFirstRowPos(int* x, int* y);
    void calculatePath();
    bool isMoving();
    void startMov(float* posX, float* posY, float* movX, float* movY, int w, int h);
    void startMovX(float* posX, float* movX, int w);
    void startMovY(float* posY, float* movY, int h);
    void stopMovX();
    void stopMovY();
    void restartMovX();
    void restartMovY();
    void nextStep();
    void calcMovX(frontiers const& neighbors);
    void calcMovY(frontiers const& neighbors);

    /// MAPEVENT.CPP
    frontiers getNeighbors(float posX, float posY, int w, int h);
    frontiers getEnlargedNeighbors(float posX, float posY, int w, int h);
    void updateEventMap(frontiers voisins);
    bool getEventMap(unsigned char mapDir, const std::string name, const std::string value);
    bool getEventMap(unsigned char mapDir, const std::string name);


private :

    void calculatePath(int num_row);
    void readThroughTile(int x, int y, int dir, tmx_layer* layer);
    bool findEventPair(int dir, const std::string name, const std::string value);
    bool findEventPair(int dir, const std::string name);

    tmx_map* tmx_carte;
    SDL_Renderer* renderer;//renderer global
    int scrollX; //Position du scrolling
    int scrollY;
    SDL_Rect rectMap; //position de la map dans le renderer + dimension dans le renderer
    int posInitX; //position initiale du personnage principal
    int posInitY;

    //gestion du mouvement
    float* posX, *posY, *movX, *movY;
    int movW, movH;
    bool isStoppedX, isStoppedY;

    std::vector< std::vector <int> > rows;
    std::unordered_multimap<std::string,std::string> events2[8]; //remplacer le nom par events

    tmx_property* horsMap;//attention doit nécessairement être déclarée ici. Sinon, lorsque déclarée en locale, destrcteur appelé.

    void initEvents();//réinitialise les évènements. (utilisée dans updateEventMap)
    std::vector<char*> events[MAP_EVENT_NUM]; // tableau des évènements.

};

#endif
