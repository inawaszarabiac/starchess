#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <limits>
#include "header.h"
using namespace std;

int Minimax(const char player, const int depth, int alpha, int beta, vector<UNIT_STATUS> units_vector, int player_gold, vector<int> map, vector<ORDER>& best_move, iostream& plans);

int EvaluatePosition(const char& player, vector<UNIT_STATUS>& units_vector, const vector<int>& map);

int CountMaterial (const char& player, vector<UNIT_STATUS>& unit_container, const vector<int>& map);

int CancelOrders(const vector<UNIT_STATUS>& saved_data, vector<UNIT_STATUS>& units_vector, const int& saved_gold);

vector<UNIT_STATUS> SaveUnitsParams(const vector<ORDER>& orders, const vector<UNIT_STATUS>& units_vector);

vector<vector<ORDER>> GenerateOrdersLists (vector<UNIT_STATUS>& units_vector, int& gold, vector<int>& map, const char& player);

void AppendUnitOrders(const vector<vector<ORDER>>& orders_vector, int index, int aaaa, vector<vector<ORDER>>& orders_list);

int GenerateBuilds(vector<ORDER>& orders_vector, const UNIT_STATUS& u, const int& player_gold);

int GenerateMoves(vector<ORDER>& orders_vector, const UNIT_STATUS& u, const vector<int>& map, int speed_left = 0);

int GenerateAttacks(vector<ORDER>& orders_vector, const UNIT_STATUS& u, const vector<UNIT_STATUS>& units_vector, const vector<int>& map);


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
    fstream plansfile("bot.txt");
    Minimax('P', 10, numeric_limits<int>::min(), numeric_limits<int>::max(), present_units, gold, temp_map, orders_out, plansfile);
    plansfile.close();
    
    ofstream ordersfile(argv[3]);
    for (ORDER o : orders_out)
        ordersfile << o.OrdersOut();
    ordersfile.close();
        
    return 0;
}

int CancelOrders(const vector<UNIT_STATUS>& saved_data, vector<UNIT_STATUS>& units_vector, const int& saved_gold)
{
    for (UNIT_STATUS u : saved_data)
    {
        auto it = find_if(units_vector.begin(), units_vector.end(),
                          [&](const UNIT_STATUS u1) {
                              return u1.ID == u.ID; });
        *it = u;
    }
    return saved_gold;
}

vector<UNIT_STATUS> SaveUnitsParams(const vector<ORDER>& orders, const vector<UNIT_STATUS>& units_vector)
{
    vector<UNIT_STATUS> units_to_save;
    for (ORDER o : orders)
    {
        auto it = find_if(units_vector.begin(), units_vector.end(), 
                          [&](const UNIT_STATUS u1) {
                              return u1.ID == o.ID; });
        units_to_save.push_back(*it);
    }
    for (ORDER o : orders)
    {
        if (o.action == 'A')
        {
            auto it = find_if(units_vector.begin(), units_vector.end(), 
                            [&](const UNIT_STATUS u1) {
                                return u1.ID == o.attack_parameter; });
            units_to_save.push_back(*it);
        }
    }
    return units_to_save;
}

