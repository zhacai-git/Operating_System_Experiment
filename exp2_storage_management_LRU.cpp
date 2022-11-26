/**
 * @file exp2_storage_management_LRU.cpp
 * @author zhacai-git (lzxncut@mail.ncut.edu.cn)
 * @brief Operating system experiment 2: storage management, using algorithm: Least Recently Used.
 * @version 0.1
 * @date 2022-11-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

struct Page {
  int page_id;
  int time_elapsed;
};

vector<Page> mem;
vector<int> access_sequence;

int fault_times = 0;

int LRU_execute(int m_size);
int access_time_add();
int maxinum_time_index(int m_size);
bool mem_exists(int page_id);

void mem_display(Page mem_element) {
  cout << " " << mem_element.page_id << " ";
}

int main() {
  cout << "Storage Management Demo \nUsing Algorithm: LRU" << endl;
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
  cout << "[*] Begin simulation proccess, using algorithm LRU...." << endl;
  LRU_execute(m_size);
  cout << "[*] Final Memory State: " << endl;
  for_each(mem.begin(), mem.end(), mem_display);
  cout << "\n";
  double fault_rate = (float)fault_times / access_sequence.size();
  cout << "[*] Page fault rate: " << setprecision(4) << fault_rate * 100 << "%" << endl;
  return 0;
}

int LRU_execute(int m_size) {
  for (int i = 0; i < access_sequence.size(); i++) {
    cout << "[*] Access sequence reading: " << access_sequence[i] << endl;
    cout << "Current mem data: ";
    for_each(mem.begin(), mem.end(), mem_display);
    cout << "\n";
    if (mem_exists(access_sequence[i])) {
      cout << "[*] Mem access hit..." << endl;
    } else {
      cout << "[*] Page fault...checking memory..." << endl;
      fault_times++;
      Page t_swap;
      t_swap.page_id = access_sequence[i];
      t_swap.time_elapsed = 0;
      if (mem.size() < m_size) {
        cout << "[*] Memory not full, pushing... " << access_sequence[i] << endl;
        mem.push_back(t_swap); // 调入内存
        for_each(mem.begin(), mem.end(), mem_display);
        cout << "\n";
      } else {
        cout << "[*] Memory full, found Least Recent Used Page: ";
        int max_page_index = maxinum_time_index(m_size);
        cout << mem[max_page_index].page_id << endl;
        mem[max_page_index] = t_swap;
        cout << "Modified mem data: ";
        for_each(mem.begin(), mem.end(), mem_display);
        cout << "\n";
      }
    }
    access_time_add();
  }
  return 0;
}

int access_time_add() {
  cout << "------ [*] Begin Mem State: ------" << endl;
  for (auto it = mem.begin(); it != mem.end(); it++) {
    it->time_elapsed++;
    cout << "Page id " << it->page_id << " time elapsed " << it->time_elapsed << endl;
  }
  // cout << "Page id " << iter.page_id << " time elapsed " << iter.time_elapsed << endl;
  cout << "------ [*] End Mem State ------" << endl;
  return 0;
}

bool mem_exists(int page_id) { // 检测是否存在页，存在则直接将time_elapsed置0后返回true
  for (vector<Page>::iterator it = mem.begin(); it != mem.end(); it++) {
    if (it->page_id == page_id) {
      it->time_elapsed = 0;
      return true;
    }
  }
  return false;
}

int maxinum_time_index(int m_size) { // 寻找当前时间最长的未访问页
  int max_time = mem.begin()->time_elapsed;
  int max_loc = 0;
  int loc = 0;
  for (auto it = mem.begin(); it != mem.end(); it++) {
    if (max_time < it->time_elapsed) {
      max_time = it->time_elapsed;
      max_loc = loc;
    }
    loc++;
  }
  return max_loc;
}