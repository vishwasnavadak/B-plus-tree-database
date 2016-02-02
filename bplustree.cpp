/*
A simple In-memory B+ Tree database with two fields, one being the roll number which is indexed and other being the student name which is not indexed.
Supported Operations:
1. Insertion of new student record(Roll Number and Name).
2. Searching for the name given the roll number.
3. Deletion of records.
4. Display all records present in the database.
*/

#define MAX 150
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ctime>
using namespace std;

class node;
node *pos,*nonleafpos;

/* Record Class will define variables to store roll number and name of each of the student. */
struct record
{
  node *left;
  node *right;
  int key;
  char name[256];
};

/* Node class will define each node, which has pointers to other nodes and records based on size specified. */
class node
{
public:
  int noofkeys;
  record data[MAX];
  node *parent;
  node *first;
  node *next;
  int isleaf;

  node();

  /* insertinanode() will take a record as parameter and inserts it into the node */
  void insertinanode(record value);

  /* splitanode() is used to split nodes when it has no space left to insert more records */
  record splitanode(record value);

  /* splitaparent() is used to split parent nodes and link child nodes accordingly*/
  record splitaparent(record value);

  node *nextindex(int value);
  void display();
  void displayval();
};

node::node()
{
  int i;
  for(i=0;i<=MAX;i++){
    data[i].left=NULL;
    data[i].right=NULL;
  }
  noofkeys=0;
  parent=NULL;
  first=NULL;
  next=NULL;
  isleaf=1;
}

/*Queue is used to traverse through B+ tree node levels */
class Queue
{
  node *data[100000];
  int Rear,Front;
public:
  Queue()
  {
    Rear=Front=0;
  }
  int empty()
  {
    if(Rear==Front)
    return 1;
    else
    return 0;
  }
  node *deque()
  {
    return data[Front++];
  }
  void  enque(node *value)
  {
    data[Rear++]=value;
  }
  void makeempty()
  {
    Rear=Front=0;
  }
};

/* Bplus class will define the whole tree structure and includes required functionalities, pointers and number of keys present in the tree,
where mkeys is maximum number of keys in a node */
class bplus
{
  int mkeys;
  node *root;
  int keys[100000];
  int totalnoofkeys;
public:
  bplus(int n)
  {
    mkeys=n;
    root=NULL;
    totalnoofkeys=0;
  }

  /* Receives roll number and name from main functions and initiates insert operation.*/
  void insert(int value,char ch[256]);

  /* checkindex() function receives entered values from insert function and checks whether entered roll number is already exist in the database.*/
  bool checkindex(int value,int choice);

  /* displaytree() will display the tree structure in the way it is created.*/
  void displaytree();

  /* displaytreeval() will display all roll numbers and corresponding names one per each line.*/
  void displaytreeval();

  /* search function takes roll number and returns boolean values depending on whether it's present or not. If present its corresponding name will be displayed.*/
  bool search(int value,int choice);

  /* Delete() takes roll number from main function and deletes it from the B plus tree.*/
  void Delete(int value);
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

node *node::nextindex(int value)
{
  int i;
  if(this->data[0].left==NULL && this->data[0].right==NULL)
  return(this);
  else{
    for(i=0;i<noofkeys ;i++)
    {
      if(value <data[i].key)
      return data[i].left;
      else if(value==data[i].key)
      return data[i].right;
      else if(value>data[i].key&& value< data[i+1].key)
      return data[i].right;
    }
    return data[i-1].right;

  }

}

void node::insertinanode(record value)
{
  int i;
  for(i=noofkeys-1;i>=0 && data[i].key>value.key;i--)
  data[i+1]=data[i];
  data[i+1]=value;
  data[i+1].left=value.left;
  data[i+1].right=value.right;
  noofkeys++;
}

record node::splitanode(record value)
{
  node *newblock;
  record myrecord;
  int i,j,centre;
  centre=(noofkeys)/2;
  newblock=new node;

  /* Divide the leaf node in two parts(original and newblock)*/
  if(value.key>data[centre].key)
  {
    for(i=centre+1,j=0;i<=noofkeys;i++,j++)
    newblock->data[j]=data[i];
    newblock->noofkeys=noofkeys-centre-1;
    noofkeys=noofkeys-newblock->noofkeys;
    newblock->insertinanode(value);
    newblock->first=newblock;
    newblock->parent=parent;
    myrecord.key=newblock->data[0].key;
    myrecord.right=newblock;
    myrecord.left=this;
    this->next=newblock;
  }
  else
  {
    for(i=centre,j=0;i<noofkeys;i++,j++)
    newblock->data[j]=data[i];
    newblock->noofkeys=noofkeys-centre;
    noofkeys=noofkeys-newblock->noofkeys;
    insertinanode(value);
    newblock->parent=parent;
    myrecord.key=newblock->data[0].key;
    myrecord.right=newblock;
    myrecord.left=this;
    this->next=newblock;

  }
  return(myrecord);
}

record node::splitaparent(record value)
{
  node *newblock;
  record myrecord;
  int i,j,centre;
  centre=(noofkeys-2)/2;
  newblock=new node;

