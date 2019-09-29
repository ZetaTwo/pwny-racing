#include <string.h>
#include <sys/param.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Thing.hpp"
#include <stdio.h>

const char *Thing::get_name() const {

	return path;
}

unsigned int Thing::get_size() const {

	return size;
}

unsigned int Thing::get_date() const {

	return date;
}

bool Thing::fail() const {

	return (loaded == false);
}

bool Thing::larger(Thing *e) const {

	return (size > e->get_size());
}

bool Thing::newer(Thing *e) const {

	return (date > e->get_date());
}

void Thing::load(char * thing){

	realpath(thing, path);
	if (stat(path, &information) == 0){
		loaded = true;
	} else {
		loaded = false;
	}
}

Thing::Thing() : size (0), loaded (false){

	//
}

Thing::~Thing(){

	//
}
