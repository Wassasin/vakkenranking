#pragma once

#include "evaluation.hpp"
#include "mapping.hpp"

#include <string>

namespace vakkenranking
{
	class printer
	{
		void operator=(printer&) = delete;
		printer() = delete;
		
		static std::string round(double x, size_t digits)
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
	
	public:
		static void print_csv(const std::vector<evaluation>& old_data, const std::vector<evaluation>& new_data)
		{
			const auto mapping_f = mapping::create_mapping_f(old_data);
			const std::string sep = ",";
			
			for(const auto& e : new_data)
			{
				const auto e_old = mapping_f(e.code);
				
				std::cout << '"' << e.code << '"' << sep
					<< '"' << round(e.course_grade.avg(), 1) << '"' << sep
					<< '"' << round(e.course_grade.stdev(), 1) << '"' << sep
					<< '"' << e.course_grade.count() << '"' << sep
					<< '"' << round(e.teacher_grade.avg(), 1) << '"' << sep
					<< '"' << round(e.teacher_grade.stdev(), 1) << '"' << sep
					<< '"' << e.teacher_grade.count() << '"' << sep;
				
				if(e_old.to)
					std::cout
						<< '"' << round(e_old.to.get().course_grade.avg(), 1) << '"' << sep
						<< '"' << round(e_old.to.get().course_grade.stdev(), 1) << '"' << sep
						<< '"' << e_old.to.get().course_grade.count() << '"'
						<< std::endl;
				else
					std::cout
						<< '"' << '"' << sep
						<< '"' << '"' << sep
						<< '"' << '"'
						<< std::endl;
			}
		}
	
