#include <string>
class people
{
private:
    std::string name;
    int years;
    int id;
    void setID();
public:
    people();
    people(std::string name,int years);
    ~people();
    std::string get_name () const;
    int get_years () const;
    int get_id () const;
    void set_years(int new_years);
};
