#if ! defined ( THING_H )
#define THING_H

#include <sys/stat.h>

class Thing
{
	public :
		static const unsigned int MAX_SIZE	= 512;
		static const unsigned int FILE		= 0x8;
		static const unsigned int FOLDER	= 0x4;

		const char *get_name() const;
		bool fail() const;
		bool larger(Thing *t) const;
		bool newer(Thing *t) const;
		virtual void load(char *thing);
		unsigned int get_size() const;
		unsigned int get_date() const;
		virtual const char *get_type() const = 0;

		Thing();
		virtual ~Thing();

	protected :
		char path[MAX_SIZE + 1];
		unsigned int size, date;
		bool loaded;
		struct stat information;
};

#endif
