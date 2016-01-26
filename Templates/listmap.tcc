// $Id: listmap.tcc,v 1.7 2015-04-28 19:22:02-07 - - $
// Partner: Poureya Sohbati (psohbati@ucsc.edu)
// Partner: Kaveh Daryabeygi (kdaryabe@ucsc.edu)

#include "listmap.h"
#include "trace.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap::node.
/////////////////////////////////////////////////////////////////
//

//
// listmap::node::node (link*, link*, const value_type&)
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::node::node (node* next, node* prev,
                                     const value_type& value):
            link (next, prev), value (value) {
}

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

//
// listmap::~listmap()
//
template <typename Key, typename Value, class Less>
listmap<Key,Value,Less>::~listmap() {
   node* curr = anchor();
   node* walker;
   curr = curr->next;
   walker = curr;
   while(curr != anchor()){
       walker = walker->next;
       delete curr;
       curr = walker;
   } 
   TRACE ('l', (void*) this);
}


//
// iterator listmap::insert (const value_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::insert (const value_type& pair) {
   node* curr = anchor();
   node* nn; 
  
   iterator it = find(pair.first);
   if(it != curr)  
       it->second = pair.second;
   else if(curr == curr->next){
       nn = new node(curr, curr, pair);
       curr->next = nn;
       curr->prev = nn;
   }
   else{
       curr = curr->next;
       while(curr != anchor() and less(curr->value.first, pair.first))
            curr = curr->next;
       nn = new node(curr, curr->prev,pair);
       curr->prev->next = nn;
       curr->prev = nn;
   }
   TRACE ('l', &pair << "->" << pair);
   return nn;
}

//
// listmap::find(const key_type&)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::find (const key_type& that) {
   TRACE ('l', that); 
   node* curr = anchor();
   curr = curr->next;
   while(curr != anchor() and less(curr->value.first, that))
        curr = curr->next;
   if(curr != anchor() and not less(curr->value.first, that) 
      and not less(that, curr->value.first))
        return curr;
   return anchor();
}

//
// iterator listmap::erase (iterator position)
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator
listmap<Key,Value,Less>::erase (iterator position){
   TRACE ('l', &*position);
   node* curr = position.where;
   curr->next->prev = curr->prev;
   curr = curr->next;
   delete curr->prev->next;
   curr->prev->next = curr;
   return curr;
}


//
/////////////////////////////////////////////////////////////////
// Operations on listmap::iterator.
/////////////////////////////////////////////////////////////////
//

//
// listmap::value_type& listmap::iterator::operator*()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type&
listmap<Key,Value,Less>::iterator::operator*() {
   TRACE ('l', where);
   return where->value;
}

//
// listmap::value_type* listmap::iterator::operator->()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::value_type*
listmap<Key,Value,Less>::iterator::operator->() {
   TRACE ('l', where);
   return &(where->value);
}

//
// listmap::iterator& listmap::iterator::operator++()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator++() {
   TRACE ('l', where);
   where = where->next;
   return *this;
}

//
// listmap::iterator& listmap::iterator::operator--()
//
template <typename Key, typename Value, class Less>
typename listmap<Key,Value,Less>::iterator&
listmap<Key,Value,Less>::iterator::operator--() {
   TRACE ('l', where);
   where = where->prev;
   return *this;
}


//
// bool listmap::iterator::operator== (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator==
            (const iterator& that) const {
   return this->where == that.where;
}

//
// bool listmap::iterator::operator!= (const iterator&)
//
template <typename Key, typename Value, class Less>
inline bool listmap<Key,Value,Less>::iterator::operator!=
            (const iterator& that) const {
   return this->where != that.where;
}

