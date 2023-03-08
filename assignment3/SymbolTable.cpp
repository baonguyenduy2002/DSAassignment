#include "SymbolTable.h"

void SymbolTable::check(string& command, string& task, string& id, string& type, int& num_parameter, string& data, int& scope, string& funcid, int& m, int& c1, int& c2){
    regex idformat("[a-z][a-zA-Z0-9_]*");
    regex numformat("[0-9]*");
    regex strformat("'[a-zA-Z0-9 ]*'");
    regex funcdataformat("[a-z][a-zA-Z0-9_]*\\((([0-9]*|'[a-zA-Z0-9 ]*'|[a-z][a-zA-Z0-9_]*)(,([0-9]*|'[a-zA-Z0-9 ]*'|[a-z][a-zA-Z0-9_]*))*)?\\)");

    if (command=="PRINT"){
        task = command;
        return;
    }
    if (command=="BEGIN"){
        task = command;
        scope++;
        return;
    }
    if (command=="END"){
        task = command;
        if (scope==0) {throw UnknownBlock();} else scope--;
        return;
    }

    int place1=command.find(" ");
    task=command.substr(0,place1);
    if (task=="LINEAR"){
        string mstring, cstring;
        int place2 = command.find(" ",place1+1);
        mstring=command.substr(place1+1, place2-place1-1);
        int place3 = command.find(" ", place2+1);
        cstring=command.substr(place2+1, place3-place2-1);
        if (regex_match(mstring, numformat)==0 || regex_match(cstring, numformat)==0){
            throw InvalidInstruction(command);
        }
        m = stoi(mstring);
        c1 = stoi(cstring);
        return;
    }
    if (task=="QUADRATIC"){
        string mstring, c1string, c2string;
        int place2 = command.find(" ",place1+1);
        mstring=command.substr(place1+1, place2-place1-1);
        int place3 = command.find(" ", place2+1);
        c1string=command.substr(place2+1, place3-place2-1);
        int place4 = command.find(" ", place3+1);
        c2string=command.substr(place3+1, place4-place3-1);
        if (regex_match(mstring, numformat)==0 || regex_match(c1string, numformat)==0 || regex_match(c2string, numformat)==0){
            throw InvalidInstruction(command);
        }
        m = stoi(mstring);
        c1 = stoi(c1string);
        c2 = stoi(c2string);
        return;
    }
    if (task=="DOUBLE"){
        string mstring, cstring;
        int place2 = command.find(" ",place1+1);
        mstring=command.substr(place1+1, place2-place1-1);
        int place3 = command.find(" ", place2+1);
        cstring=command.substr(place2+1, place3-place2-1);
        if (regex_match(mstring, numformat)==0 || regex_match(cstring, numformat)==0){
            throw InvalidInstruction(command);
        }
        m = stoi(mstring);
        c1 = stoi(cstring);
        return;
    }
    if (task=="INSERT"){
        string parastring;
        int place2=command.find(" ",place1+1);
        id=command.substr(place1+1,place2-place1-1);
        if (place2 != -1){
            parastring = command.substr(place2+1);
            if (regex_match(parastring, numformat)==0){
                throw InvalidInstruction(command);
            }
            num_parameter = stoi(parastring);
        } else num_parameter = -1;
        if (regex_match(id, idformat)==0){
            throw InvalidInstruction(command);
        }
        if (id=="number"||id=="string"||id=="void") throw InvalidInstruction(command);
        return;
    }
    if (task=="ASSIGN"){
        int place2=command.find(" ",place1+1);
        id=command.substr(place1+1,place2-place1-1);
        data=command.substr(place2+1);
        if (regex_match(id, idformat)==0 || ((regex_match(data, numformat)==0 && regex_match(data, strformat)==0 && regex_match(data, idformat)==0) && regex_match(data, funcdataformat)==0)){
            throw InvalidInstruction(command);
        }
        if (data=="number"||data=="string"||id=="void") throw InvalidInstruction(command);
        return;
    }
    if (task=="LOOKUP"){
        id=command.substr(place1+1);
        if (regex_match(id, idformat)==0){
            throw InvalidInstruction(command);
        }
        return;
    }
    if (task=="CALL"){
        funcid=command.substr(place1+1);
        if (regex_match(funcid,funcdataformat)==0){
            throw InvalidInstruction(command);
        }
    }
}