int Minimax(const char player, const int depth, int alpha, int beta, vector<UNIT_STATUS> units_vector, int player_gold, vector<int> map, vector<ORDER>& best_move, iostream& plans)
{
    if (depth == 0 ) //|| game_over()
    {
        return EvaluatePosition(player, units_vector, map);
    }
    if (player == 'P') 
    {
        int best_score = numeric_limits<int>::min();
        plans << "depth " << to_string(depth) << endl;
        for (vector<ORDER> OrdersList : GenerateOrdersLists(units_vector, player_gold, map, player )) 
        {
            string to_stream = string();
            for (ORDER o : OrdersList)
                to_stream = to_stream + o.OrdersOut();

            stringstream ordersstream(to_stream);
            vector<UNIT_STATUS> current_units_params = SaveUnitsParams(OrdersList, units_vector);
            int current_gold = player_gold;
                       
            ExecuteOrders(ordersstream, units_vector, 'P', player_gold);
            int current_score = Minimax('E', depth - 1, alpha, beta, units_vector, player_gold, map, best_move, plans);            
            
            for (ORDER o : OrdersList)
                plans << o.OrdersOut() << endl;
            plans << player_gold << ' ' << current_score << endl;    
            for(UNIT_STATUS u : units_vector)
            {
                u.StatusOut(plans);
                if (u.type == 'B' && u.is_building != '0')
                    plans << u.rounds_to_release;
            }
            plans << endl;
            
            player_gold = CancelOrders(current_units_params, units_vector, current_gold);
            
            best_score = max(best_score, current_score);
            if (best_score > alpha)
            {
                alpha = best_score;
                best_move = OrdersList;
            }
            if (beta <= alpha)
                break;
        }
        return best_score;
    } else {
        int best_score = numeric_limits<int>::max();
        plans << "depth " << to_string(depth) << endl;
        for (vector<ORDER> OrdersList : GenerateOrdersLists(units_vector, player_gold, map, player )) 
        {
            string to_stream = string();
            for (ORDER o : OrdersList)
                to_stream = to_stream + o.OrdersOut();
                        
            stringstream ordersstream(to_stream);
            vector<UNIT_STATUS> current_units_params = SaveUnitsParams(OrdersList, units_vector);
            int current_gold = player_gold;
            
            ExecuteOrders(ordersstream, units_vector, 'E', player_gold);
            int current_score = Minimax('P', depth - 1, alpha, beta, units_vector, player_gold, map, best_move, plans);
            
            for (ORDER o : OrdersList)
                plans << o.OrdersOut() <<  endl;
            plans << player_gold << current_score << endl;    
            for(UNIT_STATUS u : units_vector)
            {
                u.StatusOut(plans);
                if (u.type == 'B' && u.is_building != '0')
                    plans << u.rounds_to_release;
            }
            plans << endl;
            
            player_gold = CancelOrders(current_units_params, units_vector, current_gold);
            
            best_score = min(best_score, current_score);
            if (best_score < beta)
            {
                beta = best_score;
            }
            beta = min(beta, best_score);
            if (beta <= alpha)
                break;
        }
        return best_score;
    }
}

vector<vector<ORDER>> GenerateOrdersLists (vector<UNIT_STATUS>& units_vector, int& player_gold, vector<int>& map, const char& player)
{
    vector<UNIT_STATUS> ally_units;
    for (UNIT_STATUS u : units_vector)
    {
        if (u.belongs_to != player)
            map[u.Y * map.back() + u.X] = 1;
        else
        {
            ally_units.push_back(u);
            if (u.type == 'B' && u.is_building != '0')
                --u.rounds_to_release;

            if (u.type == 'B' && u.rounds_to_release == 0)
                {
                    int last_id = 0;
                    for (UNIT_STATUS u : units_vector)
                        if (u.ID > last_id)
                            last_id = u.ID;
                    
                    UNIT_STATUS new_unit(player, u.is_building, last_id + 1, u.X, u.Y);
                    
                    units_vector.push_back(new_unit);
                    ally_units.push_back(new_unit);
                    u.is_building = '0';   
                }
        }
    }
    
    vector<vector<ORDER>> orders(ally_units.size(), vector<ORDER>(0));
    int count = 0;
    int possible_lists = 1;
    
    for (UNIT_STATUS u : ally_units)
        {
            if (u.type == 'B')
            {
                possible_lists *= GenerateBuilds(orders[count], u, player_gold);
            }
            else
            {
                possible_lists *= GenerateAttacks(orders[count], u, units_vector, map);
                possible_lists *= GenerateMoves(orders[count], u, map);     
            }
            count++;
        }         

    vector<vector<ORDER>> orders_list(possible_lists, vector<ORDER>(0));
    int index = 0;
    AppendUnitOrders(orders, index, possible_lists, orders_list);
       
    return orders_list;
}


void AppendUnitOrders(const vector<vector<ORDER>>& orders_vector, int index, int aaaa, vector<vector<ORDER>>& orders_list)
{
    int times_to_repeat = aaaa/orders_vector[index].size();
    int k = 0; 
    
    for (int j = 0; j < orders_list.capacity(); j++)
    {
        if (!(j % times_to_repeat))
            k++;
        if (k == orders_vector[index].size())
            k = 0;
        if (orders_vector[index][k].parameter == ' ' && orders_vector[index][k].attack_parameter == -1 && orders_vector[index][k].X == -1 && orders_vector[index][k].Y == -1)
            continue;
        
        orders_list[j].push_back(orders_vector[index][k]);
    }

    index++;
    if (index == orders_vector.size())
        return;
    AppendUnitOrders(orders_vector, index, times_to_repeat, orders_list);
}

int GenerateBuilds(vector<ORDER>& orders_vector, const UNIT_STATUS& u, const int& player_gold)
{
    int possible_orders = 0;
    ORDER temp(u.ID, 'B');
    orders_vector.push_back(temp);
    
    if (u.is_building == '0')
        for (int i = 6; i < 8; i++)
        {
            if (player_gold >= unit[i].cost && unit[i].type != 'B')
            {
                temp.parameter = unit[i].type;
                orders_vector.push_back(temp);
                possible_orders++;
                
            }
        }
        return possible_orders + 1;
}

