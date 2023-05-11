#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <limits>
#include "header.h"
using namespace std;

int main(int argc, char* argv[])
{       
    
    ifstream mapfile(argv[1]);
    vector<vector<char>> mapvv;
    string map_line;
    while (getline(mapfile, map_line))
    {
        vector<char> map_line1(map_line.begin(), map_line.end());
        mapvv.push_back(map_line1);
    }   mapfile.close();
    
    int squares_number = mapvv[1].size() * mapvv.size();
    vector<int> temp_map(squares_number, 0);
    for (int i = 0; i < squares_number; i++)
        if (mapvv[i / mapvv[0].size()][i % mapvv[0].size()] == '9')
            temp_map[i] = 1;
    temp_map.push_back(mapvv[0].size());

    vector<UNIT_STATUS> present_units;
    UNIT_STATUS load_unit;
    string load;
    int gold;
    
    ifstream statusfile(argv[2]);
    statusfile >> gold;
    while (statusfile >> load_unit.belongs_to >> load_unit.type >> load_unit.ID >> load_unit.X >> load_unit.Y >> load_unit.stamina)
    {        
        load_unit.speed = load_unit.Speed();
        
        load_unit.is_building = ' ';
        if (load_unit.type == 'B')
            statusfile >> load_unit.is_building;

        present_units.push_back(load_unit);
    }
    statusfile.close();
    
    vector<ORDER> orders_out;
    fstream plansfile("bot2.txt");
    //Minimax('P', 2, numeric_limits<int>::min(), numeric_limits<int>::max(), present_units, gold, temp_map, orders_out, plansfile);
    plansfile.close();
    
    ofstream ordersfile(argv[3]);
    for (ORDER o : orders_out)
        ordersfile << o.OrdersOut();
    ordersfile.close();
        
    return 0;
}