  /* Divide the parent node in two parts(original and newblock)*/
  if(value.key>data[centre].key)
  {

    for(i=centre+1,j=0;i<=noofkeys;i++,j++)
    newblock->data[j]=data[i];
    newblock->noofkeys=noofkeys-centre-1;
    noofkeys=noofkeys-newblock->noofkeys;
    newblock->insertinanode(value);
    newblock->first=newblock;
    newblock->parent=parent;
    this->data[this->noofkeys].right=newblock->data[0].left;
    newblock->data[1].left=newblock->data[0].right;
    myrecord.key=newblock->data[0].key;
    myrecord.right=newblock;
    myrecord.left=this;
    this->isleaf=0;
    newblock->isleaf=0;

    /*Deleting the first key from node newblock*/
    for(i=1;i<newblock->noofkeys;i++)
    newblock->data[i-1]=newblock->data[i];
    newblock->noofkeys--;

    /*setting the parent*/
    for(i=0;i<newblock->noofkeys;i++)
    {
      newblock->data[i].right->parent=newblock;
    }
  }
  else
  {
    for(i=centre,j=0;i<noofkeys;i++,j++)
    newblock->data[j]=data[i];
    newblock->noofkeys=noofkeys-centre;
    noofkeys=noofkeys-newblock->noofkeys;
    insertinanode(value);
    newblock->parent=parent;
    this->data[this->noofkeys].right=newblock->data[0].left;
    newblock->data[1].left=newblock->data[0].right;
    myrecord.key=newblock->data[0].key;
    myrecord.right=newblock;
    myrecord.left=this;

    /*  Delete the first key from node newblock */
    for(i=1;i<newblock->noofkeys;i++)
    newblock->data[i-1]=newblock->data[i];
    newblock->noofkeys--;

    /* setting the parent */
    for(i=0;i<newblock->noofkeys;i++)
    {
      newblock->data[i].right->parent=newblock;
    }
  }
  return(myrecord);
}

bool bplus::checkindex(int value,int choice)
{
  if(choice==3){
    for(int i=0;i<totalnoofkeys;i++)
    {
      if(keys[i]==value)
      return true;
    }
  }
  if(choice==1){
    for(int i=0;i<totalnoofkeys;i++)
    {
      if(keys[i]==value)
      return true;
    }
    keys[totalnoofkeys]=value;
    totalnoofkeys=totalnoofkeys+1;
  }

  return false;
}


bool bplus::search(int value,int choice)
{
  Queue q1,q2;
  node *present;
  int i;
  q1.enque(root);
  while(!q1.empty())
  {
    q2.makeempty();
    cout<<"\n";
    while(!q1.empty())
    {

      present=q1.deque();
      if(present->isleaf){

        for(i=0;i<present->noofkeys;i++){
          if(present->data[i].key==value )
          {
            if(choice!=3){
              cout<<"\n Student Details: ";
              cout<<"\n Roll number: "<<present->data[i].key<<"\n Name: "<<present->data[i].name<<endl;
            }
            pos=present;
            return true ;
          }
        }

      }
      if(!present->isleaf)
      {
        q2.enque(present->data[0].left);
        for(int i=0;i<present->noofkeys;i++){
          if(present->data[i].key==value)
          nonleafpos=present;
          q2.enque(present->data[i].right);
        }

      }
    }
    q1=q2;
  }
  return false;
}

void bplus::displaytree()
{
  Queue q1,q2;
  node *present;
  q1.enque(root);
  while(!q1.empty())
  {
    q2.makeempty();
    cout<<"\n";
    while(!q1.empty())
    {
      present=q1.deque();
      present->display();cout<<"  ";
      if(!present->isleaf)
      {
        q2.enque(present->data[0].left);
        for(int i=0;i<present->noofkeys;i++)
        q2.enque(present->data[i].right);

      }
    }
    q1=q2;
  }
}


void bplus::displaytreeval()
{
  Queue q1,q2;
  node *present;
  cout<<endl<<endl<<"The list of Student roll number and corresponding names are as follows"<<endl;
  q1.enque(root);
  while(!q1.empty())
  {
    q2.makeempty();
    cout<<"\n";
    while(!q1.empty())
    {

      present=q1.deque();
      if(present->isleaf){
        present->displayval();
      }
      if(!present->isleaf)
      {
        q2.enque(present->data[0].left);
        for(int i=0;i<present->noofkeys;i++)
        q2.enque(present->data[i].right);

      }
    }
    q1=q2;
  }
}


void bplus::insert(int value,char ch[256])
{
  record myrecord;
  node *present,*temp;

  /*check for duplicate entry*/
  if(checkindex(value,1))
  {
    cout<<"Duplicate key entered!Please reenter the students roll number and name\n";
    return;
  }

  myrecord.key=value;
  strcpy(myrecord.name,ch);
  myrecord.left=NULL;
  myrecord.right=NULL;
  if(root==NULL)
  {
    root = new node;
    root->insertinanode(myrecord);
  }
  else
  {
    present=root;
    while(!(present->isleaf))
    present=present->nextindex(value);
    if(present->noofkeys<mkeys)
    present->insertinanode(myrecord);

    else
    {
      if(present->isleaf){
        myrecord=present->splitanode(myrecord);
        present->isleaf=1;
      }
      else{
        myrecord=present->splitaparent(myrecord);
        present->isleaf=0;
      }

      while(1)
      {
        if(present==root)
        {
          temp=new node;
          temp->data[0]=myrecord;
          temp->first=root;
          temp->parent=NULL;
          temp->data[0].left=myrecord.left;
          temp->data[0].right=myrecord.right;
          temp->noofkeys=1;
          root=temp;
          root->isleaf=0;
          temp->first->parent=temp;
          temp->data[0].right->parent=temp;
          temp->data[0].left->parent=temp;
          return;

        }
        else
        {
          present=present->parent;

          if(present->data[0].left!=NULL)
          present->isleaf=0;

          if(present->noofkeys < mkeys)
          {
            present->insertinanode(myrecord);
            return;
          }
          else
          {
            if(present->isleaf){
              myrecord=present->splitanode(myrecord);
              present->isleaf=1;
            }
            else{
              myrecord=present->splitaparent(myrecord);
              present->isleaf=0;
            }

          }
        }

      }
    }

  }
}



void bplus::Delete(int value)
{
  node *left,*right;
  record *centre;
  node *p,*q;
  int i,j,centreindex;
  if(search(value,3)){
    p=pos;
  }

  for(i=0;p->data[i].key != value; i++)
  ;


  if(p->isleaf){
    for(i=i+1;i<p->noofkeys;i++)
    p->data[i-1]=p->data[i];
    p->noofkeys--;

    /* Deleting the value from the array keys[] which keeps track of all the keys entered.*/
    for(i=0;i<totalnoofkeys;i++)
    if(keys[i]==value){
      for(j=i;j<totalnoofkeys;j++)
      keys[j]=keys[j+1];
    }
    totalnoofkeys--;
  }


  while(p->isleaf)
  {

    if(p->noofkeys >= mkeys/2 )
    return;
    if(p==root )
    {
      if(p->noofkeys>0)
      return;
      else
      {
        root=p->data[0].left;
        return;
      }
    }


    q=p->parent;
    if(q->data[0].left==p || q->data[0].right==p)
    {
      left=q->data[0].left;
      right=q->data[0].right;
      centre=&(q->data[0]);
      centreindex=0;
    }
    else
    {
      for(i=0;i<q->noofkeys;i++)
      if(q->data[i].left==p || q->data[i].right==p)
      break;
      left=q->data[i].left;
      right=q->data[i].right;
      centre=&(q->data[i]);
      centreindex=i;
    }

    if(right->noofkeys >mkeys/2)
    {
      left->data[left->noofkeys].key=centre->key;
      left->noofkeys++;
      centre->key=right->data[0].key;
      for(i=1;i<right->noofkeys;i++)
      right->data[i-1]=right->data[i];
      right->noofkeys--;
      return;
    }

    /* merge left and right */
    else
    {
      left->data[left->noofkeys].key=centre->key;
      left->noofkeys++;
      for(j=0;j<right->noofkeys;j++)
      left->data[left->noofkeys+j]=right->data[j];
      left->noofkeys+=right->noofkeys;

      /*delete the record from the parent*/
      for(i=centreindex+1;i<q->noofkeys ;i++)
      q->data[i-1]=q->data[i];
      q->noofkeys--;
      p=q;
    }


  }

  q=nonleafpos;
  while(!q->isleaf){
    for(i=0;i<q->noofkeys;i++){
      if(q->data[i].key==value){
        for(i=i+1;i<q->noofkeys;i++)
        q->data[i-1]=q->data[i];
        q->noofkeys--;
        break;
      }

    }
    if(i==q->noofkeys)
    break;
  }

  p=q;

  while(!p->isleaf)
  {
    if(p->noofkeys >= mkeys/2 )
    return;
    if(p==root )
    {
      if(p->noofkeys>0)
      return;
      else
      {
        root=p->first;
        return;
      }
    }

    q=p->parent;
    if(q->data[0].left==p || q->data[0].right==p)
    {
      left=q->data[0].left;
      right=q->data[0].right;
      centre=&(q->data[0]);
      centreindex=0;
    }
    else
    {
      for(i=1;i<q->noofkeys;i++)
      if(q->data[i].right==p)
      break;
      left=q->data[i-1].left;
      right=q->data[i].right;
      centre=&(q->data[i]);
      centreindex=i;
    }

    /*case 1 : left has one extra key, move a key from left*/
    if(left->noofkeys > mkeys/2)
    {
      for(i=right->noofkeys-1;i>=0;i--)
      right->data[i+1]=right->data[i];
      right->noofkeys ++;
      right->data[0].key=centre->key;
      centre->key=left->data[left->noofkeys - 1].key;
      left->noofkeys--;
      return;
    }

    /* case 2 : right has one extra key, move a key from right*/
    else

    if(right->noofkeys >mkeys/2)
    {
      left->data[left->noofkeys].key=centre->key;
      left->noofkeys++;
      centre->key=right->data[0].key;
      for(i=1;i<right->noofkeys;i++)
      right->data[i-1]=right->data[i];
      right->noofkeys--;
      return;
    }

    /*merge left and right*/
    else
    {
      left->data[left->noofkeys].key=centre->key;
      left->noofkeys++;
      for(j=0;j<right->noofkeys;j++)
      left->data[left->noofkeys+j]=right->data[j];
      left->noofkeys+=right->noofkeys;

      /*delete the record from the parent*/
      for(i=centreindex+1;i<q->noofkeys ;i++)
      q->data[i-1]=q->data[i];
      q->noofkeys--;
      p=q;
    }

  }


}



int main(int argc, char **argv)
{
  int number,value,choice;
  clock_t begin_clock,end_clock;
  char name[256];
  ifstream input;

  /* Please enter large number of values per node if the input data is large. */
  cout<<"Enter number of values per node: "<<endl;
  cin>>number;
  if(cin.fail()){
    cout<<"Invalid Input"<<endl;
    return 0;
  }
  bplus b(number);
  if(argc>1){
    if(strcmp(argv[1],"-i")==0 && argc==4){
      value=atoi(argv[2]);
      b.insert(value,argv[3]);
      cout<<"Inserted successfully ";
    }
    else if(strcmp(argv[1],"-if")==0 && argc==3){
      input.open(argv[2]);
      begin_clock=clock();
      while(input>>value>>name){
        b.insert(value,name);
      }
      end_clock=clock();
      cout<<"Input from file is successfully inserted into database in time "<<(double)(end_clock-begin_clock)/CLOCKS_PER_SEC<<" seconds"<<endl;
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
    cout<<"1)Insert\n2)Search\n3)Delete\n4)Print\n5)Quit";
    cout<<"\nEnter your choice : ";
    cin>>choice;
    if(cin.fail()){
      cout<<"Invalid Input"<<endl;
      continue;
    }
    switch(choice)
    {
      case 1: cout<<"\nEnter Roll Number: ";
      cin >> value;
      if(cin.fail()){
        cout<<"Invalid Input"<<endl;
        continue;
      }
      cout<<"enter the name: ";
      cin>>name;
      b.insert(value,name);
      cout<<"\nTree after insertion : ";
      b.displaytree();
      break;
      case 2: cout<<"\nEnter Roll Number : ";
      cin>>value;
      if(cin.fail()){
        cout<<"Invalid Input"<<endl;
        continue;
      }
      if(!b.search(value,2))
      cout<<" Students Record number not found!";
      break;
      case 3: ;
      cout<<"\nEnter a data : ";
      cin>>value;
      if(cin.fail()){
        cout<<"Invalid Input"<<endl;
        continue;
      }
      if(b.checkindex(value,3)){
        b.Delete(value);
        b.displaytree();
      }
      else
      cout<<"Students Record number not found!";
      break;

      case 4: b.displaytree();
      b.displaytreeval();
      break;
    }

  }while(choice!=5);

  return 0;
}
