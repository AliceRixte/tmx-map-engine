#include "map.hpp"

using namespace std;



bool Map::isMoving()
{
    return !(isStoppedX && isStoppedY);
}

void Map::startMovX(float* a_posX, float* a_movX, int w)
{
    posX = a_posX;
    movX = a_movX;
    movW = w;
    try
    {
        if(!posX || !posY)
            throw invalid_argument("L'une des positions du mouvement n'a pas été initialisée.");
        updateEventMap(getNeighbors(*posX,*posY,movW,movH));
    }catch(exception const& e)
    {cerr<<e.what()<<endl;}

    isStoppedX = false;

}

void Map::startMovY(float* a_posY, float* a_movY, int h)
{
    posY = a_posY;
    movY = a_movY;
    movH = h;
    try
    {
        if(!posX || !posY)
            throw invalid_argument("L'une des positions du mouvement n'a pas été initialisée.");
        updateEventMap(getNeighbors(*posX,*posY,movW,movH));
    }catch(exception const& e)
        {cerr<<e.what()<<endl;}

    isStoppedY = false;
}

void Map::startMov(float* a_posX, float* a_posY,float* a_movX, float* a_movY, int w, int h)
{
    posX = a_posX;
    movX = a_movX;
    movW = w;
    posY = a_posY;
    movY = a_movY;
    movH = h;

    isStoppedX = false;
    isStoppedY = false;

    try
    {
        if(!posX || !posY)
            throw invalid_argument("L'une des positions du mouvement n'a pas été initialisée.");
        updateEventMap(getNeighbors(*posX,*posY,movW,movH));
    }catch(exception const& e)
        {cerr<<e.what()<<endl;}
}

void Map::stopMovX()
{
    isStoppedX = true;
}

void Map::stopMovY()
{
    isStoppedY = true;
}

void Map::restartMovX()
{
    isStoppedX = false;
}

void Map::restartMovY()
{
    isStoppedY = false;
}

void Map::nextStep()
{

    try
    {
        if(!posX || !posY)
            throw invalid_argument("L'une des positions du mouvement n'a pas été initialisée.");
        frontiers neighbors = getNeighbors(*posX,*posY,movW,movH);
        calcMovX(neighbors);
        calcMovY(neighbors);
        updateEventMap(getNeighbors(*posX,*posY,movW,movH));
    }catch(exception const& e)
        {cerr<<e.what()<<endl;}
}

void Map::calcMovX(frontiers const& neighbors)
{
    if(*movX != 0 && !isStoppedX)
    {

        ///calcul de la marge possible de mouvement à droite et à gauche
        float margeGauche = 0.;
        float margeDroite = 0.;

        if(*movX > 0.)//si on va à droite
            margeGauche = (float)tmx_carte->tile_width - mod_float(*posX , tmx_carte->tile_width);
        else //si on va à gauche
        {
            margeGauche = mod_float(*posX,tmx_carte->tile_width);
            if(neighbors.isNeighbor[LEFT])
                margeGauche += (float)tmx_carte->tile_width;//si on va à gauche, il faut ajouter le tile de droite qui n sera pas pris en compte
        }

        if(*movX > 0.)//si on va à droite
            margeDroite = (float)tmx_carte->tile_width - mod_float(*posX + (float)movW , tmx_carte->tile_width);
        else //si on va à gauche
        {
            margeDroite = mod_float(*posX + (float)movW,tmx_carte->tile_width);
            if(neighbors.isNeighbor[RIGHT])
                margeDroite += (float)tmx_carte->tile_width;
        }

        ///le mouvement est alors le minimum ci-dessous
        float marge = min(margeDroite,margeGauche);
        float mouv = min(marge,(float)fabs(*movX));
        if(*movX<0)
            mouv= -mouv;
        cout<<"mouv : "<<mouv<<endl;
        *posX += mouv;
        *movX -= mouv;
    }
}

void Map::calcMovY(frontiers const& neighbors)
{
    if(*movY != 0 && !isStoppedY)
    {
        ///calcul de la marge possible de mouvement à droite et à gauche

        float margeHaut = 0.;
        float margeBas = 0.;

        if(*movY > 0.)//si on va en bas
            margeHaut = (float)tmx_carte->tile_height - mod_float(*posY , tmx_carte->tile_height);
        else //si on va en haut
        {
            margeHaut = mod_float(*posY,tmx_carte->tile_height);
            if(neighbors.isNeighbor[UP])
                margeHaut += (float)tmx_carte->tile_height;
        }

        if(*movY > 0.)//si on va en bas
            margeBas = (float)tmx_carte->tile_height - mod_float(*posY + (float)movH , tmx_carte->tile_height);
        else //si on va en haut
        {
            margeBas = mod_float(*posY + (float)movH,tmx_carte->tile_height);
            if(neighbors.isNeighbor[DOWN])
                margeBas += (float)tmx_carte->tile_height;
        }


        ///le mouvement est alors le minimum ci-dessous
        float marge = min(margeBas,margeHaut);
        float mouv = min(marge,(float)fabs(*movY));
        if(*movY<0)
            mouv = -mouv;


        *posY += mouv;
        *movY -= mouv;
    }
}

