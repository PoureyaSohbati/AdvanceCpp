#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"

using bigvalue_t = vector<unsigned char>;

bigint::bigint (const string& that) {
  auto itor = that.cend()-1;
  negative = false;
   
  while (itor != that.cbegin()-1) {
    if (*itor == '_')
      negative = true;
    else
      big_value.push_back(*itor);
    --itor;
  }
  for (auto it = big_value.cend()-1; it != big_value.cbegin() - 1; --it){
    if(*it != '0')
      break; 
    big_value.pop_back();
  }
  if(big_value.size() == 0)
    negative = false;
}

bigvalue_t do_bigadd (const bigvalue_t& left, const bigvalue_t& right){
  bigvalue_t sum;
  int carry = 0;
  int num;

  auto l_itor = left.cbegin(); 
  auto r_itor = right.cbegin();
  for(;l_itor != left.cend() || r_itor != right.cend(); ++l_itor, ++r_itor){
    if(l_itor == left.cend()){
      num = (*r_itor - '0') + carry;
      --l_itor;
    }
    else if(r_itor == right.cend()){
      num = (*l_itor - '0') + carry;
      --r_itor;
    }
    else
      num = (*l_itor - '0') + (*r_itor - '0') + carry;
    carry = num / 10;
    num %= 10;
    sum.push_back(num + '0');
  }
  if(carry)
    sum.push_back(carry + '0');
  for (auto it = sum.cend() - 1; it != sum.cbegin() - 1; --it){
    if(*it != '0')
      break;
    sum.pop_back();
  } 
  return sum;
}

bigvalue_t do_bigsub (const bigvalue_t& left, const bigvalue_t& right){
   bigvalue_t sub;
   int borrow = 0;
   int num;

   auto l_itor = left.cbegin();
   auto r_itor = right.cbegin();
   for(;l_itor != left.cend() || r_itor != right.cend(); ++l_itor, ++r_itor){
       if(r_itor == right.cend()){
           num = (*l_itor - '0') + borrow;
           --r_itor;
       }
       else
           num = (*l_itor - '0') + borrow - (*r_itor - '0');
       
       borrow = 0;
       if (num < 0) {
           num += 10;
           borrow = -1;
       }
       sub.push_back(num + '0');
   }
   auto it = sub.cend() - 1;
   for (; it != sub.cbegin() - 1; --it){
      if(*it != '0')
         break;
      sub.pop_back();
   }

   return sub;
}

bool do_bigless (const bigvalue_t& left, const bigvalue_t& right){
   if(left.size() > right.size())
      return true;
   else if(left.size() == right.size()){
        auto l_itor = left.cend()-1;
        auto r_itor = right.cend()-1;
        for(;l_itor != left.cbegin()-1; --l_itor, --r_itor){
            if(*l_itor > *r_itor)
                return true;
            else if(*l_itor < *r_itor)
                return false;
       }
   }
   return false;
}


bigint operator+ (const bigint& left, const bigint& right) {
   string temp = "";
   bigvalue_t tempv;

   if(left.negative == right.negative) {
       tempv = do_bigadd(left.big_value, right.big_value); 
       if(left.negative)
           temp += '_';
   }
   else {
       if(do_bigless(left.big_value, right.big_value)){
           tempv = do_bigsub(left.big_value, right.big_value);
           if(left.negative)
              temp += '_';
       }  
       else {
           tempv = do_bigsub(right.big_value, left.big_value); 
           if(right.negative)
              temp += '_';
       }
   }
   for(auto itor = tempv.cend() - 1; itor != tempv.cbegin()-1; --itor)
       temp += *itor;
   return temp;
}

bigint operator- (const bigint& left, const bigint& right) {
   string temp = "";
   bigvalue_t tempv;

   if(left.negative != right.negative) {
       tempv = do_bigadd(left.big_value, right.big_value);
       if(left.negative)
           temp += '_';
   }
   else {
       if(do_bigless(left.big_value, right.big_value)){
           tempv = do_bigsub(left.big_value, right.big_value);
           if(left.negative)
              temp += '_';
       }
       else {
           tempv = do_bigsub(right.big_value, left.big_value);
           if(!right.negative)
              temp += '_';
       }
   }
   for(auto itor = tempv.cend() - 1; itor != tempv.cbegin()-1; --itor)
       temp += *itor;
   return temp;
}

bigint operator+ (const bigint& right) {
  bigint newValue = right;
  return newValue;
}

bigint operator- (const bigint& right) {
   bigint newValue = right;
   newValue.negative = !right.negative;
   return newValue;
}

//
// Multiplication algorithm.
//
 
bigvalue_t do_bigmul (const bigvalue_t& left, const bigvalue_t& right){
   bigvalue_t mul;
   int carry;
   int num;

   for(size_t i = 0; i < left.size() + right.size(); i++)
       mul.push_back('0');
   for(size_t i = 0; i < left.size(); i++){
       carry = 0;
       for(size_t j = 0; j < right.size(); j++){
           num = (mul[i+j] - '0') + ((left[i] - '0') * (right[j] - '0')) + carry;
           mul[i+j] = (num % 10) + '0';
           carry = num / 10;
       }
       mul[i+right.size()] = carry + '0';
   }
   return mul;
}