int GenerateMoves(vector<ORDER>& orders_vector, const UNIT_STATUS& u, const vector<int>& map, int speed_left)
{
    int possible_orders = 0;
    if (!speed_left)
        speed_left = u.speed;
    ORDER temp(u.ID, 'M');
    orders_vector.push_back(temp);
    int map_height = (map.size()-1)/(map.back());
    
    for (int x = max(0, u.X - speed_left); x <= min(map.back(), u.X + speed_left); x++)
        for (int y = max(0, u.Y - speed_left); y <= min(map_height, u.Y + speed_left); y++)
        {
            int d = u.MeasureDistance(x, y);
            if (d <= u.speed && !map[y * map[-1] + x] && d > 0)
            {
                temp.X = x;
                temp.Y = y;
                orders_vector.push_back(temp);
                possible_orders++;
            }
        }
    return possible_orders + 1;
}

int GenerateAttacks(vector<ORDER>& orders_vector, const UNIT_STATUS& u, const vector<UNIT_STATUS>& units_vector, const vector<int>& map)
{
    int possible_orders = 0;
    ORDER temp(u.ID, 'A');
    int temp_speed = u.speed;
    int index = u.IdentifyType();
    int map_height = (map.size()-1)/(map.back());
    
    for (UNIT_STATUS ut : units_vector)
    {
        if (ut.belongs_to == 'E')
        {
            temp.attack_parameter = ut.ID;
            int d = u.MeasureDistance(ut.X, ut.Y);
            if (d <= temp_speed + unit[index].range - 1)
                for (int x = max(0, ut.X - unit[index].range); x <= min(map.back(), ut.X + unit[index].range); x++)
                    for (int y = max(0, ut.Y - unit[index].range); y <= min(map_height, ut.Y + unit[index].range); y++)
                    {
                        int d1 = u.MeasureDistance(x, y);
                        if (d1 <= temp_speed - 1 && !map[y * map[-1] + x])
                        {
                            ORDER move_order(u.ID, 'M');
                            move_order.X = x;
                            move_order.Y = y;
                            orders_vector.push_back(move_order);
                            orders_vector.push_back(temp);
                            temp_speed -= (d1 + 1);
                            if (temp_speed > 0)
                                possible_orders += GenerateMoves(orders_vector, u, map, temp_speed);
                        }
                    }
        }
    }
    return possible_orders + 1;
}

int EvaluatePosition(const char& player, vector<UNIT_STATUS>& units_vector, const vector<int>& map)
{
    char enemy = 'E';
    int player_eval = CountMaterial(player, units_vector, map);
    int enemy_eval = CountMaterial(enemy, units_vector, map);
    
    int evaluation = player_eval - enemy_eval;
    
    return evaluation;
}

int CountMaterial (const char& player, vector<UNIT_STATUS>& unit_container, const vector<int>& map)
{
    const int knight_value =    400;
    const int swordsman_value = 250;
    const int archer_value =    250;
    const int pikeman_value =   200;
    const int ram_value =       500;
    const int catapult_value =  800;
    const int worker_value =    100;
    const int base_value =      1000000;
    int material = 0;
    int miners_counter = 0;
    
    for (UNIT_STATUS u : unit_container){
        
        if (u.belongs_to == player)
        {
            int index = u.IdentifyType();
            switch(u.type)
            {
                case 'K':
                    material += knight_value -(unit[index].stamina - u.stamina);
                    break;
                    
                case 'S':
                    material += swordsman_value -(unit[index].stamina - u.stamina);            
                    break;
                    
                case 'A':                
                    material += archer_value -(unit[index].stamina - u.stamina);
                    break;
                    
                case 'P':                
                    material += pikeman_value -(unit[index].stamina - u.stamina);            
                    break;
                    
                case 'R':     
                    material += ram_value -(unit[index].stamina - u.stamina);            
                    break;
                    
                case 'C':     
                    material += catapult_value -(unit[index].stamina - u.stamina);            
                    break;
                    
                case 'W':      
                    material += worker_value -(unit[index].stamina - u.stamina);           
                    break;
                    
                case 'B':       
                    material += base_value -(unit[index].stamina - u.stamina);
            }
        }
        if (u.belongs_to == player && u.type == 'W' && map[u.Y * map.back() + u.X] == '6' && miners_counter < 3)
        {
            miners_counter++;
            material += 400;
        }
    }
    
    return material;
}