//<><><><><><><><><><><><> support function <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
int SymbolTable::linear_insert(string command, int& i_slotcount, long long code, int m, int c1, int scope, string id){
    while (hash[(code%m+c1*i_slotcount)%m].id!=""){
        if (hash[(code%m+c1*i_slotcount)%m].code == code && hash[(code%m+c1*i_slotcount)%m].scope == scope) {throw Redeclared(id);}
        i_slotcount++;              //slot count
        if (i_slotcount==m) {throw Overflow(command);}
    }

    return (code%m+c1*i_slotcount)%m;
}

int SymbolTable::quadratic_insert(string command, int& i_slotcount, long long code, int m, int c1, int c2, int scope, string id){
    while (hash[(code%m+c1*i_slotcount+c2*i_slotcount*i_slotcount)%m].id!=""){
        if (hash[(code%m+c1*i_slotcount+c2*i_slotcount*i_slotcount)%m].code == code && hash[(code%m+c1*i_slotcount+c2*i_slotcount*i_slotcount)%m].scope == scope) {throw Redeclared(id);}
        i_slotcount++;          //slot count
        if (i_slotcount==m) {throw Overflow(command);}
    }

    return (code%m+c1*i_slotcount+c2*i_slotcount*i_slotcount)%m;
}

int SymbolTable::doublehash_insert(string command, int& i_slotcount, long long code, int m, int c1, int scope, string id){
    while (hash[(code%m + c1*i_slotcount*(1+(code%(m-2))))%m].id!=""){
        if (hash[(code%m + c1*i_slotcount*(1+(code%(m-2))))%m].code == code && hash[(code%m + c1*i_slotcount*(1+(code%(m-2))))%m].scope == scope) {throw Redeclared(id);}
        i_slotcount++;          //slot count
        if (i_slotcount==m) {throw Overflow(command);}
    }

    return (code%m + c1*i_slotcount*(1+(code%(m-2))))%m;
}

