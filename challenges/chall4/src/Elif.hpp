#if ! defined ( ELIF_H )
#define ELIF_H

#include "Thing.hpp"

class Elif : public Thing {
	
	public :
		const char *get_type() const;
		void load(char *thing);

		Elif();
		~Elif();
};

#endif
