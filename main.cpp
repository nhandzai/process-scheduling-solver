#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

class ProcessScheduling
{
private:
    int num_process;
    int quantum_time;
    vector<string> name_process;
    vector<int> arrival_time;
    vector<int> cpu_burst;
    vector<int> priority;

    vector<string> scheduling_chart;
    vector<int> turnaround_time;
    vector<int> waiting_time;
    double avg_tt;
    double avg_wt;

public:
    template <typename T>
    void mySwap(T &a, T &b)
    {
        T temp = a;
        a = b;
        b = temp;
    }
    bool allEmpty(const vector<int> &vec)
    {
        for (int i = 0; i < vec.size(); ++i)
        {
            if (vec[i] > 0)
            {
                return false;
            }
        }
        return true;
    }
    void clearData()
    {
        scheduling_chart.clear();
        turnaround_time.clear();
        waiting_time.clear();
        avg_tt = 0;
        avg_wt = 0;
    }
    bool readFromFile(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "Cannot to open the file: " << filename << endl;
            return false;
        }

        file >> num_process >> quantum_time;

        string line;
        while (getline(file, line))
        {
            if (line.empty())
                continue;

            istringstream iss(line);
            string name;
            int arrival, burst, prio = 0;

            iss >> name >> arrival >> burst >> prio;

            name_process.push_back(name);
            arrival_time.push_back(arrival);
            cpu_burst.push_back(burst);
            priority.push_back(prio);
        }

        file.close();
        return true;
    }
    void sortProcess()
    {
        for (int i = 0; i < num_process; i++)
        {
            for (int j = 0; j < num_process - i - 1; j++)
            {
                if (arrival_time[j] > arrival_time[j + 1])
                {
                    mySwap(arrival_time[j], arrival_time[j + 1]);
                    mySwap(cpu_burst[j], cpu_burst[j + 1]);
                    mySwap(name_process[j], name_process[j + 1]);
                    mySwap(priority[j], priority[j + 1]);
                }
            }
        }
    }
    void FCFS()
    {
        sortProcess();

        int completion_time = arrival_time[0];
        for (int i = 0; i < num_process; i++)
        {
            scheduling_chart.push_back(to_string(completion_time));
            scheduling_chart.push_back(name_process[i]);
            completion_time += cpu_burst[i];
            turnaround_time.push_back(completion_time - arrival_time[i]);
            waiting_time.push_back(turnaround_time[i] - cpu_burst[i]);
            avg_tt += turnaround_time[i];
            avg_wt += waiting_time[i];
        }
        scheduling_chart.push_back(to_string(completion_time));

        avg_tt /= num_process;
        avg_wt /= num_process;
    }
    void RR()
    {
        sortProcess();
        int completion_time = arrival_time[0];
        vector<int> burst_remain(cpu_burst);
        vector<int> ct_each_process(num_process, 0);

        scheduling_chart.push_back(to_string(completion_time));

        bool end = false;
        while (!end)
        {
            for (int i = 0; i < num_process; i++)
            {
                if (burst_remain[i] > 0)
                {
                    scheduling_chart.push_back(name_process[i]);
                    completion_time += min(burst_remain[i], quantum_time);
                    burst_remain[i] = max(burst_remain[i] - quantum_time, 0);
                    ct_each_process[i] = completion_time;
                    scheduling_chart.push_back(to_string(completion_time));
                }
            }
            end = allEmpty(burst_remain);
        }

        for (int i = 0; i < num_process; i++)
        {
            turnaround_time.push_back(ct_each_process[i] - arrival_time[i]);
            waiting_time.push_back(turnaround_time[i] - cpu_burst[i]);
            avg_tt += turnaround_time[i];
            avg_wt += waiting_time[i];
        }
        avg_tt /= num_process;
        avg_wt /= num_process;
    }

    static bool sortbysec(pair<int, int> a, pair<int, int> b)
    {
        return (a.second < b.second);
    }

    void SJF()
    {
        turnaround_time.resize(num_process);
        waiting_time.resize(num_process);
        sortProcess();
        int completion_time = arrival_time[0];
        vector<bool> done(num_process, false);
        scheduling_chart.push_back(to_string(completion_time));
        int shortest_index = -1;
        vector<pair<int, int>> ready_list;
        while (count(done.begin(), done.end(), false) > 0)
        {
            for (int i = 0; i < num_process; i++)
            {
                if (!done[i] && arrival_time[i] <= completion_time && find_if(ready_list.begin(), ready_list.end(), [i](const auto &pair)
                                                                              { return pair.first == i; }) == ready_list.end())
                {
                    ready_list.push_back(make_pair(i, cpu_burst[i]));
                }
            }

            sort(ready_list.begin(), ready_list.end(), sortbysec);

            shortest_index = ready_list[0].first;

            scheduling_chart.push_back(name_process[shortest_index]);
            completion_time += cpu_burst[shortest_index];
            scheduling_chart.push_back(to_string(completion_time));
            turnaround_time[shortest_index] = completion_time - arrival_time[shortest_index];
            waiting_time[shortest_index] = (turnaround_time[shortest_index] - cpu_burst[shortest_index]);
            avg_tt += turnaround_time[shortest_index];
            avg_wt += waiting_time[shortest_index];
            done[shortest_index] = true;
            ready_list.erase(ready_list.begin());
        }

        avg_tt /= num_process;
        avg_wt /= num_process;
    }

    void Priority()
    {
        turnaround_time.resize(num_process);
        waiting_time.resize(num_process);
        
        int priority_index = 0;
        int fst_priority = INT_MAX;
        vector<bool> done(num_process, false);
        sortProcess();
        vector<int> cpu_burst_left(cpu_burst);
        int completion_time = arrival_time[0];
        vector<pair<int, int>> ready_list;
        ready_list.push_back(make_pair(0, priority[0]));
        while (ready_list.size() < num_process)
        {

            for (int i = 0; i < num_process; ++i)
            {
                if (arrival_time[i] == completion_time && find_if(ready_list.begin(), ready_list.end(), [i](const auto& pair)
                    { return pair.first == i; }) == ready_list.end())
                {
                        ready_list.push_back(make_pair(i, priority[i]));
                    
                }
            }
            int previous = 0;
            sort(ready_list.begin(), ready_list.end(), sortbysec);
            for (int i = 0; i < ready_list.size(); ++i)
            {
                if (arrival_time[ready_list[i].first] < completion_time && !done[ready_list[i].first])
                {
                    previous = ready_list[i].first;
                    break;
                }
            }
            for (int i = 0; i < ready_list.size(); ++i)
            {
                if (arrival_time[ready_list[i].first] <= completion_time && priority[ready_list[i].first] < priority[previous] || ready_list.size() == 1)
                {
                    int tmp = arrival_time[ready_list[i].first] - arrival_time[previous];
                    cpu_burst_left[previous] -= tmp;
                    scheduling_chart.push_back(to_string(completion_time));
                    scheduling_chart.push_back(name_process[ready_list[i].first]);
                    fst_priority = priority[ready_list[i].first];
                    break;
                }
                else if (priority[ready_list[i].first] == priority[previous] && cpu_burst[previous] != 0)
                {
                    --cpu_burst_left[previous];
                }
                
            }
            ++completion_time;
        }
        --completion_time;
        
        scheduling_chart.pop_back();

        while (count(done.begin(), done.end(), false) > 0)
        {
            priority_index = ready_list[0].first;
            completion_time += cpu_burst_left[priority_index];
            scheduling_chart.push_back(name_process[priority_index]);
            scheduling_chart.push_back(to_string(completion_time));
            turnaround_time[priority_index] = completion_time - arrival_time[priority_index];
            waiting_time[priority_index] = (turnaround_time[priority_index] - cpu_burst[priority_index]);
            avg_tt += turnaround_time[priority_index];
            avg_wt += waiting_time[priority_index];
            done[priority_index] = true;
            ready_list.erase(ready_list.begin());
        }

        avg_tt /= num_process;
        avg_wt /= num_process;
    }

    bool printFile(const string &filename)
    {
        ofstream file(filename);
        if (!file.is_open())
        {
            cerr << "Cannot to open the file: " << filename << endl;
            return false;
        }
        file << "Scheduling chart: ";
        for (int i = 0; i < scheduling_chart.size(); i++)
        {
            if (i != 0)
                file << "~";
            file << scheduling_chart[i];
        }
        file << endl;

        for (int i = 0; i < name_process.size(); i++)
        {
            file << name_process[i] << ": " << setw(10) << " "
                 << "TT = " << setw(10) << left << turnaround_time[i] << " "
                 << "WT = " << setw(10) << left << waiting_time[i]
                 << endl;
        }

        file << "Average:" << setw(6) << " " << fixed << setprecision(2)
             << "TT = " << setw(10) << avg_tt << " "
             << "WT " << setw(10) << avg_wt << endl;
        clearData();
        return true;
    }
};