bigint operator* (const bigint& left, const bigint& right) {
   string temp = "";
   bigvalue_t tempv;

   tempv = do_bigmul(left.big_value, right.big_value);
   if (right.negative!= left.negative)
       temp += '_';
   for(auto itor = tempv.cend() - 1; itor != tempv.cbegin()-1; --itor)
       temp += *itor;
   return temp;
}

//
// Division algorithm.
//

void multiply_by_2 (bigint::bigvalue_t& big_value) {
   big_value = do_bigadd(big_value, big_value);
}

void divide_by_2 (bigint::bigvalue_t& big_value) {
   int carry = 0;
   int num;
   bigvalue_t temp; 

   for(auto itor = big_value.end()-1; itor != big_value.begin()-1; --itor){
        num = (*itor - '0') + carry;
        temp.insert(temp.begin(), (num / 2)+'0'); 
        carry = (num % 2) * 10;
   }
   for (auto it = temp.cend()-1; it != temp.cbegin() - 1; --it){
      if(*it != '0')
         break;
      temp.pop_back();
   } 
   big_value = temp;
}


bigint::quot_rem divide (const bigint& left, const bigint& right) {
   if (right.big_value.size() == 0) throw domain_error ("divide by 0");
   static bigvalue_t zero {};
   if (right.big_value.size() == 0) 
       throw domain_error ("bigint::divide");
   bigvalue_t divisor = right.big_value;
   bigvalue_t quotient {};
   bigvalue_t remainder = left.big_value;
   bigvalue_t power_of_2 {'1'};
   while (do_bigless(remainder, divisor)) {
      multiply_by_2 (divisor);
      multiply_by_2 (power_of_2);  
   }
   while (do_bigless (power_of_2, zero)) {
      if (not do_bigless (divisor, remainder)) {
         remainder = do_bigsub(remainder, divisor);
         quotient = do_bigadd(quotient, power_of_2);
      }
      divide_by_2 (divisor);
      divide_by_2 (power_of_2);
   }
   return {quotient, remainder};
}

bigint operator/ (const bigint& left, const bigint& right) {
   bigint temp;
   temp.big_value = divide (left, right).first;
   for (auto it = temp.big_value.cend()-1; it != temp.big_value.cbegin() - 1; --it){
      if(*it != '0')
         break;
      temp.big_value.pop_back();
   }
   if(left.negative != right.negative && temp.big_value.size() != 0)
       temp.negative = true;
   else
       temp.negative = false;
   return temp;
}

bigint operator% (const bigint& left, const bigint& right) {
   bigint temp;
   temp.big_value = divide (left, right).second;
   for (auto it = temp.big_value.end()-1; it != temp.big_value.begin() - 1; --it){
      if(*it != '0')
         break;
      temp.big_value.pop_back();
   }
   if(left.negative && temp.big_value.size() != 0)
       temp.negative = true;
   else
       temp.negative = false;
   return temp;
}

bool operator== (const bigint& left, const bigint& right) {
  if(left.negative != right.negative || right.big_value.size() != left.big_value.size())
     return false;
  auto l_itor = left.big_value.cbegin();
  auto r_itor = right.big_value.cbegin();
  for(; l_itor != left.big_value.cend(); ++l_itor, ++r_itor)
     if(*l_itor != *r_itor)
         return false;
  return true;
}

bool operator< (const bigint& left, const bigint& right) {
   if(left.negative == right.negative)
       return do_bigless(right.big_value, left.big_value);
   else if(left.negative)
       return true;
   else 
       return false;
}

ostream& operator<< (ostream& out, const bigint& that) {
   int counter = 0;

   if(that.negative){
       out << '_';
       ++counter;
   }
   if(that.big_value.size() == 0){
       out << '0';
       ++counter;
   }
   else {
       for(auto itor = that.big_value.cend() - 1; itor != that.big_value.cbegin() - 1; --itor){
           out << *itor;
           ++counter;
           if (counter % 69 == 0){
               out << "/" << endl;
           }
       }
   }
   return out;
}


bigint pow (const bigint& base, const bigint& exponent) {
   DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
   bigint zero {};
   if (base.big_value.size() == 0) return zero;
   bigint base_copy = base;
   bigint expt = exponent;
   bigint one {"1"};
   bigint result {"1"};
   if (expt < zero) {
      base_copy = one / base_copy;
      expt = -expt;
   }
   while (expt > zero) {
      if ((expt.big_value.at(0) - '0') & 1) { //odd
         result = result * base_copy;
         expt.big_value = do_bigsub(expt.big_value, one.big_value);
      }else { //even
         base_copy = base_copy * base_copy;
         divide_by_2(expt.big_value);
      }
   }
   DEBUGF ('^', "result = " << result);
   return result;
}
