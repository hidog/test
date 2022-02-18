
/*
沒定義底下的macro,ubuntu環境下跑出這個訊息

/usr/local/include/boost/bind.hpp:36:1: note: #pragma message: The practice of declaring the Bind placeholders (_1, _2, ...) in the global namespace is deprecated. Please use <boost/bind/bind.hpp> + using namespace boost::placeholders, or define BOOST_BIND_GLOBAL_PLACEHOLDERS to retain the current behavior.
   36 | BOOST_PRAGMA_MESSAGE(
      | ^~~~~~~~~~~~~~~~~~~~

*/
#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <boost/foreach.hpp>
#include <iostream>


#define MARK_MACRO \
	std::cout << __FILE__ << " " << __FUNCTION__ << " " << __LINE__ << "\n\n";



// 
void	boost_json_example_1()
{
	MARK_MACRO

	char str[1000]	=	"{\"id\":\"AX13441\", \"name\":\"John\", \"age\":33, \"nick_name\":\"meiiiii\", \"height\":178.14}";

	std::stringstream	ss(str);
	boost::property_tree::ptree		pt;

	read_json( ss, pt );

	std::cout << "id : " << pt.get<std::string>("id").c_str() << "\n"
			  << "name : " << pt.get<std::string>("name").c_str() << "\n"
			  << "age : " << pt.get<int>("age") << "\n"
			  << "nick_name : " << pt.get<std::string>("nick_name").c_str() << "\n"
			  << "height : " << pt.get<double>("height") << "\n"
			  << "\n\n";
}


// 
void	boost_json_example_2()
{
	MARK_MACRO

	char str[1000]	=	"{\"id\":\"AX13441\"}";

	std::stringstream	ss(str);
	boost::property_tree::ptree		pt;

	try
	{
		read_json( ss, pt );

		std::cout << "id : " << pt.get<std::string>("id").c_str() << "\n"
				  << "name : " << pt.get<std::string>("name").c_str() << "\n"
				  << "\n\n";
	}
	catch ( boost::property_tree::ptree_error& e )
	{
		std::cout << e.what() << "\n\n";
	}
}



// 
void	boost_json_example_3()
{
	MARK_MACRO

	char str[1000]	=	"{\"id_list\":[0,1,2,3,4,5,6,7]}";

	std::stringstream	ss(str);
	boost::property_tree::ptree		pt;


	read_json( ss, pt );
	std::vector<int>	vec;

	BOOST_FOREACH( boost::property_tree::ptree::value_type &v, pt.get_child("id_list") )
	{
		//std::cout << v.second.data().c_str() << "\n";
		vec.push_back( v.second.get<int>("") );
	}

	for( int i = 0; i < vec.size(); i++ )
		std::cout << vec[i] << " ";	

	std::cout << "\n\n";
}


