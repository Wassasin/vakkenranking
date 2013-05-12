#pragma once

#include "evaluation.hpp"
#include "csv_parser.hpp"

#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/optional.hpp>

namespace vakkenranking
{
	class complete_parser
	{
		static evaluation parse_filename(const std::string& str)
		{
			//Antwoorden_Vragenlijst_NWI-I00032 (2013-01-29)_67288_Geavanceerd Programmeren.xls.csv
			const static boost::regex r("Antwoorden_Vragenlijst_(?:NWI-)?([^\\_^-]+) \\((.+)\\)_([0-9]+)_(.+).xls\\.csv");
		
			boost::smatch match;
			if(!boost::regex_match(str, match, r))
				throw std::runtime_error(std::string("Can not parse filename: ") + str);
		
			return evaluation(match[1], match[4], match[3], match[2]);
		}
		
		static size_t find_key(const std::vector<std::string>& line, const std::string& path)
		{
			const static std::vector<std::string> keys = {
				"[Ik geef deze cursus het volgende rapportcijfer] Ik geef deze cursus het volgende rapportcijfer",
				"[I would rate this course] On a scale from 1 to 10 (10 being excellent) I would rate this course"
			};
			
			for(const auto& key : keys)
			{
				size_t key_i = std::distance(line.begin(), std::find(line.begin(), line.end(), key));
				if(key_i < line.size())
					return key_i;
			}
			
			throw std::runtime_error(std::string("Can not find key in ") + path);
		}
		
		static void parse_file(evaluation& e, const std::string& path)
		{
			csv_parser parser(path);
			size_t key_i;
			
			{
				std::vector<std::string> line;
				if(!parser.read(line))
					throw std::runtime_error("No headerline");
				
				key_i = find_key(line, path);
			}
						
			for(std::vector<std::string> line; parser.read(line);)
				if(line.size() > 1 && line.at(key_i) != "") //Answer has not been filled in
					e.ratings.push_back(boost::lexical_cast<double>(line.at(key_i)));
		}
	
		void operator=(complete_parser&) = delete;
		complete_parser() = delete;
	
	public:
		static evaluation parse(const boost::filesystem::path& p)
		{
			evaluation e = parse_filename(p.filename().string());
			parse_file(e, p.string());
			return e;
		}
	};
}