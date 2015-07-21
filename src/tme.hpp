
#ifndef TMX_MAP_ENGINE
#define TMX_MAP_ENGINE

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <tmx.h>


//Showin

namespace tme
{
  //The direction's and the corner's orders are clockwise.
  //tme directions : 
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

  //tme corners :
#define CORNER_UR 4
#define CORNER_DR 5
#define CORNER_DL 6
#define CORNER_UL 7

  //these two types don't have to be use by the user
  typedef std::unordered_multimap<std::string,std::string>* surroundProperties;
  typedef std::unordered_multimap<std::string,std::string> properties;

  //in mapEvent.cpp
  class Surroundings
  {
  public : 
    Surroundings(surroundProperties props);//This constructor don't have to be used by the user
    bool getEvent(int direction, std::string name);
    bool getEvent(int direction, std::string name, std::string value);
  private :
    surroundProperties srnd;
  };



  class TmxME
  {
  public :
    TmxME(SDL_Renderer* , char* fichier, SDL_Rect posMap);
    void setCameraPosition(int x, int y);
    SDL_Rect posMapToPosRend(SDL_Rect pos);
    void display();
  private :
    tmx_map* tmxMap;
    SDL_Renderer* renderer;//this has to be the same renderer than the user's renderer
    int scrollX; //scrolling position regarding the map
    int scrollY;
    SDL_Rect rectMap; //position de la map dans le renderer + dimension dans le renderer
  };



}
#endif
