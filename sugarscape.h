#ifndef SUGARSCAPE_H
#define SUGARSCAPE_H

#include <vector>
#include <string>

using namespace std;

class Model;
class Grid;
class Agent;


class Agent
{
  Model* model;
  Grid* grid;
  const int metabolism;
  const int vision;
  int sugar_amount;
  pair<int,int> coord;

  Agent(Model*, int m, int v);

  void move();
  void harvest();
  void eat();
  void die();

  vector<pair<int, int>> shuffledNeighbourhood();

  friend class Model;
  friend class Grid;
};


class Grid
{
  const int dim; // dimensions are dim * dim
  vector<vector<int>> sugar_amount;
  vector<vector<int>> max_sugar_amount;
  vector<vector<Agent*>> agents; // nullptr represents an empty cell

  Grid(int dim);

  void addAgent(Agent* agent); // adds the agent on a random empty cell
  bool empty(int x, int y) const; // true if the cell (x,y) has 0 agents
  void setSugar(); // default sugar distribution
  void incrementSugarAmount();

  friend class Model;
  friend class Agent;
};


class Model
{
  public:
  Model(string name, int n, int d, int v, int m, int is);
  ~Model();

  void showAgents();
  void showSugar();
  void run(int N, int recording_interval); // add default =1
  void CSV();

  private: 
  const string model_name; 
  // used to create a unique csv file that reports on number of agents
  
  int num_iterations;

  int num_agents;
  const int grid_dim;
  const int max_vision;
  const int max_metabolism;
  const int initial_sugar_amount; // for agents

  vector<pair<int,int>> records; // num_agents through time

  Grid grid;
  vector<Agent*> agents; // we are rarely removing the agents

  // used in showAgents and showSugar
  void show(char (*func)(int, int, Grid& grid));

  friend Agent;
  friend Grid;
};

#endif // SUGARSCAPE_H