int main()
{
    ProcessScheduling scheduler;
    string fileName;
    int choice;

    cout << "Enter the file name: ";
    cin >> fileName;
    if (!scheduler.readFromFile(fileName))
        return 0;

    while (true)
    {
        cout << "----- Menu -----" << endl
             << "1. First Come First Serve (FCFS)" << endl
             << "2. Round - Robin (RR)" << endl
             << "3. Shortest Job First (SJF)" << endl
             << "4. Priority" << endl
             << "5. Exit" << endl
             << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            scheduler.FCFS();
            if (scheduler.printFile("FCFS.txt"))
                cout << "Task completed\n";
            else
            {
                cout << "Failed to solve\n";
                return 0;
            }
            break;
        case 2:
            scheduler.RR();
            if (scheduler.printFile("RR.txt"))
                cout << "Task completed\n";
            else
            {
                cout << "Failed to solve\n";
                return 0;
            }
            break;
        case 3:
            scheduler.SJF();
            if (scheduler.printFile("SJF.txt"))
                cout << "Task completed\n";
            else
            {
                cout << "Failed to solve\n";
                return 0;
            }
            break;

        case 4:
            scheduler.Priority();
            if (scheduler.printFile("Priority.txt"))
                cout << "Task completed\n";
            else
            {
                cout << "Failed to solve\n";
                return 0;
            }
            break;

        case 5:
            cout << "Exiting the program." << endl;
            return 0;
        default:
            cout << "Invalid choice. Please try again." << endl;
            break;
        }
    }
    return 0;
}
