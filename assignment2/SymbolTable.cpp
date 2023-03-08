#include "SymbolTable.h"

node::node(string id, string type, string stat, int block){
    this->id = id;
    this->type = type;
    this->block = block;
    this->stat = stat;
    this->parent = nullptr;
    this->left = nullptr;
    this->right = nullptr;
}

SymbolTable::SymbolTable(){
    this->root = nullptr;
}

SymbolTable::~SymbolTable(){
    deleteTree(this->root);
}

void SymbolTable::deleteTree(node* tmp){
    if(tmp != nullptr){
        if (tmp->left != nullptr){
            deleteTree(tmp->left);
        }
        if (tmp->right != nullptr){
            deleteTree(tmp->right);
        }
        delete tmp;
    }
}

void SymbolTable::check(string& command, string& task, string& id, string& type, string& stat, string& data, int& block){
    regex idformat("[a-z][a-zA-Z0-9_]*");
    regex numformat("[0-9]*");
    regex strformat("'[a-zA-Z0-9 ]*'");
    regex funcformat("\\(((number|string)(,(number|string))*)?\\)->(number|string)");
    regex funcdataformat("[a-z][a-zA-Z0-9_]*\\((([0-9]*|'[a-zA-Z0-9 ]*'|[a-z][a-zA-Z0-9_]*)(,([0-9]*|'[a-zA-Z0-9 ]*'|[a-z][a-zA-Z0-9_]*))*)?\\)");
    if (command=="PRINT"){
        task = command;
        return;
    }
    if (command=="BEGIN"){
        task = command;
        block++;
        return;
    }
    if (command=="END"){
        task = command;
        if (block==0) throw UnknownBlock(); else block--;
        return;
    }

    int place1=command.find(" ");
    task=command.substr(0,place1);
    if (task=="INSERT"){
        int place2=command.find(" ",place1+1);
        id=command.substr(place1+1,place2-place1-1);
        int place3=command.find(" ",place2+1);
        type=command.substr(place2+1,place3-place2-1);
        stat=command.substr(place3+1);
        if (regex_match(id, idformat)==0||(type!="number"&&type!="string"&&regex_match(type, funcformat)==0)||(stat!="false"&&stat!="true")){
            throw InvalidInstruction(command);
        }
    }
    if (task=="ASSIGN"){
        int place2=command.find(" ",place1+1);
        id=command.substr(place1+1,place2-place1-1);
        data=command.substr(place2+1);
        if (regex_match(id, idformat)==0 || ((regex_match(data, numformat)==0 && regex_match(data, strformat)==0 && regex_match(data, idformat)==0) && regex_match(data, funcdataformat)==0)){
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

void SymbolTable::splay(node* temp, int& numSplay){
    while(temp->parent!=nullptr){
        if (temp->parent==this->root){
            numSplay++;
            if (temp->block==temp->parent->block){
            int comp = temp->id.compare(temp->parent->id);
            if (comp<0) {
                temp=zig(temp->parent);
            } else {
                temp=zag(temp->parent);
            }
            } else if (temp->block > temp->parent->block){
                temp=zag(temp->parent);
            } else temp=zig(temp->parent);
        } else{
            numSplay++;
            int comp1, comp2;
            if (temp->block==temp->parent->block){
                comp1 = temp->id.compare(temp->parent->id);
            } else if (temp->block > temp->parent->block){
                comp1 = 1;
            } else comp1 = -1;
            if (temp->parent->block==temp->parent->parent->block){
                comp2 = temp->parent->id.compare(temp->parent->parent->id);
            } else if (temp->parent->block > temp->parent->parent->block){
                comp2 = 1;
            } else comp2 = -1;
            
            if (comp1<0 && comp2<0){
                temp=zig_zig(temp);
            } else if (comp1>=0 && comp2>=0){
                temp=zag_zag(temp);
            } else if (comp1<0 && comp2>=0){
                temp=zig_zag(temp);
            } else{
                temp=zag_zig(temp);
            }
        }
    }
    this->root=temp;
}

node* SymbolTable::zig(node* parentnode){
    node* current=parentnode->left;
    parentnode->left=current->right;
    if (current->right!=nullptr){
        current->right->parent=parentnode;
    }
    current->right=parentnode;

    node* tmp=parentnode->parent;
    parentnode->parent=current;
    current->parent=tmp;
    if (tmp!=nullptr){
    if (tmp->right==parentnode){
        tmp->right=current;
    } else {
        tmp->left=current;
    }
    }
    return current;
}

node* SymbolTable::zag(node* parentnode){
    node* current=parentnode->right;
    parentnode->right=current->left;
    if (current->left!=nullptr){
        current->left->parent=parentnode;
    }
    current->left=parentnode;

    node* tmp=parentnode->parent;
    parentnode->parent=current;
    current->parent=tmp;
    if (tmp!=nullptr){
    if (tmp->right==parentnode){
        tmp->right=current;
    } else {
        tmp->left=current;
    }
    }
    return current;
}

node* SymbolTable::zig_zig(node* current){
    node* parentnode=current->parent;
    node* grandnode=current->parent->parent;
    node* CA=parentnode->right;
    node* DA=current->right;
    current->parent=grandnode->parent;
    if (grandnode->parent!=nullptr){
        if (grandnode->parent->right==grandnode){
            grandnode->parent->right=current;
        } else grandnode->parent->left=current;
    }
    grandnode->parent=parentnode;
    parentnode->parent=current;
    current->right=parentnode;
    parentnode->right=grandnode;
    grandnode->left=CA;
    if (CA!=nullptr){
        CA->parent=grandnode;
    }
    parentnode->left=DA;
    if (DA!=nullptr){
        DA->parent=parentnode;
    }
    return current;
}

node* SymbolTable::zag_zag(node* current){
    node* parentnode=current->parent;
    node* grandnode=current->parent->parent;
    node* CA=parentnode->left;
    node* DA=current->left;
    current->parent=grandnode->parent;
    if (grandnode->parent!=nullptr){
        if (grandnode->parent->right==grandnode){
            grandnode->parent->right=current;
        } else grandnode->parent->left=current;
    }
    grandnode->parent=parentnode;
    parentnode->parent=current;
    current->left=parentnode;
    parentnode->left=grandnode;
    grandnode->right=CA;
    if (CA!=nullptr){
        CA->parent=grandnode;
    }
    parentnode->right=DA;
    if (DA!=nullptr){
        DA->parent=parentnode;
    }
    return current;
}

node* SymbolTable::zig_zag(node* current){
    node* parentnode=current->parent;
    node* grandnode=current->parent->parent;
    node* A=current->left;
    node* B=current->right;
    current->left=grandnode;
    current->right=parentnode;
    current->parent=grandnode->parent;
    if (grandnode->parent!=nullptr){
        if (grandnode->parent->right==grandnode){
            grandnode->parent->right=current;
        } else grandnode->parent->left=current;
    }
    grandnode->parent=current;
    parentnode->parent=current;
    grandnode->right=A;
    if (A!=nullptr){
        A->parent=grandnode;
    }
    parentnode->left=B;
    if (B!=nullptr){
        B->parent=parentnode;
    }
    return current;
}

node* SymbolTable::zag_zig(node* current){
    node* parentnode=current->parent;
    node* grandnode=current->parent->parent;
    node* A=current->right;
    node* B=current->left;
    current->right=grandnode;
    current->left=parentnode;
    current->parent=grandnode->parent;
    if (grandnode->parent!=nullptr){
        if (grandnode->parent->left==grandnode){
            grandnode->parent->left=current;
        } else grandnode->parent->right=current;
    }
    grandnode->parent=current;
    parentnode->parent=current;
    grandnode->left=A;
    if (A!=nullptr){
        A->parent=grandnode;
    }
    parentnode->right=B;
    if (B!=nullptr){
        B->parent=parentnode;
    }
    return current;
}

void SymbolTable::INSERT(string command, string id, string type, string stat, int block, string& idstore){
    int numComp=0;
    int numSplay=0;
    idstore+=id;
    idstore+=",";
    node* newnode = new node(id, type, stat, block);
    if (stat=="true"){
        newnode->block=0;
    }
    if (this->root==nullptr){
        this->root=newnode;
    } else {
        node* temp=this->root;
        node* y;
        while (temp!=nullptr){
            y=temp;
            if (newnode->id==temp->id && newnode->block==temp->block){
                delete newnode;
                throw Redeclared(command);
            }
            int cmp = newnode->id.compare(temp->id);
            numComp++;
            if (newnode->block==temp->block){
                if (cmp>=0){
                    temp=temp->right;
                } else {
                    temp=temp->left;
            }
            } else if (newnode->block > temp->block){
                temp=temp->right;
            } else {
                temp=temp->left;
            }
        }
        if (newnode->type!="number" && newnode->type!="string"){
            if (newnode->block!=0) {delete newnode; throw InvalidDeclaration(command);}
        }
        if (newnode->block == y->block){
        int cmp = newnode->id.compare(y->id);
        if (cmp>0){
            y->right=newnode;
        } else {
            y->left=newnode;
        }
        } else if (newnode->block > y->block){
            y->right=newnode;
        } else y->left=newnode;
        newnode->parent=y;
    }
    splay(newnode, numSplay);
    cout<<numComp<<" "<<numSplay<<endl;
}

void SymbolTable::ASSIGN(string command, string id, string data, int block){
    int numSplay=0;
    int numComp=0;
    int numCompfunc=0;
    int numCompid=0;
    regex idformat("[a-z][a-zA-Z0-9_]*");
    regex numformat("[0-9]*");
    regex strformat("'[a-zA-Z0-9 ]*'");
    regex funcformat("\\(((number|string)(,(number|string))*)?\\)->(number|string)");
    regex funcdataformat("[a-z][a-zA-Z0-9_]*\\((([0-9]*|'[a-zA-Z0-9 ]*'|[a-z][a-zA-Z0-9_]*)(,([0-9]*|'[a-zA-Z0-9 ]*'|[a-z][a-zA-Z0-9_]*))*)?\\)");
    string datatype;
    node* here=nullptr;
    int block1=block;
    if (regex_match(data, numformat)==1){
        datatype="number";
    } else if (regex_match(data, strformat)==1){
        datatype="string";
    } else if (regex_match(data, idformat)==1){
        while (here==nullptr && block>=0){
            numCompid=0;
            node* temp=this->root;
            here=search(temp, data, block, numCompid);
            block--;
        }
        if (here!=nullptr){
            datatype=here->type;
        } else throw Undeclared(command);
    } else if (regex_match(data, funcdataformat)==1){
        int place01=data.find("(");
        string funcid=data.substr(0,place01);
        node* temp=this->root;
        while (temp!=nullptr){
            numCompfunc++;
            if (temp->id==funcid && temp->block==0){
                here=temp;
                break;
            }
            int cmp=funcid.compare(temp->id);
            if (cmp>=0){
                temp=temp->right;
            } else temp=temp->left;
        }
        if (here!=nullptr){
            splay(here, numSplay);
        }
        if (here==nullptr){
            throw Undeclared(command);
        } else if (regex_match(here->type, funcformat)==1){
            int count1=0;
            int count2=0;
            int datasize = data.size();
            for (int i=0;i<datasize;i++){
                if ((data[i])==','){
                    count1++;
                }
            }
            int typesize = here->type.size();
            for (int i=0;i<typesize;i++){
                if ((here->type[i])==','){
                    count2++;
                }
            }
            if (count1==count2){
                string fake;
                fake+='(';
                int pplace=place01;         
                int nplace=data.find(",");  
                for (int i=0; i<count1; i++){
                    string check=data.substr(pplace+1,nplace-pplace-1);
                    if (regex_match(check, numformat)==1){
                        fake+="number";
                    } else if (regex_match(check, strformat)==1){
                        fake+="string";
                    } else if (regex_match(check, idformat)==1){
                        node* ulatr;
                        node* temp=this->root;
                        ulatr=search(temp, check, block1, numCompid);
                        if (ulatr!=nullptr){
                            fake+=ulatr->type;
                            splay(ulatr, numSplay);
                        }
                    }
                    fake+=",";
                    pplace=nplace; 
                    nplace=data.find(",",pplace+1);
                }
                nplace=data.find(")");
                string check=data.substr(pplace+1,nplace-pplace-1);
                    if (check!=""){
                    if (regex_match(check, numformat)==1){
                        fake+="number";
                    } else if (regex_match(check, strformat)==1){
                        fake+="string";
                    } else if (regex_match(check, idformat)==1){
                        node* ulatr;
                        node* temp=this->root;
                        ulatr=search(temp, check, block1, numCompid);
                        if (ulatr!=nullptr){
                            fake+=ulatr->type;
                            splay(ulatr, numSplay);
                        }
                    }
                    }
                fake+=')';
                int abc=here->type.find(")");
                string check1=here->type.substr(0,abc+1);
                if (fake!=check1) throw TypeMismatch(command);
                else {
                    int abcd=here->type.find(">");
                    string check2=here->type.substr(abcd+1);
                    datatype=check2;
                }
            } else throw TypeMismatch(command);
        } else throw TypeMismatch(command);
    }
    here=nullptr;
    int block2=block1;
    while (here==nullptr && block2>=0){
        node* temp=this->root;
        here=search(temp, id, block2);
        block2--;
    }
    if (here==nullptr) throw Undeclared(command);
    else if (here->type!="number" && here->type!="string"){
        throw TypeMismatch(command);
    } else {
        here=nullptr;
        while(block1>=0){
            node* temp=this->root;
            numComp=0;
            while(temp!=nullptr){
            numComp++;
            int cmp=id.compare(temp->id);
            if (cmp>0){
            temp=temp->right;
            } else if (cmp<0) temp=temp->left;
            else {
                here=temp;
                break;
            }
            }
            if (here!=nullptr) break;
            block1--;
        }
    splay(here, numSplay);
    cout<<numComp+numCompfunc+numCompid<<" "<<numSplay<<endl;
    }
}

node* SymbolTable::search(node* temp, string id, int block){
    if (temp==nullptr) return temp;
    while (temp!=nullptr){
        if (temp->block==block){
            int cmp = id.compare(temp->id);
            if (cmp==0){
                return temp;
            } else if (cmp>0){
                temp=temp->right;
            } else temp=temp->left;
        } else if (block > temp->block){
            temp=temp->right;
        } else temp=temp->left;
    }
    return nullptr;
}

node* SymbolTable::search(node* temp, string id, int block, int& numCompa){
    if (temp==nullptr) return temp;
    while (temp!=nullptr){
        numCompa++;
        if (temp->block==block){
            int cmp = id.compare(temp->id);
            if (cmp==0){
                return temp;
            } else if (cmp>0){
                temp=temp->right;
            } else temp=temp->left;
        } else if (block > temp->block){
            temp=temp->right;
        } else temp=temp->left;
    }
    return nullptr;
}

void SymbolTable::BEGIN(){
}

void SymbolTable::END(int block, string& idstore){
    int numSplay;
    int deleteblock = block + 1;
    string cur;
    int pplace = 0;
    int nplace = idstore.find(",",1);
    int idstoresize=idstore.size();
    while (nplace<idstoresize){
    cur = idstore.substr(pplace+1,nplace-pplace-1);
    node* here=nullptr;
    node* temp=this->root;
    here= search(temp, cur, deleteblock);
    if (here!=nullptr){
    if (here->block==deleteblock){
        splay(here, numSplay);
        node* lefttree = here->left;
        node* righttree = here->right;
        if (lefttree==nullptr){
            this->root=righttree;
        } else {
            node* tmp=lefttree;
            while (tmp->right!=nullptr){
                tmp=tmp->right;
            }
            splay(tmp, numSplay);
            this->root=tmp;
            this->root->right=righttree;
            if (righttree!=nullptr){
                righttree->parent=this->root;
            }
        }
        delete here;
        idstore.erase(pplace+1,nplace-pplace);
        idstoresize=idstore.size();
    }
    }
    pplace=nplace;
    nplace=idstore.find(",",pplace+1);
    }
}

void SymbolTable::LOOKUP(string id, string command, int block){
    node* here=nullptr;
    int numSplay=0;
    while (here==nullptr && block>=0){
        node* temp=this->root;
        here = search(temp, id, block);
        block--;
    }
    if (here==nullptr){
        throw Undeclared(command);
    } else{
        splay(here, numSplay);
        cout<<this->root->block<<endl;
    }
}

void SymbolTable::PRINT(){
    string result;
    if (this->root==nullptr) return;
    node* temp=this->root;
    preOrder(temp, result);
    int resultsize=result.size();
    result.resize(resultsize-1);
    cout<<result<<endl;
    return;
}

void SymbolTable::preOrder(node* current, string& result){
    if (current==nullptr)   return;
    result+=current->id;
    result+="//";
    string num=to_string(current->block);
    result+=num;
    result+=" ";
    preOrder(current->left, result);
    preOrder(current->right, result);
}

void SymbolTable::run(string filename)
{
    string command, task, id, type, data, stat;
    int block = 0;
    string idstore=",";
    ifstream infile(filename);
    while (getline(infile, command)){
        check(command, task, id, type, stat, data, block);
        if (task=="INSERT") INSERT(command, id, type, stat, block, idstore);
        else if (task=="ASSIGN") ASSIGN(command, id, data, block);
        else if (task=="BEGIN") BEGIN();
        else if (task=="END") END(block, idstore);
        else if (task=="LOOKUP") LOOKUP(id, command, block);
        else if (task=="PRINT") PRINT();
    }
    if (block>0){
        throw UnclosedBlock(block);
    }
}
