/*
A simple In-memory B+ Tree database with two fields, one being the roll number which is indexed and other being the student name which is not indexed.
Supported Operations:
1. Insertion of new student record(Roll Number and Name).
2. Deletion of records.
3. Searching for the name given the roll number.
4. Display all records present in the database.
*/
#define MAX 5
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>

using namespace std;
class node;
/* Record Class will define variables to store roll number and name of each of the student. */
struct record
{
    node *next;
    int key;
    char name[256];
};
/* Node class will define each node, which has pointers to other nodes and records based on size specified. */
class node
{
public:
    int noofkeys;
    record data[MAX];
    node *father;
    node *first;
    node();
    int leafnode();
    /* insertinanode() will take a record as parameter and inserts it into the node */
    void insertinanode(record x);
    /* splitanode() is used to split nodes when it has no space left to insert more records */
    record splitanode(record x);
    /* splitaparent() is used to split parent nodes and link child nodes accordingly*/
    record splitaparent(record x);
    node *nextindex(int x);
    void display();
    void displayval();
};

void node::display()
{
    int i;
    cout<<"(";
    for(i=0;i<noofkeys;i++)
    cout<<data[i].key<<" ";
    cout<<")";
}
void node::displayval()
{
    int i;
    for(i=0;i<noofkeys;i++)
    cout<<data[i].key<<" -> "<<data[i].name<<endl;
}

node *node::nextindex(int x)
{
    int i;
    if(x<data[0].key)
    return(first);
    for(i=0;i<noofkeys ;i++)
    if(x <= data[i].key)
    return data[i-1].next;
    return data[i-1].next;
}


int node::leafnode()
{
    if(data[0].next==NULL)
    return 1;
    return 0;
}

void node::insertinanode(record x)
{
    int i;
    for(i=noofkeys-1;i>=0 && data[i].key>x.key;i--)
    data[i+1]=data[i];
    data[i+1]=x;
    noofkeys++;
}

record node::splitanode(record x)
{
    node *T;
    record myrecord;
    int i,j,centre;
    centre=(noofkeys)/2;
    //cout<<"\ncentre= "<<centre;
    T=new node;
    if(x.key>data[centre].key)  //Divide the node in two parts(original and T)
    {
        for(i=centre+1,j=0;i<=noofkeys;i++,j++)
        T->data[j]=data[i];
        T->noofkeys=noofkeys-centre-1;
        noofkeys=noofkeys-T->noofkeys;
        T->insertinanode(x);
        T->first=T->data[0].next;
        T->father=father;
        myrecord.key=T->data[0].key;
        myrecord.next=T;

    }
    else
    {
        for(i=centre,j=0;i<noofkeys;i++,j++)
        T->data[j]=data[i];
        T->noofkeys=noofkeys-centre;
        noofkeys=noofkeys-T->noofkeys;
        insertinanode(x);
        T->father=father;
        myrecord.key=T->data[0].key;
        myrecord.next=T;

    }
    return(myrecord);
}

record node::splitaparent(record x)
{
    node *T;
    record myrecord;
    int i,j,centre;
    centre=(noofkeys-2)/2;
    T=new node;
    if(x.key>data[centre].key)  //Divide the node in two parts(original and T)
    {
        for(i=centre+1,j=0;i<=noofkeys;i++,j++)
        T->data[j]=data[i];
        T->noofkeys=noofkeys-centre-1;
        noofkeys=noofkeys-T->noofkeys;
        T->insertinanode(x);
        T->first=T->data[0].next;
        T->father=father;
        myrecord.key=T->data[0].key;
        myrecord.next=T;
        //Delete the first key from node T
        for(i=1;i<T->noofkeys;i++)
        T->data[i-1]=T->data[i];
        T->noofkeys--;
    }
    else
    {
        for(i=centre,j=0;i<noofkeys;i++,j++)
        T->data[j]=data[i];
        T->noofkeys=noofkeys-centre;
        noofkeys=noofkeys-T->noofkeys;
        insertinanode(x);
        T->father=father;
        myrecord.key=T->data[0].key;
        myrecord.next=T;
        //   Delete the first key from node T
        for(i=1;i<T->noofkeys;i++)
        T->data[i-1]=T->data[i];
        T->noofkeys--;
    }
    return(myrecord);
}


node::node()
{
    for(int i=0;i<=MAX;i++)
    data[i].next=NULL;
    noofkeys=0;
    father=NULL;
    first=NULL;
}
/*Queue is used to traverse through B+ tree node levels */
class Q
{
    node *data[256];
    int R,F;
public:
    Q()
    {
        R=F=0;
    }
    int empty()
    {
        if(R==F)
        return 1;
        else
        return 0;
    }
    node *deque()
    {
        return data[F++];
    }
    void  enque(node *x)
    {
        data[R++]=x;
    }
    void makeempty()
    {
        R=F=0;
    }
};

class bplus
{
    int mkeys; //maximum number of keys in a node
    node *root;
    int *keys;
    int noofkeys;
public:
    bplus(int n)
    {
        mkeys=n;
        root=NULL;
        noofkeys=0;
    }
    void insert(int x,char ch[256]);
    bool checkindex(int x);
    void displaytree();
    void displaytreeval();
    bool search(int x);
    void Delete(int x);
};

bool bplus::checkindex(int x)
{

    for(int i=0;i<=noofkeys;i++)
    {
        if(keys[i]==x)
        return true;
    }
    keys[noofkeys]=x;
    noofkeys=noofkeys+1;
    return false;
}