		static void print(const std::vector<evaluation>& old_data, const std::vector<evaluation>& new_data)
		{
			const static size_t slice_count = 3;
	
			std::vector<std::vector<evaluation>> slices(slice_count);
			const size_t slice_size = ceil((double)new_data.size() / (double)slice_count);
		
			for(size_t i = 0; i < slice_count; ++i)
				for(size_t j = i * slice_size; j < (i+1) * slice_size && j < new_data.size(); ++j)
					slices[i].push_back(new_data[j]);
					
			const auto mapping_f = mapping::create_mapping_f(old_data);
			size_t n = 0;

			std::cout << "<!DOCTYPE html><html>\n<head>\n"
				<< "\t<meta charset=\"UTF-8\" />\n"
				<< "\t<title>OLC III - Vakkenranking 2013-2014</title>\n"
				<< "\t<link rel=\"stylesheet\" type=\"text/css\" media=\"all\" href=\"http://olc.cs.ru.nl/assets/twentyeleven.css\">\n"
				<< "\t<style>\n"
				<< "\t\t.entry-content {\n"
				<< "\t\t\tpadding: 0;\n"
				<< "\t\t}\n"
				<< "\n"
				<< "\t\t.entry-content table, .comment-content table {\n"
				<< "\t\t\tborder-bottom: none;\n"
				<< "\t\t}\n"
				<< "\n"
				<< "\t\t.entry-content td, .comment-content td {\n"
				<< "\t\t\tpadding: 7px 6px 7px 0;\n"
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
				<< "\t\ttable.legend {\n"
				<< "\t\t\twidth: 30%;\n"
				<< "\t\t}\n"
				<< "\t\ttable.legend td {\n"
				<< "\t\t\tborder: none;\n"
				<< "\t\t\tpadding: 3px 2px 3px 0px;\n"
				<< "\t\t}\n"
				<< "\t\ttd.legend-left {\n"
				<< "\t\t\ttext-align: center;\n"
				<< "\t\t}\n"
				<< "\t</style>\n"
				<< "</head>\n"
				<< "<body>\n"
				<< "<header class=\"entry-header\">\n"
				<< "\t<h1 class=\"entry-title\">Vakkenranking Informatica 2013-2014</h1>\n"
				<< "\t<h2>door Wouter Geraedts &lt;radboud@woutergeraedts.nl&gt;</h2>\n"
				<< "</header>\n"
				<< "<br />\n"
				<< "<div class=\"entry-content\" role=\"main\">\n";
	
			for(size_t i = 0; i < slices.size(); ++i)
			{
				const auto& slice = slices[i];
	
				if(i == 0)
				{
					std::cout << "<table style=\"width: 33%; float: left\">\n";
			
					std::cout << "<tr>\n"
						<< "\t<td style=\"text-align: center\"><strong>#</strong></td>\n"
						<< "\t<td><strong>Code</strong></td>\n"
						<< "\t<td><strong>Naam</strong></td>\n"
						<< "\t<td><strong>x</strong></td>\n"
						<< "\t<td><strong>n</strong></td>\n"
						<< "\t<td><strong>σ</strong></td>\n"
						<< "\t<td><strong>x<sub>oud</sub></strong></td>\n"
						<< "\t<td>&nbsp;</td>\n"
						<< "</tr>\n";
				}
				else
				{
					std::cout << "<table style=\"padding-left: 2%; width: 33%; float: left\">\n";
			
					std::cout << "<tr>\n"
						<< "\t<td style=\"text-align: center\"><strong>#</strong></td>\n"
						<< "\t<td><strong>Code</strong></td>\n"
						<< "\t<td><strong>Naam</strong></td>\n"
						<< "\t<td><strong>x</strong></td>\n"
						<< "\t<td><strong>n</strong></td>\n"
						<< "\t<td><strong>σ</strong></td>\n"
						<< "\t<td><strong>x<sub>oud</sub></strong></td>\n"
						<< "\t<td>&nbsp;</td>\n"
						<< "</tr>\n";
				}
		
				for(size_t j = 0; j < slice.size(); ++j)
				{
					const evaluation& e = slice[j];
					const auto e_old = mapping_f(e.code);
					
					const double avg = e.course_grade.avg();
			
					if(j == slice.size() - 1) //Last element in slice
						std::cout << "<tr class=\"vakkenranking-bottom-border\">\n";
					else
						std::cout << "<tr>\n";
			
					std::cout << "\t<td style=\"text-align: center\">" << n << "</td>\n"
						<< "\t<td>" << e.code << "</td>\n"
						<< "\t<td>" << e.name << "</td>\n"
						<< "\t<td>" << round(avg, 1) << "</td>\n"
						<< "\t<td>" << e.course_grade.count() << "</td>\n"
						<< "\t<td>" << round(e.course_grade.stdev(), 1) << "</td>\n";
			
					if(e_old.to)
					{
						const static size_t threshold = 5;
						const static double max_diff = 1.0;
						const static double sig_diff = 0.4;
			
						const double old_avg = e_old.to.get().course_grade.avg();
						const double diff = avg - old_avg;
			
						std::cout
							<< "\t<td>" << round(old_avg, 1) << "</td>\n"
							<< "\t<td style=\"text-align: center\">"
							<< "<div style=\"color: ";
			
						if(e.course_grade.count() <= threshold || e_old.to.get().course_grade.count() <= threshold)
							std::cout << "#BBB";
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
							std::cout << "▼▼";
						else if(diff <= -sig_diff)
							std::cout << "▼";
						
						std::cout << "</div>";
					}
					else
					{
						std::cout << "\t<td>&nbsp;</td>\n";
						std::cout << "\t<td style=\"text-align: center\">";
						
						if(e_old.mapping_type == mapping::type_e::unfound)
							std::cout << "<span style=\"font-size: 10px\">?</span>";
						else if(e_old.mapping_type == mapping::type_e::added)
							std::cout << "<span style=\"font-size: 10px\">!</span>";
						else
							std::cout << "<span style=\"font-size: 10px\">666</span>";
							
						std::cout << "</td>\n";
					}
		
					std::cout << "</tr>\n";
					n++;
				}
		
				std::cout << "</table>\n" << std::endl;
			}
	
			std::cout << "<div style=\"clear: both; font-size: 0;\">&nbsp;</div>\n"
				<< "<table class=\"legend\">\n"
				<< "<tr><td class=\"legend-left\">▲</td><td>&rarr;</td><td>0.4 ≤ δ ≤ 1.0</td></tr>\n"
				<< "<tr><td class=\"legend-left\">▲▲</td><td>&rarr;</td><td>δ > 1.0</td></tr>\n"
				<< "<tr><td class=\"legend-left\"><span style=\"color: #BBB\">▼ grijs</span></td><td>&rarr;</td><td>niet significant aantal deelnemers</td></tr>\n"
				<< "<tr><td class=\"legend-left\"><span style=\"color: red\">▼ kleur</span></td><td>&rarr;</td><td>significant aantal deelnemers</td></tr>\n"
				<< "<tr><td class=\"legend-left\">!</td><td>&rarr;</td><td>cursus nieuw in 2013-2014</td></tr>\n"
				<< "<tr><td class=\"legend-left\">?</td><td>&rarr;</td><td>geen enquêteresultaten uit 2012-2013</td></tr>\n"
				<< "</table></div>"
				<< "</body></html>";
		}
	};
}
