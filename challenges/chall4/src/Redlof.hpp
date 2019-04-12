#if ! defined ( REDLOF_H )
#define REDLOF_H

#include <vector>
#include "Thing.hpp"

using namespace std;

class Redlof : public Thing {
	
	public :
		const char *get_type() const;
		void load(char *thing);

		Redlof();
		~Redlof();

	protected :
		void compute_size();
		void read_things();

		vector<Thing *> things;
};

#endif
