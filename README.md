# NuLOB

Low latency order matching system written in C++.

## Quickstart

To compile and run the orderbook, run these commands from the terminal:

1. Clone repo: git clone https://github.com/4b41.mini-lob.git
2. Select directory: cd mini-lob
3. Compile program: g++ -std=c++20 -Iinclude src/orderbook.cpp src/main.cpp -o Main
4. Run the program: ./Main

## ToDo

* Create makefile
* Implement feed to parse incoming order data from txt file
