#include <iostream>
#include <algorithm>

#include <boost/filesystem.hpp>

#include "csv_parser.hpp"

int main(int argc, char** argv)
{
	using namespace vakkenranking;
	using namespace boost::filesystem;
	
	path p("data/");
	
	const directory_iterator it_end;
	for(directory_iterator it(p); it != it_end; ++it)
	{
		const std::string filename(it->path().string());
		csv_parser parser(filename);
		
		std::vector<std::string> line;
		while(parser.read(line))
			std::copy(
				line.begin(),
				line.end(),
				std::ostream_iterator<std::string>(std::cout, "|")
			);
	}
	
	return 0;
}
