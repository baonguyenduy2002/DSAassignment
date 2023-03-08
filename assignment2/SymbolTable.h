#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include "main.h"

class node{
private:
    string id;
    string type;
    string data;
    string stat;
    int block;
    node* right;
    node* left;
    node* parent;
    friend class SymbolTable;

public:
    node(string id, string type, string stat, int block);
};

class SymbolTable
{
private:
    node* root;
public:
    SymbolTable();                  //Constructor
    ~SymbolTable();                 //Destructor
    void deleteTree(node* tmp);
    void splay(node* temp, int& numSplay);
    node* zig(node* temp);
    node* zag(node* temp);
    node* zig_zig(node* temp);
    node* zag_zag(node* temp);
    node* zig_zag(node* temp);
    node* zag_zig(node* temp);
    node* search(node* temp, string id, int block);
    node* search(node* temp, string id, int block, int& numCompa);
    void preOrder(node* current, string& result);
    void run(string filename);
    void check(string& command, string& task, string& id, string& stat, string& type, string& data, int& block);
    void INSERT(string command, string id, string type, string stat, int block, string& idstore);
    void ASSIGN(string command, string id, string data, int block);
    void BEGIN();
    void END(int block, string& idstore);
    void LOOKUP(string id, string command, int block);
    void PRINT();

};
#endif
