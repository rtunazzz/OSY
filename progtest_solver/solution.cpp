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

/*
**************************** WORKFLOW ****************************
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

**************************** TIPS ****************************
 - Make sure we're serving multiple ships & customers at once

**************************** TODO ****************************

*/

class JobInfo {
   public:
    bool forWorker = false;
    bool forSales = false;
    bool done = false;
    function<void()> job;

    JobInfo(bool _forWorker, bool _forSales, bool _done, function<void()> _job) : forWorker(_forWorker), forSales(_forSales), done(_done), job(_job){};
};

class CCargoPlanner {
   private:
    vector<thread> m_Workers;
    vector<thread> m_Sales;

    vector<ACustomer> m_Customers;

    map<string, pair<int, vector<CCargo>>> m_DestCargoData;
    mutex m_DMtx;

    queue<JobInfo> m_JobQueue;
    mutex m_QMtx;
    condition_variable m_QCond;

    void worker(int index);
    void salesman(int index);

    void handleDestination(AShip& ship);
    void handleQuote(const ACustomer& customer, AShip& ship, int shipCustomerCnt);
    void handleLoad(AShip& ship);

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

void CCargoPlanner::worker(int index) {
    // cout << "[" << index << "]"
    //      << " Worked started." << endl;

    while (true) {
        unique_lock<mutex> qlock(m_QMtx);
        m_QCond.wait(qlock, [this] { return (!m_JobQueue.empty()) && (m_JobQueue.front().forWorker); });

        JobInfo jobData = m_JobQueue.front();
        // cout << "[" << index << "] W - " << m_JobQueue.size() << " - Found a job, done: " << (jobData.done ? "YES" : "Not yet...") << endl;
        if (jobData.done) {
            break;
        }
        m_JobQueue.pop();
        // cout << "[" << index << "] W - " << m_JobQueue.size() << " - Job assigned" << endl;

        if (m_JobQueue.empty()) {
            cout << "[" << index << "] S - Queue is empty! Notifying all." << endl;
            // unlock the queue since we don't need it locked anymore
            qlock.unlock();
            m_QCond.notify_all();
        } else {
            // unlock the queue since we don't need it locked anymore
            qlock.unlock();
        }

        // do the job
        jobData.job();
        // cout << "[" << index << "] S - Job done." << endl;
    }

    // cout << "[" << index << "]"
    //      << " Worker ended." << endl;
}

void CCargoPlanner::salesman(int index) {
    // cout << "[" << index << "]"
    //      << " Salesman started." << endl;

    while (true) {
        unique_lock<mutex> qlock(m_QMtx);
        m_QCond.wait(qlock, [this] { return (!m_JobQueue.empty()) && (m_JobQueue.front().forSales); });

        JobInfo jobData = m_JobQueue.front();
        // cout << "[" << index << "] S - " << m_JobQueue.size() << " - Found a job, done: " << (jobData.done ? "YES" : "Not yet...") << endl;
        if (jobData.done) {
            break;
        }
        m_JobQueue.pop();
        // cout << "[" << index << "] S - " << m_JobQueue.size() << " - Job assigned" << endl;

        if (m_JobQueue.empty()) {
            cout << "[" << index << "] S - Queue is empty! Notifying all." << endl;
            // unlock the queue since we don't need it locked anymore
            qlock.unlock();
            m_QCond.notify_all();
        } else {
            // unlock the queue since we don't need it locked anymore
            qlock.unlock();
        }

        // do the job
        jobData.job();
        // cout << "[" << index << "] S - Job done." << endl;
    }

    // cout << "[" << index << "]"
    //      << " Salesman ended." << endl;
}

void CCargoPlanner::handleDestination(AShip& ship) {
    // Salesman method

    int customerCnt = m_Customers.size();

    // cout << "Adding jobs to quote " << customerCnt << " customers for destination " << ship->Destination() << endl;
    // Create a job to handle quoting for each customer
    for (auto const& it : m_Customers) {
        unique_lock<mutex> qlock(m_QMtx);
        m_JobQueue.emplace(
            /* forWorker */ false,
            /* forSales */ true,
            /* done */ false,
            /* job */ bind(&CCargoPlanner::handleQuote, this, it, ship, customerCnt));
        cout << "Job handleQuote to " << ship->Destination() << " added" << endl;
    }

    m_QCond.notify_all();
}

void CCargoPlanner::handleQuote(const ACustomer& customer, AShip& ship, int shipCustomerCnt) {
    // Salesman method

    vector<CCargo> quotedCargo;
    cout << ship->Destination() << " Quoting a customer for destination" << endl;
    customer->Quote(ship->Destination(), quotedCargo);
    cout << ship->Destination() << " Customer quoted" << endl;

    // merge vectors together
    unique_lock<mutex> dlock(m_DMtx);
    auto& cargoData = m_DestCargoData[ship->Destination()];
    auto& dataVector = cargoData.second;

    dataVector.insert(dataVector.end(), quotedCargo.begin(), quotedCargo.end());
    cargoData.first += 1;
    cout << ship->Destination() << " - count is currently " << cargoData.first << " (" << shipCustomerCnt << ")" << endl;

    if (cargoData.first == shipCustomerCnt) {
        unique_lock<mutex> qlock(m_QMtx);
        m_JobQueue.emplace(
            /* forWorker */ true,
            /* forSales */ false,
            /* done */ false,
            /* job */ bind(&CCargoPlanner::handleLoad, this, ship));

        cout << m_JobQueue.size() << " Job handleLoad to " << ship->Destination() << " added" << endl;

        qlock.unlock();
        m_QCond.notify_one();
    }
}

void CCargoPlanner::handleLoad(AShip& ship) {
    // Worker method

    // cout << "Handling load for destination: " << ship->Destination() << endl;
    vector<CCargo> finalLoad;

    unique_lock<mutex> dlock(m_DMtx);
    const auto& cargo = m_DestCargoData[ship->Destination()].second;
    dlock.unlock();

    int fees = SeqSolver(cargo, ship->MaxWeight(), ship->MaxVolume(), finalLoad);
    cout << "Loading ship to '" << ship->Destination() << "' with fees: " << fees << endl;

    ship->Load(finalLoad);

    // why does this fix it
    // m_QCond.notify_one();
}

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
    cout << "*********************************" << endl;
    cout << "Starting " << sales << " sales and " << workers << " workers" << endl;
    cout << "*********************************" << endl;
    // initialize salesThreads and workThreads, those will be waiting for ships
    for (int i = 0; i < sales; i++) {
        m_Sales.emplace_back(&CCargoPlanner::salesman, this, i);
    }

