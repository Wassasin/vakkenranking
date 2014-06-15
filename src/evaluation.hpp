#pragma once

#include <string>

#include "grade.hpp"

namespace vakkenranking
{
	struct evaluation
	{
		std::string code, name, id, time;
		
		grade course_grade;
		grade teacher_grade;
		
		evaluation()
		: code()
		, name()
		, id()
		, time()
		, course_grade()
		, teacher_grade()
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
		, course_grade()
		, teacher_grade()
		{}
	};
}