// 
void	boost_json_example_4()
{
	MARK_MACRO

	char str[7000]	=	"{\"space_data\":[{\"space_id\":1,\"activate\":false,\"work_mode\":1,\"compare_edge\":true,"
						"\"general_mode_thr\":5.00,\"training_mode_thr\":30.00,\"space_tag\":\"space-1\",\"dwin_type\":1,"
						"\"dwin_p1_x\":-1,\"dwin_p1_y\":-1,\"dwin_p2_x\":-1,\"dwin_p2_y\":-1,\"dwin_p3_x\":-1,\"dwin_p3_y\":-1,\"dwin_p4_x\":-1,\"dwin_p4_y\":-1},"
						"{\"space_id\":2,\"activate\":false,\"work_mode\":1,\"compare_edge\":true,\"general_mode_thr\":5.00,\"training_mode_thr\":30.00,"
						"\"space_tag\":\"space-2\",\"dwin_type\":1,\"dwin_p1_x\":-1,\"dwin_p1_y\":-1,\"dwin_p2_x\":-1,\"dwin_p2_y\":-1,\"dwin_p3_x\":-1,"
						"\"dwin_p3_y\":-1,\"dwin_p4_x\":-1,\"dwin_p4_y\":-1},"
						"{\"space_id\":3,\"activate\":false,\"work_mode\":1,\"compare_edge\":true,\"general_mode_thr\":5.00,\"training_mode_thr\":30.00,"
						"\"space_tag\":\"space-3\",\"dwin_type\":1,\"dwin_p1_x\":-1,\"dwin_p1_y\":-1,\"dwin_p2_x\":-1,\"dwin_p2_y\":-1,"
						"\"dwin_p3_x\":-1,\"dwin_p3_y\":-1,\"dwin_p4_x\":-1,\"dwin_p4_y\":-1},"
						"{\"space_id\":4,\"activate\":false,\"work_mode\":1,\"compare_edge\":true,\"general_mode_thr\":5.00,\"training_mode_thr\":30.00,"
						"\"space_tag\":\"space_4\",\"dwin_type\":1,\"dwin_p1_x\":-1,\"dwin_p1_y\":-1,\"dwin_p2_x\":-1,\"dwin_p2_y\":-1,\"dwin_p3_x\":-1,\"dwin_p3_y\":-1,"
						"\"dwin_p4_x\":-1,\"dwin_p4_y\":-1},"
						"\{\"space_id\":5,\"activate\":false,\"work_mode\":1,\"compare_edge\":true,\"general_mode_thr\":5.00,"
						"\"training_mode_thr\":30.00,\"space_tag\":\"space-5\",\"dwin_type\":1,\"dwin_p1_x\":-1,\"dwin_p1_y\":-1,\"dwin_p2_x\":-1,\"dwin_p2_y\":-1,"
						"\"dwin_p3_x\":-1,\"dwin_p3_y\":-1,\"dwin_p4_x\":-1,\"dwin_p4_y\":-1},"
						"{\"space_id\":6,\"activate\":false,\"work_mode\":1,\"compare_edge\":true,\"general_mode_thr\":5.00,\"training_mode_thr\":30.00,"
						"\"space_tag\":\"space-6\",\"dwin_type\":1,\"dwin_p1_x\":-1,\"dwin_p1_y\":-1,\"dwin_p2_x\":-1,\"dwin_p2_y\":-1,\"dwin_p3_x\":-1,\"dwin_p3_y\":-1,"
						"\"dwin_p4_x\":-1,\"dwin_p4_y\":-1},"
						"{\"space_id\":7,\"activate\":false,\"work_mode\":1,\"compare_edge\":true,\"general_mode_thr\":5.00,\"training_mode_thr\":30.00,"
						"\"space_tag\":\"space-7\",\"dwin_type\":1,\"dwin_p1_x\":-1,\"dwin_p1_y\":-1,\"dwin_p2_x\":-1,\"dwin_p2_y\":-1,\"dwin_p3_x\":-1,\"dwin_p3_y\":-1,"
						"\"dwin_p4_x\":-1,\"dwin_p4_y\":-1},"
						"{\"space_id\":8,\"activate\":false,\"work_mode\":1,\"compare_edge\":true,\"general_mode_thr\":5.00,\"training_mode_thr\":30.00,"
						"\"space_tag\":\"space-8\",\"dwin_type\":1,\"dwin_p1_x\":-1,\"dwin_p1_y\":-1,\"dwin_p2_x\":-1,\"dwin_p2_y\":-1,\"dwin_p3_x\":-1,\"dwin_p3_y\":-1,"
						"\"dwin_p4_x\":-1,\"dwin_p4_y\":-1}]}";

	std::stringstream	ss(str);
	boost::property_tree::ptree		pt;

	read_json( ss, pt );
	std::vector<int>	vec;

	BOOST_FOREACH( boost::property_tree::ptree::value_type &v, pt.get_child("space_data") )
	{
		std::cout << v.second.get<int>("space_id") << " ";		
		std::cout << v.second.get<bool>("activate") << " ";
		std::cout << v.second.get<int>("work_mode") << " ";
		std::cout << v.second.get<double>("general_mode_thr") << " ";
		std::cout << v.second.get<std::string>("space_tag").c_str() << "\n";
	}

	//for( int i = 0; i < vec.size(); i++ )
		//std::cout << vec[i] << " ";	
}









//
void	boost_json_example()
{
	boost_json_example_1();
	boost_json_example_2();
	boost_json_example_3();
	boost_json_example_4();

	std::cout << "\n";
}
