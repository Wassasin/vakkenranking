#pragma once

#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>
#include <functional>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/optional.hpp>
#include <boost/program_options.hpp>

#include "csv_parser.hpp"
#include "summary_parser.hpp"
#include "complete_parser.hpp"

#include "printer.hpp"

namespace vakkenranking
{

class cli
{
	cli() = delete;
	cli(cli&) = delete;
	cli& operator=(cli&) = delete;

	enum class output_e
	{
		HTML,
		CSV
	};

	struct options
	{
		output_e output = output_e::HTML;
		std::string datadir = "./data";
	};

	static std::vector<vakkenranking::evaluation> parse_data(const std::string& str, std::function<vakkenranking::evaluation(const boost::filesystem::path&)> parse_f)
	{
		using namespace vakkenranking;
		using namespace boost::filesystem;

		std::vector<evaluation> result;
		std::set<std::string> codes;

		path p(str);

		const directory_iterator it_end;
		for(directory_iterator it(p); it != it_end; ++it)
		{
			std::cerr << "Parsing " << it->path() << std::endl;
			result.emplace_back(parse_f(it->path()));
			if(!codes.insert(result[result.size()-1].code).second)
				throw std::runtime_error(std::string("Already added course ")+result[result.size()-1].code);
		}

		return result;
	}

	static int interpret(options& opt, int argc, char** argv)
	{
		std::string outputstr = "html";

		boost::program_options::options_description o_general("General options");
		o_general.add_options()
		("help,h", "display this message")
		("outputtype,t", boost::program_options::value(&outputstr), "{html, csv} (defaults to html)")
		("datadir,d", boost::program_options::value<decltype(opt.datadir)>(&opt.datadir), "specify in which directory to search for the data files (default ./data)");

		boost::program_options::variables_map vm;
		boost::program_options::positional_options_description pos;

		boost::program_options::options_description options("Allowed options");
		options.add(o_general);

		try
		{
			boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(options).positional(pos).run(), vm);
		} catch(boost::program_options::unknown_option &e)
		{
			std::cerr << "Unknown option --" << e.get_option_name() << ", see --help" << std::endl;
			return -1;
		}

		try
		{
			boost::program_options::notify(vm);
		} catch(const boost::program_options::required_option &e)
		{
			std::cerr << "You forgot this: " << e.what() << std::endl;
			return 1;
		}

		if(vm.count("help"))
		{
			std::cout
				<< "Parsing application for Radboud University evaluations. [https://github.com/Wassasin/vakkenranking]" << std::endl
				<< "Usage: ./vakkenranking [options]" << std::endl
				<< std::endl
				<< o_general;

			return 1;
		}

		if(outputstr == "html" || outputstr == "")
			opt.output = output_e::HTML;
		else if(outputstr == "csv")
			opt.output = output_e::CSV;
		else
		{
			std::cerr << "Unrecognized output type \"" << outputstr << "\"" << std::endl;
			return 1;
		}

		return 0;
	}

	static int act(const options& opt)
	{
		auto old_data = parse_data(opt.datadir+"/old", complete_parser::parse);
		auto new_data = parse_data(opt.datadir+"/new", complete_parser::parse);

		std::sort(
			new_data.begin(),
			new_data.end(),
			[&](const evaluation& a, const evaluation& b) -> bool
			{
				return(a.course_grade.avg() > b.course_grade.avg());
			}
		);

		if(opt.output == output_e::HTML)
			printer::print(old_data, new_data);
		else if(opt.output == output_e::CSV)
			printer::print_csv(old_data, new_data);
		else
			throw std::logic_error("Unimplemented output type");

		return 0;
	}

public:
	static int run(int argc, char** argv)
	{
		options opt;
		int result = interpret(opt, argc, argv);
		if(result != 0)
			return result;
		return act(opt);
	}
};

}
