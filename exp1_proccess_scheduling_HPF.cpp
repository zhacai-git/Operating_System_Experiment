/**
 * @file exp1_proccess_scheduling_HPF.cpp
 * @author zhacai-git (lzxncut@mail.ncut.edu.cn)
 * @brief Operating system experiment 1: proccess scheduling, using algorithm: High Priority First.
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
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
  int priority;
};

vector<proccess> proc_list;
vector<proccess> finished_proc;

int HPF_scheduler(int proc_nums);

int executer(proccess *proc);

int main() {
  cout << "Proccess scheduling demo \nAlgorithm: HPF" << endl;
  cout << "Please enter the number of proccess:";
  int proc_nums;
  cin >> proc_nums;
  for (int i = 0; i < proc_nums;i++) {
    proccess input;
    cout << "proccess number " + to_string(i + 1) + " name: ";
    cin >> input.proccess_name;
    cout << "proccess number " + to_string(i + 1) + " required time(by seconds): ";
    cin >> input.required_time;
    cout << "proccess number " + to_string(i + 1) + " priority: ";
    cin >> input.priority;
    input.pid = i;
    cout << "proccess PID: " + to_string(input.pid) + " allocated\n";
    input.stat = initialized;
    proc_list.push_back(input);
  }
  cout << "saved proccess list:\n";
  for (int i = 0; i < proc_nums;i++) {
    cout << "PID: " + to_string(proc_list[i].pid) + " NAME: " + proc_list[i].proccess_name + + " STAT: " + to_string(proc_list[i].stat) << endl;
  }

  HPF_scheduler(proc_nums);
  cout << "-----All proccess finished-----" << endl;
  for (int i = 0; i < proc_nums;i++) {
    cout << "PID: " + to_string(finished_proc[i].pid) + " / NAME: " + finished_proc[i].proccess_name + " / Priority: " + to_string(finished_proc[i].priority) + " / STAT: " + to_string(finished_proc[i].stat) << " / Time consumed: " + to_string(finished_proc[i].required_time) << endl;
  }
  return 0;
}

int HPF_scheduler(int proc_nums) {
  int max_priority = proc_list[0].priority;
  int max_priority_loc = 0;
  while (true) {
    max_priority = proc_list[0].priority;
    max_priority_loc = 0;
    for (int i = 0;i < proc_list.size(); i++) {
      if (max_priority < proc_list[i].priority) {
        max_priority = proc_list[i].priority;
        max_priority_loc = i;
      }
    }
    executer(&proc_list[max_priority_loc]);
    proc_list.erase(proc_list.begin() + max_priority_loc);
    if (proc_list.size() == 0) return 0;
  }
  
}

int executer(proccess *proc) {
  cout << "executing pid: " + to_string(proc->pid) << endl;
  cout << "-----Current Proccess------ \n";
  cout << "PID: " + to_string(proc->pid) + " NAME: " + proc->proccess_name + + " STAT: " + to_string(proc->stat) + " Priority: " + to_string(proc->priority) + " required time: " + to_string(proc->required_time) << endl;
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