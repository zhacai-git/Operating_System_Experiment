/**
 * @file exp1_proccess_scheduling_FCFS.cpp
 * @author zhacai-git (lzxncut@mail.ncut.edu.cn)
 * @brief Operating system experiment 1: proccess scheduling, using algorithm: First Come First Service.
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <iostream>
#include <string>
#include <unistd.h>

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
}proccessList[128];


int FCFS_Scheduler(int proc_nums);

int isBlockProcess() {
  return 0;
}

int main() {
  cout << "Proccess scheduling demo \n Algorithm: FCFS" << endl;
  cout << "Please enter the number of proccess(no greater than 128):";
  int proc_nums;
  cin >> proc_nums;
  for (int i = 0; i < proc_nums;i++) {
    cout << "proccess number " + to_string(i + 1) + " name: ";
    cin >> proccessList[i].proccess_name;
    cout << "proccess number " + to_string(i + 1) + " required time: ";
    cin >> proccessList[i].required_time;
    proccessList[i].pid = i;
    cout << "proccess PID: " + to_string(proccessList[i].pid) + " allocated\n";
    proccessList[i].stat = initialized;
  }
  cout << "saved proccess list:\n";
  for (int i = 0; i < proc_nums;i++) {
    cout << "PID: " + to_string(proccessList[i].pid) + " NAME: " + proccessList[i].proccess_name + + " STAT: " + to_string(proccessList[i].stat) << endl;
  }
  FCFS_Scheduler(proc_nums);
  cout << "-----All proccess finished-----" << endl;
  for (int i = 0; i < proc_nums;i++) {
    cout << "PID: " + to_string(proccessList[i].pid) + " NAME: " + proccessList[i].proccess_name + + " STAT: " + to_string(proccessList[i].stat) << endl;
  }
  return 0;
}

int FCFS_Scheduler(int proc_nums) {
  cout << "Running PROC LIST By FCFS" << endl;
  for (int i = 0; i < proc_nums; i++) {
    proccessList[i].stat = running;
    cout << "-----Current Proccess------ \n";
    cout << "PID: " + to_string(proccessList[i].pid) + " NAME: " + proccessList[i].proccess_name + + " STAT: " + to_string(proccessList[i].stat) + " required time: " + to_string(proccessList[i].required_time) << endl;
    for (int timer = proccessList[i].required_time; timer > 0; timer--) {
      // cout << timer << endl;
      cout << "PID " + to_string(proccessList[i].pid) + " RUNNING, remain time:" + to_string(timer) + "\n";
      sleep(1);
    }
    proccessList[i].stat = terminated;
    cout << "PID: " + to_string(proccessList[i].pid) + " Terminated" << endl;
  }
  return 0;
}