int SymbolTable::linear_assign(string command, int& i_slotcount, string id, int m, int c1, int scope){
    long long code;
    code = scope;
    int len = id.size();
    for (int i=0; i<len; i++){
        if (id[i]-48>=10){
            code = code*100 + (id[i]-48);
        } else {
            code = code*10 + (id[i]-48);
        }
    }

    int i=0;

    while (scope>=0){
        i=0;
        while (hash[(code%m+c1*i)%m].id!=id || hash[(code%m+c1*i)%m].scope!=scope){
            i++;          //slot count
            if (i==m) break;
        }
        i_slotcount=i;
        if (hash[(code%m+c1*i_slotcount)%m].id==id && hash[(code%m+c1*i_slotcount)%m].scope==scope) break;
        scope--;
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
    if (scope<0) {throw Undeclared(id);}

    return (code%m+c1*i_slotcount)%m;
}

int SymbolTable::quadratic_assign(string command, int& i_slotcount, string id, int m, int c1, int c2, int scope){
    long long code;
    code = scope;
    int len = id.size();
    for (int i=0; i<len; i++){
        if (id[i]-48>=10){
            code = code*100 + (id[i]-48);
        } else {
            code = code*10 + (id[i]-48);
        }
    }

    int i=0;

    while (scope>=0){
        i=0;
        while (hash[(code%m+c1*i+c2*i*i)%m].id!=id || hash[(code%m+c1*i+c2*i*i)%m].scope!=scope){
            i++;          //slot count
            if (i==m) break;
        }
        i_slotcount=i;
        if (hash[(code%m+c1*i_slotcount+c2*i_slotcount*i_slotcount)%m].id==id && hash[(code%m+c1*i_slotcount+c2*i_slotcount*i_slotcount)%m].scope==scope) break;
        scope--;
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
    if (scope<0) {throw Undeclared(id);}

    return (code%m+c1*i_slotcount+c2*i_slotcount*i_slotcount)%m;
}

int SymbolTable::doublehash_assign(string command, int& i_slotcount, string id, int m, int c1, int scope){
    long long code;
    code = scope;
    int len = id.size();
    for (int i=0; i<len; i++){
        if (id[i]-48>=10){
            code = code*100 + (id[i]-48);
        } else {
            code = code*10 + (id[i]-48);
        }
    }

    int i=0;

    while (scope>=0){
        i=0;
        while (hash[(code%m + c1*i*(1+(code%(m-2))))%m].id!=id || hash[(code%m + c1*i*(1+(code%(m-2))))%m].scope!=scope){
            i++;          //slot count
            if (i==m) break;
        }
        i_slotcount=i;
        if (hash[(code%m + c1*i_slotcount*(1+(code%(m-2))))%m].id==id && hash[(code%m + c1*i_slotcount*(1+(code%(m-2))))%m].scope==scope) break;
        scope--;
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
    if (scope<0) {throw Undeclared(id);}

    return (code%m + c1*i_slotcount*(1+(code%(m-2))))%m;
}


//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>

void SymbolTable::LINEAR(int c1, int m, string command){
    if ((hashfunc=="LINEAR")||(hashfunc=="QUADRATIC")||(hashfunc=="DOUBLE")){
        throw InvalidInstruction(command);
    }
    if (m<=0) throw InvalidInstruction(command);
    this->m = m;
    hash = new Node[m];
    hashfunc = "LINEAR";
    this->c1 = c1;
}

void SymbolTable::QUADRATIC(int c1, int c2, int m, string command){
    if ((hashfunc=="LINEAR")||(hashfunc=="QUADRATIC")||(hashfunc=="DOUBLE")){
        throw InvalidInstruction(command);
    }
    if (m<=0) throw InvalidInstruction(command);
    this->m = m;
    hash = new Node[m];
    hashfunc = "QUADRATIC";
    this->c1 = c1;
    this->c2 = c2;
}

void SymbolTable::DOUBLE(int c1, int m, string command){
    if ((hashfunc=="LINEAR")||(hashfunc=="QUADRATIC")||(hashfunc=="DOUBLE")){
        throw InvalidInstruction(command);
    }
    if (m<=0) throw InvalidInstruction(command);
    this->m = m;
    hash = new Node[m];
    hashfunc = "DOUBLE";
    this->c1 = c1;
}

void SymbolTable::INSERT(string command, string id, string type, int scope, int num_parameter){
    if (!((hashfunc=="LINEAR")||(hashfunc=="QUADRATIC")||(hashfunc=="DOUBLE"))){
        throw InvalidInstruction(command);
    }

    Node* newnode = new Node(id, type, scope, num_parameter);
    int i_slotcount=0;
    int index;

    if (num_parameter>=0 && scope!=0){
        delete newnode;
        throw InvalidDeclaration(id);
    }

    if (m==0) throw Overflow(command);

    if (hashfunc=="LINEAR"){
        index = linear_insert(command, i_slotcount, newnode->code, this->m, this->c1, scope, id);
    } else if (hashfunc=="QUADRATIC"){
        index = quadratic_insert(command, i_slotcount, newnode->code, this->m, this->c1, this->c2, scope, id);
    } else if (hashfunc=="DOUBLE"){
        index = doublehash_insert(command, i_slotcount, newnode->code, this->m, this->c1, scope, id);
    }

    hash[index].id = newnode->id;
    hash[index].type = newnode->type;
    hash[index].scope = newnode->scope;
    hash[index].num_parameter = newnode->num_parameter;
    hash[index].code = newnode->code;

    delete newnode;
    cout<<i_slotcount<<endl;
}

void SymbolTable::ASSIGN(string command, string id, string data, int scope){
    if (!((hashfunc=="LINEAR")||(hashfunc=="QUADRATIC")||(hashfunc=="DOUBLE"))){
        throw InvalidInstruction(command);
    }

    regex idformat("[a-z][a-zA-Z0-9_]*");
    regex numformat("[0-9]*");
    regex strformat("'[a-zA-Z0-9 ]*'");
    regex funcdataformat("[a-z][a-zA-Z0-9_]*\\((([0-9]*|'[a-zA-Z0-9 ]*'|[a-z][a-zA-Z0-9_]*)(,([0-9]*|'[a-zA-Z0-9 ]*'|[a-z][a-zA-Z0-9_]*))*)?\\)");
    
    int i_slotcount=0;
    int index1;
    int index;

    string datatype;
    if (regex_match(data, numformat)==1){
        datatype="number";
    } else if (regex_match(data, strformat)==1){
        datatype="string";
    } else if (regex_match(data, idformat)==1){                                   //gan id
        if (hashfunc=="LINEAR") index1=linear_assign(command, i_slotcount, data, this->m, this->c1, scope);
        else if (hashfunc=="QUADRATIC") index1=quadratic_assign(command, i_slotcount, data, this->m, this->c1, this->c2, scope);
        else if (hashfunc=="DOUBLE") index1=doublehash_assign(command, i_slotcount, data, this->m, this->c1, scope);
        if (hash[index1].num_parameter>=0) {throw TypeMismatch(command);}

        if (hashfunc=="LINEAR"){
            index = linear_assign(command, i_slotcount, id, this->m, this->c1, scope);
        } else if (hashfunc=="QUADRATIC"){
            index = quadratic_assign(command, i_slotcount, id, this->m, this->c1, this->c2, scope);
        } else if (hashfunc=="DOUBLE"){
            index = doublehash_assign(command, i_slotcount, id, this->m, this->c1, scope);
        }
        if (hash[index].num_parameter>=0) {throw TypeMismatch(command);}

        datatype="iddata";
    } else if (regex_match(data, funcdataformat)==1){                                   //gan function
        int place1 = data.find("(");
        string funcdata = data.substr(0, place1);
        if (hashfunc=="LINEAR") index1=linear_assign(command, i_slotcount, funcdata, this->m, this->c1, scope);
        else if (hashfunc=="QUADRATIC") index1=quadratic_assign(command, i_slotcount, funcdata, this->m, this->c1, this->c2, scope);
        else if (hashfunc=="DOUBLE") index1=doublehash_assign(command, i_slotcount, funcdata, this->m, this->c1, scope);
        datatype="funcdata";
        if (hash[index1].num_parameter<0) {throw TypeMismatch(command);}
        if (hash[index1].type=="void") {
            throw TypeMismatch(command);
        }

        string datafuncinput;

        int place2 = data.find("(");
        int place = data.find(",", place2+1);
        int paracount=0;
        string add;
        datafuncinput+="(";             //input data func
        while (place!=-1){
            paracount++;
            add = data.substr(place2+1, place-place2-1);
            if (regex_match(add, numformat)==1){
                datafuncinput+="number";
            } else if (regex_match(add, strformat)==1){
                datafuncinput+="string";
            } else if (regex_match(add, idformat)==1){
                int index3;
                if (hashfunc=="LINEAR") index3=linear_assign(command, i_slotcount, add, this->m, this->c1, scope);
                else if (hashfunc=="QUADRATIC") index3=quadratic_assign(command, i_slotcount, add, this->m, this->c1, this->c2, scope);
                else if (hashfunc=="DOUBLE") index3=doublehash_assign(command, i_slotcount, add, this->m, this->c1, scope);
                if (hash[index3].num_parameter>=0) {throw TypeMismatch(command);}
                if (hash[index3].type==""){
                    if (hash[index1].data==""){
                        throw TypeCannotBeInferred(command);
                    } else datafuncinput+="unknown";
                } else datafuncinput+=hash[index3].type;
            }
            datafuncinput+=",";
            place2 = place;
            place = data.find(",", place2+1);
        }
        place = data.find(")");
        add = data.substr(place2+1,place-place2-1);
        if (regex_match(add, numformat)==1){
                datafuncinput+="number";
        } else if (regex_match(add, strformat)==1){
            datafuncinput+="string";
        } else if (regex_match(add, idformat)==1){
            int index3;
            if (hashfunc=="LINEAR") index3=linear_assign(command, i_slotcount, add, this->m, this->c1, scope);
            else if (hashfunc=="QUADRATIC") index3=quadratic_assign(command, i_slotcount, add, this->m, this->c1, this->c2, scope);
            else if (hashfunc=="DOUBLE") index3=doublehash_assign(command, i_slotcount, add, this->m, this->c1, scope);
            if (hash[index3].num_parameter>=0) {throw TypeMismatch(command);}
            if (hash[index3].type==""){
                if (hash[index1].data==""){
                    throw TypeCannotBeInferred(command);
                } else datafuncinput+="unknown";
            } else datafuncinput+=hash[index3].type;
        }
        if (add!="") paracount++;
        datafuncinput+=")";

        if (hash[index1].data==""){
            int checkunknown = datafuncinput.find("unknown");
            if (checkunknown!=-1) {throw TypeCannotBeInferred(command);}
            if (hash[index1].num_parameter!=paracount) {throw TypeMismatch(command);}
            hash[index1].data=datafuncinput;
        } else {
            if (hash[index1].data!=datafuncinput){
                int infer1 = hash[index1].data.find("(");
                int infer2 = datafuncinput.find("(");
                int inferunknown = data.find("(");
                int infer1go = hash[index1].data.find(",", infer1+1);
                int infer2go = datafuncinput.find(",",infer2+1);
                int inferunknowngo = data.find(",", inferunknown+1);

                string add1, add2, addunknown;
                while (infer1go!=-1 || infer2go!= -1){
                    add1 = hash[index1].data.substr(infer1+1, infer1go-infer1-1);
                    add2 = datafuncinput.substr(infer2+1, infer2go-infer2-1);
                    addunknown = data.substr(inferunknown+1, inferunknowngo-inferunknown-1);
                    // if (add1=="unknown"&&add2=="unknown") {throw TypeCannotBeInferred(command);}
                    if (add1=="string"&&add2=="number") {throw TypeMismatch(command);}
                    if (add1=="number"&&add2=="string") {throw TypeMismatch(command);}
                    if (add2=="unknown") {
                        int indexunknown;
                        int ia=0;
                        if (hashfunc=="LINEAR") indexunknown = linear_assign(command, ia, addunknown, this->m, this->c1, scope);
                        else if (hashfunc=="QUADRATIC") indexunknown = quadratic_assign(command, ia, addunknown, this->m, this->c1, this->c2, scope);
                        else if (hashfunc=="DOUBLE") indexunknown = doublehash_assign(command, ia, addunknown, this->m, this->c1, scope);
                        if (hash[indexunknown].type=="") hash[indexunknown].type = add1;
                        else {
                            if (add1 != hash[indexunknown].type) throw TypeMismatch(command);
                        }
                    }

                    infer1 = infer1go;
                    infer2 = infer2go;
                    inferunknown = inferunknowngo;
                    infer1go = hash[index1].data.find(",", infer1+1);
                    infer2go = datafuncinput.find(",",infer2+1);
                    inferunknowngo = data.find(",", inferunknown+1);
                }
                infer1go = hash[index1].data.find(")");
                infer2go = datafuncinput.find(")");
                inferunknowngo = data.find(")");
                add1 = hash[index1].data.substr(infer1+1, infer1go-infer1-1);
                add2 = datafuncinput.substr(infer2+1, infer2go-infer2-1);
                addunknown = data.substr(inferunknown+1, inferunknowngo-inferunknown-1);
                // if (add1=="unknown"&&add2=="unknown") {throw TypeCannotBeInferred(command);}
                if (add1=="string"&&add2=="number") {throw TypeMismatch(command);}
                if (add1=="number"&&add2=="string") {throw TypeMismatch(command);}
                if (add2=="unknown") {
                    int indexunknown;
                    int ia=0;
                        if (hashfunc=="LINEAR") indexunknown = linear_assign(command, ia, addunknown, this->m, this->c1, scope);
                        else if (hashfunc=="QUADRATIC") indexunknown = quadratic_assign(command, ia, addunknown, this->m, this->c1, this->c2, scope);
                        else if (hashfunc=="DOUBLE") indexunknown = doublehash_assign(command, ia, addunknown, this->m, this->c1, scope);
                        if (hash[indexunknown].type=="") hash[indexunknown].type = add1;
                        else {
                            if (add1 != hash[indexunknown].type) throw TypeMismatch(command);
                        }
                }
            }
        }
    }

    if (hashfunc=="LINEAR"){
        index = linear_assign(command, i_slotcount, id, this->m, this->c1, scope);
    } else if (hashfunc=="QUADRATIC"){
        index = quadratic_assign(command, i_slotcount, id, this->m, this->c1, this->c2, scope);
    } else if (hashfunc=="DOUBLE"){
        index = doublehash_assign(command, i_slotcount, id, this->m, this->c1, scope);
    }
    if (hash[index].num_parameter>=0) {throw TypeMismatch(command);}

    if (datatype=="iddata" || datatype=="funcdata"){
        datatype = hash[index1].type;
    }

    if (datatype=="void"){
        
        throw TypeMismatch(command);
    }

    if (hash[index].type=="" && datatype==""){
        
        throw TypeCannotBeInferred(command);
    }
    if (hash[index].type=="" && datatype!=""){
        hash[index].type = datatype;
        cout<<i_slotcount<<endl;
        return;
    }
    if (hash[index].type!="" && datatype==""){
        hash[index1].type = hash[index].type;
        cout<<i_slotcount<<endl;
        return;
    }
    if (hash[index].type!="" && datatype!=""){
        if (datatype!=hash[index].type) {throw TypeMismatch(command);}
        cout<<i_slotcount<<endl;
        return;
    }
}

void SymbolTable::BEGIN(string command){
    if (!((hashfunc=="LINEAR")||(hashfunc=="QUADRATIC")||(hashfunc=="DOUBLE"))){
        throw InvalidInstruction(command);
    }
}

void SymbolTable::END(int scope, string command){
    if (!((hashfunc=="LINEAR")||(hashfunc=="QUADRATIC")||(hashfunc=="DOUBLE"))){
        throw InvalidInstruction(command);
    }

    int scopeclear = scope+1;
    for (int i=0; i<m; i++){
        if (hash[i].scope==scopeclear){
            hash[i].id = "";
            hash[i].type="";
            hash[i].data="";
            hash[i].code=-1;
            hash[i].scope=-1;
            hash[i].num_parameter=-1;
        }
    }
}

void SymbolTable::LOOKUP(string id, string command, int scope){
    if (!((hashfunc=="LINEAR")||(hashfunc=="QUADRATIC")||(hashfunc=="DOUBLE"))){
        throw InvalidInstruction(command);
    }

    int scopefind = scope;
    while (scopefind>=0){
        for (int i=0; i<m; i++){
            if (hash[i].id==id && hash[i].scope==scopefind){
                cout<<i<<endl;
                return;
            }
        }
        scopefind--;
    }
    if (scopefind==-1) {throw Undeclared(id);}
}

void SymbolTable::PRINT(string command){
    if (!((hashfunc=="LINEAR")||(hashfunc=="QUADRATIC")||(hashfunc=="DOUBLE"))){
        throw InvalidInstruction(command);
    }

    int flag=0;
    for (int i=0; i<m; i++){
        if (hash[i].id!=""){
            flag=1;
            break;
        }
    }
    if (flag==0) return;
    string print;
    for (int i=0; i<m; i++){
        if (hash[i].id!=""){
            print+=to_string(i);
            print+=" ";
            print+=hash[i].id;
            print+="//";
            print+=to_string(hash[i].scope);
            print+=";";
        }
    }
    print.pop_back();
    cout<<print<<endl;
}

void SymbolTable::CALL(string command, string data, int scope){
    if (!((hashfunc=="LINEAR")||(hashfunc=="QUADRATIC")||(hashfunc=="DOUBLE"))){
        throw InvalidInstruction(command);
    }

    regex idformat("[a-z][a-zA-Z0-9_]*");
    regex numformat("[0-9]*");
    regex strformat("'[a-zA-Z0-9 ]*'");
    regex funcdataformat("[a-z][a-zA-Z0-9_]*\\((([0-9]*|'[a-zA-Z0-9 ]*'|[a-z][a-zA-Z0-9_]*)(,([0-9]*|'[a-zA-Z0-9 ]*'|[a-z][a-zA-Z0-9_]*))*)?\\)");

    int i_slotcount=0;
        int index1;
        int place1 = data.find("(");
        string funcdata = data.substr(0, place1);
        if (hashfunc=="LINEAR") index1=linear_assign(command, i_slotcount, funcdata, this->m, this->c1, scope);
        else if (hashfunc=="QUADRATIC") index1=quadratic_assign(command, i_slotcount, funcdata, this->m, this->c1, this->c2, scope);
        else if (hashfunc=="DOUBLE") index1=doublehash_assign(command, i_slotcount, funcdata, this->m, this->c1, scope);
        if (hash[index1].num_parameter<0) {throw TypeMismatch(command);}
        if (hash[index1].type!="" && hash[index1].type!="void") {throw TypeMismatch(command);}

        string datafuncinput;

        int place2 = data.find("(");
        int place = data.find(",", place2+1);
        int paracount=0;
        string add;
        datafuncinput+="(";             //input data func
        while (place!=-1){
            paracount++;
            add = data.substr(place2+1, place-place2-1);
            if (regex_match(add, numformat)==1){
                datafuncinput+="number";
            } else if (regex_match(add, strformat)==1){
                datafuncinput+="string";
            } else if (regex_match(add, idformat)==1){
                int index3;
                if (hashfunc=="LINEAR") index3=linear_assign(command, i_slotcount, add, this->m, this->c1, scope);
                else if (hashfunc=="QUADRATIC") index3=quadratic_assign(command, i_slotcount, add, this->m, this->c1, this->c2, scope);
                else if (hashfunc=="DOUBLE") index3=doublehash_assign(command, i_slotcount, add, this->m, this->c1, scope);
                if (hash[index3].num_parameter>=0) {throw TypeMismatch(command);}
                if (hash[index3].type==""){
                    if (hash[index1].data==""){
                        throw TypeCannotBeInferred(command);
                    } else datafuncinput+="unknown";
                } else datafuncinput+=hash[index3].type;
            }
            datafuncinput+=",";
            place2 = place;
            place = data.find(",", place2+1);
        }
        place = data.find(")");
        add = data.substr(place2+1,place-place2-1);
        if (regex_match(add, numformat)==1){
                datafuncinput+="number";
        } else if (regex_match(add, strformat)==1){
            datafuncinput+="string";
        } else if (regex_match(add, idformat)==1){
            int index3;
            if (hashfunc=="LINEAR") index3=linear_assign(command, i_slotcount, add, this->m, this->c1, scope);
            else if (hashfunc=="QUADRATIC") index3=quadratic_assign(command, i_slotcount, add, this->m, this->c1, this->c2, scope);
            else if (hashfunc=="DOUBLE") index3=doublehash_assign(command, i_slotcount, add, this->m, this->c1, scope);
            if (hash[index3].num_parameter>=0) {throw TypeMismatch(command);}
            if (hash[index3].type==""){
                if (hash[index1].data==""){
                        throw TypeCannotBeInferred(command);
                    } else datafuncinput+="unknown";
            } else datafuncinput+=hash[index3].type;
        }
        if (add!="") paracount++;
        datafuncinput+=")";
        if (hash[index1].data==""){
            int checkunknown = datafuncinput.find("unknown");
            if (checkunknown!=-1) {throw TypeCannotBeInferred(command);}
            if (hash[index1].num_parameter!=paracount) {throw TypeMismatch(command);}
            hash[index1].data=datafuncinput;
        } else {
            if (hash[index1].data!=datafuncinput){
                int infer1 = hash[index1].data.find("(");
                int infer2 = datafuncinput.find("(");
                int inferunknown = data.find("(");
                int infer1go = hash[index1].data.find(",", infer1+1);
                int infer2go = datafuncinput.find(",",infer2+1);
                int inferunknowngo = data.find(",", inferunknown+1);

                string add1, add2, addunknown;
                while (infer1go!=-1 || infer2go!= -1){
                    add1 = hash[index1].data.substr(infer1+1, infer1go-infer1-1);
                    add2 = datafuncinput.substr(infer2+1, infer2go-infer2-1);
                    addunknown = data.substr(inferunknown+1, inferunknowngo-inferunknown-1);
                    // if (add1=="unknown"&&add2=="unknown") {throw TypeCannotBeInferred(command);}
                    if (add1=="string"&&add2=="number") {throw TypeMismatch(command);}
                    if (add1=="number"&&add2=="string") {throw TypeMismatch(command);}
                    if (add2=="unknown") {
                        int indexunknown;
                        int ia=0;
                        if (hashfunc=="LINEAR") indexunknown = linear_assign(command, ia, addunknown, this->m, this->c1, scope);
                        else if (hashfunc=="QUADRATIC") indexunknown = quadratic_assign(command, ia, addunknown, this->m, this->c1, this->c2, scope);
                        else if (hashfunc=="DOUBLE") indexunknown = doublehash_assign(command, ia, addunknown, this->m, this->c1, scope);
                        if (hash[indexunknown].type=="") hash[indexunknown].type = add1;
                        else {
                            if (add1 != hash[indexunknown].type) throw TypeMismatch(command);
                        }
                    }

                    infer1 = infer1go;
                    infer2 = infer2go;
                    inferunknown = inferunknowngo;
                    infer1go = hash[index1].data.find(",", infer1+1);
                    infer2go = datafuncinput.find(",",infer2+1);
                    inferunknowngo = data.find(",", inferunknown+1);
                }
                infer1go = hash[index1].data.find(")");
                infer2go = datafuncinput.find(")");
                inferunknowngo = data.find(")");
                add1 = hash[index1].data.substr(infer1+1, infer1go-infer1-1);
                add2 = datafuncinput.substr(infer2+1, infer2go-infer2-1);
                addunknown = data.substr(inferunknown+1, inferunknowngo-inferunknown-1);
                // if (add1=="unknown"&&add2=="unknown") {throw TypeCannotBeInferred(command);}
                if (add1=="string"&&add2=="number") {throw TypeMismatch(command);}
                if (add1=="number"&&add2=="string") {throw TypeMismatch(command);}
                if (add2=="unknown") {
                    int indexunknown;
                    int ia=0;
                        if (hashfunc=="LINEAR") indexunknown = linear_assign(command, ia, addunknown, this->m, this->c1, scope);
                        else if (hashfunc=="QUADRATIC") indexunknown = quadratic_assign(command, ia, addunknown, this->m, this->c1, this->c2, scope);
                        else if (hashfunc=="DOUBLE") indexunknown = doublehash_assign(command, ia, addunknown, this->m, this->c1, scope);
                        if (hash[indexunknown].type=="") hash[indexunknown].type = add1;
                        else {
                            if (add1 != hash[indexunknown].type) throw TypeMismatch(command);
                        }
                }
            }
        }

    if (hash[index1].num_parameter<0) {cout<<"7:";throw TypeMismatch(command);}
    if (hash[index1].type!="" && hash[index1].type!="void") {throw TypeMismatch(command);}
    hash[index1].type="void";
    cout<<i_slotcount<<endl;
}

void SymbolTable::run(string filename)
{
    string command, task, id, type, data, funcid;
    int scope = 0;
    int m,c1,c2;
    int num_parameter;
    ifstream infile(filename);
    while (getline(infile, command)){
        check(command, task, id, type, num_parameter, data, scope, funcid, m, c1, c2);
        if (task=="INSERT") INSERT(command, id, type, scope, num_parameter);
        else if (task=="ASSIGN") ASSIGN(command, id, data, scope);
        else if (task=="BEGIN") BEGIN(command);
        else if (task=="END") END(scope, command);
        else if (task=="LOOKUP") LOOKUP(id, command, scope);
        else if (task=="PRINT") PRINT(command);
        else if (task=="CALL") CALL(command, funcid, scope);
        else if (task=="LINEAR") LINEAR(c1, m, command);
        else if (task=="QUADRATIC") QUADRATIC(c1, c2, m, command);
        else if (task=="DOUBLE") DOUBLE(c1, m, command);
    }
    if (scope>0){
        throw UnclosedBlock(scope);
    }
}