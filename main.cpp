#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include "header.h"
using namespace std;

void CheckWinner(int& last_round, vector<UNIT_STATUS>& units_vector);

COORDINATES LocalizeBase(const vector<vector<char>>& map_container,const char& which_player);

bool game_over = false;

char disqualified_player = ' ';


int main() 
{
    vector<vector<char>> map;
    ifstream mapfile("mapa.txt");
    map = GetMap(mapfile);
    mapfile.close();
    
    int squares_number = map[0].size() * map.size();
    int temp_map[squares_number] = {};
    for (int i = 0; i < squares_number; i++)
    {
        int k = i % map[0].size();
        int l = i / map[0].size();
        if (map[l][k] == '9')
            temp_map[i] = 1;
    }
    temp_map[-1] = map[0].size();
    
    COORDINATES base_coords[2];
    base_coords[0] = LocalizeBase(map, '1');
    base_coords[1] = LocalizeBase(map, '2');
    
    UNIT_STATUS PlayerBase('P', 0, base_coords[0].X, base_coords[0].Y);
    UNIT_STATUS EnemyBase('E', 1, base_coords[1].X, base_coords[1].Y);
    
    vector<UNIT_STATUS> present_units;
    present_units.push_back(PlayerBase);
    present_units.push_back(EnemyBase);
    
    int gold = 2000;
    
    char players[2] = {'P', 'E'};
    int active_player = 0;
    int round = 0;
    while (round < 10 && !game_over)
    {
        active_player == 0 ? system("./bot mapa.txt status.txt rozkazy.txt") : system("./bot2 mapa.txt status.txt rozkazy.txt");
        
        for (UNIT_STATUS u : present_units) 
        {        
            if (u.belongs_to == players[active_player])
            {
                u.speed = u.Speed();
                
                if (u.type == 'B' && u.rounds_to_release > 0){
                    u.rounds_to_release--;}
                if (u.type == 'B' && u.rounds_to_release == 0){
                    AddNewUnit(present_units, players[active_player]);}
            }
            else
                temp_map[u.Y * map[0].size() + u.X];
        }
        
        int mining_workers = 0;
        for(UNIT_STATUS u : present_units)
        {
            if (u.type == 'W' && map[u.Y][u.X] == '6' && u.belongs_to == players[active_player])
                mining_workers++;
        }
        gold += mining_workers * 50;
                    
        ifstream orderfile("rozkazy.txt");
        ExecuteOrders(orderfile, present_units, players[active_player], gold);
        orderfile.close();

        
        ofstream outstatusfile("status.txt");
        outstatusfile << gold << endl;    
        for(UNIT_STATUS u : present_units)
            u.StatusOut(outstatusfile);
        outstatusfile.close();
      
        if (active_player)
            round++;
        
        active_player = ++active_player % 2;
    }
    
    CheckWinner(round, present_units);

    return 0;    
}

COORDINATES LocalizeBase(const vector<vector<char>>& map_container, const char& which_player)
{
    int i = 0, j = 0;
    for (vector<char> u : map_container)
    {
        auto it = find(u.begin(), u.end(), which_player);
        if ( it != u.end())
        {
            j = distance(u.begin(), it);
            return {i, j};
        }
        i++;
    }
    return {-1, -1};
}

void CheckWinner(int& last_round, vector<UNIT_STATUS>& units_vector)
{
    char winner;
    if (last_round >= 1)
    {
        int player_wins = 0;
        int enemy_wins = 0;
        for (UNIT_STATUS u : units_vector){
            if (u.belongs_to == 'P')
                player_wins++;
            else
                enemy_wins++;
        }
        if (player_wins > enemy_wins)
            winner = 'P';
        else if (enemy_wins > player_wins)
            winner = 'E';
        else
        {
            cout << "It's a draw!" << endl;
            return;
        }
    }
    
    for (UNIT_STATUS u : units_vector)
    {
        if (u.type == 'B')
            winner = u.belongs_to;
    }
    
    if (winner == 'P')
        cout << "Player wins!" << endl;
    else 
        cout << "Enemy wins!" <<  endl;
}
