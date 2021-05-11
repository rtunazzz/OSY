#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#define NUMBER_OF_THREADS 2
using namespace std;
//-------------------------------------------
class CounterClass {
   private:
    unsigned long long c;

   public:
    CounterClass() {
        c = 0;
    }

    void adding(int tid, int max) {
        for (int i = 1; i <= max; i++) {
            c = c + 1;
        }
    }

    unsigned long long getSum() {
        return c;
    }
};

//-------------------------------------------
int main(int argc, char* argv[]) {
    CounterClass counterObj;
    vector<thread> threads;
    int values[NUMBER_OF_THREADS];

    // Check arguments
    if (argc != 3 || sscanf(argv[1], "%d", &values) != 1 || sscanf(argv[2], "%d", &values) != 1)) {
        printf("Usage: %s integer_1 interger_2\n", argv[0]);
        return 1;
    }

    printf("Main:     Start\n");

    // Create threads
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        printf("Main:     Creating thread %d\n", i);
        threads.push_back(thread(&CounterClass::adding, &counterObj, i, values[i]));  // <--- #2
    }

    // Wait for threads
    for (int i = 0; i < NUMBER_OF_THREADS; i++)
        threads[i].join();

    printf("Main:     Stop\n");
    return 0;
}
