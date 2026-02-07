// collision.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <vector>

struct location_data {
    int x, y, width, height;
    location_data() = default;
    location_data(const int& x, const int& y, const int& width, const int& height) : x(x), y(y), width(width), height(height) {}
};

namespace collisionl {
    class logic {
    public:
        static bool collision(const location_data& a, const location_data& b) {
            bool collisionX = a.x + a.width >= b.x && b.x + b.width >= a.x;
            bool collisionY = a.y + a.height >= b.y && b.y + b.height >= a.y;
            return collisionX && collisionY;
        }
    };
}



class Player {
public:
    location_data location;
};

Player player;

class enemy {
public: 
    location_data location;
    bool on_path; //intended for a walk route the enemy should take when not los with player
    bool line_of_sight;
};

class bullet {
public:
    location_data location;
    int x_velocity, y_velocity;
};

class wall {
public:
    location_data location;
};

class cell;

class Map {
public:
    std::vector<std::vector<cell>> cells;
};

class cell {
public:
    //reminder to figure out how to reserve size to these vectors to avoid constant realocation.
    std::vector<std::unique_ptr<bullet>> enemy_bullets;
    std::vector<std::unique_ptr<bullet>> player_bullets;
    std::vector<enemy> enemies;
    std::vector<wall> walls;
    Map* map;
    location_data location;
    location_data temp;

    cell(const int& xpos, const int& ypos) : location( xpos, ypos, 40, 40 ) {}

    void bullet_check() {
        std::erase_if(enemy_bullets, [&](auto& b) {
            b->location.x += b->x_velocity;
            b->location.y += b->y_velocity;

            if (!collisionl::logic::collision(b->location, location)) {
                int grid_x = b->location.x / 40;
                int grid_y = b->location.y / 40;
                auto& neighbor = map->cells[grid_y][grid_x];
                neighbor.enemy_bullets.push_back(std::move(b));
                return true;
            }
            
            for (wall& w : walls) {
                if (collisionl::logic::collision(b->location, w.location)) {
                    return true;
                }
            }
            if (collisionl::logic::collision(b->location, player.location)) {
                //insert health logic
                return true;
            }
            return false;
        });
        
        std::erase_if(player_bullets, [&](auto& b) {
            b->location.x += b->x_velocity;
            b->location.y += b->y_velocity;

            if (!collisionl::logic::collision(b->location, location)) {
                int grid_x = b->location.x / 40;
                int grid_y = b->location.y / 40;
                auto& neighbor = map->cells[grid_y][grid_x];
                neighbor.enemy_bullets.push_back(std::move(b));
                return true;
            }

            for (wall& w : walls) {
                if (collisionl::logic::collision(b->location, w.location)) {
                    return true;
                }
            }
            for (enemy& e : enemies) {
                if (collisionl::logic::collision(b->location, e.location)) {
                    //insert health logic
                    return true;
                }
            }
            return false;
            });
    }
};


int main()
{
    std::cout << "Hello World!\n";
    return 0;
}

