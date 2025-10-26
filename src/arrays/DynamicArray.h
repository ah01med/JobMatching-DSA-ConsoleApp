#pragma once
#include <cstddef>
#include <utility>


template<typename T>
class DynamicArray {
T* data_{};
std::size_t sz_{};
std::size_t cap_{};
public:
DynamicArray() = default;
~DynamicArray(){ delete[] data_; }


DynamicArray(const DynamicArray& o): data_(nullptr), sz_(0), cap_(0){
reserve(o.sz_);
for(std::size_t i=0;i<o.sz_;++i) push_back(o[i]);
}
DynamicArray& operator=(const DynamicArray& o){
if(this==&o) return *this;
clear(); reserve(o.sz_);
for(std::size_t i=0;i<o.sz_;++i) push_back(o[i]);
return *this;
}


void clear(){ delete[] data_; data_=nullptr; sz_=0; cap_=0; }
std::size_t size() const { return sz_; }
bool empty() const { return sz_==0; }


void reserve(std::size_t n){
if(n<=cap_) return;
std::size_t newcap = cap_? cap_*2: 8;
if(newcap<n) newcap=n;
T* nd = new T[newcap];
for(std::size_t i=0;i<sz_;++i) nd[i]=data_[i];
delete[] data_; data_=nd; cap_=newcap;
}
void push_back(const T& v){ if(sz_==cap_) reserve(cap_? cap_*2: 8); data_[sz_++]=v; }
void push_back(T&& v){ if(sz_==cap_) reserve(cap_? cap_*2: 8); data_[sz_++]=std::move(v); }


T& operator[](std::size_t i){ return data_[i]; }
const T& operator[](std::size_t i) const { return data_[i]; }


template<typename Compare>
void quicksort(Compare cmp){ if(sz_>1) qs(0,(int)sz_-1,cmp); }


private:
template<typename Compare>
void qs(int l, int r, Compare cmp){
int i=l, j=r; T p = data_[(l+r)/2];
while(i<=j){
while(cmp(data_[i],p)) ++i;
while(cmp(p,data_[j])) --j;
if(i<=j){ T t=data_[i]; data_[i]=data_[j]; data_[j]=t; ++i; --j; }
}
if(l<j) qs(l,j,cmp);
if(i<r) qs(i,r,cmp);
}
};