// $Id: bigint.cpp,v 1.61 2014-06-26 17:06:06-07 - - $
//Bryan Ambriz
#include <cstdlib>
#include <exception>
#include <limits>
#include <stack>
#include <stdexcept>
#include<string.h>
using namespace std;

#include "bigint.h"
#include "debug.h"
// intialize an empty bigint
bigint::bigint (): negative (false), big_value (new bigvalue_t ()){
}
//bigint copy
bigint::bigint (const bigint &that): negative (false),
                     big_value (new bigvalue_t (*that.big_value)){
         this->negative = that.negative;
         }
         
bigint::bigint (long that): negative (false),big_value (new bigvalue_t()) {
   DEBUGF ('~', this << " -> " << big_value)
   // it is easier to iterate through a string since its a char array
   String LV = to_string(that);
   //bool isnegative = false;
   uint itor = 0;
   if( LV[0] == '_'){
        this->negative = true;
        ++itor;
   }
   // we iterate through thr string to find the instances of 0-9
//   we send error if random char
   for( unint j = LV.size()-1; j>=itor; --j){
        if( LV[j] <= '9' and LV[j] >= '0'){
            this->big_value.push_back(LV[j]);
        } else{
            cout << "Error cannot parse " << LV[j] << " into an int."
            exit(1); 
        }
        if(size == '0') break;
   }
}

bigint::bigint (const string& that): negative (false) {
   auto itor = that.cbegin();
   //bool isnegative = false;
   if (itor != that.cend() and *itor == '_') {
      this->negative = true;
      ++itor;
   }
   
    char currc;
   for (auto e = that.cend(); e != itor; --e) {
      currc = *e;
      this->big_value->push_back (currc);
   }
   currc = *itor;
   this->big_value->push_back (currc);
  // int newval = 0;
  // while (itor != that.end()) newval = newval * 10 + *itor++ - '0';
  // big_value = isnegative ? - newval : + newval;
   DEBUGF ('~', this << " -> " << big_value)
}

bigint &bigint::operator= (const bigint &that) {
   if (this == &that) return *this;
   
   delete this->big_value;
   this->negative = that.negative;
   this->big_value = new bigvalue_t (*that.big_value);
   return *this;
}

bigint operator+ (const bigint& left, const bigint& right) {
bigint bsum;
   if( left.negative == right.negative ) {
         bsum.big_value=  new bigvalue_t(do_bigadd( left->big_value, right->big_value));
         } else{ if(do_bigless(right->big_value, left->big_value)){ 
                     bsum.big_value= new bigvalue_t(do_bigsub( left->big_value, right->big_value));
                   } else {if(do_bigless(left->big_value, right->big_value)){
                               bsum.big_value= new bigvalue_t(do_bigsub(right->big_value, left-big_value));
                               }else {
                                    bsum.big_value->push_back('0');
                               }
                         }
            }
  // return left.big_value + right.big_value;
  return bsum;
}

bigint operator- (const bigint& left, const bigint& right) {
// same as operator+
  // return left.big_value - right.big_value;
  
  bigint bdif;
  if( left.negative != right.negative ) {
         bdif.big_value=  new bigvalue_t(do_bigadd( left->big_value, right->big_value));
         } else{ if(do_bigless(right->big_value, left->big_value)){ 
                     bdif.big_value= new bigvalue_t(do_sub( left->big_value, right->big_value));
                   } else { 
                           if(do_bigless(left->big_value, right->big_value)){
                         bdif.big_value=new bigvalue_t (do_sub(right->big_value, left->big_value));
                         } else {
                                bdif.big_value->push_back('0');
                         }
                         }
            }
            return bdif;
}

bigint operator+ (const bigint& right) {

bigint bsum;
   if( this.negative == right.negative ) {
         bsum.big_value=  new bigvalue_t(do_bigadd( this->big_value, right->big_value));
         } else{ if(do_bigless(right->big_value, this->big_value)){ 
                     bsum.big_value= new bigvalue_t (do_bigsub( this->big_value, right->big_value));
                   } else {if(do_bigless(this->big_value, right->big_value)){
                               bsum.big_value= new bigvalue_t(do_bigsub(right->big_value, this->big_value));
                               }else {
                                    bsum.big_value->push_back('0');
                               }
                         }
            }
            return bsum;
  // return +right.big_value;
}

