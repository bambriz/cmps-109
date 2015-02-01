// $Id: listmap.tcc,v 1.5 2014-07-09 11:50:34-07 - - $

#include "listmap.h"
#include "trace.h"


template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (const value_type& pair):
pair(pair), prev(NULL), next(NULL) {
}

//template <typename Key, typename Value, class Less>
//listmap<Key,Value,Less>::listmap (): head(NULL), tail (NULL) {
//}

template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap () {
   TRACE ('l', (void*) this);
}

// 
// listmap::empty()
//
template <typename Key, typename Value, class Less>
bool listmap<Key,Value,Less>::empty() const {
   return head == NULL;
}


//new Iterators

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::begin () {
   return iterator (this, head);
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::end () {
   return iterator (this, NULL);
}  


//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
//typename listmap<Key,Value,Less>::iterator
void listmap<Key,Value,Less>::insert (const xpair<Key,Value>& pair) {
   TRACE ('l', &pair << "->" << pair);
   //return iterator();
   Less less;
   node* temp = new node(pair);
  // node temp = node(pair);
 //  temp->pair = pair;
   //assert(temp!=NULL);
   temp->next = NULL;
   temp->prev = NULL;
   auto itor = begin();
   if(head == NULL){
       head=temp;
      tail=temp;
      ++itor;
      delete temp;
      return;
   }
   node* t = head;
   while(t!=NULL){
       if(!less (temp->pair.first, t->pair.first)){
             if(temp->pair.first == t->pair.first){
                  t->pair.second = temp->pair.second;
                  break;
             }
             temp->prev = t->prev;
             temp->next = t;
             break;
       } else{
               if(t->next == tail){
                   temp->next = tail;
                   //anchor()->prev = temp;
                   t->next = temp;
                   break;
               }
         }
        t=t->next;
        ++itor;        
   }
   delete temp;
   delete t;
   return ;
   
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) const {
   TRACE ('l', that);
   //return iterator();
    look = head;
   while (look != NULL && look->pair.first.compare(that) != 0){
     
      look = look->next;
   }

   return iterator(this,look);
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position) {
   TRACE ('l', &*position);
   return iterator();
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//


template <typename Key, typename Value, class Less>
xpair<Key,Value>& listmap<Key,Value,Less>::iterator::operator* () {
   TRACE ('l', where->pair);
   return where->pair;
}

template <typename Key, typename Value, class Less>
xpair<Key,Value> *listmap<Key,Value,Less>::iterator::operator-> () {
   TRACE ('l', where->pair);
   return &(where->pair);
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++ () {
   TRACE ('l', "First: " << map << ", " << where);
   TRACE ('l', "Second: " << map->head << ", " << map->tail);
   where = where->next;
   return *this;
}

template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator-- () {
   where = where->prev;
   return *this;
}

template <typename Key, typename Value, class Less>
bool listmap<Key,Value,Less>::iterator::operator==
(const iterator& that) const {
   return this->where == that.where;
}


//
// bool listmap::iterator::operator!= (const iterator&)
//
//template <typename Key, typename Value, class Less>
//inline bool listmap<Key,Value,Less>::iterator::operator!=
  //          (const iterator& that) const {
   //return this->where != that.where;
//}
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::iterator::iterator (listmap *map,
node *where): map (map), where (where){
}