bool bplus::search(int x)
{
    Q q1,q2;
    node *p;
    int i;
    q1.enque(root);
    while(!q1.empty())
    {
        q2.makeempty();
        cout<<"\n";
        while(!q1.empty())
        {

            p=q1.deque();
            if(p->leafnode()){
                for(i=0;i<(noofkeys+(noofkeys/2));i++)
                if(p->data[i].key==x && p->data[i].next==NULL)
                {
                    cout<<"\n Student Details: ";
                    cout<<"\n Roll number: "<<p->data[i].key<<"\n Name: "<<p->data[i].name<<endl;
                    return true ;
                }
            }
            if(!p->leafnode())
            {
                q2.enque(p->first);
                for(int i=0;i<p->noofkeys;i++)
                q2.enque(p->data[i].next);

            }
        }
        q1=q2;
    }
    return false;
}
void bplus::displaytree()
{
    Q q1,q2;
    node *p;
    q1.enque(root);
    while(!q1.empty())
    {
        q2.makeempty();
        cout<<"\n";
        while(!q1.empty())
        {
            p=q1.deque();
            p->display();cout<<"  ";
            if(!p->leafnode())
            {
                q2.enque(p->first);
                for(int i=0;i<p->noofkeys;i++)
                q2.enque(p->data[i].next);

            }
        }
        q1=q2;
    }
}

void bplus::displaytreeval()
{
    Q q1,q2;
    node *p;
    cout<<endl<<endl<<"The list of Student roll number and corresponding names are as follows"<<endl;
    q1.enque(root);
    while(!q1.empty())
    {
        q2.makeempty();
        cout<<"\n";
        while(!q1.empty())
        {

            p=q1.deque();
            if(p->leafnode()){
                p->displayval();
            }
            if(!p->leafnode())
            {
                q2.enque(p->first);
                for(int i=0;i<p->noofkeys;i++)
                q2.enque(p->data[i].next);

            }
        }
        q1=q2;
    }
}

void bplus::insert(int x,char ch[256])
{
    record myrecord;
    node *p,*q;
    if(checkindex(x))
    {
        cout<<"Duplicate key entered!Please reenter the students roll number and name\n";
        return;
    }
    myrecord.key=x;
    strcpy(myrecord.name,ch);
    myrecord.next=NULL;
    if(root==NULL)
    {
        root = new node;
        root->insertinanode(myrecord);
    }
    else
    {
        p=root;
        while(!(p->leafnode()))
        p=p->nextindex(x);
        if(p->noofkeys<mkeys)
        p->insertinanode(myrecord);

        else
        {
            if(p->leafnode())
            myrecord=p->splitanode(myrecord);
            else
            myrecord=p->splitaparent(myrecord);
            while(1)
            {
                if(p==root)
                {
                    q=new node;
                    q->data[0]=myrecord;
                    q->first=root;
                    q->father=NULL;
                    q->noofkeys=1;
                    root=q;
                    q->first->father=q;
                    q->data[0].next->father=q;
                    return;

                }
                else
                {
                    p=p->father;
                    if(p->noofkeys < mkeys)
                    {
                        p->insertinanode(myrecord);
                        return;
                    }
                    else
                    {
                        if(p->leafnode())
                        myrecord=p->splitanode(myrecord);
                        else
                        myrecord=p->splitaparent(myrecord);
                    }
                }

            }
        }

    }
}
int main(int argc, char **argv)
{
    int n,x,op;
    char ch[256];
    ifstream input;
    cout<<"Enter number of values per node: "<<endl;
    cin>>n;
    bplus b(n);
    if(argc>1){
        if(strcmp(argv[1],"-i")==0 && argc==4){
            x=atoi(argv[2]);
            b.insert(x,argv[3]);
            cout<<"Inserted successfully ";
        }
        else if(strcmp(argv[1],"-if")==0 && argc==3){
            input.open(argv[2]);
            while(input>>x>>ch){
                b.insert(x,ch);
            }
            cout<<"Input from file is successfully inserted into database";
        }
        else if(strcmp(argv[1],"-h")==0){
            cout<<"Command Line Options"<<endl;
            cout<<"-i <roll_number> <name> : for inserting one record through command line"<<endl;
            cout<<"-if <filename>.txt      : for inserting records from the data present in file"<<endl;
            cout<<"-h                      : for displaying this help text"<<endl;
        }
        else{
            cout<<"Invalid Option, use bplus -h for listing all options";
        }
    }

    do {
        cin.clear();
        cin.ignore();
        cout<<endl<<"Run Time Options: "<<endl;
        cout<<"1)Insert\n2)Search\n4)Print\n5)Quit";
        cout<<"\nEnter your choice : ";
        cin>>op;
        if(cin.fail()){
            cout<<"Invalid Input"<<endl;
            continue;
        }

        switch(op)
        {
            case 1: cout<<"\nEnter Roll Number: ";
            cin >> x;
            if(cin.fail()){
                cout<<"Invalid Input"<<endl;
                continue;
            }
            cout<<"enter the name: ";
            cin>>ch;
            b.insert(x,ch);
            cout<<"\nTree after insertion : ";
            b.displaytree();
            break;
            case 2: cout<<"\nEnter Roll Number : ";
            cin>>x;
            if(cin.fail()){
                cout<<"Invalid Input"<<endl;
                continue;
            }
            if(!b.search(x))
            cout<<" Students Roll number not found!";
            break;
            case 4: b.displaytree();
            b.displaytreeval();
            break;
        }

    }while(op!=5);
    return 0;
}
