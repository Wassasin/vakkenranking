#pragma once

#include <string>
#include <vector>

namespace vakkenranking
{
	struct evaluation
	{
		typedef double rating_t;
	
		std::string code, name, id, time;
		std::vector<rating_t> ratings;
	
		evaluation()
		: code()
		, name()
		, id()
		, time()
		, ratings()
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
		, ratings()
		{}
		
		inline size_t count() const
		{
			return ratings.size();
		}
		
		double avg() const
		{
			const double c = count();
			double result = 0.0;

			for(const rating_t r : ratings)
				result += r / c;
			
			return result;
		}
		
		double stdev() const
		{
			const double c = count();
			const double a = avg();
			double result = 0.0;
			
			for(const rating_t r : ratings)
				result += std::pow(r - a, 2.0) / c;
		
			return std::sqrt(result);
		}
	};
}
