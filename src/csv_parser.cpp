#include "csv_parser.hpp"

#include <stdexcept>
#include <boost/tokenizer.hpp>

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
		std::string line, buffer;
	
		size_t last_quote = buffer.find_first_of('"');
		bool inside_quotes = false;
	
		while(std::getline(s, buffer))
		{
			while(last_quote != std::string::npos)
			{
				inside_quotes = !inside_quotes;
				last_quote = buffer.find_first_of('"', last_quote+1);
			}

			line.append(buffer);

			if(inside_quotes)
			{
				line.append("\n");
				continue;
			}
		
			boost::tokenizer<boost::escaped_list_separator<char>> tok(line);
			result.assign(tok.begin(), tok.end());
		
			return true;
		}
		
		return false;
	}
}
