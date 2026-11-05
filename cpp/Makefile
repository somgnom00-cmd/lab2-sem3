CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

SOURCES = main.cpp array.cpp singly.cpp doubly.cpp stack.cpp queue.cpp tree.cpp \
          task1.cpp set.cpp task3.cpp task4.cpp task5.cpp hashtable.cpp task7.cpp
HEADERS = array.h singly.h doubly.h stack.h queue.h tree.h \
          task1.h set.h task3.h task4.h task5.h hashtable.h task7.h

lab2: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o lab2

.PHONY: all clean
all: lab2

clean:
	rm -f lab2
