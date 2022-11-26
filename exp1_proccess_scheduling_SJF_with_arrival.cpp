/**
 * @file exp1_proccess_scheduling_SJF_with_arrival.cpp
 * @author zhacai-git (lzxncut@mail.ncut.edu.cn)
 * @brief Operating system experiment 1: proccess scheduling, using algorithm: Short Job First.
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
// with arrival time
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

#define running 1;
#define terminated 2;
#define blocked 0;
#define initialized 3;

struct proccess {
  int pid;
  string proccess_name;
  int stat;
  int required_time;
  int arrival_time;
};

vector<proccess> proc_list;
vector<proccess> finished_proc;

int SJF_scheduler(int proc_num);

int executer(proccess *proc);

int main() {
  cout << "Proccess scheduling demo \nAlgorithm: SJF" << endl;
  cout << "Please enter the number of proccess:";
  int proc_nums;
  cin >> proc_nums;
  for (int i = 0; i < proc_nums;i++) {
    proccess input;
    cout << "proccess number " + to_string(i + 1) + " name: ";
    cin >> input.proccess_name;
    cout << "proccess number " + to_string(i + 1) + " required time(by seconds): ";
    cin >> input.required_time;
    input.pid = i;
    cout << "proccess PID: " + to_string(input.pid) + " allocated\n";
    input.stat = initialized;
    proc_list.push_back(input);
  }
  cout << "saved proccess list:\n";
  for (int i = 0; i < proc_nums;i++) {
    cout << "PID: " + to_string(proc_list[i].pid) + " NAME: " + proc_list[i].proccess_name + + " STAT: " + to_string(proc_list[i].stat) << endl;
  }
  
  SJF_scheduler(proc_nums);
  cout << "-----All proccess finished-----" << endl;
  for (int i = 0; i < proc_nums;i++) {
    cout << "PID: " + to_string(finished_proc[i].pid) + " / NAME: " + finished_proc[i].proccess_name + + " / STAT: " + to_string(finished_proc[i].stat) << " / Time consumed: " + to_string(finished_proc[i].required_time) << endl;
  }
  return 0;
}

int SJF_scheduler(int proc_nums) {
  cout << "Running PROC LIST By SJF" << endl;
  int min_required_time = proc_list[0].required_time;
  int min_proccess_loc = 0;
  while (true) {
    // proc_it = proc_list.begin();
    min_required_time = proc_list[0].required_time;
    min_proccess_loc = 0;
    for (int i = 0;i < proc_list.size(); i++) {
      if (min_required_time > proc_list[i].required_time) {
        min_required_time = proc_list[i].required_time;
        min_proccess_loc = i;
      }
    }
    // cout << "found proccess pid: " + to_string(proc_list[min_proccess_loc].pid) << " required time: " + to_string(proc_list[min_proccess_loc].required_time) << endl;
    executer(&proc_list[min_proccess_loc]);
    proc_list.erase(proc_list.begin() + min_proccess_loc);
    if (proc_list.size() == 0) return 0;
  }
}

int executer(proccess *proc) {
  cout << "executing pid: " + to_string(proc->pid) << endl;
  cout << "-----Current Proccess------ \n";
    cout << "PID: " + to_string(proc->pid) + " NAME: " + proc->proccess_name + + " STAT: " + to_string(proc->stat) + " required time: " + to_string(proc->required_time) << endl;
    for (int timer = proc->required_time; timer > 0; timer--) {
      cout << "PID " + to_string(proc->pid) + " RUNNING, remain time:" + to_string(timer) << "\n";
      usleep(1000000);
      // sleep(1);
    }
    proc->stat = terminated;
    cout << "PID: " + to_string(proc->pid) + " Terminated" << endl;
    finished_proc.push_back(*proc);
  return 0;
}