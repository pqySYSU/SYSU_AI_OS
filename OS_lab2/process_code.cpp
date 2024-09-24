#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm> // for transform
#include <queue> // for queue

using namespace std;

struct Process {
  unsigned int pid;
  int instruct_num;
  int needed_resources;
  vector<unsigned int> waiting_for_pids;
};

int main() {
    ifstream inputFile("process.txt");

    if (!inputFile.is_open()) {
        cerr << "无法打开文件 process.txt" << endl;
        return 1;
    }

    // 读取第一行 n, m
    int n, m;
    string line;
    getline(inputFile, line);
    stringstream ss(line);
    ss >> m >> n;

    vector<Process> processes;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        Process p;
        string pid_str, instruct_num_str, needed_resources_str, waiting_for_pids_str;
        
        getline(ss, pid_str, '\t');
        getline(ss, instruct_num_str, '\t');
        getline(ss, needed_resources_str, '\t');
        getline(ss, waiting_for_pids_str); 

        p.pid = stoi(pid_str);
        p.instruct_num = stoi(instruct_num_str);
        p.needed_resources = stoi(needed_resources_str);

        stringstream waiting_pids_ss(waiting_for_pids_str);
        string pid;
        while (getline(waiting_pids_ss, pid, '\t')) {
            if (!pid.empty()) {
                p.waiting_for_pids.push_back(stoi(pid));
            }
        }

        processes.push_back(p);
    }

    inputFile.close();

    vector<int> result;
    queue<Process> ready_queue; // 队列：<进程索引, 剩余指令数>
    queue<Process> blocked_queue; // 阻塞队列
    int instruction_count = 0;

    while (1) {
        // 填充队列
        vector<int> to_remove;
        for (size_t i = 0; i < processes.size(); ++i) {
            if (processes[i].instruct_num > 0 && processes[i].needed_resources == 0) { // 进程未完成且不需要等待其他进程
                ready_queue.push(processes[i]);
                to_remove.push_back(i);
                if (ready_queue.size() == m) break; // 队列已满
            }
        }
        for(int i = to_remove.size() - 1; i >= 0; i--) {
            processes.erase(processes.begin() + to_remove[i]); // 删除processes中已经加入ready_queue队列的进程
        }

        if(ready_queue.size()==0 && processes.size()==0) break;

        int current_instructions = n; // 每次执行 n 条指令

        while (!ready_queue.empty() && current_instructions > 0) {
            Process current_process = ready_queue.front();
            ready_queue.pop();

            if(current_process.instruct_num <= current_instructions)
            {
                result.push_back(current_process.pid);
                // current_instructions = current_instructions - current_process.instruct_num;
                for (size_t j = 0; j < processes.size(); ++j) {
                    if (processes[j].needed_resources > 0) {
                        auto it = find(processes[j].waiting_for_pids.begin(), processes[j].waiting_for_pids.end(), current_process.pid);
                        if (it != processes[j].waiting_for_pids.end()) {
                            processes[j].waiting_for_pids.erase(it);
                            processes[j].needed_resources--;
                        }
                    }
                }

                for (size_t i = 0; i < processes.size(); ++i) {
                    if (processes[i].instruct_num > 0 && processes[i].needed_resources == 0) { // 进程未完成且不需要等待其他进程
                        ready_queue.push(processes[i]);
                        processes.erase(processes.begin() + i);
                        break; 
                    }
                }    
            }
            else
            {
                result.push_back(current_process.pid);
                current_process.instruct_num -= current_instructions;
                Process temp_process;
                temp_process.pid = current_process.pid; // 可选，如果需要保存 pid
                temp_process.instruct_num = current_process.instruct_num;
                temp_process.needed_resources = current_process.needed_resources;
                temp_process.waiting_for_pids = current_process.waiting_for_pids;
                ready_queue.push(temp_process);
            }
        }

        // while (ready_queue.size() < m) {
        //     if(blocked_queue.size()!=0)
        //     {
        //         ready_queue.push(blocked_queue.front());
        //         blocked_queue.pop();
        //         if(ready_queue.size()>=m) break;
        //         if(blocked_queue.size()==0) break;
        //     }
        //     else break;
        // }

    }

    ofstream outputFile("result.txt");
    if (outputFile.is_open()) {
        for (size_t i = 0; i < result.size(); ++i) {
            outputFile << result[i] << (i < result.size() - 1 ? "," : "");
        }
        outputFile.close();
    } else {
        cerr << "无法打开文件 result.txt" << endl;
        return 1;
    }

    return 0;
}