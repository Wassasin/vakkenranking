#include "csv_parser.hpp"

#include <stdexcept>

namespace vakkenranking
{
	csv_parser::csv_parser(const std::string& filename)
	: s(filename.c_str())
	{
		if(!s.is_open())
			throw std::runtime_error("Can not open file");
	}
	
	bool csv_parser::read(std::vector<std::string>& result)
	{
		std::string line;
	
		if(!std::getline(s, line))
			return false;
		
		tokenizer tok(line);
		result.assign(tok.begin(), tok.end());
		
		return true;
	}
}
