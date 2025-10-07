// berkeley.cpp
// Compile: g++ -std=c++17 berkeley.cpp -pthread -O2 -o berkeley
// Run: ./berkeley

#include <bits/stdc++.h>
#include <thread>
#include <mutex>
using namespace std;

int const N = 4; // including master index 0 as master
vector<long long> clocksMs(N);
mutex io_mtx;
random_device rd;
mt19937 rng(rd());

int randDelay(int minMs=20, int maxMs=200){
    uniform_int_distribution<int> d(minMs, maxMs);
    return d(rng);
}

void printClocks(const string &header) {
    lock_guard<mutex> lk(io_mtx);
    cout << header << "\n";
    for (int i=0;i<N;i++) {
        cout << " P" << i << ": " << clocksMs[i] << "ms\n";
    }
}

void master_round() {
    // Master (P0) polls each node for its clock (simulated with delays)
    vector<long long> reported(N);
    reported[0] = clocksMs[0]; // master own time

    for (int i=1;i<N;i++) {
        int d1 = randDelay();
        this_thread::sleep_for(chrono::milliseconds(d1)); // request delay
        // node receives request and replies (simulate node processing negligible)
        int d2 = randDelay();
        // round-trip: d1 + d2
        long long nodeTime = clocksMs[i] + d1; // simulate node clock advanced by d1
        // we get nodeTime after d2 more ms
        reported[i] = nodeTime + d2; // approximate time at master reception
    }

    // compute offsets relative to master
    vector<long long> offsets(N);
    long long sum = 0;
    for (int i=0;i<N;i++){
        offsets[i] = reported[i] - reported[0]; // desired difference (node - master)
        sum += (reported[i] - reported[0]); // sum of differences
    }
    // compute average offset to apply (Berkeley uses average but often excludes outliers)
    long long avgOffset = sum / N;

    // Master computes adjustments: each node should change by (avgOffset - offsets[i])
    {
        lock_guard<mutex> lk(io_mtx);
        cout << "\nMaster reported times (ms):\n";
        for (int i=0;i<N;i++) cout << " P" << i << ": " << reported[i] << "\n";
        cout << "Average offset (relative to master): " << avgOffset << "ms\n";
    }

    // send adjustments
    for (int i=0;i<N;i++){
        long long adjust = avgOffset - offsets[i];
        // simulate network delay
        this_thread::sleep_for(chrono::milliseconds(randDelay(10,80)));
        clocksMs[i] += adjust; // apply adjustment (could be gradual but we directly set)
        lock_guard<mutex> lk(io_mtx);
        cout << "Adjusting P" << i << " by " << adjust << "ms -> new clock " << clocksMs[i] << "ms\n";
    }
}

int main(){
    // initialize clocks with different offsets and small drifts
    clocksMs[0] = 100000; // master base ms (e.g. epoch ms)
    clocksMs[1] = 100250;
    clocksMs[2] = 99900;
    clocksMs[3] = 100500;

    cout << "Berkeley algorithm simulation (master = P0)\n";
    printClocks("Initial clocks:");
    cout << "\nRunning one synchronization round...\n";
    master_round();
    printClocks("\nClocks after Berkeley adjustment:");
    return 0;
}
