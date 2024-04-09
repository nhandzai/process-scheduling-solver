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
    void readFromFile(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "Cannot to open the file: " << filename << endl;
            return;
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
        }
        scheduling_chart.push_back(to_string(completion_time));

        for (int i = 0; i < num_process; i++)
        {
            avg_tt += turnaround_time[i];
            avg_wt += waiting_time[i];
        }
        avg_tt /= num_process;
        avg_wt /= num_process;
    }
    void RR()
    {
        sortProcess();
    }

    void printFile(const string &filename)
    {
        ofstream file(filename);
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
    }
};

int main()
{
    ProcessScheduling scheduler;
    scheduler.readFromFile("input.txt");
    scheduler.FCFS();
    scheduler.printFile("FCFS.txt");
    return 0;
}
