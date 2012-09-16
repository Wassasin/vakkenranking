#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "csv_parser.hpp"

bool check_rating_row(const std::vector<std::string>& line, size_t& rating)
{
	if(line.size() != 3)
		return false;
	
	for(size_t i = 1; i <= 10; i++)
	{
		std::stringstream cmp_str;
		cmp_str << i << " (";
		
		if(i < 10)
			cmp_str << '0';
		
		cmp_str << i << ")";
		
		if(cmp_str.str() != line[0])
			continue;
		
		rating = i;
		return true;
	}
	
	return false;
}

double round(double x, size_t digits)
{
	double multiplier = std::pow(10, digits);
	return std::round(x * multiplier) / multiplier;
}

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
		
		std::map<double, double> rating_map;
		std::vector<std::string> line;
		while(parser.read(line))
		{
			size_t rating;
			if(!check_rating_row(line, rating))
				continue;
			
			rating_map[(double)rating] = boost::lexical_cast<double>(line[1]);
		}
		
		double count = 0;
		for(std::pair<double, double> rating : rating_map)
			count += rating.second;
		
		double avg = 0;
		for(std::pair<double, double> rating : rating_map)
			avg += (rating.first / count) * rating.second;
		
		double stddev = 0;
		for(std::pair<double, double> rating : rating_map)
			stddev += std::pow(rating.first - avg, 2) * rating.second;
		
		stddev = std::sqrt(stddev / count);
		
		std::cout
			<< filename << '\n'
			<< "\tcount: " << count
			<< " avg: " << round(avg, 1)
			<< " stddev: " << round(stddev, 2)
			<< std::endl;
	}
	
	return 0;
}
