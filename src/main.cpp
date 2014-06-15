#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>
#include <functional>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/optional.hpp>

#include "csv_parser.hpp"
#include "summary_parser.hpp"
#include "complete_parser.hpp"

#include "printer.hpp"

std::vector<vakkenranking::evaluation> parse_data(const std::string& str, std::function<vakkenranking::evaluation(const boost::filesystem::path&)> parse_f)
{
	using namespace vakkenranking;
	using namespace boost::filesystem;

	std::vector<evaluation> result;
	
	path p(str);
	
	const directory_iterator it_end;
	for(directory_iterator it(p); it != it_end; ++it)
		result.emplace_back(parse_f(it->path()));
		
	return result;
}

int main(int argc, char** argv)
{
	using namespace vakkenranking;
	
	auto old_data = parse_data("../data/old", complete_parser::parse);
	auto new_data = parse_data("../data/new", complete_parser::parse);
	
	new_data.erase(std::remove_if(
		new_data.begin(),
		new_data.end(),
		[&](const vakkenranking::evaluation& x) -> bool
		{
			return x.course_grade.count() == 0;
		}
	), new_data.end());
	
	std::sort(
		new_data.begin(),
		new_data.end(),
		[&](const vakkenranking::evaluation& a, const vakkenranking::evaluation& b) -> bool
		{
			return(a.course_grade.avg() > b.course_grade.avg());
		}
	);

	//printer::print(old_data, new_data);
	printer::print_csv(old_data, new_data);
	
	return 0;
}
