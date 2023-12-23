#include <iostream>
#include "sugarscape.h"

using namespace std;

int main()
{
  //              n  d   v  m  is
  Model m("prvi", 40, 50, 7, 5, 10);
  m.showAgents();
  m.showSugar();
  cout<<endl;
  m.run(1000,10);
  m.showAgents();
  m.showSugar();
  m.CSV();
}