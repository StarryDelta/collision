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
    int x_velocity, y_velocity;
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


//I need to make a bool flag to check wether or not an object has moved in the frame already so it doesn't move more than once per frame. It might also be necessary to make a vector to check collisions with that object on that frame... -\_("_")_/-  Honestly at 60fps, one frame might not even matter.
//I actually put all the collision and position logic in here lmao.
class cell {
public:
    //reminder to figure out how to reserve size to these vectors to avoid constant realocation.
    std::vector<std::unique_ptr<bullet>> enemy_bullets;
    std::vector<std::unique_ptr<bullet>> player_bullets;
    std::vector<std::shared_ptr<enemy>> enemies;
    std::vector<wall> walls;
    Map* map;
    location_data location;
    int cellpos_x, cellpos_y;

    cell(const int& xpos, const int& ypos) : cellpos_x(xpos / 40), cellpos_y(ypos / 40), location(xpos, ypos, 40, 40) {}

    void bullet_check() {
        //checks collision and cell.
        std::erase_if(enemy_bullets, [&](auto& b) {
            //bullet movement
            b->location.x += b->x_velocity;
            b->location.y += b->y_velocity;

            //checks if the bullet is within the cell's coords. Moves the bullet to the correct cell if not
            if (!collisionl::logic::collision(b->location, location)) {
                
                //checks which cell it belongs in.
                int grid_x = b->location.x / 40;
                int grid_y = b->location.y / 40;
                auto& neighbor = map->cells[grid_y][grid_x];
                
                //moves it to the proper cell.
                neighbor.enemy_bullets.push_back(std::move(b));
                
                //ensures deletion from this cell. Idk if it'll spout an error bc I moved a uniq_ptr... oh well.
                return true;
            }
            
            //checks wall collision, deletes if it hits.
            for (wall& w : walls) {
                if (collisionl::logic::collision(b->location, w.location)) {
                    return true;
                }
            }
            
            //player logic. not much to see here.
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
            for (auto& e : enemies) {
                if (collisionl::logic::collision(b->location, e->location)) {
                    //insert health logic
                    return true;
                }
            }
            return false;
            });

        //checks collision for enemies, as well as cell movement.
        std::erase_if(enemies, [&](auto& e) {
            //move enemy.
            e->location.x += e->x_velocity;
            e->location.y += e->y_velocity;

            //If the enemy doesn't have collision with the cell, it'll be moved to the correct cell.
            if (!collisionl::logic::collision(e->location, location)) {
                int grid_x = e->location.x / 40;
                int grid_y = e->location.y / 40;
                auto& neighbor = map->cells[grid_y][grid_x]; //could probably be changed to a .emplace but idgaf
                neighbor.enemies.push_back(std::move(e));
                return true;
            }

            //add an if statment to check the velocity direction of the enemy. If it checked all 8 surrounding for every one it's in, would be annoying.
            if (collisionl::logic::collision(e->location, map->cells[cellpos_x - 1][cellpos_y - 1].location) && this->location.x > 0 && this->location.y > 0) {
                map->cells[cellpos_x - 1][cellpos_y - 1].enemies.emplace_back(e);
            }

            //I need to change this so it moves the enemy 1 square back... it's a little tricky bc of the intended pathing mechanic... guess I come back if I expand beyond collision
            for (wall& w : walls) {
                if (collisionl::logic::collision(e->location, w.location)) {
                    return true;
                }
            }
            return false;
            });
    }

    void enemy_check() {

    }
};


int main()
{
    std::cout << "Hello World!\n";
    return 0;
}