    for (int i = 0; i < workers; i++) {
        m_Workers.emplace_back(&CCargoPlanner::worker, this, i);
    }
}

void CCargoPlanner::Stop(void) {
    // when this gets called, there won't be any more ships
    //  = fill all ships we currently have
    //  and join threads back into the main thread
    // wait till the queue is empty
    cout << "[STOP] Waiting for queue to be empty..." << endl;

    unique_lock<mutex> qlock(m_QMtx);
    m_QCond.wait(qlock, [this] { return (m_JobQueue.empty()); });

    cout << "[STOP] " << m_JobQueue.size() << " Queue is empty!" << endl;

    // add a new end "job" for all workers
    m_JobQueue.emplace(
        /* forWorker */ true,
        /* forSales */ true,
        /* done */ true,
        /* job */ nullptr);

    qlock.unlock();

    // wake all threads
    m_QCond.notify_all();

    for (thread& t : m_Sales) {
        t.join();
    }

    for (thread& t : m_Workers) {
        t.join();
    }
}

// registers a customer that gets passed in
void CCargoPlanner::Customer(ACustomer customer) {
    m_Customers.push_back(customer);
}

// Adds a ship
void CCargoPlanner::Ship(AShip ship) {
    // return from this method should be instant (no computation)

    // handle transfer of the ship to worker & sales threads
    // Get destination
    // Afterwards ask all registered customers for a list of items
    //      (will be done in the handleDestination method)
    unique_lock<mutex> qlock(m_QMtx);
    cout << m_JobQueue.size() << " Adding a ship to: " << ship->Destination() << endl;
    m_JobQueue.emplace(
        /* forWorker */ false,
        /* forSales */ true,
        /* done */ false,
        /* job */ bind(&CCargoPlanner::handleDestination, this, ship));

    qlock.unlock();
    m_QCond.notify_one();
    // cout << "Job for destination " << ship->Destination() << " added & notified one." << endl;
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
