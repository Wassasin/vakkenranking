#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include "csv_parser.hpp"

struct evaluation
{
	std::string code, name, id, time;
	
	evaluation(
		const std::string& code,
		const std::string& name,
		const std::string& id,
		const std::string& time
	)
	: code(code)
	, name(name)
	, id(id)
	, time(time)
	{}
	
	static evaluation parse(const std::string& str)
	{
		//Resultaten_Vragenlijst_44773_Functioneel Programmeren_IBC006 (2012-01-23)_xls.xls.csv
		const static boost::regex r("Resultaten_Vragenlijst_([0-9]+)_(.+)_([^\\_]+) \\((.+)\\)_xls\\.xls\\.csv");
		
		boost::smatch match;
		if(!boost::regex_match(str, match, r))
			throw std::runtime_error("Regex failed");
		
		return evaluation(match[3], match[2], match[1], match[4]);
	}
};

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
		csv_parser parser(it->path().string());
		
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
		
		evaluation e = evaluation::parse(it->path().filename().string());
		
		std::cout
			<< e.code << " - " << e.name << '\n'
			<< "\tcount: " << count
			<< " avg: " << round(avg, 1)
			<< " stddev: " << round(stddev, 2)
			<< std::endl;
	}
	
	return 0;
}
