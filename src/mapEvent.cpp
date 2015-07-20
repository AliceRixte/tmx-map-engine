#include "map.hpp"

using namespace std;

void Map::initEvents()
{
    vector<char*> vect_vide(0);
    for(int i = 0; i<MAP_EVENT_NUM ; i++)
        events[i] = vect_vide;

    unordered_multimap<string,string> event_vide;
    for(int i = 0; i<8; i++)
        events2[i] = event_vide;
}





bool Map::findEventPair(int dir, const string name, const string value)
{
    auto range = events2[dir].equal_range(name);
    pair<const string,string> pair_tmp = make_pair(name,value);
    bool found = find(range.first,range.second,pair_tmp) != range.second;
    return found;
}

bool Map::findEventPair(int dir, const string name)
{
    return events2[dir].count(name) > 0;
}

bool Map::getEventMap(unsigned char mapDir, const string name, const string value)
{
    bool retour = false;
    if(!retour && (mapDir & MAP_UP) != 0)
        retour = findEventPair(UP,name,value);
    if(!retour && (mapDir & MAP_LEFT) != 0)
        retour = findEventPair(LEFT,name,value);
    if(!retour && (mapDir & MAP_DOWN) != 0)
        retour = findEventPair(DOWN,name,value);
    if(!retour && (mapDir & MAP_RIGHT) != 0)
        retour = findEventPair(RIGHT,name,value);
    if(!retour && (mapDir & MAP_CORNER_UL) != 0)
        retour = findEventPair(CORNER_UL,name,value);
    if(!retour && (mapDir & MAP_CORNER_DL) != 0)
        retour = findEventPair(CORNER_DL,name,value);
    if(!retour && (mapDir & MAP_CORNER_DR) != 0)
        retour = findEventPair(CORNER_DR,name,value);
    if(!retour && (mapDir & MAP_CORNER_UR) != 0)
        retour = findEventPair(CORNER_UR,name,value);
    return retour;
}

bool Map::getEventMap(unsigned char mapDir, const string name)
{
    bool retour = false;
    if(!retour && (mapDir & MAP_UP) != 0)
        retour = findEventPair(UP,name);
    if(!retour && (mapDir & MAP_LEFT) != 0)
        retour = findEventPair(LEFT,name);
    if(!retour && (mapDir & MAP_DOWN) != 0)
        retour = findEventPair(DOWN,name);
    if(!retour && (mapDir & MAP_RIGHT) != 0)
        retour = findEventPair(RIGHT,name);
    if(!retour && (mapDir & MAP_CORNER_UL) != 0)
        retour = findEventPair(CORNER_UL,name);
    if(!retour && (mapDir & MAP_CORNER_DL) != 0)
        retour = findEventPair(CORNER_DL,name);
    if(!retour && (mapDir & MAP_CORNER_DR) != 0)
        retour = findEventPair(CORNER_DR,name);
    if(!retour && (mapDir & MAP_CORNER_UR) != 0)
        retour = findEventPair(CORNER_UR,name);
    return retour;
}


void Map::readThroughTile(int x, int y, int dir, tmx_layer* layer)
{
    tmx_property* prop = tmx_get_tile(tmx_carte,layer->content.gids[y*tmx_carte->width + x])->properties;

    while(prop)
    {
        try
        {
            if(x == 1)
            cout<<x<<" "<<y<<" "<<prop->name<<" "<<prop->value<<endl;
            string prop_name = prop->name;
            string prop_value = prop->value;
            events2[dir].insert(make_pair(prop_name,prop_value));
        }
        catch(exception const& e)
        {
            cerr<<"Erreur dans les propriétés des tile : "<< e.what()<<endl;
        }

        prop = prop->next;
    }

}



