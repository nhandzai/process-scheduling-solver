#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

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
            break;

        case 4:
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
