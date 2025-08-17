#include <string>
#include "class.h"
 
people::people(){
    this->name = "None";
    this->years = 0;
    this->id = 0;
}
people::people(std::string name,int years): name(name),years(years){
    setID();
}
std::string people::get_name() const {
    return this->name;
}
int people::get_years() const{
    return this->years;
}
int people::get_id() const{
    return this->id;
}
void people::set_years(int new_years){
    this->years = new_years;
}
people::~people(){}
void people::setID(){
    this->id = years + this->name[0];
}