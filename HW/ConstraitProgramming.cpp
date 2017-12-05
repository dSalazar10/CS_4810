

#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/driver.hh>
#include <string>
using namespace Gecode;

class Schedule : public Space {
protected:
	IntVarArray I;
	IntVarArray classRooms, times, instructors, courses;

public:
	Schedule(int numRooms, int numInstructors, int numCourses, int numDepts) :
		classRooms(*this, numInstructors * numCourses, 0, numRooms - 1),
		times(*this, numInstructors * numCourses, 0, 6),
		instructors(*this, numInstructors * numCourses, 0, numInstructors - 1),
		courses(*this, numInstructors * numCourses, 0, numCourses * numDepts - 1)
	{
		// Restrict the occurrence of times to 1
		for(int i = 0; i < 7; ++i)
			count(*this, times, i, IRT_LQ, 1);

		// Restrict the occurrence of Courses to 2
		for (int i = 0; i <= numCourses; ++i)
			count(*this, courses, i, IRT_LQ, 2);

		// Restrict the occurrence of Instructors to 2
		for (int i = 0; i < numInstructors; ++i)
			count(*this, instructors, i, IRT_LQ, 2);

		// Restrict courses to two different departments
		int department1Min = 0, department1Max = 1,
			department2Min = 2, department2Max = 3;
		for (int i = 0; i < numInstructors * numCourses; ++i)
		{
			rel(*this, (
				// Department 1
				(instructors[i] >= 0 && instructors[i] <= 0) &&
				(courses[i] >= department1Min && courses[i] <= department1Max) ||
				// Department 2
				(instructors[i] >= 1 && instructors[i] <= 1) &&
				(courses[i] >= department2Min && courses[i] <= department2Max)
				)
			);
		}

		// Choose random elements to branch from
		branch(*this, classRooms, INT_VAR_RND(numRooms), INT_VAL_RND(numRooms));
		branch(*this, times, INT_VAR_RND(7), INT_VAL_RND(7));
		branch(*this, instructors, INT_VAR_RND(numInstructors), INT_VAL_RND(numInstructors));
		branch(*this, courses, INT_VAR_RND(numCourses), INT_VAL_RND(numCourses));

	}
	void constrain(const Space& b) {
		const Schedule& c = static_cast <const Schedule&>(b);
		for (auto i = c.courses.begin(); i != c.courses.end(); ++i)
			// Go with the lowest value (The popular classes are at the begining)
			rel(*this, courses, IRT_LE, *i);
	}

	Schedule(bool share, Schedule& s) : Space(share, s) {
		classRooms.update(*this, share, s.classRooms);
		times.update(*this, share, s.times);
		instructors.update(*this, share, s.instructors);
		courses.update(*this, share, s.courses);
	}
	virtual Space* copy(bool share) {
		return new Schedule(share, *this);
	}
	// print solution
	void print(int numRooms, int numInstructors, int numCourses) const {
		
		for (int i = 0; i < numInstructors * numCourses; ++i)
		{
			// Print out the time
			std::string time[] = { "\n8am ", "10am ","12pm ", "2pm ", "4pm ", "6pm ", "8pm "};
			std::cout << time[(times[i].val())];
			
			// Print out room
			std::string r[] = { "N Sci 112 ", "RO 105 ", "AE 137 ", "VBT 142 ", "TH 142A ", "S Sci 183 ", "PE 238 " };
			std::cout << r[(classRooms[i].val())];

			// Print out the instructor
			std::string instr[] = { "Alan Almquist ", "Karina Garbesi ", "Henry Gilbert ", "David Larson ", "Michael Lee ", "Gary Li ", "Laurie Price " };
			std::cout << instr[(instructors[i].val())];

			// Print out the Course
			std::string c[] = { "CS 4590\n", "CS 3240\n", "ENGL 2000\n", "Engl 2060\n" };
			std::cout << c[(courses[i].val())];
		}
		std::cout << std::endl;
		
	}
};

int main(int argc, char* argv[]) {
	int numRooms = 2, numInstructors = 2, numCourses = 2, numDepts = 2;
	Schedule* m = new Schedule(numRooms, numInstructors, numCourses, numDepts);
	BAB<Schedule> e(m);
	delete m;
	// print all solutions
	while (Schedule* s = e.next()) {
		s->print(numRooms, numInstructors, numCourses); delete s;
	}
	system("pause");
	return 0;
}