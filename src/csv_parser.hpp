#pragma once

#include <fstream>
#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/noncopyable.hpp>

namespace vakkenranking
{
	class csv_parser : private boost::noncopyable
	{
		typedef boost::tokenizer<boost::escaped_list_separator<char>> tokenizer;
	
		std::ifstream s;
	public:
		csv_parser(const std::string& filename);
		
		bool read(std::vector<std::string>& result);
	};
}