bigint operator- (const bigint& right) {
  // return -right.big_value;
   bigint bdif;
  if( this.negative != right.negative ) {
         bdif.big_value=  new bigvalue_t(do_bigadd( this->big_value, right->big_value));
         } else{ if(do_bigless(right->big_value, this->big_value)){ 
                     bdif.big_value=new bigvalue_t (do_sub( this->big_value, right->big_value));
                   } else { 
                           if(do_bigless(this->big_value, right->big_value)){
                         bdif.big_value= new bigvalue_t(do_sub(right->big_value, this->big_value));
                         } else {
                                bdif.big_value->push_back('0');
                         }
                         }
            }
            return bdif;
}

long bigint::to_long() const {
   if (*this <= bigint (numeric_limits<long>::min())
    or *this > bigint (numeric_limits<long>::max()))
               throw range_error ("bigint__to_long: out of range");
  // return big_value;
  long result = 0.0;
   long curr, multiplier;
   unsigned char currc;
   auto itor = this->big_value->end ();
   for (int i = this->big_value->size () - 1; i >= 0; --i) {
      currc = *itor;
      curr = currc -'0';
      if (curr != 0) {
         multiplier = 1.0;
         for (int j = 1; j <= i; ++j) {
            multiplier *= 10.0;
         }
         curr *= multiplier;
      }
      result += curr;
      --itor;
   }
   if (this->negative == true) {
      result = (-1)*(result);
   }
   return result;
}

bool abs_less (const bigvalue_t& left, const bigvalue_t& right) {
   return left < right;
}

//adittion and subtraction
private bigvalue_t do_bigadd (const bigvalue_t& v1, const bigvalue_t& v2){
    int maxV1 = v1.big_value->size();
    int maxV2 = v2.big_value->size();
    int maxV3;
    if(maxV1 < maxV2) { maxV3 = maxV2;}
    else { maxV3 = maxV1;}
    int a, b, c;
    int carry = 0;
    unsigned char uc;
    bigint V3;
    if( !do_bigless(v1,v2)){
        v3.negative = v1.negative;
    }else if(do_bigless(v1,v2)){
             v3.negative = v2.negative;
         }else {
              if(v1.negative == v2.negative){
                  v3.negative = v1.negative;
              } else{
                      v3.negative = false;
              }
          }
    auto v1ito = v1.big_value->begin();
    auto v2ito = v2.big_value->begin();
    for(int 1 = 0; i< maxV3 ; ++i){
        if(i< maxV1){
            uc = *v1ito;
            a = uc - '0';
            ++v1ito;
        }else{ a = 0;}
        if (i < maxV2){
            uc = *v2ito;
            b = uc - '0';
            ++v2ito;
        }else { b = 0;}
        c = a+b+carry;
        if(c>9){
            carry = 1;
            c-= 10;
            uc = c+'0';
        }else {
            carry = 0;
            uc = c+'0';
        }
        v3.big_value->push_back(uc);
    }
    if(carry == 1){
        uc = carry + '0';
        v3.big_value->push_back(uc);
    }
    return v3.big_value;
}

