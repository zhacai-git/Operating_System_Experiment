/**
 * @file exp2_storage_management_FIFO.cpp
 * @author zhacai-git (lzxncut@mail.ncut.edu.cn)
 * @brief Operating system experiment 2: storage management, using algorithm: First In First Out.
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

vector<int> mem;

vector<int> access_sequence;

int fault_times = 0;

int FIFO_execute(int m_size);
void mem_display(int i) {
  cout << i <<" ";
}

int main() {
  cout << "Storage Management Demo \nUsing Algorithm: FIFO" << endl;
  cout << "Please enter the size of the memory: ";
  int m_size = 0;
  cin >> m_size;
  cout << "[*] Memory size has been set to " << m_size << endl;
  cout << "Please enter the access sequences, ends with -1: ";
  int t_sequence;
  for (int i = 0;;i++) {
    cin >> t_sequence;
    if (t_sequence == -1) {
      break;
    }
    access_sequence.push_back(t_sequence);
  }
  cout << "[*] Confirmed that the access sequences are: " << endl;
  for (int i = 0;i < access_sequence.size(); i++) {
    cout << access_sequence[i] << " ";
  }
  cout << "\n";
  cout << "[*] Begin simulation proccess, using algorithm FIFO...." << endl;

  FIFO_execute(m_size);
  cout << "[*] Final Memory State: " << endl;
  for_each(mem.begin(), mem.end(), mem_display);
  cout << "\n";
  double fault_rate = (float)fault_times / access_sequence.size();
  cout << "[*] Page fault rate: " << setprecision(4) << fault_rate * 100 << "%" << endl;
  return 0;
}


int FIFO_execute(int m_size) {
  int last_replaced_loc = 0;
  for (int i = 0;i < access_sequence.size();i++) {
    cout << "Access sequence reading: " << access_sequence[i] << endl;
    cout << "Current mem data: ";
    for_each(mem.begin(), mem.end(), mem_display);
    cout << "\n";
    if (find(mem.begin(),mem.end(), access_sequence[i]) != mem.end()) { // 不缺页
      cout << "[*] Mem access hit..." << endl;
    } else {
      cout << "[*] Page fault...checking memory..." << endl;
      fault_times++;
      if (mem.size() < m_size) { // 内存未满
        cout << "[*] Memory not full, pushing... " << access_sequence[i] << endl;
        mem.push_back(access_sequence[i]); // 调入内存
        for_each(mem.begin(), mem.end(), mem_display);
        cout << "\n";
      } else {
        cout << "Memory full, replacing page " << mem[last_replaced_loc] << " to " << access_sequence[i] << endl;
        // replace(mem.begin(),mem.begin(), *mem.begin(), access_sequence[i]);
        mem[last_replaced_loc++] = access_sequence[i];
        if (last_replaced_loc == m_size) last_replaced_loc = 0;
        for_each(mem.begin(), mem.end(), mem_display);
        cout << "\n";
      }
    }
  }
  return 0;
}