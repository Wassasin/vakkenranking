#pragma once

#include "evaluation.hpp"
#include "csv_parser.hpp"

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/optional.hpp>

namespace vakkenranking
{
	class summary_parser
	{
		static evaluation parse_filename(const std::string& str)
		{
			const static boost::regex r("Antwoorden_Vragenlijst_(?:NWI-)?([^\\_^-]+) \\((.+)\\)_([0-9]+)_(.+)\\.csv");
		
			boost::smatch match;
			if(!boost::regex_match(str, match, r))
				throw std::runtime_error(std::string("Can not parse filename (summary): ") + str);
		
			return evaluation(match[1], match[4], match[3], match[2]);
		}
		
		static bool check_rating_row(const std::vector<std::string>& line, size_t& rating)
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
		
		static void parse_file(evaluation& e, const std::string& path)
		{
			csv_parser parser(path);
		
			for(std::vector<std::string> line; parser.read(line);)
			{
				size_t rating;
				if(!check_rating_row(line, rating))
					continue;
			
				size_t times = boost::lexical_cast<size_t>(line[1]);
				for(size_t i = 0; i < times; ++i)
					e.course_grade.ratings.push_back(rating);
			}
		}
	
		void operator=(summary_parser&) = delete;
		summary_parser() = delete;
	
	public:
		static evaluation parse(const boost::filesystem::path& p)
		{
			evaluation e = parse_filename(p.filename().string());
			parse_file(e, p.string());
			return e;
		}
	};
}