private bigvalue_t do_bigsub (const bigvalue_t& one, const bigvalue_t& two){
     int maxsize_x = one.big_value->size ();
   int maxsize_y = two.big_value->size ();
   int maxsize;
   if (maxsize_x < maxsize_y) { maxsize = maxsize_y; }
   else { maxsize = maxsize_x; }
   int x, y, diff;
   int borrow = 0;
   unsigned char uc;
   bigint three;
   if( !do_bigless(one,two)){
        three.negative = one.negative;
    }else if(do_bigless(one,two)){
             three.negative = two.negative;
         }else {
              if(one.negative == two.negative){
                  three.negative = false;
              } else{
                      three.negative = true;
              }
          }
          
    auto thisitor = this->big_value->begin ();
    auto thatitor = that.big_value->begin ();
    for (int i = 0; i < maxsize; ++i) {
      if (i < maxsize_x) {
         uc = *thisitor;
         x = uc - '0';
         ++thisitor;
      }else { x = 0; }
      if (i < maxsize_y) {
         uc = *thatitor;
         y = uc - '0';
         ++thatitor;
      }else { y = 0; }
      x += borrow;
      if (x < y) { x += 10; borrow = -1; }
      else { borrow = 0; }
      diff = x - y;
      uc = diff + '0';
      three.big_value->push_back (currc);
  
   }
   while(three.big_value->back () == '0'){
        three.big_value->pop_back();
   }
   return three.big_value;
}
// cheacks which vecotr is bigger for subtraction
private boolean do_bigless( const bigvalue_t& v1, const bigvalue_t& v2){
     if(v1.negative == true and v2.negative == false) return false;
     if(v1.negative == false and v2.negative == true) return true;
     if(v1.size()> v2.size()){
        return true;
     }
     if(v1.size() < v2.size()) return false;
     
     
}


// add sub
//some auxilary methods to help with the complex arithmatic
int bigint::abscompare (const bigint &that) const {
   if (this->big_value->size () < that.big_value->size ()) {
      return -1;
   }else if (this->big_value->size () > that.big_value->size ()) {
      return 1;
   }else {

     auto thisitor = this->big_value->begin ();
     auto thatitor = that.big_value->begin ();
      unsigned char thisc, thatc;
      for (int i = this->big_value->size () - 1; i >= 0; --i) {
         thisc = *thisitor;
         thatc = *thatitor;
         if (thisc < thatc) {
            return -1;
         }else if (thisc > thatc) {
            return 1;
         }
         --thisitor;
         --thatitor;
      }
   }
   return 0;
}

static bigpair popstack (stack <bigpair> &egyptstack) {
   bigpair result = egyptstack.top ();
   egyptstack.pop();
   return result;
}
//
// Multiplication algorithm.
//
bigint operator* (const bigint& left, const bigint& right) {
  // return left.big_value * right.big_value;
 /* bigint bmul;
   if(left.negative == right.negative) bmul.negative = false;
    if(left.negative != right.negative) bmul.negative =true;  
    if(!do_bigless( left.big_value, right.big_value){
        bmul.big_value= new bigvalue_t(do_mul(left.big_value, right.big_value);
    } else{
          if(do_bigless(left.big_value,right.big_value)){
               bmul.big_value= new bigvalue_t(do_mul(left.big_value, right.big_value);   
          }else{
                  bmul.big_value= new bigvalue_t(do_mul(left.big_value, right.big_value); 
             }
      }
        return bmul;     */
        
        bigint big, small;
   if (right.abscompare (*left) > -1) {
      big = right;
      small = *left;
   }else {
      big = *left;
      small = right;
   }
   bigint count = (1);
   DEBUGF ('*', *left << " * " << that);
   stack <bigpair> egyptstack;
   
   while (count.abscompare (small) <= 0) {
      egyptstack.push (bigpair (count, big));
      count = count.mul_by_2 ();
      big = big.mul_by_2 ();
   }
   
   bigint result = 0;
   bigpair top;
   while (!egyptstack.empty ()) {
      top = popstack (egyptstack);
      count = top.first;
      big = top.second;
      if (count.abscompare (small) <= 0) {
         small = (small) - (count);
         result = (result) + (big);
      }
   }

   if (left->negative != right.negative) result = -(result);
   return result;
}
/*private bigvalue_t do_bigmul (const bigvalue_t& one, const bigvalue_t& two){
     bigint p;
     auto i = one.cend();
     auto j = two.cend();
     for(;i!= one.cbegin(); --i){
        int c = 0;
        for(;j!=two.cbegin(); --j){
            int u = *i-'0';
            int v = *j-'0';
            int d = i*j+c;
            
        }
     }
}*/
//
// Division algorithm.
//

