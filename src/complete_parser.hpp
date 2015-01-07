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
			//Results_survey_NWI-IBC017_2014-11-04_758478_Calculus and Probability Theory.csv
			const static boost::regex r1(".*_(?:NWI-)?([^\\_^-]+) \\((.+)\\)_([0-9]+)_(.+)\\.csv");
			const static boost::regex r2(".*_(?:NWI-)?([^\\_^-]+)_(.+)_([0-9]+)_(.+)\\.csv");

			boost::smatch match;
			if(!boost::regex_match(str, match, r1) && !boost::regex_match(str, match, r2))
				throw std::runtime_error(std::string("Can not parse filename (complete): ") + str);
		
			return evaluation(match[1], match[4], match[3], match[2]);
		}
		
		static size_t find_course_grade_key(const std::vector<std::string>& line, const std::string& path)
		{
			const static std::vector<std::string> keys = {
				"[Ik geef deze cursus het volgende rapportcijfer] Ik geef deze cursus het volgende rapportcijfer",
				"Ik geef deze cursus het volgende rapportcijfer [Ik geef deze cursus het volgende rapportcijfer]",
				"[I would rate this course] On a scale from 1 to 10 (10 being excellent) I would rate this course"
			};
			
			return find_key(line, keys, path, "course_grade");
		}
		
		static size_t find_teacher_grade_key(const std::vector<std::string>& line, const std::string& path)
		{
			const static std::vector<std::string> keys = {
				"[Ik geef de docent(en) het volgende rapportcijfer] Ik geef de docent(en) het volgende rapportcijfer",
				"Ik geef de docent(en) het volgende rapportcijfer [Ik geef de docent(en) het volgende rapportcijfer]",
				"[I would rate the lecturer(s)/teacher(s)] On a scale from 1 to 10 (10 being excellent) I would rate the lecturer(s)/teacher(s)"
			};
			
			return find_key(line, keys, path, "teacher_grade");
		}
		
		static size_t find_key(const std::vector<std::string>& line, const std::vector<std::string>& keys, const std::string& path, const std::string& function_name)
		{
			for(const auto& key : keys)
			{
				size_t key_i = std::distance(line.begin(), std::find(line.begin(), line.end(), key));
				if(key_i < line.size())
					return key_i;
			}
			
			throw std::runtime_error(std::string("Can not find key for ") + function_name + " in " + path);
		}
		
		static void parse_file(evaluation& e, const std::string& path)
		{
			csv_parser parser(path);
			size_t course_grade_key_i, teacher_grade_key_i;
			
			{
				std::vector<std::string> line;
				if(!parser.read(line))
					throw std::runtime_error("No headerline");
				
				course_grade_key_i = find_course_grade_key(line, path);
				teacher_grade_key_i = find_teacher_grade_key(line, path);
			}

			size_t i = 0;
			for(std::vector<std::string> line; parser.read(line); ++i)
			{
				if(line.size() > 1)
				{
					if(line.at(course_grade_key_i) != "") //Answer has not been filled in
						e.course_grade.ratings.push_back(boost::lexical_cast<double>(line.at(course_grade_key_i)));
					
					if(line.at(teacher_grade_key_i) != "") //Answer has not been filled in
						e.teacher_grade.ratings.push_back(boost::lexical_cast<double>(line.at(teacher_grade_key_i)));
				}
			}
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
