/*
 * CS106L Assignment 5: TreeBook
 * Created by Fabio Ibanez with modifications by Jacob Roberts-Baca.
 */

#include <iostream>
#include <string>

class User
{
public:
  User(const std::string& name);
  User (const User& rhs);
  User& operator = (const User& rhs);
  ~User();
  User (User&& rhs) = delete;
  User& operator = (User&& rhs) = delete;
  void add_friend(const std::string& name);
  std::string get_name() const;
  size_t size() const;
  void set_friend(size_t index, const std::string& name);
  User& operator += (User& other);
  bool operator < (const User& other) const;
  /** 
   * STUDENT TODO:
   * Your custom operators and special member functions will go here!
   */
  friend std::ostream& operator << (std::ostream& out,const User& user);
private:
  std::string _name;
  std::string* _friends;
  size_t _size;
  size_t _capacity;
};




std::ostream& operator << (std::ostream& out,const User& user);