#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <string>
#include <windows.h> 

using namespace std;
const int N = 5; // liczba filozofów
const int EAT_TIME = 5; // czas jedzenia w sekundach
const int THINK_TIME = 5; // czas myślenia w sekundach

mutex mtx; // mutex do synchronizacji dostępu do konsoli
condition_variable cv; // zmienna warunkowa do sygnalizowania dostępności widelców
vector<bool> forks(N, true); // wektor przechowujący stan widelców (true - wolny, false - zajęty)
vector<int> eating(N, 0); // wektor przechowujący liczbę posiłków zjedzonych przez każdego filozofa
vector<bool> hungry(N, false); // wektor przechowujący stan filozofa ( true - glodny, false - nieglodny )

// Funkcja do czyszczenia konsoli
void clear_console() {
	system("cls");
}

// funkcja pomocnicza do wypisywania stanu filozofów na konsolę
void print_status(int id) {
	unique_lock<mutex> lock(mtx); // blokujemy mutex do konsoli
	clear_console(); // czyszczenie konsoli

	cout << "====== Filozofowie =======================" << endl; // wypisujemy filozofów
	for (int i = 0; i < N; i++) {
		if (hungry[i] == true && eating[i] == 0) {
			cout << "| Filozof " << i + 1 << ": glodny | " << endl;
		}
		else if (eating[i] == 0) {
			cout << "| Filozof " << i + 1 << ": mysli  | " << endl;
		}
		else if (eating[i] > 0) {
			cout << "| Filozof " << i + 1 << ": je     | " << endl;
		}
	}

	cout << endl;

	cout << "\n====== Widelce =======================" << endl;
	for (int i = 0; i < N; i++) {
		if (forks[i] == true) {
			cout << "| Widelec " << i + 1 << ": wolny  |" << endl;
		}
		if (forks[i] == false) {
			cout << "| Widelec " << i + 1 << ": zajety |" << endl;
		}
	}

	cout << "\n\n";
	lock.unlock(); // odblokowujemy mutex do konsoli
}

// funkcja symulująca zachowanie filozofa
void philosopher(int id) {
	int left = id; // indeks lewego widelca
	int right = (id + 1) % N; // indeks prawego widelca
	while (true) {

		// filozof myśli
		print_status(id);
		this_thread::sleep_for(chrono::seconds(THINK_TIME));

		// filozof próbuje zjeść
		hungry[id] = true;
		print_status(id);
		unique_lock<mutex> lock(mtx); // blokujemy mutex do widelców ( zapobiegamy zakleszczeniu )
		cv.wait(lock, [left, right] {return forks[left] && forks[right]; }); // czekamy na wolne widelce
		forks[left] = forks[right] = false; // bierzemy widelce ( false - zajęte )
		eating[id]++; // zwiększamy liczbę posiłków filozofa ( dodanie filozofa do jedzacych )
		lock.unlock(); // odblokowujemy mutex do widelców

		// filozof je
		hungry[id] = false;
		print_status(id);
		this_thread::sleep_for(chrono::seconds(EAT_TIME));

		// filozof kończy jeść
		lock.lock(); // blokujemy mutex do widelców
		forks[left] = forks[right] = true; // odkładamy widelce
		eating[id]--; // zmniejszamy liczbę posiłków filozofa ( dodanie filozofa do myslacych )
		cv.notify_all(); // informujemy innych filozofów o zwolnieniu widelców ( informuje potencjalnie zablokowanych filozofow ze widelce sa wolne )
		lock.unlock(); // odblokowujemy mutex do widelców

	}
}

int main() {
	vector<thread> philosophers; // wektor wątków dla filozofów
	for (int i = 0; i < N; i++) {
		philosophers.push_back(thread(philosopher, i)); // tworzymy i uruchamiamy wątek dla każdego filozofa
	}
	for (auto& p : philosophers) {
		p.join(); // czekamy na zakończenie wątków (nigdy się nie skończą)
	}
}