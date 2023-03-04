#include <iostream>
#include <pqxx/pqxx>

int main() {

    try
    {
        const std::string opt = "host=localhost port=5432 dbname=airporttest connect_timeout=1 user=test password=testpassword";

        pqxx::connection c(opt);

        pqxx::work test(c);

        pqxx::row row = test.exec1("SELECT name FROM test WHERE (id = 2)");

        test.commit();

        std::cout << "retuned: " << row.at(0).as<std::string>() << '\n';
        
    }
    catch(std::exception const &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

}