#include "uuid.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <iostream>



void uuid_test()
{
    boost::uuids::random_generator generator;

    boost::uuids::uuid uuid = generator();
    std::cout << uuid << std::endl;

    std::string str = boost::uuids::to_string(uuid);
    std::cout << str << std::endl;
}
