#include <random>
#include <iostream>
#include <SFML/Graphics.hpp>

// Random float between [0, 1).
static std::mt19937 rng;
#define frand() std::uniform_real_distribution <float> (0.f, 1.f)(rng)

// World Dimensions and Point States.
#define DIM 512
#define EMPTY 0
#define TREE 1
#define FIRE 2

// Controls the state of the world.
class World
{
  int currMap[DIM][DIM] = { EMPTY };
  int nextMap[DIM][DIM] = { EMPTY };
  float p = 0.006f; // Probability that a new tree appears.
  float q = 57.f; // the ratio p/f.
  float f = p / q; // Probability that a tree catches on fire.


  inline int checkBounds(int z)
  { // Helper function.
    if (z < 0) return DIM;
    if (z > DIM) return 0;
    return z;
  }

  inline bool neighborsOnFire(int x, int y)
  { // Helper function.
    for (int tx = x - 1; tx <= x + 1; ++tx){
      for (int ty = y - 1; ty <= y + 1; ++ty){
        tx = checkBounds(tx);
        ty = checkBounds(ty);
        if (currMap[tx][ty] == FIRE) return true;
      }
    }
    return false;
  }

public:
  void UpdateCell(int x, int y)
  { /* Rules:
     * 1) A burning tree turns into an empty spot.
     * 2) A tree will burn if one of its neighbors is burning.
     * 3) A tree will spontaneously combust with probability f.
     * 4) A new tree will grow in an empty spot with probability p.
     */

     float prob = frand();

     // Am I on fire?
     if (currMap[x][y] == FIRE){
       nextMap[x][y] = EMPTY;
     }

     // Are my neighbors on fire and I'm a tree?
     else if (neighborsOnFire(x, y) && currMap[x][y] == TREE){
       nextMap[x][y] = FIRE;
     }

     // Will I be set on fire if I'm a tree?
     else if (f > prob && currMap[x][y] == TREE){
       nextMap[x][y] = FIRE;
     }

     // Can I be a happy little tree? (rip Bob Ross).
     else if (p > prob && currMap[x][y] == EMPTY){
       nextMap[x][y] = TREE;
     }

     else {
       nextMap[x][y] = currMap[x][y];
     }
  }

  void UpdateMap()
  {
    for (int x = 0; x < DIM; ++x){
      for (int y = 0; y < DIM; ++y){
        currMap[x][y] = nextMap[x][y];
      }
    }
  }

  void Render(sf::RenderWindow* window)
  {
    sf::RectangleShape pt(sf::Vector2f(1, 1));
    for (int x = 0; x < DIM; ++x){
      for (int y = 0; y < DIM; ++y){
        if (currMap[x][y] == EMPTY) continue;
        if (currMap[x][y] == TREE) pt.setFillColor(sf::Color::Green);
        if (currMap[x][y] == FIRE) pt.setFillColor(sf::Color::Red);
        pt.setPosition(sf::Vector2f(x, y));
        window->draw(pt);
      }
    }
  }
};

// Main.
int main()
{
  // Initialize window.
  sf::RenderWindow window(sf::VideoMode(DIM, DIM), "Forest Fire");
  window.setVerticalSyncEnabled(true);

  // ZA WARUDO!
  World* world = new World();

  // Loop.
  while (window.isOpen()){
    sf::Event event;
    while (window.pollEvent(event)){
      if (event.type == sf::Event::Closed) window.close();
      if (event.key.code == sf::Keyboard::Escape) window.close();
    }

    for (int x = 0; x < DIM; ++x){
      for (int y = 0; y < DIM; ++y){
        world->UpdateCell(x, y);
      }
    }

    world->UpdateMap();
    window.clear();
    world->Render(&window);
    window.display();
  }

  // Clean up.
  delete world;
  world = nullptr;
  return 0;
}