//*******************************************Fonctions propres au tower defense**************************************************

void Map::calculatePath()
{

   /* ///Recherche des propriétés des chemins (récupération du tile initial, de la direction et du nombre de rails)
    tmx_property* property = tmx_carte->properties;
    bool foundX =  false;
    bool foundY = false;
    bool foundDir = false;
    bool foundNum = false;

    int xInit,yInit, numRow;
    Direction dirInit;

    //récupération des paramètres relativement au chemin
    while(property && (!foundX || !foundY || !foundDir || !foundNum))
    {
        if(!foundX && strcmp(property->name,"first_row_pos_x") == 0)
        {
            foundX = true;
            xInit = atoi(property->value);
        }
        else if(!foundY && strcmp(property->name,"first_row_pos_y") == 0)
        {
            foundY = true;
            yInit = atoi(property->value);
        }
        else if(!foundDir && strcmp(property->name,"dir") == 0)
        {
            foundDir = true;
            char* dirInit_tamp = property->value;
            if(strcmp(dirInit_tamp, "UP") == 0)
                dirInit = UP;
            else if(strcmp(dirInit_tamp, "LEFT") == 0)
                dirInit = LEFT;
            else if(strcmp(dirInit_tamp, "DOWN") == 0)
                dirInit = DOWN;
            else if(strcmp(dirInit_tamp, "RIGHT") == 0)
                dirInit = RIGHT;
        }
        else if(!foundNum && strcmp(property->name,"row_number") == 0)
        {
            foundNum = true;
            numRow = atoi(property->value);
        }
        property = property->next;
    }
    if(!foundX || !foundY || !foundNum || !foundDir)
        cout<<"Erreur : les paramètres concernant le chemin n'ont pas tous été trouvés."<<endl;


    event_map coll = MAP_COLLISION;//collision qui dirige le chemin
    Direction dir;
    int x ,y;
    int posX, posY;

    vector<tmx_property*> frontieres[4];//ce frontieres est temporaires
    vector<tmx_property*> vect_vide(0);
    for(int i =0 ; i < 4; i++)
        frontieres[i] = vect_vide;

    tmx_layer* layer = tmx_carte->ly_head;
    string value = "";

    for(int i = 0; i < numRow; i++)
    {

        dir = dirInit;
        x = xInit;
        y = yInit + i;

        bool stop = false;

        if ((x == 0 && dir == LEFT) || (y == 0 && dir == UP) ||
                (x == tmx_carte->width - 1 && dir == RIGHT) || (y == tmx_carte->height - 1 && dir == DOWN))
            stop = true;

        while(!stop)
        {
            rows[x][y] = i;
            vector<tmx_property*> vect_vide(0);
            for(int i =0 ; i < 4; i++)
                frontieres[i] = vect_vide;
            cout<<x<<" "<<y<<" "<<dir<<endl;
            // frontieresgetBorders(posX,posY,tmx_carte->tile_width, tmx_carte->tile_height,frontieres, NULL);
            if(y>0)
                frontieres[0].push_back(tmx_get_tile(tmx_carte,layer->content.gids[(y-1)*tmx_carte->width + x])->properties);
            else if(dir!=UP)
                frontieres[0].push_back(horsMap);
            else
                stop = true;

            if(x>0)
                frontieres[1].push_back(tmx_get_tile(tmx_carte,layer->content.gids[y*tmx_carte->width + x - 1])->properties);
            else if(dir != LEFT)
                frontieres[1].push_back(horsMap);
            else
                stop = true;

            if(y < tmx_carte->height - 1)
                frontieres[2].push_back(tmx_get_tile(tmx_carte,layer->content.gids[(y+1)*tmx_carte->width + x])->properties);
            else if(dir != DOWN)
                frontieres[2].push_back(horsMap);
            else
                stop = true;

            if(x < tmx_carte->width - 1)
                frontieres[3].push_back(tmx_get_tile(tmx_carte,layer->content.gids[y*tmx_carte->width + x + 1])->properties);
            else if(dir != RIGHT)
                frontieres[3].push_back(horsMap);
            else
                stop = true;

            //for(int i = 0; i<4; i++)
            //   cout<<frontieres[i][0]->name<<endl;
            this->updateEventMap(frontieres, NULL);

            tmx_property* propPath = tmx_get_tile(tmx_carte,layer->content.gids[y*tmx_carte->width + x])->properties;
            bool trouvePath = strcmp(propPath->name,"path") == 0;
            while(propPath && !trouvePath)
            {
                propPath = propPath->next;
                if(propPath && propPath->name)
                    trouvePath = strcmp(propPath->name,"path") == 0;
            }
            if(!trouvePath)
                cout<<"Erreur : le chemin n'est pas correct (un tile de type path était attendu)"<<endl;
            else
            {
                //value = intToString(i);
                string tamp = intToString(i);
                propPath->value = "0";//(char*) tamp.c_str();
               // cout<<propPath->value<<endl;
            }


            switch(dir)
            {
            case UP :
                if(this->getEventMap(MAP_LEFT,coll,value))
                {
                    if(this->getEventMap(MAP_UP,coll,value))
                        dir = RIGHT;
                }
                else
                    dir = LEFT;
                break;
            case LEFT :
                if(this->getEventMap(MAP_DOWN,coll,value))
                {
                    if(this->getEventMap(MAP_LEFT,coll,value))
                        dir = UP;
                }
                else
                    dir = DOWN;
                break;
            case DOWN :
                if(this->getEventMap(MAP_RIGHT,coll,value))
                {
                    if(this->getEventMap(MAP_DOWN,coll,value))
                        dir = LEFT;
                }
                else
                    dir = RIGHT;
                break;
            case RIGHT :
                if(this->getEventMap(MAP_UP,coll,value))
                {
                    if(this->getEventMap(MAP_RIGHT,coll,value))
                        dir = DOWN;
                }
                else
                    dir = UP;
                break;
            }


            switch(dir)
            {
            case UP :
                if(!this->getEventMap(MAP_UP,MAP_LIMIT))
                    y--;
                else
                    stop = true;
                break;
            case LEFT :
                if(!this->getEventMap(MAP_LEFT,MAP_LIMIT))
                x--;
                else
                    stop = true;
                break;
            case DOWN :
                y++;
                break;
            case RIGHT :
                x++;
                break;
            }
        }
        cout<<"boucle suivante"<<endl;
        coll = MAP_PATH;
    }*/
}