bigint bigint::mul_by_2 () {
   bigint double_this = (*this);
   double_this.big_value = do_bigadd(double_this.big_value, *this.big_value);
   return double_this;
}

friend void divide_by_2 (bigint::unumber& unumber_value) {
   unumber_value /= 2;
}


friend bigint::quot_rem divide (const bigint& left, const bigint& right) {
   if (right == 0) throw domain_error ("divide by 0");
  // using unumber = unsigned long;
  // static unumber zero = 0;
   if (right == 0) throw domain_error ("bigint::divide");
  /* unumber divisor = right.big_value;
   unumber quotient = 0;
   unumber remainder = left.big_value;
   unumber power_of_2 = 1;
   while (abs_less (divisor, remainder)) {
      multiply_by_2 (divisor);
      multiply_by_2 (power_of_2);
   }
   while (abs_less (zero, power_of_2)) {
      if (not abs_less (remainder, divisor)) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divide_by_2 (divisor);
      divide_by_2 (power_of_2);
   }
   return {quotient, remainder};*/
   bigint numer = *left;
   bigint denom = right;
   bigint count = (1);
   // ('/', *this << " /% " << that);
   stack <bigpair> egyptstack;
   
   while (denom.abscompare (numer) <= 0) {
      egyptstack.push (bigpair (count, denom));
      count = count.mul_by_2 ();
      denom = denom.mul_by_2 ();
   }
   
   bigint quotient = 0;
   bigint remainder = numer;
   while (!egyptstack.empty ()) {
      bigpair top = popstack (egyptstack);
      count = top.first;
      denom = top.second;
      if (denom.abscompare (remainder) <= 0) {
         remainder = remainder - denom;
         quotient = (quotient) + count;
      }
   }
   
   return  {quotient, remainder};
}

bigint operator/ (const bigint& left, const bigint& right) {
   return divide (left, right).first;
}

bigint operator% (const bigint& left, const bigint& right) {
   return divide (left, right).second;
   
}

bool operator== (const bigint& left, const bigint& right) {
  // return left.big_value == right.big_value;
   //if(do_bigless(left, right) return false;
   //if(do_bigless(right,left) return false;
   //for loop to compare and iterate both) return true;
   if(left.negative != right.negative) return false;
   if(do_bigless(left.big_value,right.big_value) return false;
   if(do_bigless(right.big_value, left.big_value) return false;
   auto thisitor = left.big_value->begin ();
   auto thatitor = right.big_value->begin ();
   unsigned char thisc, thatc;
   for (int i = left.big_value->size () - 1; i >= 0; --i) {
            thisc = *thisitor;
            thatc = *thatitor;
            if (thisc != thatc) {
               return false;
            }
            ++thisitor;
            ++thatitor;
         }
         return true;
}

bool operator< (const bigint& left, const bigint& right) {
   return do_bigless(left.big_value , right.big_value);
}

ostream& operator<< (ostream& out, const bigint& that) {
  // out << that.big_value;
  // return out;
   if (that.negative == true) {
      out << "-";
   }
   int count = 0;
   unsigned char currc;
   auto itor = that.big_value->end();
   for (int i = that.big_value->size () - 1; i >= 0; --i) {
      ++count;
      if (count == 70) {
         out << "\\" << endl;
         count = 1;
      }
      currc = *itor;
      out << currc;
      --itor;
   }
   return out;
}


bigint pow (const bigint& base, const bigint& exponent) {
   DEBUGF ('^', "base = " << base << ", exponent = " << exponent);
   if (base == 0) return 0;
   bigint base_copy = base;
   long expt = exponent.to_long();
   bigint result = 1;
   if (expt < 0) {
      base_copy = 1 / base_copy;
      expt = - expt;
   }
   while (expt > 0) {
      if (expt & 1) { //odd
         result = result * base_copy;
         --expt;
      }else { //even
         base_copy = base_copy * base_copy;
         expt /= 2;
      }
   }
   DEBUGF ('^', "result = " << result);
   return result;
}