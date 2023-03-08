#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class Node {
public:
    string id;
    string type;
    string data;
    long long code;
    int scope;
    int num_parameter;

    friend class SymbolTable;
public:
    Node(): id(""), type(""), data(""), code(-1), scope(-1), num_parameter(-1) {};
    Node(string id, string type, int scope, int num_parameter){
        this->id = id;
        this->type = type;
        this->scope = scope;
        this->num_parameter = num_parameter;
        code = scope;
        int len = id.size();
        for (int i=0; i<len; i++){
            if (id[i]-48>=10){
                code = code*100 + (id[i]-48);
            } else {
                code = code*10 + (id[i]-48);
            }
        }
    }
    ~Node() {};
};

class SymbolTable
{
public:
    Node* hash;
public:
    string hashfunc;
    int m, c1, c2;

    SymbolTable() {
        hash = nullptr;
    }
    ~SymbolTable() {
        delete[] hash;
    }

    //<><><><><><><><><><><><> support function <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
    int linear_insert(string command, int& i_slotcount, long long code, int m, int c1, int scope, string id);
    int quadratic_insert(string command, int& i_slotcount, long long code, int m, int c1, int c2, int scope, string id);
    int doublehash_insert(string command, int& i_slotcount, long long code, int m, int c1, int scope, string id);
    int linear_assign(string command, int& i_slotcount, string id, int m, int c1, int scope);
    int quadratic_assign(string command, int& i_slotcount, string id, int m, int c1, int c2, int scope);
    int doublehash_assign(string command, int& i_slotcount, string id, int m, int c1, int scope);

    //<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
    void run(string filename);
    void check(string& command, string& task, string& id, string& type, int& num_parameter, string& data, int& scope, string& funcid, int& m, int& c1, int& c2);
    void LINEAR(int c1, int m,string command);
    void QUADRATIC(int c1, int c2, int m, string command);
    void DOUBLE(int c1, int m, string command);
    void INSERT(string command, string id, string type, int scope, int num_parameter);
    void ASSIGN(string command, string id, string data, int scope);
    void BEGIN(string command);
    void END(int scope, string command);
    void LOOKUP(string id, string command, int scope);
    void PRINT(string command);
    void CALL(string command, string data, int scope);
};

#endif