/*void Map::calcMovX(float* posX, float* movX , int w, border frontieres)
{
    if(*movX != 0)
    {

        ///calcul de la marge possible de mouvement à droite et à gauche
        float margeGauche = 0.;
        float margeDroite = 0.;

        if(*movX > 0.)//si on va à droite
            margeGauche = (float)tmx_carte->tile_width - mod_float(*posX , tmx_carte->tile_width);
        else //si on va à gauche
        {
            margeGauche = mod_float(*posX,tmx_carte->tile_width);
            if(frontieres[1].size()>0)
                margeGauche += (float)tmx_carte->tile_width;//si on va à gauche, il faut ajouter le tile de droite qui n sera pas pris en compte
        }

        if(*movX > 0.)//si on va à droite
            margeDroite = (float)tmx_carte->tile_width - mod_float(*posX + (float)w , tmx_carte->tile_width);
        else //si on va à gauche
        {
            margeDroite = mod_float(*posX + (float)w,tmx_carte->tile_width);
            if(frontieres[3].size()>0)
                margeDroite += (float)tmx_carte->tile_width;
        }

        ///le mouvement est alors le minimum ci-dessous
        float marge = min(margeDroite,margeGauche);
        float mouv = min(marge,(float)fabs(*movX));
        if(*movX<0)
            mouv = -mouv;
        *posX += mouv;
        *movX -= mouv;
    }
}

void Map::calcMovY(float* posY, float* movY , int h, border frontieres)
{
    if(*movY != 0)
    {
        ///calcul de la marge possible de mouvement à droite et à gauche

        float margeHaut = 0.;
        float margeBas = 0.;

        if(*movY > 0.)//si on va en bas
            margeHaut = (float)tmx_carte->tile_height - mod_float(*posY , tmx_carte->tile_height);
        else //si on va en haut
        {
            margeHaut = mod_float(*posY,tmx_carte->tile_height);
            if(frontieres[0].size()>0)
                margeHaut += (float)tmx_carte->tile_height;
        }

        if(*movY > 0.)//si on va en bas
            margeBas = (float)tmx_carte->tile_height - mod_float(*posY + (float)h , tmx_carte->tile_height);
        else //si on va en haut
        {
            margeBas = mod_float(*posY + (float)h,tmx_carte->tile_height);
            if(frontieres[2].size()>0)
                margeBas += (float)tmx_carte->tile_height;
        }


        ///le mouvement est alors le minimum ci-dessous
        float marge = min(margeBas,margeHaut);
        float mouv = min(marge,(float)fabs(*movY));
        if(*movY<0)
            mouv = -mouv;

        *posY += mouv;
        *movY -= mouv;
    }
}


*/
