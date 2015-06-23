
main : main.o
	g++ $^ -o $@ -lIrrlicht -lpython2.7 -lboost_python-py27

main.o : main.cpp
	g++ -c $^ -I /usr/include/python2.7/ -std=c++11
