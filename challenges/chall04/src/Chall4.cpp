#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include "Thing.hpp"
#include "Elif.hpp"
#include "Redlof.hpp"

using namespace std;

static char data1[PATH_MAX];
static char data2[PATH_MAX];

void banner(){

    cout << "                               pwny.racing presents..." << endl << endl;

    cout << "\x1b[1;35m\x1b[47m▄█████████\x1b[0m\x1b[1;35m▄\x1b[0m \x1b[1;35m\x1b[4" \
    "7m▄███\x1b[0m     \x1b[1;35m\x1b[47m▄███\x1b[0m \x1b[1;35m\x1b[47m▄███\x1b[0m " \
    "  \x1b[1;35m\x1b[47m▄███\x1b[0m \x1b[1;35m\x1b[47m▄█████████\x1b[0m\x1b[1;35m▄" \
    "\x1b[0m \x1b[1;35m\x1b[47m▄███\x1b[0m\n\x1b[0;45m\x1b[1;35m████▀▀▀████\x1b[0m " \
    "\x1b[1;35m████     ████\x1b[0m \x1b[1;35m█████▄ ████\x1b[0m \x1b[0;45m\x1b[1;3" \
    "5m▀▀▀▀▀▀▀████\x1b[0m \x1b[1;35m████\x1b[0m\n\x1b[1;35m████▄▄▄████\x1b[0m \x1b[" \
    "1;35m████  ▄  ████\x1b[0m \x1b[1;35m███████████\x1b[0m \x1b[1;35m    ▄▄▄███"    \
    "\x1b[0;45m\x1b[1;35m▀\x1b[0m \x1b[1;35m████\x1b[0m\n\x1b[1;35m██████████\x1b[0" \
    ";45m\x1b[1;35m▀\x1b[0m \x1b[1;35m████▄███▄████\x1b[0m \x1b[1;35m████\x1b[45m▀"  \
    "\x1b[0m\x1b[1;35m██████\x1b[0m     \x1b[0;45m\x1b[1;35m▀▀▀███\x1b[0m\x1b[1;35m" \
    "▄\x1b[0m \x1b[1;35m████\x1b[0m\n\x1b[1;35m████\x1b[0;35m▀▀▀▀▀▀\x1b[0m  \x1b[1;" \
    "35m██████\x1b[0;45m\x1b[1;35m▀██████\x1b[0m \x1b[1;35m████\x1b[0m \x1b[35m▀"    \
    "\x1b[0m\x1b[0;45m\x1b[1;35m▀████\x1b[0m \x1b[1;35m▄▄▄▄▄▄▄████\x1b[0m \x1b[0;45" \
    "m\x1b[1;35m▀▀▀▀\x1b[0m\n\x1b[1;35m████\x1b[0m        \x1b[1;35m████\x1b[0;45m"  \
    "\x1b[1;35m▀\x1b[0;35m▀\x1b[0m \x1b[0;35m▀\x1b[1;35m\x1b[0;45m\x1b[1;35m▀████"   \
    "\x1b[0m \x1b[1;35m████\x1b[0m   \x1b[1;35m████\x1b[0m \x1b[1;35m██████████\x1b" \
    "[0;45m\x1b[1;35m▀\x1b[0m \x1b[1;35m████\x1b[0m\n\x1b[0;45m\x1b[1;35m▀▀▀▀\x1b[0" \
    "m        \x1b[0;45m\x1b[1;35m▀▀▀▀\x1b[0m     \x1b[0;45m\x1b[1;35m▀▀▀▀\x1b[0m "  \
    "\x1b[0;45m\x1b[1;35m▀▀▀▀\x1b[0m   \x1b[0;45m\x1b[1;35m▀▀▀▀\x1b[0m \x1b[0;45m"   \
    "\x1b[1;35m▀▀▀▀▀▀▀▀▀▀\x1b[0m  \x1b[0;45m\x1b[1;35m▀▀▀▀\x1b[0m\n" << endl << endl;
}

int again(){

    char str[24];
    cout << "again? (yes please/no thanks): " << flush;
    cin.getline(str, sizeof(str));

    if (str[0] == 'y' || str[0] == 'Y'){
        cout << endl;
        return 1;
    }

    return 0;
}

int main(){

    Thing *a = NULL;
    Thing *b = NULL;

    banner();
    while (1){

		cout << "thing1: " << flush;
		cin.getline(data1, PATH_MAX);

		cout << "thing2: " << flush;
		cin.getline(data2, PATH_MAX);

		if (opendir(data1) == NULL){
			a = new Elif();
		} else {
			a = new Redlof();
		}

		if (opendir(data2) == NULL){
			b = new Elif();
		} else {
			b = new Redlof();
		}

		b->load(data2);
		a->load(data1);

		if (a->fail()){
			cout << "\x1b[31;1merror:\x1b[0m cannot load '" << data1 << "'" << endl;
			return 1;
		}

		if (b->fail()){
			cout << "\x1b[31;1merror:\x1b[0m cannot load '" << data2 << "'" << endl;
			return 1;
		}

		cout << "1:" << endl;
		cout << "\t name: " << a->get_name() << endl;
		// cout << "\t type: " << a->get_type() << endl;
		cout << "\t size: " << a->get_size() << (a->larger(b) ? " (larger)" : "") << endl;
		cout << "\t date: " << a->get_date() << (a->newer(b) ? " (newer)" : "") << endl;

		cout << endl;

		cout << "2: " << endl;
		cout << "\t name : " << b->get_name() << endl;
		// cout << "\t type : " << b->get_type() << endl;
		cout << "\t size : " << b->get_size() << (b->larger(a) ? " (larger)" : "") << endl;
		cout << "\t date : " << b->get_date() << (b->newer(a) ? " (newer)" : "") << endl << endl;
        
        if (!again())
            break;

        delete a;
        delete b;
    }

    return 0;
}
