#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/optional.hpp>

#include "csv_parser.hpp"

struct evaluation
{
	std::string code, name, id, time;
	
	evaluation()
	: code()
	, name()
	, id()
	, time()
	{}
	
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

struct stats
{
	double count, avg, stddev;
	boost::optional<double> old_avg;
	bool old_relevant, old_missing, new_course;
	
	stats()
	: count()
	, avg()
	, stddev()
	, old_avg()
	, old_relevant(true)
	, old_missing(false)
	, new_course(false)
	{}
};

typedef std::pair<evaluation, stats> entry;

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

std::string round(double x, size_t digits)
{
	double multiplier = std::pow(10, digits);
	
	std::stringstream str;
	str << std::round(x * multiplier) / multiplier;

	size_t find = str.str().find('.');
	if(find == std::string::npos)
	{
		find = str.str().size();
		str << '.';
	}
	
	int current_digits = str.str().size() - find - 1;
	int todo_digits = (int)digits - current_digits;
	
	if(todo_digits > 0)
		for(int i = 0; i < todo_digits; i++)
			str << '0';
	
	return str.str();
}

void parse_courses(std::map<std::string, entry>& entry_map)
{
	using namespace vakkenranking;

	csv_parser parser("courses.csv");
	for(std::vector<std::string> line; parser.read(line);)
	{
		if(line.size() < 3)
			continue;
		
		auto it = entry_map.find(line[0]);
		if(it == entry_map.end())
			continue;
	
		try
		{	
			entry_map[it->first].second.old_avg = boost::lexical_cast<double>(line[2]);
		}
		catch(std::bad_cast)
		{}
		
		if(line.size() >= 4)
		{
			stats& s = entry_map[it->first].second;
		
			if(line[3] == "x")
				s.old_relevant = false;
			else if(line[3] == "nieuwe cursus")
				s.new_course = true;
			else if(line[3] == "ontbreekt")
				s.old_missing = true;
		}
	}
}

void parse_renaming(std::map<std::string, entry>& entry_map)
{
	using namespace vakkenranking;
	
	csv_parser parser("renaming.csv");
	for(std::vector<std::string> line; parser.read(line);)
	{
		if(line.size() < 4)
			continue;
		
		auto it = entry_map.find(line[0]);
		if(it == entry_map.end())
			continue;
	
		entry_map[it->first].first.code = line[2];
		entry_map[it->first].first.name = line[3];
	}
}

int main(int argc, char** argv)
{
	using namespace vakkenranking;
	using namespace boost::filesystem;
	
	std::map<std::string, entry> entry_map;
	
	path p("data/");
	
	const directory_iterator it_end;
	for(directory_iterator it(p); it != it_end; ++it)
	{
		csv_parser parser(it->path().string());
		
		std::map<double, double> rating_map;
		for(std::vector<std::string> line; parser.read(line);)
		{
			size_t rating;
			if(!check_rating_row(line, rating))
				continue;
			
			rating_map[(double)rating] = boost::lexical_cast<double>(line[1]);
		}
		
		stats s;
		for(std::pair<double, double> rating : rating_map)
			s.count += rating.second;
		
		for(std::pair<double, double> rating : rating_map)
			s.avg += (rating.first / s.count) * rating.second;
		
		for(std::pair<double, double> rating : rating_map)
			s.stddev += std::pow(rating.first - s.avg, 2.0) * rating.second;
		
		s.stddev = std::sqrt(s.stddev / s.count);
		
		evaluation e = evaluation::parse(it->path().filename().string());
		
		entry_map[e.code] = std::make_pair(e, s);
	}
	
	parse_courses(entry_map);
	
	std::vector<std::string> entry_ordening;
	for(const auto& entry : entry_map)
		entry_ordening.push_back(entry.first);
	
	std::sort(
		entry_ordening.begin(),
		entry_ordening.end(),
		[&](const std::string& a, const std::string& b)
		{
			return entry_map[a].second.avg > entry_map[b].second.avg;
		}
	);
	
	/*for(const auto& key : entry_ordening)
	{
		const evaluation& e = entry_map[key].first;
	
		std::cout << '"' << e.code << '"' << ','
			<< '"' << e.name << '"' << ','
			<< '"' << e.code << '"' << ','
			<< '"' << e.name << '"' << std::endl;
	}*/
	
	parse_renaming(entry_map);
	
	std::cout << "<!DOCTYPE html><html>\n<head>\n"
		<< "\t<meta charset=\"UTF-8\" />\n"
		<< "\t<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"http://olciii.nl/wp-content/themes/twentyeleven/style.css\">\n"
		<< "\t<style>\n"
		<< "\t\t.entry-content table, .comment-content table {\n"
		<< "\t\t\tborder-bottom: none;\n"
		<< "\t\t}\n"
		<< "\n"
		<< "\t\t.entry-content td, .comment-content td {\n"
		<< "\t\t\tpadding: 6px 10px 6px 0;\n"
		<< "\t\t}\n"
		<< "\n"
		<< "\t\t.vakkenranking-bottom-border td {\n"
		<< "\t\t\tborder-bottom: 1px solid #DDD;\n"
		<< "\t\t}\n"
		<< "\n"
		<< "\t\tbody {\n"
		<< "\t\t\tbackground: none;\n"
		<< "\t\t\tfont: 13px \"Helvetica Neue\", Helvetica, Arial, sans-serif;\n"
		<< "\t\t}\n"
		<< "\t</style>\n"
		<< "</head>\n"
		<< "<body>\n"
		<< "<header class=\"entry-header\">\n"
		<< "\t<h1 class=\"entry-title\">Vakkenranking Informatica & Informatiekunde 2011-2012</h1>\n"
		<< "\t<h2>Opleidingscommissie Informatica & Informatiekunde 2012-2013</h2>\n"
		<< "</header>\n"
		<< "<div class=\"entry-content\" role=\"main\">\n";
	
	for(size_t i = 0; i < entry_ordening.size(); i++)
	{
		const evaluation& e = entry_map[entry_ordening[i]].first;
		const stats& s = entry_map[entry_ordening[i]].second;
		
		if(i == 0)
		{
			std::cout << "<table style=\"width: 32%; float: left\">\n";
			
			std::cout << "<tr>\n"
				<< "\t<td><strong>Vak (vakcode)</strong></td>\n"
				<< "\t<td><strong>Cijfer</strong></td>\n"
				<< "\t<td><strong>n</strong></td>\n"
				<< "\t<td><strong>σ</strong></td>\n"
				<< "\t<td>&nbsp;</td>\n"
				<< "</tr>\n";
		}
		else if(i == entry_ordening.size() / 3 || i == entry_ordening.size() / 3 * 2)
		{
			std::cout << "<table style=\"padding-left: 2%; width: 34%; float: left\">\n";
			
			std::cout << "<tr>\n"
				<< "\t<td><strong>Vak (vakcode)</strong></td>\n"
				<< "\t<td><strong>Cijfer</strong></td>\n"
				<< "\t<td><strong>n</strong></td>\n"
				<< "\t<td><strong>σ</strong></td>\n"
				<< "\t<td>&nbsp;</td>\n"
				<< "</tr>\n";
		}
		
		/*std::cout
			<< e.code << " - " << e.name << '\n'
			<< "\tcount: " << s.count
			<< " avg: " << round(s.avg, 1)
			<< " stddev: " << round(s.stddev, 2)
			<< '\n';*/
		
		if(i == entry_ordening.size() / 3 - 1 || i == entry_ordening.size() / 3 * 2 - 1 || i == entry_ordening.size() - 1)
			std::cout << "<tr class=\"vakkenranking-bottom-border\">\n";
		else
			std::cout << "<tr>\n";
		
		std::cout << "\t<td>" << e.name << " (" << e.code << ")</td>\n"
			<< "\t<td>" << round(s.avg, 1) << "</td>\n"
			<< "\t<td>" << s.count << "</td>\n"
			<< "\t<td style=\"font-size: 12px\"><span style=\"font-size: 10px\">&plusmn;</span>" << round(s.stddev, 1) << "</td>\n";
		
		std::cout << "\t<td style=\"text-align: center\">";		
		if(s.old_avg)
		{
			double max_diff = 1.0;
			double sig_diff = 0.4;
			double diff = s.avg - s.old_avg.get();
			
			std::cout << "<div style=\"color: ";
			
			if(!s.old_relevant || s.count <= 5)
				std::cout << "lightgray";
			else if(diff >= max_diff)
				std::cout << "green";
			else if(diff >= sig_diff)
				std::cout << "green";
			else if(diff <= -max_diff)
				std::cout << "red";
			else if(diff <= sig_diff)
				std::cout << "red";
			
			std::cout << "\">";
			
			if(diff >= max_diff)
				std::cout << "▲▲";
			else if(diff >= sig_diff)
				std::cout << "▲";
			else if(diff <= -max_diff)
				std::cout << "▼";
			else if(diff <= -sig_diff)
				std::cout << "▼▼";
				
			std::cout << "</div>";
		}
		else if(s.old_missing)
			std::cout << "<span style=\"font-size: 10px\">?</span>";
		else if(s.new_course)
			std::cout << "<span style=\"font-size: 10px\">!</span>";
		
		std::cout << "</td>\n"
			<< "</tr>\n";
		
		if(i == entry_ordening.size() / 3 - 1 || i == entry_ordening.size() / 3 * 2 - 1 || i == entry_ordening.size() - 1)
			std::cout << "</table>\n";
		
		std::cout << std::endl;
	}
	
	std::cout << "<div style=\"clear: both;\">&nbsp;</div>\n"
		<< "<p>▲ = 0.4 ≤ δ < 1.0; ▲▲ = δ > 1.0; <span style=\"color: lightgray\">▼ grijs</span> = niet significant; <span style=\"color: red\">▼ kleur</span> = significant; ! = cursus nieuw in 2011-2012; ? = geen enquêteresultaten uit 2010-2011</p>\n"
		<< "</body></html>";
	
	return 0;
}
