#ifndef __PROGTEST__
#include <pthread.h>
#include <semaphore.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cfloat>
#include <chrono>
#include <climits>
#include <cmath>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <deque>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "progtest_solver.h"
#include "sample_tester.h"
using namespace std;
#endif /* __PROGTEST__ */

/* ---------------- WORKFLOW ----------------
- Number of threads spawned is specified by the Start method
- When Ship gets called, Sales threads need to:
    - Get destination
    - Ask all registered customers for a list of items
      they want to send to the destination (CCustomer::Quote)
        - CCustomer::Quote is blocking so we need to put all
          Sales threads to work
    - Multiple threads can be asking one customer at once
        - e.g. Customer A can take a while to respond with a quote to a destination A
          but can respond instantly when another thread asks for a quote to a destination B
- Once we have quotes from all customers, we need to pick which cargo will and which won't
  go on the Ship. Worker threads will be doing that, not Sales threads.
- Once that's decided and calculated, CShip::Load gets called.
*/

/* ---------------- TIPS ----------------
 - Make sure we're serving multiple ships & customers at once
*/

class CCargoPlanner {
   private:
   public:
    static int SeqSolver(const vector<CCargo>& cargo,
                         int maxWeight,
                         int maxVolume,
                         vector<CCargo>& load);
    void Start(int sales,
               int workers);
    void Stop(void);

    void Customer(ACustomer customer);
    void Ship(AShip ship);
};

// Tests our algorithm for loading ships
int CCargoPlanner::SeqSolver(const vector<CCargo>& cargo,
                             int maxWeight,
                             int maxVolume,
                             vector<CCargo>& load) {
    // base solver is implemented, we can add a custom one later
    return ProgtestSolver(cargo, maxWeight, maxVolume, load);
}

// Creates a number of sales and worker threads specified
void CCargoPlanner::Start(int sales,
                          int workers) {
    // initialize salesThreads and workThreads, those will be waiting for ships
}

void CCargoPlanner::Stop(void) {
    // when this gets called, there won't be any more ships
    //  = fill all ships we currently have
    //  and join threads back into the main thread
}

// registers a customer that gets passed in
void CCargoPlanner::Customer(ACustomer customer) {
}

// Adds a ship
void CCargoPlanner::Ship(AShip ship) {
    // return from this method should be instant (no computation)

    // add a ship

    // handle transfer of the ship to worker & sales threads
}

//-------------------------------------------------------------------------------------------------
#ifndef __PROGTEST__
int main(void) {
    CCargoPlanner test;
    vector<AShipTest> ships;
    vector<ACustomerTest> customers{make_shared<CCustomerTest>(), make_shared<CCustomerTest>()};

    ships.push_back(g_TestExtra[0].PrepareTest("New York", customers));
    ships.push_back(g_TestExtra[1].PrepareTest("Barcelona", customers));
    ships.push_back(g_TestExtra[2].PrepareTest("Kobe", customers));
    ships.push_back(g_TestExtra[8].PrepareTest("Perth", customers));
    // add more ships here

    for (auto x : customers)
        test.Customer(x);

    test.Start(3, 2);

    for (auto x : ships)
        test.Ship(x);

    test.Stop();

    for (auto x : ships)
        cout << x->Destination() << ": " << (x->Validate() ? "ok" : "fail") << endl;
    return 0;
}
#endif /* __PROGTEST__ */
