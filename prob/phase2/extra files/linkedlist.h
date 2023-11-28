#include <iostream>
#include<vector>
using namespace std;
class List{
struct Node {
   vector<int> data;
   struct Node *prev;
   struct Node *next;
};
Node* head = NULL;
Node* tail=NULL;

void insert(vector<int> newdata) {
   struct Node* newnode = (struct Node*) malloc(sizeof(struct Node));
   newnode->data = newdata;
   Node* prev=head;
   Node* current=head->next;
   while(current!=tail)
   {
    if(current->data[0]>newnode->data[0])//put condition first for corner cases like where we must insert right after head
    break;
    prev=current;
    current=current->next;
   }
   prev->next=newnode;
   newnode->prev=prev;
   newnode->next=current;
   current->prev=newnode;

}
void display() {
   struct Node* ptr;
   ptr = head;
   while(ptr != NULL) {
    ptr = ptr->next;
    cout<< ptr->data[0] <<" ";

   }
}


};