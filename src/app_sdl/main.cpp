#include "core/rules_conway.hpp"
#include "core/engine.hpp"
#include "renderer.hpp"
#include "convex_hull/convex_hull.hpp"

int main(int, char**)
{
  bool test = true;
  if (test) {
    ConvexHull ch;
    Engine e(50, 30, ch);
    e.setDistanceCalculator(ConvexHull::calculateDistances);
    e.setCalculatingDistances(true);
    Renderer r(e, 20);
    
    while (r.render()) {
      // loop until window closed
    }
  }
  return 0;
}

