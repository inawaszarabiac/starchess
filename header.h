#ifndef HEADER_H
#define HEADER_H

class COORDINATES
{
public:
    int X, Y;
    
    int MeasureDistance(const int& targetX, const int& targetY) const
    {
        return abs(X - targetX) + abs(Y - targetY);   
    }
};

class UNIT 
{
public:
    char type;
	int stamina;
    int speed;
    int cost;
    int range;
    int build_time;
    
    UNIT(const char& x1, const int& x2, const int& x3, const int& x4, const int& x5, const int& x6)
    {
        type = x1;
        stamina = x2;
        speed = x3;
        cost = x4;
        range = x5;
        build_time = x6;
    }
    
    UNIT(const char& x1, const int& x2, const int& x3)
    {
        type = x1;
        stamina = x2;
        speed = x3;
    }
    
    UNIT()
    {
        
    }
};

const std::array<UNIT, 8> unit = {UNIT ('K', 70, 5, 400, 1, 5), UNIT ('S', 60, 2, 250, 1, 3), UNIT ('A', 40, 2, 250, 5, 3), UNIT ('P', 50, 2, 200, 2, 3), UNIT ('R', 90, 2, 500, 1, 4), UNIT ('C', 50, 2, 800, 7, 6), UNIT ('W', 20, 2, 100, 1, 1), UNIT ('B', 200, 0)};

const int attack_table[7][8] = {{35,35,35,35,35,50,35,35},{30,30,30,20,20,30,30,30},{15,15,15,15,10,10,15,15},{35,15,15,15,10,10,15,10},{40,40,40,40,40,40,40,50},{10,10,10,10,10,10,10,50},{5,5,5,5,5,5,5,1}};

class UNIT_STATUS : public COORDINATES
{
public:
    char belongs_to;
    char type;
    int ID;
    int stamina;
    char is_building;
    int speed;
    int rounds_to_release;
    
    UNIT_STATUS(const char& x1, const char& x2, const int& x3, const int& x4, const int& x5)
    {
        belongs_to = x1;
        type = x2;
        ID = x3;
        X = x4;
        Y = x5;
        stamina = Stamina();
        speed = Speed();
        is_building = ' ';
        rounds_to_release = -1;
    }
    
    UNIT_STATUS(const char& x1, const int& x3, const int& x4, const int& x5)
    {
        belongs_to = x1;
        type = 'B';
        ID = x3;
        X = x4;
        Y = x5;
        stamina = 200;
        speed = 0;
        is_building = '0';
        rounds_to_release = -1;
    }
    
    UNIT_STATUS()
    {
        
    }
    
    
    void StatusOut(std::ostream& file) const
    {
        std::string stat_out = std::string() + belongs_to + ' ' + type + ' ' + std::to_string(ID) + ' ' + std::to_string(X) + ' ' + std::to_string(Y) + ' ' + std::to_string(stamina) + ' ' + is_building;

        file << stat_out << std::endl;
    }
    
    int Stamina() const
    {
        return unit[IdentifyType()].stamina;
        return 0;
    }
    
    int Speed() const
    {
        return unit[IdentifyType()].speed;
        return 0;
    }
    
    int IdentifyType() const
    {
        for (int i = 0; i < 8; i++)
            if (unit[i].type == type) 
                return i;
        return 0;
    }
};

void ERROR()
{
    std::cout << "BLEDNY ROZKAZ" << std::endl;   
}

class ORDER : public COORDINATES
{
private:
    int d;
    int unit_type_index;
    int target_type_index;
    int index;
    char unit_to_build;
    
public:
    int ID;
    char action;
    char parameter;
    int attack_parameter;
    
    ORDER()
        : ID(-1), action(' '), parameter(' '), attack_parameter(-1) 
        {
            X = -1;
            Y = -1;
        }
        
    ORDER(const int& id, const char& act)
        : ID(id), action(act), parameter(' '), attack_parameter(-1) 
        {
            X = -1;
            Y = -1;
        }
    ORDER(const int& id, const char& act, const char& param)
        : ID(id), action(act), parameter(param)
        {}
        
    std::string OrdersOut() const
    {
        std::string order_out = std::string() + std::to_string(ID) + ' ' + action + ' ';
        
        switch(action)
        {
            case 'M':
                order_out = order_out + std::to_string(X) + ' ' + std::to_string(Y) + '\n';
                break;
                
            case 'A':
                order_out = order_out + std::to_string(attack_parameter) + '\n';
                break;
                
            case 'B':
                order_out = order_out + parameter + '\n';
        }
        
        
        return order_out;
    }
    
