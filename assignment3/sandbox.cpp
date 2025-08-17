/*
 * CS106L Assignment 3: Make a Class
 * Created by Fabio Ibanez with modifications by Jacob Roberts-Baca.
 */
#include "class.cpp"
void sandbox() {
  people Liyi{"Liyi", 18};
  std::cout << Liyi.get_name() << "`s years is" << Liyi.get_years() << "and id is" << Liyi.get_id() << std::endl;
  Liyi.set_years(19);
  std::cout << Liyi.get_name() << "`s years is" << Liyi.get_years() << "and id is" << Liyi.get_id() << std::endl;
}