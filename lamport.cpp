// lamport.cpp
// Compile: g++ -std=c++17 lamport.cpp -pthread -O2 -o lamport
// Run: ./lamport

#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;

struct Message {
    int from;
    int to;
    int timestamp; // Lamport clock of sender
    string text;
};

int const N = 4;
vector<int> clockL(N, 0);
mutex mtx;
condition_variable cv;
deque<Message> mailbox;

void send_message(int from, int to, const string &text, int sendDelayMs) {
    // simulate network delay
    this_thread::sleep_for(chrono::milliseconds(sendDelayMs));
    Message m{from, to, 0, text};
    {
        lock_guard<mutex> lk(mtx);
        m.timestamp = clockL[from];
        mailbox.push_back(m);
    }
    cv.notify_all();
}

void process_run(int id) {
    // Each process performs some local events and sends messages
    // We'll do a small scripted sequence for clarity
    // increment for local event
    {
        lock_guard<mutex> lk(mtx);
        clockL[id] += 1; // local event
        cout << "[P" << id << "] local event -> clock=" << clockL[id] << "\n";
    }
    this_thread::sleep_for(chrono::milliseconds(100 + id*50));

    // send to next process (cyclic)
    int target = (id + 1) % N;
    {
        lock_guard<mutex> lk(mtx);
        clockL[id] += 1; // send event increments before attach
        cout << "[P" << id << "] sending to P" << target << " with ts=" << clockL[id] << "\n";
    }
    send_message(id, target, "hi", 50 + id*20);

    // wait and process incoming messages
    bool done = false;
    while (!done) {
        unique_lock<mutex> lk(mtx);
        cv.wait_for(lk, chrono::milliseconds(200));
        // process all messages for me
        for (auto it = mailbox.begin(); it != mailbox.end();) {
            if (it->to == id) {
                int rec_ts = it->timestamp;
                // receive event: clock = max(local, rec_ts) + 1
                clockL[id] = max(clockL[id], rec_ts) + 1;
                cout << "[P" << id << "] received from P" << it->from << " (ts=" << rec_ts << ") -> clock=" << clockL[id] << "\n";
                it = mailbox.erase(it);
            } else ++it;
        }
        // finish condition: after some time local events done
        static int steps = 0;
        steps++;
        if (steps > 6) done = true;
        lk.unlock();
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    // final local event
    {
        lock_guard<mutex> lk(mtx);
        clockL[id] += 1;
        cout << "[P" << id << "] final local event -> clock=" << clockL[id] << "\n";
    }
}

int main(){
    cout << "Lamport clock simulation with " << N << " processes.\n";
    vector<thread> workers;
    for (int i=0;i<N;i++) workers.emplace_back(process_run, i);
    for (auto &t: workers) t.join();

    cout << "\nFinal clocks:\n";
    for (int i=0;i<N;i++) cout << "P" << i << " : " << clockL[i] << "\n";
    return 0;
}