    bool CheckIfOrderValid(const UNIT_STATUS& au, const int* map = nullptr, UNIT_STATUS tu = UNIT_STATUS())
    {
        switch (action)
        {
            case 'M':
                d = MeasureDistance(au.X, au.Y);
                if (d <= au.speed && !map[Y * map[-1] + X]) 
                    return true;
                break;
                
            case 'A':
                d = MeasureDistance(tu.X, tu.Y);
                if (au.speed >= 1 && unit[unit_type_index].range >= d) 
                    return true;
                break;
            
            case 'B':
                auto it = std::find_if(unit.begin(),
                                  unit.end(), 
                                  [&](const UNIT& unit){
                                      return unit.type == parameter; });
                index = std::distance(unit.begin(), it);
                if (au.is_building ==  '0' && it != unit.end())
                    return true;
                
                break;
        }
        ERROR();
        return false;
    }
    
    void ExecuteMove(UNIT_STATUS& u)
    {
        d = MeasureDistance(u.X, u.Y);
        u.X = X;
        u.Y = Y;
        u.speed -= d;
    }
    
    void ExecuteAttack(UNIT_STATUS& au, UNIT_STATUS& tu, std::vector<UNIT_STATUS>& units_vector)
    {   
        unit_type_index = au.IdentifyType();
        target_type_index = tu.IdentifyType();

        tu.stamina -= attack_table[unit_type_index][target_type_index];
        au.speed--;
        if (tu.stamina <= 0 && tu.type == 'B') {}
            //game_over == true;
        if (tu.stamina <= 0)
            units_vector.erase(units_vector.begin() + index);
    }
    
    void ExecuteBuild(UNIT_STATUS& u, int &player_gold) 
    {
        player_gold -= unit[index].cost;
        u.is_building = parameter;
        u.rounds_to_release = unit[index].build_time;
    }
};

class PLAYER
{
public:
    char player;
    bool disqualified;
    
    PLAYER()
        : disqualified(false) {}
};

std::vector<std::vector<char>> GetMap(std::istream& map_file)
{
    std::vector<std::vector<char>> temp_map;
    std::string map_line;
    while (std::getline(map_file, map_line))
    {
        std::vector<char> map_line1(map_line.begin(), map_line.end());
        temp_map.push_back(map_line1);
    }
    return temp_map;
}

int IdentifyIndex(const int& id, const std::vector<UNIT_STATUS>& units_vector)
{
    int unit_index = 0;
    for (UNIT_STATUS u : units_vector) 
    {   
        if (u.ID == id) 
            return unit_index;
        unit_index++;
    }   
    return -1;
}


void ExecuteOrders(std::istream& orders_source, std::vector<UNIT_STATUS>& units_vector, const char& active_player, int& player_gold)
{
    ORDER order_to_execute;
    while (orders_source >> order_to_execute.ID >> order_to_execute.action) 
    {
        int unit_index = IdentifyIndex(order_to_execute.ID, units_vector);
        UNIT_STATUS& active_unit = units_vector[unit_index];
        
        if (active_unit.belongs_to != active_player)
            {
                ERROR();
                continue;
            }
        
        switch (order_to_execute.action)
        {
            case 'M':
                orders_source >> order_to_execute.X >> order_to_execute.Y;
                
                if (order_to_execute.CheckIfOrderValid(active_unit))
                    order_to_execute.ExecuteMove(active_unit);
                break;
            
            case 'A':
            {
                orders_source >> order_to_execute.attack_parameter;

                int target_index = IdentifyIndex(order_to_execute.attack_parameter, units_vector);
                UNIT_STATUS& target_unit = units_vector[target_index];
                            
                if (order_to_execute.CheckIfOrderValid(active_unit, nullptr, target_unit))
                    order_to_execute.ExecuteAttack(active_unit, target_unit, units_vector);
                break;
            }
                
            case 'B':
                orders_source >> order_to_execute.parameter;
                if (order_to_execute.CheckIfOrderValid(active_unit))
                    order_to_execute.ExecuteBuild(active_unit, player_gold);
                break;
                            
            default:
                ERROR();
                break;
        }
    }
}

void AddNewUnit(std::vector<UNIT_STATUS>& units_vector, const char& actp)
{
    int last_id = 0;
    for (UNIT_STATUS u : units_vector)
        if (u.ID > last_id)
            last_id = u.ID;
    
    int i = 0;
    for (UNIT_STATUS u : units_vector)
    {
        if (u.belongs_to == actp && u.type == 'B')
        {
            UNIT_STATUS new_unit(actp, u.is_building, last_id + 1, u.X, u.Y);
            units_vector.push_back(new_unit);
            break;
        }
        i++;
    }
    units_vector[i].is_building = '0';   
}

#endif 
