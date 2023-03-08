#include "SymbolTable.h"

Node::Node(string id, string type, int level){
    this->id = id;
    this->type = type;
    this->level = level;
    this->pre = nullptr;
    this->next = nullptr;
}

SymbolTable::SymbolTable(){
    this->head = nullptr;
    this->tail = nullptr;
}

SymbolTable::~SymbolTable(){
    while (head!=nullptr){
        Node* tmp=head;
        head=head->next;
        delete tmp;
    }
}

void SymbolTable::check(string& command, string& task, string& id, string& type, string& data, int& level){
    regex idformat("[a-z][a-zA-Z0-9_]*");
    regex numformat("[0-9]*");
    regex strformat("'[a-zA-Z0-9 ]*'");
    if (command=="PRINT"||command=="RPRINT"){
        task=command;
        return;
    }
    if (command=="BEGIN"){
        task=command;
        level++;
        return;
    }
    if (command=="END"){
        task=command;
        if (level==0) throw UnknownBlock(); else level--;
        return;
    }
    int place1=command.find(" ");
    task=command.substr(0,place1);
    if (task=="INSERT"){
        int place2=command.find(" ",place1+1);
        id=command.substr(place1+1,place2-place1-1);
        type=command.substr(place2+1);
        if (regex_match(id, idformat)==0|| (type!="number"&&type!="string")){
            throw InvalidInstruction(command);
        }
    }
    if (task=="ASSIGN"){
        int place2=command.find(" ",place1+1);
        id=command.substr(place1+1,place2-place1-1);
        data=command.substr(place2+1);
        if (regex_match(id, idformat)==0 || (regex_match(data, numformat)==0 && regex_match(data, strformat)==0 && regex_match(data, idformat)==0)){
            throw InvalidInstruction(command);
        }
    }
    if (task=="LOOKUP"){
        id=command.substr(place1+1);
        if (regex_match(id, idformat)==0){
            throw InvalidInstruction(command);
        }
    }

}

void SymbolTable::INSERT(string command, string id, string type, int level){
    Node* newNode= new Node(id, type, level);
    if (this->head==nullptr){
        this->head=this->tail=newNode;
    }
    else {
        Node* temp=this->head;
        while (temp!=nullptr){
            if ((newNode->id==temp->id) && (newNode->level==temp->level)){
                delete newNode;
                throw Redeclared(command);
            }
            else temp=temp->next;
        }
        tail->next=newNode;
        newNode->pre=tail;
        tail=newNode;
    }
    cout<<"success"<<endl;
}

void SymbolTable::ASSIGN(string command, string id, string data){
    regex idformat("[a-z][a-zA-Z0-9_]*");
    regex numformat("[0-9]*");
    regex strformat("'[a-zA-Z0-9 ]*'");
    string datatype;
    Node* tmp=tail;
    if (regex_match(data, numformat)==1){
        datatype="number";
    } else if (regex_match(data, strformat)==1){
        datatype="string";
    } else if (regex_match(data, idformat)==1){
        while (tmp!=nullptr){
            if (tmp->id==data){
                if (tmp->data!="") data=tmp->data;
                datatype=tmp->type;
                break;
            }
            tmp=tmp->pre;
        }
        if (tmp==nullptr) throw Undeclared(command);
    }
    tmp=tail;
    while (tmp!=nullptr){
        if (tmp->id==id){
            if (tmp->type!=datatype) throw TypeMismatch(command);
            else {
                if (data!="") tmp->data=data;
                cout<<"success"<<endl;
                return;
            }
        }
        tmp=tmp->pre;
    }
    if (tmp==nullptr){
        throw Undeclared(command);
    }
}

void SymbolTable::BEGIN(){}

void SymbolTable::END(int level){
    while (tail->level==level+1){
        Node* tmp=tail;
        tail=tail->pre;
        tail->next=nullptr;
        delete tmp;
    }
}

void SymbolTable::LOOKUP(string id, string command){
    Node* temp=tail;
    while (temp!=nullptr){
        if (temp->id==id){
            cout<<temp->level<<endl;
            return;
        }
        temp=temp->pre;
    }
    throw Undeclared(command);
}

void SymbolTable::PRINT(){
    Node* tmp=head;
    while (tmp!=nullptr){
        Node* temp=tmp->next;
        while (temp!=nullptr){
            if (temp->id==tmp->id){
                break;
            }
            temp=temp->next;
            }
            if (temp==nullptr){
                cout<<tmp->id<<"//"<<tmp->level<<" ";
        }
        tmp=tmp->next;
    }
    cout<<endl;
}

void SymbolTable::RPRINT(){
    Node* tmp=tail;
    while (tmp!=nullptr){
        Node* temp=tmp->next;
        while (temp!=nullptr){
            if (temp->id==tmp->id){
                break;
            }
            temp=temp->next;
            }
            if (temp==nullptr){
                cout<<tmp->id<<"//"<<tmp->level<<" ";
        }
        tmp=tmp->pre;
    }
    cout<<endl;
}

void SymbolTable::run(string filename)
{
    string command, task, id, type, data;
    int level=0;
    ifstream infile(filename);
    while (getline(infile, command)){
        check(command, task, id, type, data, level);
        if (task=="INSERT") INSERT(command, id, type, level);
        else if (task=="ASSIGN") ASSIGN(command, id, data);
        else if (task=="BEGIN") BEGIN();
        else if (task=="END") END(level);
        else if (task=="LOOKUP") LOOKUP(id, command);
        else if (task=="PRINT") PRINT();
        else if (task=="RPRINT") RPRINT();
    }
    if (level>0){
        throw UnclosedBlock(level);
    }
}