void Map::updateEventMap(frontiers voisins)
{
    initEvents();
    //attention, cast nécessaire car sinon bug au niveau des comparaisons quand voisins.x1/y1 négatifs.

    if(!(voisins.x2<0 || voisins.y2<0 || voisins.x1 >= (int)tmx_carte->width || voisins.y1 >= (int)tmx_carte->height))
    {
        bool map_limits[4] = {false,false,false,false};
        if(voisins.x1 < 0)
        {
            voisins.x1 = -1;
            map_limits[LEFT] = true;
        }
        if(voisins.y1 < 0)
        {
            map_limits[UP] = true;
            voisins.y1 = -1;
        }
        if(voisins.x2 >= (int)tmx_carte->width)
        {
            map_limits[RIGHT] = true;
            voisins.x2 = tmx_carte->width;
        }
        if(voisins.y2 >= (int)tmx_carte->height)
        {
            map_limits[DOWN] = true;
            voisins.y2 = tmx_carte->height;
        }

        for(int i = 0; i<4; i++)
            if(voisins.isNeighbor[i])
                events2[i].insert(make_pair("limit",""));


        tmx_layer* layer = tmx_carte->ly_head;
        if(voisins.isNeighbor[UP] && !map_limits[UP])
        {
            for(int x = voisins.x1 + 1; x < voisins.x2; x++)
                readThroughTile(x,voisins.y1,UP,layer);
        }

        if(voisins.isNeighbor[LEFT] && !map_limits[LEFT])
            for(int y = voisins.y1 + 1 ; y < voisins.y2; y++)
                readThroughTile(voisins.x1,y,LEFT,layer);
        if(voisins.isNeighbor[DOWN] && !map_limits[DOWN])
            for(int x = voisins.x1 + 1; x < voisins.x2; x++)
                readThroughTile(x,voisins.y2,DOWN,layer);
        if(voisins.isNeighbor[RIGHT] && !map_limits[RIGHT])
            for(int y = voisins.y1 + 1; y < voisins.y2; y++)
                readThroughTile(voisins.x2,y,RIGHT,layer);
    }
}


//la fonction getNeighbors permet de récupérer la position des tiles voisins s'ils touchent à TOLERANCE_FRONTIERE près le rectangle envoyé
//les arguments envoyés doiventetre corrects : aucune partie du rectangle ne doit sortir de la map
//cette fonction devra être probablement passée en private
frontiers Map::getNeighbors(float posX,float posY, int w, int h) //retourne un tableau de taille 4
{
    if(fabs(1 - (posX - (int)posX) < TOLERANCE_FRONTIERE))
        posX = (int)posX;
    if(fabs(1 - (posY - (int)posY) < TOLERANCE_FRONTIERE))
        posY = (int)posY;
    frontiers voisins;

    float modPosX = mod_float(posX,tmx_carte->tile_width);//position relative au tile de gauche
    float modPosY = mod_float(posY,tmx_carte->tile_height);//position relative au tile du haut
    float modDimX = mod_float(posX + (float)w,tmx_carte->tile_width);//position relative au tile de droite
    float modDimY = mod_float(posY + (float)h,tmx_carte->tile_height);//position relative au tile du bas

    voisins.x1 = (int) posX / tmx_carte->tile_width - 1;
    voisins.y1 = (int) posY / tmx_carte->tile_height- 1;
    voisins.x2 = ((int) posX + w) / tmx_carte->tile_width + 1;
    voisins.y2 = ((int) posY + h) / tmx_carte->tile_height + 1;

   /* if(((float) tmx_carte->tile_height - modPosY) <TOLERANCE_FRONTIERE)
    {
        voisins.y1 ++;
        voisins.isNeighbor[UP] = true;
    }
    else*/
        voisins.isNeighbor[UP] = modPosY < TOLERANCE_FRONTIERE;


    if(modDimY < TOLERANCE_FRONTIERE)
    {
        voisins.y2 --;
        voisins.isNeighbor[DOWN] = true;
    }
    //else
      //  voisins.isNeighbor[DOWN] = ((float) tmx_carte->tile_height - modDimY) <TOLERANCE_FRONTIERE;

   /* if(((float) tmx_carte->tile_width - modPosX) <TOLERANCE_FRONTIERE)
    {
        voisins.x1 ++;
        voisins.isNeighbor[LEFT] = true;
    }
    else*/
        voisins.isNeighbor[LEFT] = modPosX < TOLERANCE_FRONTIERE;

    if(modDimX < TOLERANCE_FRONTIERE)
    {
        //cout<<"tol"<<endl;
        voisins.x2--;
        voisins.isNeighbor[RIGHT] = true;
    }
    /*else
        voisins.isNeighbor[RIGHT] = ((float) tmx_carte->tile_width - modDimX) < TOLERANCE_FRONTIERE;*/
    cout<<"pos "<<posX<<" "<<posY<<endl;
    cout<<"voisins "<<voisins.x1<<" "<<voisins.x2<<" "<<voisins.y1<<" "<<voisins.y2<<endl;


    return voisins;
}



