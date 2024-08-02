NuLOB
======
Low latency order matching system written in C++.

Quickstart
=====
To compile and run the orderbook, run these commands from the terminal:

Clone repo: git clone https://github.com/4b41.mini-lob.git
Select directory: cd mini-lob
Compile program: g++ -std=c++20 -Iinclude src/orderbook.cpp src/main.cpp -o Main
Run the program: ./Main

ToDo
=====
* Create makefile
* Implement feed to parse incoming order data from txt file
