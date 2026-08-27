#include <iostream>
using namespace std;
template <typename T> class Stack {
public:
  Stack() : size(10), topindex(-1) { data = new T[size]; }
  ~Stack() {
    delete[] data;
    data = nullptr;
  }

  void push(T name);
  void pop();
  T top();
  bool isEmpty();
  bool isFull();
  friend bool Dyck(int arr[], int n);

private:
  T *data;
  int size;
  int dyckarr[10]; // Dyck arr
};

template <typename T> void Stack<T>::push(T name) {}
bool Dyck(int arr[],
          int n) { // arr[] is the stack, n is the number of operations
  int size = 0;    // stack starts empty

  for (int i = 0; i < n; i++) {
    size += arr[i];

    if (size < 0) // at any point, if stack underflows
      return false;
  }

  return size ==
         0; // returns true only if stack is empty at the end, else false.
}

int main() { cout << "Hello World" << endl; }
