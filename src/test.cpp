#include <iostream>
#include <pqxx/pqxx>

int main() {

    try
    {
        const std::string opt = "host=localhost port=5432 dbname=test connect_timeout=1 user=test password=test";

        pqxx::connection c(opt);

        pqxx::work test(c);

        pqxx::row row = test.exec1("SELECT 1");

        test.commit();

        std::cout << row.at(0).as<int>() << '\n';
        
    }
    catch(std::exception const &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

}