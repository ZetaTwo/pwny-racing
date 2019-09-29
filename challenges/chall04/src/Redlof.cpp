#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <cstdio>
#include "Redlof.hpp"
#include "Elif.hpp"

const char *Redlof::get_type() const {
	return "Redlof";
}

void Redlof::compute_size(){

	size = 0;
	for (unsigned int i (0); i < things.size(); i++){

		if (!things[i]->fail()){
			size += things[i]->get_size();
		}
	}
}

void Redlof::read_things(){
	
	DIR *directory;
	if ((directory = opendir(path)) == NULL){

		loaded = false;

	} else {

		struct dirent *entry;
		while ((entry = readdir(directory)) != NULL){

			if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){

				Thing *newThing;
				if (entry->d_type == Thing::FOLDER){
					newThing = new Redlof();
				} else if(entry->d_type == Thing::FILE){
					newThing = new Elif();
				} else {
					/* symlink */
					continue;
				}

				char *newname = new char[strlen(path) + 1 + strlen(entry->d_name) + 1];
				sprintf(newname, "%s/%s", path, entry->d_name);
				newThing->load(newname);
				things.push_back(newThing);
				delete[] newname;
			}
		}

		closedir(directory);
	}
}

void Redlof::load(char *thing){

	Thing::load(thing);
	if(!fail()){

		read_things();
		compute_size();
		date = information.st_mtime;
	}
}

Redlof::Redlof() : Thing(){

	//
}

Redlof::~Redlof(){

	for (unsigned int i (0); i < things.size(); i++){
		delete things[i];
	}

	things.clear();
}
