#include <string.h>
#include "Elif.hpp"

const char *Elif::get_type() const {
	
	return "Elif";
}

void Elif::load(char *thing){

	Thing::load(thing);
	if (!fail()){
		size = information.st_size;
		date = information.st_mtime;
	}
}

Elif::Elif() : Thing(){

	//
}

Elif::~Elif(){

	//
}
