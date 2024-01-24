#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <windows.h> 

using namespace std;
const int N = 5; // number of philosophers
const int EAT_TIME = 5; // eating time in seconds
const int THINK_TIME = 5; // thinking time in seconds

mutex mtx; // mutex for synchronizing access to the console
condition_variable cv; // conditional variable to signal fork availability
vector<bool> forks(N, true); // vector to store the state of forks (true - available, false - in use)
vector<int> eating(N, 0); // vector to store the number of meals eaten by each philosopher
vector<bool> hungry(N, false); // vector to store the state of each philosopher (true - hungry, false - not hungry)

// Function to clear the console
void clear_console() {
    system("cls");
}

// Helper function to print the status of philosophers to the console
void print_status(int id) {
    unique_lock<mutex> lock(mtx); // lock the console mutex
    clear_console(); // clear the console

    cout << "====== Philosophers =======================" << endl;
    for (int i = 0; i < N; i++) {
        if (hungry[i] == true && eating[i] == 0) {
            cout << "| Philosopher " << i + 1 << ": hungry | " << endl;
        }
        else if (eating[i] == 0) {
            cout << "| Philosopher " << i + 1 << ": thinking  | " << endl;
        }
        else if (eating[i] > 0) {
            cout << "| Philosopher " << i + 1 << ": eating     | " << endl;
        }
    }

    cout << endl;

    cout << "\n====== Forks =======================" << endl;
    for (int i = 0; i < N; i++) {
        if (forks[i] == true) {
            cout << "| Fork " << i + 1 << ": available  |" << endl;
        }
        if (forks[i] == false) {
            cout << "| Fork " << i + 1 << ": in use |" << endl;
        }
    }

    cout << "\n\n";
    lock.unlock(); // unlock the console mutex
}

// Function simulating the behavior of a philosopher
void philosopher(int id) {
    int left = id; // index of the left fork
    int right = (id + 1) % N; // index of the right fork
    while (true) {

        // Philosopher thinks
        print_status(id);
        this_thread::sleep_for(chrono::seconds(THINK_TIME));

        // Philosopher tries to eat
        hungry[id] = true;
        print_status(id);
        unique_lock<mutex> lock(mtx); // lock the fork mutex (prevent deadlock)
        cv.wait(lock, [left, right] {return forks[left] && forks[right]; }); // wait for available forks
        forks[left] = forks[right] = false; // take the forks (set to false - in use)
        eating[id]++; // increase the number of meals eaten by the philosopher (add philosopher to eating)
        lock.unlock(); // unlock the fork mutex

        // Philosopher eats
        hungry[id] = false;
        print_status(id);
        this_thread::sleep_for(chrono::seconds(EAT_TIME));

        // Philosopher finishes eating
        lock.lock(); // lock the fork mutex
        forks[left] = forks[right] = true; // put down the forks
        eating[id]--; // decrease the number of meals eaten by the philosopher (add philosopher to thinking)
        cv.notify_all(); // inform other philosophers about the availability of forks (notify potentially blocked philosophers that the forks are free)
        lock.unlock(); // unlock the fork mutex

    }
}

int main() {
    vector<thread> philosophers; // vector of threads for philosophers
    for (int i = 0; i < N; i++) {
        philosophers.push_back(thread(philosopher, i)); // create and start a thread for each philosopher
    }
    for (auto& p : philosophers) {
        p.join(); // wait for the threads to finish (they will never finish)
    }
}
