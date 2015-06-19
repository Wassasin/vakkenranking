#pragma once

#include "evaluation.hpp"
#include "csv_parser.hpp"

#include <functional>
#include <vector>
#include <map>
#include <memory>

#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

namespace vakkenranking
{
	class mapping
	{
	public:
		typedef std::string code_t;
		enum class type_e
		{
			unchanged,
			iteration,
			added,
			unfound
		};
		
		template<typename T>
		struct result_t
		{
			type_e mapping_type;
			boost::optional<T> to;
			
			result_t()
			: mapping_type(type_e::added)
			, to()
			{}
			
			result_t(type_e mt)
			: mapping_type(mt)
			, to()
			{}
			
			result_t(type_e mt, const T& ct)
			: mapping_type(mt)
			, to(ct)
			{}
		};
		
		typedef std::map<code_t, result_t<code_t>> mapping_t;
		
	private:
		void operator=(mapping&) = delete;
		mapping() = delete;
		
		inline static std::string trim(std::string const& str)
		{
			return boost::algorithm::trim_copy(str);
		}

		static std::shared_ptr<mapping_t> load_mapping()
		{
			csv_parser parser("../data/mapping.csv");
			std::shared_ptr<mapping_t> result(new mapping_t());
		
			for(std::vector<std::string> line; parser.read(line);)
			{
				if(line[1] == "new")
					result->insert(std::pair<code_t, result_t<code_t>>(trim(line[0]), result_t<code_t>()));
				else
					result->insert(std::pair<code_t, result_t<code_t>>(trim(line[0]), result_t<code_t>(type_e::iteration, trim(line[1]))));
			}
			
			return result;
		}
		
		static result_t<evaluation> populate(const result_t<code_t>& x, const std::vector<evaluation>& old_data)
		{
			if(!x.to)
				return result_t<evaluation>(x.mapping_type);
		
			const auto e_old_it = std::find_if(
				old_data.begin(),
				old_data.end(),
				[&](const vakkenranking::evaluation& e)
				{
					return e.code == x.to.get();
				}
			);
		
			if(e_old_it == old_data.end())
				throw std::runtime_error(std::string("Could not find code in old_data: ") + x.to.get());
		
			return result_t<evaluation>(x.mapping_type, *e_old_it);
		}
	public:
		
		static std::function<result_t<evaluation>(const code_t&)> create_mapping_f(const std::vector<evaluation>& old_data)
		{
			std::shared_ptr<mapping_t> mapping(load_mapping());
			
			return [&,mapping](const code_t& x) -> result_t<evaluation>
			{
				const auto it = mapping->find(x);
				if(it != mapping->end())
					return populate(it->second, old_data);
				
				const auto e_old_it = std::find_if(
					old_data.begin(),
					old_data.end(),
					[&](const vakkenranking::evaluation& e)
					{
						return e.code == x;
					}
				);
				
				if(e_old_it == old_data.end())
					return result_t<evaluation>(type_e::unfound);
				
				return result_t<evaluation>(type_e::unchanged, *e_old_it);
			};
		}
	};
}
