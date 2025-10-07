// cristian.cpp
// Run: ./cristian

#include <bits/stdc++.h>
#include <thread>
#include <mutex>
using namespace std;

mutex io;

int randDelay(int minMs=30, int maxMs=200) {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(minMs, maxMs);
    return dis(gen);
}

struct TimeServer {
    // server has an absolute time (ms)
    long long serverClockMs;
    TimeServer(long long base): serverClockMs(base) {}
    // when request arrives after some delay, serverTime = serverClockMs + delay
    long long respondTime(long long delayMs) {
        return serverClockMs + delayMs;
    }
};

void client_behavior(int id, TimeServer &server, long long clientClockMs) {
    {
        lock_guard<mutex> lk(io);
        cout << "Client " << id << " initial clock: " << clientClockMs << "ms\n";
    }
    // t1 = client local send time
    auto t1 = chrono::steady_clock::now();
    long long t1_ms = clientClockMs; // for simulation, use ms values

    // simulate network send delay to server
    int d1 = randDelay();
    this_thread::sleep_for(chrono::milliseconds(d1));

    // server prepares response using its serverClock + time passed (d1)
    long long serverSendTs = server.respondTime(d1);

    // simulate server processing negligible then reply delay d2
    int d2 = randDelay();
    this_thread::sleep_for(chrono::milliseconds(d2));

    // client receives at t2 (simulate client's clock advanced by d1+d2)
    auto t2 = chrono::steady_clock::now();
    long long t2_ms = clientClockMs + d1 + d2;

    long long RTT = (t2_ms - t1_ms);
    long long estimatedServerTime = serverSendTs + (RTT / 2);

    {
        lock_guard<mutex> lk(io);
        cout << "\nClient " << id << " stats:\n";
        cout << "  t1 (client send) = " << t1_ms << "ms\n";
        cout << "  server sent ts   = " << serverSendTs << "ms\n";
        cout << "  t2 (client recv) = " << t2_ms << "ms\n";
        cout << "  RTT              = " << RTT << "ms\n";
        cout << "  estimated server time = " << estimatedServerTime << "ms\n";
        cout << "  adjusting client clock from " << clientClockMs << " -> " << estimatedServerTime << "\n";
    }

    // client sets its clock to estimatedServerTime
    clientClockMs = estimatedServerTime;
    {
        lock_guard<mutex> lk(io);
        cout << "Client " << id << " final clock: " << clientClockMs << "ms\n\n";
    }
}

int main() {
    cout << "Cristian algorithm simulation\n";
    long long serverBase = 200000; // ms
    TimeServer server(serverBase);

    // Simulate 3 clients contacting server with different initial clocks
    vector<long long> clients = {199500, 200300, 199900};

    vector<thread> threads;
    for (int i=0;i<(int)clients.size();i++){
        threads.emplace_back(client_behavior, i+1, ref(server), clients[i]);
        this_thread::sleep_for(chrono::milliseconds(50)); // stagger starts
    }
    for (auto &t: threads) t.join();

    return 0;
}
