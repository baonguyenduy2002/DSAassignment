#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Node{
private:
        string id;
        string type;
        string data;
        int level;
        Node* pre;
        Node* next;
        friend class SymbolTable;
public:
        Node(string id, string type, int level);
};

class SymbolTable
{
private:
    Node* head;
    Node* tail;
public:
    SymbolTable();
    ~SymbolTable();
    void run(string filename);
    void check(string& command, string& task, string& id, string& type, string& data, int& level);
    void INSERT(string command, string id, string type, int level);
    void ASSIGN(string command, string id, string data);
    void BEGIN();
    void END(int level);
    void LOOKUP(string id, string command);
    void PRINT();
    void RPRINT();
};
#endif
