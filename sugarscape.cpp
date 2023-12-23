#include "sugarscape.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <fstream>

// AGENT (to do: put in separate cpp)

Agent::Agent(Model* model, int m, int v)
  : model(model), metabolism(m), vision(v),
    grid(nullptr), sugar_amount(0),
    coord(-1, -1)
{ }


void Agent::move()
{
  for(auto p : shuffledNeighbourhood())
  {
    if(grid->empty(p.first, p.second))
    {
      grid->agents[coord.first][coord.second] = nullptr;
      coord = p;
      grid->agents[coord.first][coord.second] = this;
      return;
    }
  }
}


void Agent::harvest()
{
  sugar_amount += grid->sugar_amount[coord.first][coord.second];
  grid->sugar_amount[coord.first][coord.second] = 0;
}


void Agent::eat()
{ sugar_amount -= metabolism; }


void Agent::die()
{
  grid->agents[coord.first][coord.second] = nullptr;
  model->agents.erase(find(model->agents.begin(), model->agents.end(), this));
  model->num_agents--;
  delete this;
}


// GRID (to do: put in separate cpp)

Grid::Grid(int dim)
  : dim(dim),
    sugar_amount(dim, vector<int>(dim, 0)),
    max_sugar_amount(dim, vector<int>(dim, 0)),
    agents(dim, vector<Agent*>(dim, nullptr))
{ setSugar(); }


void Grid::addAgent(Agent* agent)
{
  for(int i = 0; i < 100; i++)
  {
    int x = rand() % dim;
    int y = rand() % dim;
    if(empty(x,y))
    { 
      agents[x][y] = agent;
      agent->coord = {x,y};
      return;
    }
  }
  throw("Too many agents!");
}


bool Grid::empty(int x, int y) const
{ return agents[x][y] == nullptr; }


// helper function for setSugar
double distance(int Ax, int Ay, int Bx, int By)
{
  double s1 = pow((Ax - Bx), 2);
  double s2 = pow((Ay - By), 2);
  return sqrt(s1 + s2);
}


// helper function for Grid(int dim)
void Grid::setSugar()
{
  for(int i = 0; i < dim; i++)
    for(int j = 0; j < dim; j++)
    {
      int d1 = distance(dim/4, dim/4, i, j);
      int d2 = distance((3*dim)/4, (3*dim)/4, i, j);
      int s = max(0, 4 - (min(d1, d2) * 8)/dim); // default function
      sugar_amount[i][j] = s;
      max_sugar_amount[i][j] = s;
    }
}


void Grid::incrementSugarAmount()
{
  for(int i = 0; i < dim; i++)
    for(int j = 0; j < dim; j++)
      if(sugar_amount[i][j] < max_sugar_amount[i][j])
        sugar_amount[i][j]++;
}


vector<pair<int, int>> Agent::shuffledNeighbourhood()
{
  // we are also considering non empty cells
  vector<pair<int,int>> neighbourhood;

  for(int i = -vision; i <= vision; i++)
  {
    if(coord.first + i < 0 || coord.first + i >= grid->dim)
      continue;
    for(int j = -vision; j <= vision; j++)
    {
      if(coord.second + j < 0 ||coord.second + j >= grid->dim)
        continue;
      neighbourhood.push_back({coord.first + i, coord.second + j});
    }
  }
  random_shuffle(neighbourhood.begin(), neighbourhood.end());
  return neighbourhood;
}


// MODEL (to do: put in separate cpp)

Model::Model(string name, int n, int d, int v, int m, int is)
  : model_name(name),
    num_iterations(0),
    num_agents(n), grid_dim(d), 
    max_vision(v), max_metabolism(m), 
    initial_sugar_amount(is), grid(d)
{
  for(int i = 0; i < num_agents; i++)
  {
    Agent* new_agent = new Agent(
      this,
      (rand() % max_metabolism) + 1,
      (rand() % max_vision) + 1
    );
    new_agent->sugar_amount = is;
    new_agent->grid = &grid;

    grid.addAgent(new_agent);
    agents.push_back(move(new_agent));
  }
}


void Model::showAgents()
{
  show([](int i, int j, Grid& grid) 
    { return grid.agents[i][j] == nullptr ? ' ' : '#'; }
  );
}


void Model::showSugar()
{
  show([](int i, int j, Grid& grid) -> char
    { return '0' + grid.sugar_amount[i][j]; }
  );
}


void Model::run(int N, int recording_interval)
{
  // record the initial num_agents
  records.push_back({num_iterations, num_agents});

  for(int i = 1; i <= N; i++)
  {
    // random_shuffle(agents.begin(), agents.end()); // TO DO
    for(int j = 0; j < agents.size(); j++)
    {
      // Important: it is possible for agents to get removed from
      // "agents" while we are in this loop (Agent::die()).
  
      agents[j]->move();
      agents[j]->harvest();
      agents[j]->eat();
      
      if(agents[j]->sugar_amount < 0)
        agents[j--]->die(); // j-- is important
    }
    grid.incrementSugarAmount();
    num_iterations++;

    // Record the number of agents.
    if(i % recording_interval == 0)
      records.push_back({num_iterations, num_agents});
  }
}


Model::~Model()
{
  for(int i = 0; i < agents.size(); i++)
    delete agents[i];
}


void Model::CSV()
{
  ofstream csv_file(model_name);
  csv_file<<"Iteration,Number of agents\n";

  for(auto r : records)
    csv_file<<r.first<<","<<r.second<<"\n";

  csv_file.close();
}


// used in showAgents and showSugar
void Model::show(char(*func)(int, int, Grid& grid))
{
  cout<<model_name<<" after "<<num_iterations<<" iterations. "
    <<"Alive agents: "<<num_agents<<"."<<endl;
  for(int i = 0; i < grid.dim; i++)
  {
    for(int j = 0; j < grid.dim; j++)
      cout<<"- - ";
    cout<<"-"<<endl;
    for(int j = 0; j < grid.dim; j++)
      cout<<"| "<<func(i, j, grid)<<" ";
    cout<<"|"<<endl;
  }  
  for(int j = 0; j < grid.dim; j++)
    cout<<"- - ";
  cout<<"-"<<endl<<endl;
}
