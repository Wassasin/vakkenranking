#pragma once

#include <vector>

namespace vakkenranking
{
	struct grade
	{
		typedef double rating_t;
	
		std::vector<rating_t> ratings;
		
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
