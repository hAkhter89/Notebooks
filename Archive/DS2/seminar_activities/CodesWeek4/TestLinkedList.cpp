// TestLinkedList.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma region "SLList usage example"
#if 1
#include <iostream>
#include "SLList.h"

int main()
{
    int data[] = { 1, 4, 5, 6, 3, 2 };
    
    ods::SLList<int> list;

    for (int i : data)
    {
        list.add(i);
    }
    
    int n = list.size();
    std::cout << "SLList::size = " << list.size() << std::endl;

    while (list.size()>0)
    {
        std::cout << list.peek() << std::endl;
        list.remove();
    }
    return 0;
}
#endif
#pragma endregion

#pragma region "DLList usage example"
#if 0
#include <iostream>
#include "DLList.h"

int main()
{
    int data[] = { 1, 4, 5, 6, 3, 2 };

    ods::DLList<int> list;

    const int N = sizeof(data) / sizeof(data[0]);

    for (int i=0; i<N; ++i)
    {
        list.add( i, data[i] );
    }

    int n = list.size();
    std::cout << "DLList::size = " << list.size() << std::endl;

    for (int i=0, N=list.size(); i<N; ++i)
    {
        std::cout << list.get(i) << std::endl; 
    }
     
    return 0;
}

#endif
#pragma endregion