///Anciennes fonctions

/*

bool Map::getEventMap(unsigned char mapDir, event_map a_event, string value)
{
    bool retour = false;
    if(!retour && (mapDir & MAP_UP) != 0)
        retour = !(find(events[a_event].begin(), events[a_event].end(), value) == events[a_event].end());
    if(!retour && (mapDir & MAP_LEFT) != 0)
        retour = !(find(events[a_event + 1].begin(), events[a_event + 1].end(), value) == events[a_event + 1].end());
    if(!retour && (mapDir & MAP_DOWN) != 0)
        retour = !(find(events[a_event + 2].begin(), events[a_event + 2].end(), value) == events[a_event + 2].end());
    if(!retour && (mapDir & MAP_RIGHT) != 0)
        retour = !(find(events[a_event + 3].begin(), events[a_event + 3].end(), value) == events[a_event + 3].end());
    if(!retour && (mapDir & MAP_CORNER_UL) != 0)
        retour = !(find(events[a_event + 4].begin(), events[a_event + 4].end(), value) == events[a_event + 4].end());
    if(!retour && (mapDir & MAP_CORNER_DL) != 0)
        retour = !(find(events[a_event + 5].begin(), events[a_event + 5].end(), value) == events[a_event + 5].end());
    if(!retour && (mapDir & MAP_CORNER_DR) != 0)
        retour = !(find(events[a_event + 6].begin(), events[a_event + 6].end(), value) == events[a_event + 6].end());
    if(!retour && (mapDir & MAP_CORNER_UR) != 0)
        retour = !(find(events[a_event + 7].begin(), events[a_event + 7].end(), value) == events[a_event + 7].end());
    return retour;
}


void Map::updateEventMap(vector< tmx_property* >* frontieres, tmx_property**  coins)
{
    initEvents();
    if(frontieres)
    {
        for(int i = 0; i<4; i++)
        {
            for(int j = 0; j<frontieres[i].size(); j++)
            {
                if(frontieres[i][j])
                {

                    tmx_property* prop_tamp = frontieres[i][j];

                    while(prop_tamp)
                    {
                        //cout<<"boucle"<<endl;
                        if(!prop_tamp->name)
                            cout<<"Une propriété n'a pas été nommée (son nom vaut NULL)."<<endl;
                        if(!prop_tamp->value)
                            cout<<"Une valeur n'a pas été nommée (son nom vaut NULL). Le nom de la propriété est "<< frontieres[i][j]->name <<endl;
                        string name_prop = prop_tamp->name;
                        if(name_prop == "bord_map")
                            events[MAP_LIMIT + i] .push_back(prop_tamp->value);
                        else if(name_prop == "collision")
                        {
                            events[MAP_COLLISION + i].push_back(prop_tamp->value);
                        }

                        else if(name_prop == "path")
                        {
                            events[MAP_PATH + i].push_back(frontieres[i][j]->value);

                            //cout<<"path "<<frontieres[i][j]->value<<endl;
                        }

                        else
                            events[MAP_DEFAULT + i].push_back(prop_tamp->value);
                        prop_tamp = prop_tamp->next;
                    }
                }
            }
        }
    }


    if(coins)
    {
        //gestion des coins

        for(int i = 0; i<4; i++)
        {
            if(coins[i])
            {
                tmx_property* prop_tamp = coins[i];
                while(prop_tamp)
                {
                    if(!prop_tamp->name)
                        cout<<"Une propriété n'a pas été nommée (son nom vaut NULL)."<<endl;
                    if(!prop_tamp->value)
                        cout<<"La valeur d'une propriété n'a pas été nommée (son nom vaut NULL)."<<endl;

                    string name_prop = prop_tamp->name;

                    if(name_prop == "bord_map")
                        events[MAP_LIMIT + 4 + i].push_back(prop_tamp->value);
                    else if(name_prop == "collision")
                        events[MAP_COLLISION + 4 + i].push_back(prop_tamp->value);
                    else
                        events[MAP_DEFAULT + 4 + i].push_back(prop_tamp->value);

                    prop_tamp = prop_tamp->next;

                }
            }
        }
    }

}


void Map::getBorders(float posX, float posY, int w, int h, border frontieres, corner coins) //retourne un tableau de taille 4
{
    tmx_layer* layer = tmx_carte->ly_head;

    if(posX<0 || posX + (float)w > (float)(tmx_carte->tile_width*tmx_carte->width) || posY + (float)h > (float)(tmx_carte->tile_height*tmx_carte->height))
    {
        cout<<"Frontière : dépassement de la map"<<endl;
    }
    float modPosX = mod_float(posX,tmx_carte->tile_width);//position relative au tile de gauche
    float modPosY = mod_float(posY,tmx_carte->tile_height);//position relative au tile du haut
    float modDimX = mod_float(posX + (float)w,tmx_carte->tile_width);//position relative au tile de droite
    float modDimY = mod_float(posY + (float)h,tmx_carte->tile_height);//position relative au tile du bas

    bool frontUp = modPosY < TOLERANCE_FRONTIERE || ((float) tmx_carte->tile_height - modPosY) <TOLERANCE_FRONTIERE;//vaut true si frontière commune en haut
    bool frontDown = modDimY < TOLERANCE_FRONTIERE || ((float) tmx_carte->tile_height - modDimY) <TOLERANCE_FRONTIERE;
    bool frontLeft = modPosX < TOLERANCE_FRONTIERE || ((float) tmx_carte->tile_width - modPosX) <TOLERANCE_FRONTIERE;
    bool frontRight = modDimX < TOLERANCE_FRONTIERE || ((float) tmx_carte->tile_width - modDimX) <TOLERANCE_FRONTIERE;

    ///calcul du nombre de tile en contact avec le personnage
    int nb_frontieres_w = 0;//nombre de tile en contact avec le haut ou le bas (c'est la même nombre)
    int nb_frontieres_h = 0;//nombre de tile en contact avec la droite ou la gauche (c'est le même nombre)

    //nb_frontiere haut et bas
    if(frontUp || frontDown)//on vérifie que c'est utile de faire le calcul
    {
        float largeur_restante = (modPosX - (float)tmx_carte->tile_width) + (float)w;//largeur en enlevant le bout du début
        if(largeur_restante <= 0)//si c'est négatif ou nul, c'est que la frontière ne comporte qu'un tile.
            nb_frontieres_w = 1;
        else
        {
            nb_frontieres_w = 1 + (int)largeur_restante / tmx_carte->tile_width;
            //la frontière comporte le bout enlevé + le résultat de la division + 1 si le reste n'est pas nul.
            if(mod_float(largeur_restante, tmx_carte->tile_width) > TOLERANCE_FRONTIERE)
                nb_frontieres_w += 1;
        }
    }


    //nb_frontiere droite et gauche
    if(frontLeft || frontRight)
    {
        float hauteur_restante = (modPosY - (float)tmx_carte->tile_height) + (float)h;//largeur en enlevant le bout du début
        if(hauteur_restante <= 0)//si c'est négatif ou nul, c'est que la frontière ne comporte qu'un tile.
            nb_frontieres_h = 1;
        else
        {
            nb_frontieres_h = 1 + (int)hauteur_restante / tmx_carte->tile_height;
            //la frontière comporte le bout enlevé + le résultat de la division + 1 si le reste n'est pas nul.
            if(mod_float(hauteur_restante,tmx_carte->tile_height)> TOLERANCE_FRONTIERE)
                nb_frontieres_h += 1;
        }
    }




    int posTileX = (int) posX / tmx_carte->tile_width;//la position en nombre de tile
    int posTileY = (int) posY / tmx_carte->tile_height;
    int posDimX = ((int)posX + w - 1)/tmx_carte->tile_width; //position en nombre de tile en comptant la taille du rectangle
    int posDimY = ((int)posY + h - 1)/tmx_carte->tile_height;


    //Gestion des coins



    if(coins)
    {
        if(frontLeft && frontUp)//coin supérieur gauche
        {

            if(posTileX > 0 && posTileY > 0)
                coins[0] = tmx_get_tile(tmx_carte,layer->content.gids[(posTileY-1)*tmx_carte->width + posTileX - 1])->properties;
            else
                coins[0] = horsMap;
        }
        if(frontLeft && frontDown)//coin inférieur gauche
        {
            if(posTileX >0 && posDimY < tmx_carte->height - 1)
                coins[1] = tmx_get_tile(tmx_carte,layer->content.gids[(posDimY + 1)*tmx_carte->width + posTileX - 1])->properties;
            else
                coins[1] = horsMap;
        }
        if(frontRight && frontDown)// coin inférieur droit
        {
            if(posDimX < tmx_carte->width - 1 && posDimY<tmx_carte->height - 1)
                coins[2] = tmx_get_tile(tmx_carte,layer->content.gids[(posDimY + 1)*tmx_carte->width + posDimX + 1])->properties;
            else
                coins[2] = horsMap;
        }
        if(frontRight && frontUp)
        {
            if(posDimX < tmx_carte->width - 1 && posTileY > 0)
                coins[3] = tmx_get_tile(tmx_carte,layer->content.gids[(posTileY - 1)*tmx_carte->width + posDimX + 1])->properties;
            else
                coins[3] = horsMap;
        }
    }


    //gestion des frontières

    if(frontieres)
    {
        if(frontUp)//frontière du haut
        {
            if(posTileY>0)//si on n'est pas au bord
                for(int i = posTileX; i < posTileX + nb_frontieres_w; i++)
                    frontieres[0].push_back(tmx_get_tile(tmx_carte,layer->content.gids[(posTileY - 1)*tmx_carte->width + i])->properties);
            else //si on est au bord
                for(int i = posTileX; i < posTileX + nb_frontieres_w; i++)
                    frontieres[0].push_back(horsMap);
        }

        if(frontLeft)//frontière de gauche
        {

            if(posTileX>0)//si on n'est pas au bord
                for(int j = posTileY; j < posTileY + nb_frontieres_h; j++)
                    frontieres[1].push_back(tmx_get_tile(tmx_carte,layer->content.gids[j*tmx_carte->width + posTileX - 1])->properties);
            else //si on est au bord
                for(int j = posTileY; j < posTileY + nb_frontieres_h; j++)
                {
                    frontieres[1].push_back(horsMap);
                }

        }
        if(frontDown)//frontière du bas
        {

            if(posDimY<tmx_carte->height - 1)//si on n'est pas au bord
                for(int i = posTileX; i < posTileX + nb_frontieres_w; i++)
                    frontieres[2].push_back(tmx_get_tile(tmx_carte,layer->content.gids[(posDimY + 1)*tmx_carte->width + i])->properties);
            else //si on est au bord
                for(int i = posTileX; i < posTileX + nb_frontieres_w; i++)
                    frontieres[2].push_back(horsMap);
        }


        if(frontRight)//frontière de droite
        {
            if(posDimX<tmx_carte->width - 1)//si on n'est pas au bord
                for(int j = posTileY; j < posTileY + nb_frontieres_h; j++)
                    frontieres[3].push_back(tmx_get_tile(tmx_carte,layer->content.gids[j*tmx_carte->width + posDimX + 1])->properties);
            else //si on est au bord
                for(int j = posTileY; j < posTileY + nb_frontieres_h; j++)
                    frontieres[3].push_back(horsMap);
        }
    }
}
*/
