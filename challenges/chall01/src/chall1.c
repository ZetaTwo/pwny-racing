#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUF_SIZE 64

struct data {
    char  buf[BUF_SIZE];
    char *ptr;
    int   guard;
};

void banner(){

    puts("\n              \e[38;2;55;0;0m▄\e[38;2;90;0;0m▄\e[38;2;113;0;0m▄\e[38;2;124;0;0m▄\e[38;2;124;0;0m▄\e[48;2;61;0;0m\e[38;2;125;0;0m▄\e[48;2;70;0;0m\e[38;2;126;0;0m▄\e[48;2;79;0;0m\e[38;2;127;0;" \
    "0m▄\e[48;2;82;0;0m\e[38;2;127;0;0m▄\e[48;2;80;0;0m\e[38;2;128;0;0m▄\e[48;2;76;0;0m\e[38;2;128;0;0m▄\e[48;2;62;0;0m\e[38;2;129;0;0m▄\e[0m\e[38;2;130;0;0m▄\e[38;2;131;0;0m▄\e[38;2;121;0;0m▄\e[38;2;10" \
    "3;0;0m▄\e[38;2;71;0;0m▄\n          \e[38;2;80;0;0m▄\e[38;2;59;0;0m▄\e[38;2;83;0;0m▀\e[48;2;91;0;0m\e[38;2;72;0;0m▄\e[48;2;113;0;0m\e[38;2;112;0;0m▄\e[48;2;124;0;0m\e[38;2;123;0;0m▄\e[48;2;124;0;0m"  \
    "\e[38;2;123;0;0m▄\e[48;2;124;0;0m\e[38;2;125;0;0m▄\e[48;2;124;0;0m\e[38;2;127;0;0m▄\e[48;2;124;0;0m\e[38;2;128;0;0m▄\e[48;2;125;0;0m\e[38;2;128;0;0m▄\e[48;2;126;0;0m\e[38;2;129;0;0m▄\e[48;2;126;0;0" \
    "m\e[38;2;130;0;0m▄\e[48;2;127;0;0m\e[38;2;131;0;0m▄\e[48;2;128;0;0m\e[38;2;131;0;0m▄\e[48;2;129;0;0m\e[38;2;131;0;0m▄\e[48;2;129;0;0m\e[38;2;132;0;0m▄\e[48;2;131;0;0m\e[38;2;133;0;0m▄\e[48;2;132;0;" \
    "0m\e[38;2;133;0;0m▄\e[48;2;133;0;0m\e[38;2;133;0;0m▄\e[48;2;134;0;0m\e[38;2;133;0;0m▄\e[48;2;134;0;0m\e[38;2;135;0;0m▄\e[48;2;114;0;0m\e[38;2;137;0;0m▄\e[48;2;69;0;0m\e[38;2;137;0;0m▄\e[0m\e[38;2;1" \
    "31;0;0m▄\e[38;2;82;0;0m▄\n          \e[38;2;69;0;0m▄\e[38;2;124;0;0m▄\e[48;2;103;0;0m\e[38;2;125;0;0m▄\e[48;2;124;0;0m\e[38;2;110;0;0m▄\e[48;2;125;0;0m\e[38;2;122;0;0m▄\e[48;2;126;0;0m\e[38;2;118;0" \
    ";0m▄\e[48;2;127;0;0m\e[38;2;100;0;0m▄\e[48;2;128;0;0m\e[38;2;86;0;0m▄\e[48;2;127;0;0m\e[38;2;113;0;0m▄\e[48;2;128;0;0m\e[38;2;129;0;0m▄\e[48;2;129;0;0m\e[38;2;130;0;0m▄\e[48;2;130;0;0m\e[38;2;131;0" \
    ";0m▄\e[48;2;131;0;0m\e[38;2;132;0;0m▄\e[48;2;131;0;0m\e[38;2;133;0;0m▄\e[48;2;132;0;0m\e[38;2;133;0;0m▄\e[48;2;133;0;0m\e[38;2;134;0;0m▄\e[48;2;134;0;0m\e[38;2;134;0;0m▄\e[48;2;134;0;0m\e[38;2;135;" \
    "0;0m▄\e[48;2;135;0;0m\e[38;2;136;0;0m▄\e[48;2;135;0;0m\e[38;2;137;0;0m▄\e[48;2;136;0;0m\e[38;2;137;0;0m▄\e[48;2;137;0;0m\e[38;2;138;0;0m▄\e[48;2;136;0;0m\e[38;2;139;0;0m▄\e[48;2;136;0;0m\e[38;2;140" \
    ";0;0m▄\e[48;2;139;0;0m\e[38;2;139;0;0m▄\e[48;2;140;0;0m\e[38;2;138;0;0m▄\e[48;2;135;0;0m\e[38;2;141;0;0m▄\e[48;2;81;0;0m\e[38;2;142;0;0m▄\e[0m\e[38;2;133;0;0m▄\e[38;2;62;0;0m▄\n        \e[38;2;98;0" \
    ";0m▄\e[48;2;92;0;0m\e[38;2;99;0;0m▄\e[0m\e[38;2;124;0;0m▀\e[38;2;84;0;0m▀\e[38;2;78;0;0m▄\e[48;2;71;0;0m\e[38;2;128;0;0m▄\e[48;2;116;0;0m\e[38;2;126;0;0m▄\e[48;2;53;0;0m\e[38;2;128;0;0m▄\e[48;2;66;" \
    "0;0m\e[38;2;130;0;0m▄\e[48;2;109;0;0m\e[38;2;131;0;0m▄\e[48;2;131;0;0m\e[38;2;131;0;0m▄\e[48;2;130;0;0m\e[38;2;132;0;0m▄\e[48;2;131;0;0m\e[38;2;133;0;0m▄\e[48;2;132;0;0m\e[38;2;134;0;0m▄\e[48;2;133" \
    ";0;0m\e[38;2;134;0;0m▄\e[48;2;134;0;0m\e[38;2;135;0;0m▄\e[48;2;134;0;0m\e[38;2;135;0;0m▄\e[48;2;135;0;0m\e[38;2;136;0;0m▄\e[48;2;136;0;0m\e[38;2;137;0;0m▄\e[48;2;137;0;0m\e[38;2;137;0;0m▄\e[48;2;13" \
    "7;0;0m\e[38;2;138;0;0m▄\e[48;2;137;0;0m\e[38;2;139;0;0m▄\e[48;2;138;0;0m\e[38;2;140;0;0m▄\e[48;2;139;0;0m\e[38;2;140;0;0m▄\e[48;2;140;0;0m\e[38;2;141;0;0m▄\e[48;2;140;0;0m\e[38;2;142;0;0m▄\e[48;2;1" \
    "41;0;0m\e[38;2;143;0;0m▄\e[48;2;142;0;0m\e[38;2;143;0;0m▄\e[48;2;142;0;0m\e[38;2;143;0;0m▄\e[48;2;141;0;0m\e[38;2;144;0;0m▄\e[48;2;144;0;0m\e[38;2;144;0;0m▄\e[48;2;144;0;0m\e[38;2;144;0;0m▄\e[48;2;" \
    "105;0;0m\e[38;2;146;0;0m▄\e[0m\e[38;2;135;0;0m▄\e[38;2;49;0;0m▄\n      \e[38;2;56;0;0m▄\e[38;2;84;0;0m▀\e[38;2;59;0;0m▀ \e[38;2;79;0;0m▄\e[48;2;96;0;0m\e[38;2;129;0;0m▄\e[48;2;128;0;0m\e[38;2;125;0" \
    ";0m▄\e[48;2;127;0;0m\e[38;2;131;0;0m▄\e[48;2;130;0;0m\e[38;2;131;0;0m▄\e[48;2;131;0;0m\e[38;2;131;0;0m▄\e[48;2;129;0;0m\e[38;2;132;0;0m▄\e[48;2;131;0;0m\e[38;2;133;0;0m▄\e[48;2;133;0;0m\e[38;2;134;" \
    "0;0m▄\e[48;2;134;0;0m\e[38;2;134;0;0m▄\e[48;2;134;0;0m\e[38;2;135;0;0m▄\e[48;2;134;0;0m\e[38;2;136;0;0m▄\e[48;2;135;0;0m\e[38;2;137;0;0m▄\e[48;2;136;0;0m\e[38;2;137;0;0m▄\e[48;2;137;0;0m\e[38;2;138" \
    ";0;0m▄\e[48;2;137;0;0m\e[38;2;138;0;0m▄\e[48;2;138;0;0m\e[38;2;139;0;0m▄\e[48;2;139;0;0m\e[38;2;140;0;0m▄\e[48;2;140;0;0m\e[38;2;140;0;0m▄\e[48;2;140;0;0m\e[38;2;141;0;0m▄\e[48;2;140;0;0m\e[38;2;14" \
    "2;0;0m▄\e[48;2;141;0;0m\e[38;2;143;0;0m▄\e[48;2;142;0;0m\e[38;2;143;0;0m▄\e[48;2;143;0;0m\e[38;2;140;0;0m▄\e[48;2;142;0;0m\e[38;2;145;0;0m▄\e[48;2;143;0;0m\e[38;2;146;0;0m▄\e[48;2;145;0;0m\e[38;2;1" \
    "45;0;0m▄\e[48;2;146;0;0m\e[38;2;146;0;0m▄\e[48;2;146;0;0m\e[38;2;147;0;0m▄\e[48;2;146;0;0m\e[38;2;148;0;0m▄\e[48;2;145;0;0m\e[38;2;149;0;0m▄\e[48;2;148;0;0m\e[38;2;148;0;0m▄\e[48;2;149;0;0m\e[38;2;" \
    "148;0;0m▄\e[48;2;80;0;0m\e[38;2;151;0;0m▄\e[0m\e[38;2;98;0;0m▄\n          \e[48;2;129;0;0m\e[38;2;129;0;0m▄\e[48;2;94;0;0m\e[38;2;65;0;0m▄\e[48;2;103;0;0m\e[38;2;124;0;0m▄\e[48;2;131;0;0m\e[38;2;13" \
    "3;0;0m▄\e[48;2;131;0;0m\e[38;2;133;0;0m▄\e[48;2;133;0;0m\e[38;2;134;0;0m▄\e[48;2;134;0;0m\e[38;2;133;0;0m▄\e[48;2;134;0;0m\e[38;2;135;0;0m▄\e[48;2;134;0;0m\e[38;2;116;0;0m▄\e[48;2;135;0;0m\e[38;2;1" \
    "33;0;0m▄\e[48;2;136;0;0m\e[38;2;136;0;0m▄\e[48;2;137;0;0m\e[38;2;138;0;0m▄\e[48;2;137;0;0m\e[38;2;139;0;0m▄\e[48;2;138;0;0m\e[38;2;140;0;0m▄\e[48;2;139;0;0m\e[38;2;140;0;0m▄\e[48;2;140;0;0m\e[38;2;" \
    "140;0;0m▄\e[48;2;140;0;0m\e[38;2;141;0;0m▄\e[48;2;141;0;0m\e[38;2;142;0;0m▄\e[48;2;142;0;0m\e[38;2;143;0;0m▄\e[48;2;143;0;0m\e[38;2;143;0;0m▄\e[48;2;143;0;0m\e[38;2;144;0;0m▄\e[48;2;142;0;0m\e[38;2" \
    ";145;0;0m▄\e[48;2;143;0;0m\e[38;2;145;0;0m▄\e[48;2;131;0;0m\e[38;2;146;0;0m▄\e[48;2;108;0;0m\e[38;2;129;0;0m▄\e[48;2;137;0;0m\e[38;2;89;0;0m▄\e[48;2;147;0;0m\e[38;2;108;0;0m▄\e[48;2;146;0;0m\e[38;2" \
    ";149;0;0m▄\e[48;2;148;0;0m\e[38;2;149;0;0m▄\e[48;2;149;0;0m\e[38;2;148;0;0m▄\e[48;2;149;0;0m\e[38;2;151;0;0m▄\e[48;2;150;0;0m\e[38;2;152;0;0m▄\e[48;2;151;0;0m\e[38;2;152;0;0m▄\e[48;2;149;0;0m\e[38;" \
    "2;152;0;0m▄\e[48;2;152;0;0m\e[38;2;150;0;0m▄\e[48;2;111;0;0m\e[38;2;154;0;0m▄\e[0m\e[38;2;117;0;0m▄\n         \e[48;2;67;0;0m\e[38;2;129;0;0m▄\e[48;2;131;0;0m\e[38;2;132;0;0m▄\e[48;2;132;0;0m\e[38;" \
    "2;133;0;0m▄\e[48;2;133;0;0m\e[38;2;134;0;0m▄\e[48;2;134;0;0m\e[38;2;134;0;0m▄\e[48;2;134;0;0m\e[38;2;134;0;0m▄\e[48;2;135;0;0m\e[38;2;132;0;0m▄\e[48;2;135;0;0m\e[38;2;137;0;0m▄\e[48;2;133;0;0m\e[38" \
    ";2;94;0;0m▄\e[0m\e[38;2;120;0;0m▀\e[48;2;138;0;0m\e[38;2;83;0;0m▄\e[48;2;138;0;0m\e[38;2;126;0;0m▄\e[48;2;137;0;0m\e[38;2;140;0;0m▄\e[48;2;140;0;0m\e[38;2;141;0;0m▄\e[48;2;140;0;0m\e[38;2;141;0;0m▄" \
    "\e[48;2;141;0;0m\e[38;2;143;0;0m▄\e[48;2;142;0;0m\e[38;2;143;0;0m▄\e[48;2;143;0;0m\e[38;2;143;0;0m▄\e[48;2;143;0;0m\e[38;2;144;0;0m▄\e[48;2;144;0;0m\e[38;2;145;0;0m▄\e[48;2;145;0;0m\e[38;2;146;0;0m" \
    "▄\e[48;2;146;0;0m\e[38;2;146;0;0m▄\e[48;2;146;0;0m\e[38;2;147;0;0m▄\e[48;2;146;0;0m\e[38;2;148;0;0m▄\e[48;2;146;0;0m\e[38;2;149;0;0m▄\e[48;2;148;0;0m\e[38;2;148;0;0m▄\e[48;2;145;0;0m\e[38;2;149;0;0" \
    "m▄\e[48;2;77;0;0m\e[38;2;150;0;0m▄\e[48;2;63;0;0m\e[38;2;89;0;0m▄\e[0m\e[38;2;143;0;0m▀\e[48;2;152;0;0m\e[38;2;110;0;0m▄\e[48;2;150;0;0m\e[38;2;153;0;0m▄\e[48;2;151;0;0m\e[38;2;154;0;0m▄\e[48;2;153" \
    ";0;0m\e[38;2;153;0;0m▄\e[48;2;154;0;0m\e[38;2;155;0;0m▄\e[48;2;155;0;0m\e[38;2;155;0;0m▄\e[48;2;153;0;0m\e[38;2;156;0;0m▄\e[48;2;156;0;0m\e[38;2;155;0;0m▄\e[48;2;125;0;0m\e[38;2;158;0;0m▄\e[0m\e[38" \
    ";2;131;0;0m▄\e[0m      pwny.racing presents...  \n       \e[38;2;78;0;0m▄\e[48;2;107;0;0m\e[38;2;134;0;0m▄\e[48;2;134;0;0m\e[38;2;133;0;0m▄\e[48;2;132;0;0m\e[38;2;135;0;0m▄\e[48;2;134;0;0m\e[38;2;1" \
    "35;0;0m▄\e[48;2;133;0;0m\e[38;2;137;0;0m▄\e[48;2;136;0;0m\e[38;2;128;0;0m▄\e[48;2;137;0;0m\e[38;2;104;0;0m▄\e[48;2;137;0;0m\e[38;2;73;0;0m▄\e[0m\e[38;2;119;0;0m▀    \e[38;2;73;0;0m▀\e[38;2;141;0;0m" \
    "▀\e[48;2;143;0;0m\e[38;2;140;0;0m▄\e[48;2;142;0;0m\e[38;2;144;0;0m▄\e[48;2;144;0;0m\e[38;2;145;0;0m▄\e[48;2;145;0;0m\e[38;2;146;0;0m▄\e[48;2;146;0;0m\e[38;2;146;0;0m▄\e[48;2;146;0;0m\e[38;2;147;0;0" \
    "m▄\e[48;2;146;0;0m\e[38;2;148;0;0m▄\e[48;2;147;0;0m\e[38;2;149;0;0m▄\e[48;2;148;0;0m\e[38;2;149;0;0m▄\e[48;2;149;0;0m\e[38;2;150;0;0m▄\e[48;2;149;0;0m\e[38;2;151;0;0m▄\e[48;2;150;0;0m\e[38;2;152;0;" \
    "0m▄\e[48;2;151;0;0m\e[38;2;152;0;0m▄\e[48;2;150;0;0m\e[38;2;152;0;0m▄\e[48;2;152;0;0m\e[38;2;150;0;0m▄\e[48;2;111;0;0m\e[38;2;154;0;0m▄\e[0m\e[38;2;128;0;0m▄\e[38;2;63;0;0m▀\e[38;2;148;0;0m▀\e[48;2" \
    ";155;0;0m\e[38;2;111;0;0m▄\e[48;2;154;0;0m\e[38;2;158;0;0m▄\e[48;2;156;0;0m\e[38;2;158;0;0m▄\e[48;2;158;0;0m\e[38;2;157;0;0m▄\e[48;2;158;0;0m\e[38;2;158;0;0m▄\e[48;2;156;0;0m\e[38;2;160;0;0m▄\e[48;" \
    "2;159;0;0m\e[38;2;144;0;0m▄\e[48;2;145;0;0m\e[38;2;141;0;0m▄\e[0m\e[38;2;160;0;0m▄\e[38;2;59;0;0m▄\n     \e[38;2;68;0;0m▄\e[48;2;120;0;0m\e[38;2;115;0;0m▄\e[48;2;134;0;0m\e[38;2;122;0;0m▄\e[48;2;13" \
    "3;0;0m\e[38;2;136;0;0m▄\e[48;2;136;0;0m\e[38;2;135;0;0m▄\e[48;2;136;0;0m\e[38;2;136;0;0m▄\e[48;2;137;0;0m\e[38;2;96;0;0m▄\e[0m\e[38;2;95;0;0m▀  \e[38;2;80;0;0m▄\e[38;2;104;0;0m▄\e[38;2;67;0;0m▄    " \
    " \e[38;2;49;0;0m▀\e[48;2;146;0;0m\e[38;2;93;0;0m▄\e[48;2;144;0;0m\e[38;2;147;0;0m▄\e[48;2;147;0;0m\e[38;2;148;0;0m▄\e[48;2;148;0;0m\e[38;2;149;0;0m▄\e[48;2;149;0;0m\e[38;2;149;0;0m▄\e[48;2;149;0;0m" \
    "\e[38;2;150;0;0m▄\e[48;2;149;0;0m\e[38;2;151;0;0m▄\e[48;2;150;0;0m\e[38;2;152;0;0m▄\e[48;2;151;0;0m\e[38;2;152;0;0m▄\e[48;2;152;0;0m\e[38;2;152;0;0m▄\e[48;2;152;0;0m\e[38;2;153;0;0m▄\e[48;2;153;0;0" \
    "m\e[38;2;155;0;0m▄\e[48;2;154;0;0m\e[38;2;155;0;0m▄\e[48;2;155;0;0m\e[38;2;155;0;0m▄\e[48;2;153;0;0m\e[38;2;156;0;0m▄\e[48;2;155;0;0m\e[38;2;155;0;0m▄\e[48;2;141;0;0m\e[38;2;158;0;0m▄\e[48;2;73;0;0" \
    "m\e[38;2;138;0;0m▄\e[0m\e[38;2;93;0;0m▄\e[48;2;65;0;0m\e[38;2;62;0;0m▄\e[0m\e[38;2;156;0;0m▀\e[48;2;160;0;0m\e[38;2;124;0;0m▄\e[48;2;159;0;0m\e[38;2;161;0;0m▄\e[48;2;160;0;0m\e[38;2;162;0;0m▄\e[48;" \
    "2;160;0;0m\e[38;2;161;0;0m▄\e[48;2;130;0;0m\e[38;2;164;0;0m▄\e[48;2;111;0;0m\e[38;2;136;0;0m▄\e[48;2;160;0;0m\e[38;2;81;0;0m▄\e[48;2;107;0;0m\e[38;2;123;0;0m▄\e[0m\e[38;2;150;0;0m▄\e[38;2;68;0;0m▄ " \
    "                      \e[38;2;62;0;0m▄\n     \e[38;2;71;0;0m▀\e[48;2;131;0;0m\e[38;2;137;0;0m▄\e[48;2;134;0;0m\e[38;2;75;0;0m▄\e[48;2;109;0;0m\e[38;2;97;0;0m▄\e[48;2;136;0;0m\e[38;2;138;0;0m▄\e[48;" \
    "2;139;0;0m\e[38;2;132;0;0m▄\e[0m  \e[38;2;100;0;0m▄\e[48;2;121;0;0m\e[38;2;143;0;0m▄\e[48;2;142;0;0m\e[38;2;139;0;0m▄\e[48;2;143;0;0m\e[38;2;142;0;0m▄\e[48;2;143;0;0m\e[38;2;142;0;0m▄\e[48;2;138;0;" \
    "0m\e[38;2;145;0;0m▄\e[48;2;113;0;0m\e[38;2;146;0;0m▄\e[48;2;85;0;0m\e[38;2;146;0;0m▄\e[48;2;52;0;0m\e[38;2;147;0;0m▄\e[0m\e[38;2;148;0;0m▄\e[38;2;136;0;0m▄\e[38;2;115;0;0m▄\e[48;2;124;0;0m\e[38;2;1" \
    "06;0;0m▄\e[48;2;146;0;0m\e[38;2;129;0;0m▄\e[48;2;149;0;0m\e[38;2;151;0;0m▄\e[48;2;151;0;0m\e[38;2;151;0;0m▄\e[48;2;152;0;0m\e[38;2;152;0;0m▄\e[48;2;152;0;0m\e[38;2;153;0;0m▄\e[48;2;152;0;0m\e[38;2;" \
    "154;0;0m▄\e[48;2;153;0;0m\e[38;2;155;0;0m▄\e[48;2;154;0;0m\e[38;2;155;0;0m▄\e[48;2;155;0;0m\e[38;2;155;0;0m▄\e[48;2;155;0;0m\e[38;2;156;0;0m▄\e[48;2;156;0;0m\e[38;2;157;0;0m▄\e[48;2;157;0;0m\e[38;2" \
    ";158;0;0m▄\e[48;2;158;0;0m\e[38;2;158;0;0m▄\e[48;2;158;0;0m\e[38;2;159;0;0m▄\e[48;2;156;0;0m\e[38;2;160;0;0m▄\e[48;2;159;0;0m\e[38;2;159;0;0m▄\e[48;2;132;0;0m\e[38;2;161;0;0m▄\e[48;2;107;0;0m\e[38;" \
    "2;128;0;0m▄\e[48;2;114;0;0m\e[38;2;107;0;0m▄\e[0m\e[38;2;155;0;0m▄\e[48;2;72;0;0m\e[38;2;54;0;0m▄\e[0m\e[38;2;160;0;0m▀\e[48;2;164;0;0m\e[38;2;107;0;0m▄\e[48;2;162;0;0m\e[38;2;166;0;0m▄\e[48;2;165;" \
    "0;0m\e[38;2;167;0;0m▄\e[48;2;158;0;0m\e[38;2;167;0;0m▄\e[48;2;83;0;0m\e[38;2;167;0;0m▄\e[48;2;64;0;0m\e[38;2;129;0;0m▄\e[48;2;133;0;0m\e[38;2;56;0;0m▄\e[48;2;141;0;0m\e[38;2;58;0;0m▄\e[48;2;74;0;0m" \
    "\e[38;2;114;0;0m▄\e[0m\e[38;2;140;0;0m▄\e[38;2;118;0;0m▄\e[38;2;79;0;0m▄               \e[38;2;73;0;0m▄\e[38;2;80;0;0m▄\e[38;2;73;0;0m▀\n      \e[38;2;71;0;0m▀ \e[38;2;140;0;0m▀\e[38;2;140;0;0m▀\e[" \
    "38;2;64;0;0m▀\e[38;2;110;0;0m▄\e[48;2;99;0;0m\e[38;2;143;0;0m▄\e[48;2;143;0;0m\e[38;2;141;0;0m▄\e[48;2;141;0;0m\e[38;2;145;0;0m▄\e[48;2;143;0;0m\e[38;2;146;0;0m▄\e[48;2;143;0;0m\e[38;2;146;0;0m▄\e[" \
    "48;2;144;0;0m\e[38;2;146;0;0m▄\e[48;2;144;0;0m\e[38;2;147;0;0m▄\e[48;2;144;0;0m\e[38;2;148;0;0m▄\e[48;2;146;0;0m\e[38;2;148;0;0m▄\e[48;2;147;0;0m\e[38;2;147;0;0m▄\e[48;2;149;0;0m\e[38;2;149;0;0m▄\e" \
    "[48;2;149;0;0m\e[38;2;150;0;0m▄\e[48;2;150;0;0m\e[38;2;150;0;0m▄\e[48;2;151;0;0m\e[38;2;150;0;0m▄\e[48;2;152;0;0m\e[38;2;151;0;0m▄\e[48;2;152;0;0m\e[38;2;153;0;0m▄\e[48;2;152;0;0m\e[38;2;154;0;0m▄"  \
    "\e[48;2;153;0;0m\e[38;2;155;0;0m▄\e[48;2;155;0;0m\e[38;2;155;0;0m▄\e[48;2;155;0;0m\e[38;2;156;0;0m▄\e[48;2;155;0;0m\e[38;2;157;0;0m▄\e[48;2;156;0;0m\e[38;2;158;0;0m▄\e[48;2;157;0;0m\e[38;2;158;0;0m" \
    "▄\e[48;2;158;0;0m\e[38;2;158;0;0m▄\e[48;2;158;0;0m\e[38;2;159;0;0m▄\e[48;2;159;0;0m\e[38;2;160;0;0m▄\e[48;2;160;0;0m\e[38;2;161;0;0m▄\e[48;2;161;0;0m\e[38;2;161;0;0m▄\e[48;2;161;0;0m\e[38;2;162;0;0" \
    "m▄\e[48;2;161;0;0m\e[38;2;163;0;0m▄\e[48;2;160;0;0m\e[38;2;164;0;0m▄\e[48;2;163;0;0m\e[38;2;162;0;0m▄\e[48;2;123;0;0m\e[38;2;164;0;0m▄\e[48;2;100;0;0m\e[38;2;113;0;0m▄\e[48;2;165;0;0m\e[38;2;96;0;0" \
    "m▄\e[48;2;114;0;0m\e[38;2;167;0;0m▄\e[0m\e[38;2;165;0;0m▄\e[38;2;91;0;0m▄\e[38;2;114;0;0m▀\e[38;2;168;0;0m▀\e[48;2;169;0;0m\e[38;2;91;0;0m▄\e[48;2;170;0;0m\e[38;2;150;0;0m▄\e[48;2;170;0;0m\e[38;2;1" \
    "72;0;0m▄\e[48;2;121;0;0m\e[38;2;172;0;0m▄\e[48;2;55;0;0m\e[38;2;173;0;0m▄\e[0m\e[38;2;165;0;0m▄\e[48;2;53;0;0m\e[38;2;120;0;0m▄\e[48;2;81;0;0m\e[38;2;79;0;0m▄\e[48;2;95;0;0m\e[38;2;51;0;0m▄\e[0m\e[" \
    "38;2;97;0;0m▀\e[38;2;83;0;0m▀\e[38;2;63;0;0m▀\e[38;2;58;0;0m▄\e[38;2;87;0;0m▄\e[38;2;119;0;0m▄\e[38;2;133;0;0m▄\e[48;2;65;0;0m\e[38;2;92;0;0m▄\e[0m\e[38;2;97;0;0m▀  \e[38;2;57;0;0m▄\e[38;2;153;0;0m" \
    "▄\e[48;2;113;0;0m\e[38;2;71;0;0m▄\e[0m\e[38;2;84;0;0m▀     \e[38;2;84;0;0m▄\n         \e[38;2;130;0;0m▄\e[48;2;122;0;0m\e[38;2;144;0;0m▄\e[48;2;143;0;0m\e[38;2;144;0;0m▄\e[48;2;141;0;0m\e[38;2;146;" \
    "0;0m▄\e[48;2;145;0;0m\e[38;2;111;0;0m▄\e[48;2;146;0;0m\e[38;2;82;0;0m▄\e[48;2;120;0;0m\e[38;2;91;0;0m▄\e[48;2;99;0;0m\e[38;2;123;0;0m▄\e[48;2;105;0;0m\e[38;2;103;0;0m▄\e[0m\e[38;2;111;0;0m▀\e[38;2;" \
    "131;0;0m▀\e[38;2;149;0;0m▀\e[48;2;150;0;0m\e[38;2;89;0;0m▄\e[48;2;152;0;0m\e[38;2;138;0;0m▄\e[48;2;150;0;0m\e[38;2;153;0;0m▄\e[48;2;151;0;0m\e[38;2;154;0;0m▄\e[48;2;153;0;0m\e[38;2;153;0;0m▄\e[48;2" \
    ";154;0;0m\e[38;2;155;0;0m▄\e[48;2;155;0;0m\e[38;2;155;0;0m▄\e[48;2;155;0;0m\e[38;2;156;0;0m▄\e[48;2;155;0;0m\e[38;2;157;0;0m▄\e[48;2;156;0;0m\e[38;2;158;0;0m▄\e[48;2;157;0;0m\e[38;2;158;0;0m▄\e[48;" \
    "2;158;0;0m\e[38;2;159;0;0m▄\e[48;2;158;0;0m\e[38;2;160;0;0m▄\e[48;2;159;0;0m\e[38;2;161;0;0m▄\e[48;2;160;0;0m\e[38;2;161;0;0m▄\e[48;2;161;0;0m\e[38;2;161;0;0m▄\e[48;2;161;0;0m\e[38;2;162;0;0m▄\e[48" \
    ";2;161;0;0m\e[38;2;163;0;0m▄\e[48;2;162;0;0m\e[38;2;164;0;0m▄\e[48;2;163;0;0m\e[38;2;164;0;0m▄\e[48;2;164;0;0m\e[38;2;165;0;0m▄\e[48;2;164;0;0m\e[38;2;166;0;0m▄\e[48;2;165;0;0m\e[38;2;166;0;0m▄\e[4" \
    "8;2;164;0;0m\e[38;2;167;0;0m▄\e[48;2;167;0;0m\e[38;2;165;0;0m▄\e[48;2;99;0;0m\e[38;2;168;0;0m▄\e[48;2;96;0;0m\e[38;2;91;0;0m▄\e[48;2;168;0;0m\e[38;2;94;0;0m▄\e[48;2;169;0;0m\e[38;2;170;0;0m▄\e[48;2" \
    ";162;0;0m\e[38;2;171;0;0m▄\e[48;2;91;0;0m\e[38;2;172;0;0m▄\e[0m\e[38;2;171;0;0m▄\e[38;2;120;0;0m▄\e[38;2;53;0;0m▄\e[38;2;80;0;0m▀\e[38;2;119;0;0m▀\e[38;2;151;0;0m▀\e[38;2;170;0;0m▀\e[38;2;176;0;0m▀" \
    "\e[38;2;176;0;0m▀\e[38;2;176;0;0m▀\e[38;2;165;0;0m▀\e[38;2;158;0;0m▀\e[38;2;140;0;0m▀\e[38;2;112;0;0m▀\e[38;2;65;0;0m▀ \e[38;2;56;0;0m▄\e[38;2;129;0;0m▄\e[38;2;185;0;0m▄\e[48;2;131;0;0m\e[38;2;134;" \
    "0;0m▄\e[0m\e[38;2;163;0;0m▀      \e[38;2;61;0;0m▄\e[48;2;97;0;0m\e[38;2;141;0;0m▄\e[0m\e[38;2;53;0;0m▀\n       \e[38;2;135;0;0m▄\e[48;2;133;0;0m\e[38;2;144;0;0m▄\e[48;2;145;0;0m\e[38;2;146;0;0m▄\e[" \
    "48;2;146;0;0m\e[38;2;120;0;0m▄\e[0m\e[38;2;137;0;0m▀\e[48;2;79;0;0m\e[38;2;97;0;0m▄\e[48;2;100;0;0m\e[38;2;149;0;0m▄\e[48;2;139;0;0m\e[38;2;149;0;0m▄\e[48;2;149;0;0m\e[38;2;93;0;0m▄\e[48;2;103;0;0m" \
    "\e[38;2;55;0;0m▄\e[0m\e[38;2;59;0;0m▄     \e[38;2;81;0;0m▀\e[38;2;154;0;0m▀\e[48;2;155;0;0m\e[38;2;123;0;0m▄\e[48;2;154;0;0m\e[38;2;157;0;0m▄\e[48;2;157;0;0m\e[38;2;157;0;0m▄\e[48;2;158;0;0m\e[38;2" \
    ";157;0;0m▄\e[48;2;158;0;0m\e[38;2;159;0;0m▄\e[48;2;158;0;0m\e[38;2;160;0;0m▄\e[48;2;159;0;0m\e[38;2;161;0;0m▄\e[48;2;160;0;0m\e[38;2;161;0;0m▄\e[48;2;161;0;0m\e[38;2;162;0;0m▄\e[48;2;161;0;0m\e[38;" \
    "2;162;0;0m▄\e[48;2;162;0;0m\e[38;2;163;0;0m▄\e[48;2;163;0;0m\e[38;2;164;0;0m▄\e[48;2;164;0;0m\e[38;2;164;0;0m▄\e[48;2;164;0;0m\e[38;2;165;0;0m▄\e[48;2;164;0;0m\e[38;2;166;0;0m▄\e[48;2;165;0;0m\e[38" \
    ";2;167;0;0m▄\e[48;2;166;0;0m\e[38;2;167;0;0m▄\e[48;2;167;0;0m\e[38;2;168;0;0m▄\e[48;2;167;0;0m\e[38;2;169;0;0m▄\e[48;2;168;0;0m\e[38;2;169;0;0m▄\e[48;2;169;0;0m\e[38;2;170;0;0m▄\e[48;2;168;0;0m\e[3" \
    "8;2;170;0;0m▄\e[48;2;170;0;0m\e[38;2;169;0;0m▄\e[48;2;92;0;0m\e[38;2;172;0;0m▄\e[48;2;80;0;0m\e[38;2;105;0;0m▄\e[48;2;172;0;0m\e[38;2;61;0;0m▄\e[48;2;173;0;0m\e[38;2;152;0;0m▄\e[48;2;171;0;0m\e[38;" \
    "2;175;0;0m▄\e[48;2;174;0;0m\e[38;2;174;0;0m▄\e[48;2;175;0;0m\e[38;2;172;0;0m▄\e[48;2;168;0;0m\e[38;2;176;0;0m▄\e[48;2;131;0;0m\e[38;2;177;0;0m▄\e[48;2;91;0;0m\e[38;2;178;0;0m▄\e[48;2;55;0;0m\e[38;2" \
    ";179;0;0m▄\e[0m\e[38;2;179;0;0m▄\e[38;2;175;0;0m▄\e[38;2;173;0;0m▄\e[38;2;175;0;0m▄\e[38;2;179;0;0m▄\e[38;2;182;0;0m▄\e[48;2;78;0;0m\e[38;2;183;0;0m▄\e[48;2;122;0;0m\e[38;2;184;0;0m▄\e[48;2;165;0;0" \
    "m\e[38;2;185;0;0m▄\e[48;2;185;0;0m\e[38;2;121;0;0m▄\e[0m\e[38;2;184;0;0m▀\e[38;2;76;0;0m▀       \e[38;2;161;0;0m▄\e[48;2;182;0;0m\e[38;2;125;0;0m▄\e[0m\n    \e[38;2;65;0;0m▄\e[48;2;49;0;0m\e[38;2;1" \
    "45;0;0m▄\e[48;2;139;0;0m\e[38;2;145;0;0m▄\e[48;2;145;0;0m\e[38;2;146;0;0m▄\e[48;2;146;0;0m\e[38;2;100;0;0m▄\e[0m\e[38;2;106;0;0m▀ \e[38;2;77;0;0m▄\e[48;2;149;0;0m\e[38;2;150;0;0m▄\e[48;2;148;0;0m\e" \
    "[38;2;145;0;0m▄\e[48;2;116;0;0m\e[38;2;58;0;0m▄\e[48;2;57;0;0m\e[38;2;140;0;0m▄\e[48;2;152;0;0m\e[38;2;151;0;0m▄\e[48;2;152;0;0m\e[38;2;150;0;0m▄\e[48;2;136;0;0m\e[38;2;154;0;0m▄\e[48;2;81;0;0m\e[3" \
    "8;2;155;0;0m▄\e[0m\e[38;2;145;0;0m▄\e[38;2;81;0;0m▄  \e[38;2;139;0;0m▄\e[48;2;83;0;0m\e[38;2;158;0;0m▄\e[48;2;101;0;0m\e[38;2;100;0;0m▄\e[48;2;159;0;0m\e[38;2;73;0;0m▄\e[48;2;160;0;0m\e[38;2;144;0;" \
    "0m▄\e[48;2;159;0;0m\e[38;2;161;0;0m▄\e[48;2;161;0;0m\e[38;2;161;0;0m▄\e[48;2;161;0;0m\e[38;2;162;0;0m▄\e[48;2;162;0;0m\e[38;2;164;0;0m▄\e[48;2;163;0;0m\e[38;2;164;0;0m▄\e[48;2;164;0;0m\e[38;2;164;0" \
    ";0m▄\e[48;2;164;0;0m\e[38;2;165;0;0m▄\e[48;2;165;0;0m\e[38;2;166;0;0m▄\e[48;2;166;0;0m\e[38;2;167;0;0m▄\e[48;2;167;0;0m\e[38;2;167;0;0m▄\e[48;2;167;0;0m\e[38;2;168;0;0m▄\e[48;2;167;0;0m\e[38;2;169;" \
    "0;0m▄\e[48;2;168;0;0m\e[38;2;170;0;0m▄\e[48;2;169;0;0m\e[38;2;170;0;0m▄\e[48;2;170;0;0m\e[38;2;171;0;0m▄\e[48;2;170;0;0m\e[38;2;171;0;0m▄\e[48;2;171;0;0m\e[38;2;171;0;0m▄\e[48;2;172;0;0m\e[38;2;172" \
    ";0;0m▄\e[48;2;173;0;0m\e[38;2;173;0;0m▄\e[48;2;171;0;0m\e[38;2;174;0;0m▄\e[48;2;173;0;0m\e[38;2;173;0;0m▄\e[48;2;132;0;0m\e[38;2;176;0;0m▄\e[48;2;55;0;0m\e[38;2;169;0;0m▄\e[48;2;103;0;0m\e[38;2;87;" \
    "0;0m▄\e[48;2;175;0;0m\e[38;2;57;0;0m▄\e[48;2;177;0;0m\e[38;2;105;0;0m▄\e[48;2;178;0;0m\e[38;2;159;0;0m▄\e[48;2;176;0;0m\e[38;2;179;0;0m▄\e[48;2;174;0;0m\e[38;2;180;0;0m▄\e[48;2;174;0;0m\e[38;2;181;" \
    "0;0m▄\e[48;2;177;0;0m\e[38;2;182;0;0m▄\e[48;2;180;0;0m\e[38;2;182;0;0m▄\e[48;2;182;0;0m\e[38;2;183;0;0m▄\e[48;2;182;0;0m\e[38;2;184;0;0m▄\e[48;2;183;0;0m\e[38;2;154;0;0m▄\e[48;2;184;0;0m\e[38;2;91;" \
    "0;0m▄\e[0m\e[38;2;182;0;0m▀\e[38;2;123;0;0m▀ \e[38;2;94;0;0m▄       \e[38;2;120;0;0m▄\e[48;2;132;0;0m\e[38;2;192;0;0m▄\e[48;2;188;0;0m\e[38;2;75;0;0m▄\e[0m\n   \e[38;2;106;0;0m▄\e[48;2;146;0;0m\e[3" \
    "8;2;146;0;0m▄\e[48;2;144;0;0m\e[38;2;145;0;0m▄\e[48;2;145;0;0m\e[38;2;148;0;0m▄\e[48;2;120;0;0m\e[38;2;128;0;0m▄\e[48;2;65;0;0m\e[38;2;69;0;0m▄\e[0m\e[38;2;61;0;0m▀ \e[48;2;124;0;0m\e[38;2;152;0;0m" \
    "▄\e[48;2;152;0;0m\e[38;2;151;0;0m▄\e[48;2;100;0;0m\e[38;2;73;0;0m▄\e[48;2;96;0;0m\e[38;2;144;0;0m▄\e[48;2;153;0;0m\e[38;2;153;0;0m▄\e[48;2;152;0;0m\e[38;2;153;0;0m▄\e[48;2;155;0;0m\e[38;2;153;0;0m▄" \
    "\e[48;2;154;0;0m\e[38;2;154;0;0m▄\e[48;2;153;0;0m\e[38;2;156;0;0m▄\e[48;2;156;0;0m\e[38;2;156;0;0m▄\e[48;2;157;0;0m\e[38;2;158;0;0m▄\e[48;2;133;0;0m\e[38;2;127;0;0m▄\e[48;2;127;0;0m\e[38;2;50;0;0m▄" \
    "\e[48;2;159;0;0m\e[38;2;135;0;0m▄\e[48;2;155;0;0m\e[38;2;161;0;0m▄\e[48;2;161;0;0m\e[38;2;157;0;0m▄\e[48;2;128;0;0m\e[38;2;162;0;0m▄\e[48;2;63;0;0m\e[38;2;158;0;0m▄\e[48;2;105;0;0m\e[38;2;96;0;0m▄"  \
    "\e[48;2;163;0;0m\e[38;2;69;0;0m▄\e[48;2;164;0;0m\e[38;2;117;0;0m▄\e[48;2;164;0;0m\e[38;2;166;0;0m▄\e[48;2;163;0;0m\e[38;2;167;0;0m▄\e[48;2;165;0;0m\e[38;2;167;0;0m▄\e[48;2;166;0;0m\e[38;2;167;0;0m▄" \
    "\e[48;2;166;0;0m\e[38;2;168;0;0m▄\e[48;2;166;0;0m\e[38;2;169;0;0m▄\e[48;2;169;0;0m\e[38;2;170;0;0m▄\e[48;2;170;0;0m\e[38;2;170;0;0m▄\e[48;2;170;0;0m\e[38;2;171;0;0m▄\e[48;2;170;0;0m\e[38;2;172;0;0m" \
    "▄\e[48;2;171;0;0m\e[38;2;173;0;0m▄\e[48;2;172;0;0m\e[38;2;172;0;0m▄\e[48;2;173;0;0m\e[38;2;171;0;0m▄\e[48;2;173;0;0m\e[38;2;143;0;0m▄\e[48;2;174;0;0m\e[38;2;167;0;0m▄\e[48;2;172;0;0m\e[38;2;176;0;0" \
    "m▄\e[48;2;175;0;0m\e[38;2;176;0;0m▄\e[48;2;176;0;0m\e[38;2;177;0;0m▄\e[48;2;175;0;0m\e[38;2;175;0;0m▄\e[48;2;177;0;0m\e[38;2;176;0;0m▄\e[48;2;178;0;0m\e[38;2;175;0;0m▄\e[48;2;157;0;0m\e[38;2;179;0;" \
    "0m▄\e[48;2;92;0;0m\e[38;2;180;0;0m▄\e[48;2;54;0;0m\e[38;2;180;0;0m▄\e[0m\e[38;2;143;0;0m▄\e[48;2;63;0;0m\e[38;2;106;0;0m▄\e[48;2;76;0;0m\e[38;2;81;0;0m▄\e[48;2;78;0;0m\e[38;2;69;0;0m▄\e[48;2;71;0;0" \
    "m\e[38;2;72;0;0m▄\e[48;2;52;0;0m\e[38;2;85;0;0m▄\e[0m\e[38;2;114;0;0m▄\e[38;2;156;0;0m▄\e[38;2;187;0;0m▄\e[48;2;59;0;0m\e[38;2;188;0;0m▄\e[48;2;139;0;0m\e[38;2;99;0;0m▄\e[0m\e[38;2;139;0;0m▀      "  \
    "\e[38;2;134;0;0m▄\e[48;2;123;0;0m\e[38;2;192;0;0m▄\e[48;2;192;0;0m\e[38;2;184;0;0m▄\e[0m\e[38;2;148;0;0m▀\n   \e[48;2;139;0;0m\e[38;2;149;0;0m▄\e[48;2;148;0;0m\e[38;2;148;0;0m▄\e[48;2;147;0;0m\e[38" \
    ";2;147;0;0m▄\e[48;2;146;0;0m\e[38;2;151;0;0m▄\e[48;2;92;0;0m\e[38;2;84;0;0m▄\e[0m  \e[48;2;88;0;0m\e[38;2;138;0;0m▄\e[48;2;152;0;0m\e[38;2;154;0;0m▄\e[48;2;133;0;0m\e[38;2;127;0;0m▄\e[0m\e[38;2;89;" \
    "0;0m▀\e[38;2;155;0;0m▀\e[38;2;155;0;0m▀\e[48;2;156;0;0m\e[38;2;54;0;0m▄\e[48;2;157;0;0m\e[38;2;72;0;0m▄\e[48;2;158;0;0m\e[38;2;107;0;0m▄\e[48;2;158;0;0m\e[38;2;151;0;0m▄\e[48;2;158;0;0m\e[38;2;149;" \
    "0;0m▄\e[0m\e[38;2;131;0;0m▀   \e[38;2;105;0;0m▀\e[48;2;163;0;0m\e[38;2;80;0;0m▄\e[48;2;161;0;0m\e[38;2;164;0;0m▄\e[48;2;164;0;0m\e[38;2;165;0;0m▄\e[48;2;164;0;0m\e[38;2;165;0;0m▄\e[48;2;142;0;0m\e[" \
    "38;2;167;0;0m▄\e[48;2;82;0;0m\e[38;2;167;0;0m▄\e[48;2;68;0;0m\e[38;2;154;0;0m▄\e[48;2;96;0;0m\e[38;2;111;0;0m▄\e[48;2;126;0;0m\e[38;2;81;0;0m▄\e[48;2;143;0;0m\e[38;2;68;0;0m▄\e[48;2;140;0;0m\e[38;2" \
    ";81;0;0m▄\e[48;2;113;0;0m\e[38;2;123;0;0m▄\e[48;2;144;0;0m\e[38;2;169;0;0m▄\e[48;2;171;0;0m\e[38;2;172;0;0m▄\e[48;2;171;0;0m\e[38;2;173;0;0m▄\e[48;2;173;0;0m\e[38;2;174;0;0m▄\e[48;2;173;0;0m\e[38;2" \
    ";175;0;0m▄\e[48;2;173;0;0m\e[38;2;176;0;0m▄\e[48;2;173;0;0m\e[38;2;175;0;0m▄\e[48;2;141;0;0m\e[38;2;176;0;0m▄\e[48;2;63;0;0m\e[38;2;165;0;0m▄\e[48;2;79;0;0m\e[38;2;63;0;0m▄\e[0m\e[38;2;145;0;0m▀\e[" \
    "38;2;179;0;0m▀\e[48;2;179;0;0m\e[38;2;76;0;0m▄\e[48;2;179;0;0m\e[38;2;117;0;0m▄\e[48;2;180;0;0m\e[38;2;148;0;0m▄\e[48;2;181;0;0m\e[38;2;169;0;0m▄\e[48;2;180;0;0m\e[38;2;181;0;0m▄\e[48;2;182;0;0m\e[" \
    "38;2;183;0;0m▄\e[48;2;182;0;0m\e[38;2;184;0;0m▄\e[48;2;184;0;0m\e[38;2;185;0;0m▄\e[48;2;184;0;0m\e[38;2;181;0;0m▄\e[48;2;185;0;0m\e[38;2;172;0;0m▄\e[48;2;185;0;0m\e[38;2;154;0;0m▄\e[48;2;186;0;0m\e" \
    "[38;2;117;0;0m▄\e[48;2;187;0;0m\e[38;2;65;0;0m▄\e[0m\e[38;2;180;0;0m▀\e[38;2;110;0;0m▀      \e[38;2;57;0;0m▄\e[38;2;178;0;0m▄\e[48;2;148;0;0m\e[38;2;192;0;0m▄\e[48;2;192;0;0m\e[38;2;192;0;0m▄\e[48;" \
    "2;192;0;0m\e[38;2;123;0;0m▄\e[0m\e[38;2;79;0;0m▀\n  \e[48;2;55;0;0m\e[38;2;104;0;0m▄\e[48;2;149;0;0m\e[38;2;151;0;0m▄\e[48;2;147;0;0m\e[38;2;151;0;0m▄\e[48;2;151;0;0m\e[38;2;127;0;0m▄\e[0m\e[38;2;1" \
    "05;0;0m▀  \e[48;2;49;0;0m\e[38;2;119;0;0m▄\e[48;2;155;0;0m\e[38;2;155;0;0m▄\e[48;2;155;0;0m\e[38;2;154;0;0m▄\e[48;2;92;0;0m\e[38;2;99;0;0m▄\e[0m\e[38;2;103;0;0m▄\e[38;2;104;0;0m▄\e[38;2;77;0;0m▄   " \
    "\e[48;2;51;0;0m\e[38;2;95;0;0m▄\e[48;2;147;0;0m\e[38;2;162;0;0m▄\e[48;2;59;0;0m\e[38;2;163;0;0m▄\e[0m\e[38;2;141;0;0m▄\e[38;2;72;0;0m▄   \e[38;2;56;0;0m▀\e[48;2;152;0;0m\e[38;2;126;0;0m▄\e[48;2;159" \
    ";0;0m\e[38;2;99;0;0m▄\e[0m\e[38;2;97;0;0m▀\e[38;2;100;0;0m▀\e[38;2;169;0;0m▀\e[48;2;170;0;0m\e[38;2;137;0;0m▄\e[48;2;170;0;0m\e[38;2;172;0;0m▄\e[48;2;171;0;0m\e[38;2;173;0;0m▄\e[48;2;172;0;0m\e[38;" \
    "2;171;0;0m▄\e[48;2;173;0;0m\e[38;2;172;0;0m▄\e[48;2;173;0;0m\e[38;2;174;0;0m▄\e[48;2;174;0;0m\e[38;2;175;0;0m▄\e[48;2;174;0;0m\e[38;2;176;0;0m▄\e[48;2;175;0;0m\e[38;2;176;0;0m▄\e[48;2;176;0;0m\e[38" \
    ";2;177;0;0m▄\e[48;2;176;0;0m\e[38;2;178;0;0m▄\e[48;2;177;0;0m\e[38;2;179;0;0m▄\e[48;2;177;0;0m\e[38;2;179;0;0m▄\e[48;2;179;0;0m\e[38;2;178;0;0m▄\e[48;2;179;0;0m\e[38;2;178;0;0m▄\e[48;2;129;0;0m\e[3" \
    "8;2;181;0;0m▄\e[0m\e[38;2;179;0;0m▄\e[38;2;111;0;0m▄                 \e[38;2;86;0;0m▄\e[38;2;180;0;0m▄\e[48;2;120;0;0m\e[38;2;157;0;0m▄\e[48;2;190;0;0m\e[38;2;61;0;0m▄\e[48;2;156;0;0m\e[38;2;161;0;" \
    "0m▄\e[48;2;192;0;0m\e[38;2;170;0;0m▄\e[0m\e[38;2;152;0;0m▀\n \e[38;2;71;0;0m▄\e[48;2;148;0;0m\e[38;2;152;0;0m▄\e[48;2;150;0;0m\e[38;2;148;0;0m▄\e[48;2;152;0;0m\e[38;2;153;0;0m▄\e[48;2;78;0;0m\e[38;" \
    "2;93;0;0m▄\e[0m  \e[38;2;73;0;0m▄\e[48;2;155;0;0m\e[38;2;156;0;0m▄\e[48;2;156;0;0m\e[38;2;154;0;0m▄\e[48;2;157;0;0m\e[38;2;158;0;0m▄\e[48;2;158;0;0m\e[38;2;145;0;0m▄\e[48;2;128;0;0m\e[38;2;70;0;0m▄" \
    "\e[0m\e[38;2;83;0;0m▀   \e[38;2;70;0;0m▄\e[48;2;157;0;0m\e[38;2;164;0;0m▄\e[48;2;162;0;0m\e[38;2;162;0;0m▄\e[48;2;162;0;0m\e[38;2;165;0;0m▄\e[48;2;164;0;0m\e[38;2;165;0;0m▄\e[48;2;165;0;0m\e[38;2;1" \
    "65;0;0m▄\e[48;2;147;0;0m\e[38;2;167;0;0m▄\e[48;2;61;0;0m\e[38;2;167;0;0m▄\e[0m\e[38;2;136;0;0m▄\e[48;2;128;0;0m\e[38;2;169;0;0m▄\e[48;2;169;0;0m\e[38;2;167;0;0m▄\e[48;2;166;0;0m\e[38;2;169;0;0m▄\e[" \
    "48;2;99;0;0m\e[38;2;171;0;0m▄\e[0m\e[38;2;142;0;0m▄\e[38;2;49;0;0m▄ \e[38;2;73;0;0m▀\e[48;2;149;0;0m\e[38;2;124;0;0m▄\e[48;2;173;0;0m\e[38;2;174;0;0m▄\e[48;2;174;0;0m\e[38;2;175;0;0m▄\e[48;2;176;0;" \
    "0m\e[38;2;176;0;0m▄\e[48;2;176;0;0m\e[38;2;177;0;0m▄\e[48;2;176;0;0m\e[38;2;178;0;0m▄\e[48;2;177;0;0m\e[38;2;179;0;0m▄\e[48;2;178;0;0m\e[38;2;179;0;0m▄\e[48;2;179;0;0m\e[38;2;179;0;0m▄\e[48;2;179;0" \
    ";0m\e[38;2;181;0;0m▄\e[48;2;180;0;0m\e[38;2;182;0;0m▄\e[48;2;181;0;0m\e[38;2;182;0;0m▄\e[48;2;182;0;0m\e[38;2;182;0;0m▄\e[48;2;180;0;0m\e[38;2;183;0;0m▄\e[48;2;182;0;0m\e[38;2;183;0;0m▄\e[48;2;183;" \
    "0;0m\e[38;2;183;0;0m▄\e[48;2;180;0;0m\e[38;2;185;0;0m▄\e[48;2;124;0;0m\e[38;2;185;0;0m▄\e[48;2;58;0;0m\e[38;2;187;0;0m▄\e[0m\e[38;2;171;0;0m▄\e[38;2;131;0;0m▄\e[38;2;87;0;0m▄\e[38;2;57;0;0m▄   \e[3" \
    "8;2;58;0;0m▄\e[38;2;80;0;0m▄\e[38;2;114;0;0m▄\e[38;2;156;0;0m▄\e[38;2;191;0;0m▄\e[48;2;95;0;0m\e[38;2;192;0;0m▄\e[48;2;166;0;0m\e[38;2;192;0;0m▄\e[48;2;192;0;0m\e[38;2;182;0;0m▄\e[48;2;105;0;0m\e[3" \
    "8;2;61;0;0m▄\e[0m\e[38;2;157;0;0m▄\e[48;2;151;0;0m\e[38;2;186;0;0m▄\e[0m\e[38;2;182;0;0m▀\n \e[48;2;114;0;0m\e[38;2;141;0;0m▄\e[48;2;152;0;0m\e[38;2;155;0;0m▄\e[48;2;147;0;0m\e[38;2;155;0;0m▄\e[48;" \
    "2;155;0;0m\e[38;2;155;0;0m▄\e[0m\e[38;2;90;0;0m▀   \e[48;2;158;0;0m\e[38;2;152;0;0m▄\e[48;2;156;0;0m\e[38;2;160;0;0m▄\e[48;2;155;0;0m\e[38;2;160;0;0m▄\e[0m\e[38;2;65;0;0m▄     \e[48;2;124;0;0m\e[38" \
    ";2;159;0;0m▄\e[48;2;164;0;0m\e[38;2;165;0;0m▄\e[48;2;163;0;0m\e[38;2;165;0;0m▄\e[48;2;166;0;0m\e[38;2;165;0;0m▄\e[48;2;167;0;0m\e[38;2;165;0;0m▄\e[48;2;165;0;0m\e[38;2;165;0;0m▄\e[48;2;168;0;0m\e[3" \
    "8;2;164;0;0m▄\e[0m\e[38;2;135;0;0m▀ \e[38;2;95;0;0m▀\e[48;2;171;0;0m\e[38;2;75;0;0m▄\e[48;2;171;0;0m\e[38;2;171;0;0m▄\e[48;2;169;0;0m\e[38;2;173;0;0m▄\e[48;2;173;0;0m\e[38;2;172;0;0m▄\e[48;2;173;0;" \
    "0m\e[38;2;175;0;0m▄\e[48;2;123;0;0m\e[38;2;99;0;0m▄\e[48;2;159;0;0m\e[38;2;84;0;0m▄\e[48;2;176;0;0m\e[38;2;167;0;0m▄\e[48;2;173;0;0m\e[38;2;177;0;0m▄\e[48;2;177;0;0m\e[38;2;178;0;0m▄\e[48;2;178;0;0" \
    "m\e[38;2;178;0;0m▄\e[48;2;179;0;0m\e[38;2;179;0;0m▄\e[48;2;177;0;0m\e[38;2;180;0;0m▄\e[48;2;178;0;0m\e[38;2;181;0;0m▄\e[48;2;178;0;0m\e[38;2;182;0;0m▄\e[48;2;178;0;0m\e[38;2;182;0;0m▄\e[48;2;179;0;" \
    "0m\e[38;2;183;0;0m▄\e[48;2;179;0;0m\e[38;2;183;0;0m▄\e[48;2;180;0;0m\e[38;2;185;0;0m▄\e[48;2;181;0;0m\e[38;2;185;0;0m▄\e[48;2;183;0;0m\e[38;2;185;0;0m▄\e[48;2;185;0;0m\e[38;2;186;0;0m▄\e[48;2;185;0" \
    ";0m\e[38;2;187;0;0m▄\e[48;2;185;0;0m\e[38;2;188;0;0m▄\e[48;2;185;0;0m\e[38;2;188;0;0m▄\e[48;2;187;0;0m\e[38;2;188;0;0m▄\e[48;2;188;0;0m\e[38;2;188;0;0m▄\e[48;2;189;0;0m\e[38;2;191;0;0m▄\e[48;2;189;" \
    "0;0m\e[38;2;191;0;0m▄\e[48;2;191;0;0m\e[38;2;176;0;0m▄\e[48;2;192;0;0m\e[38;2;138;0;0m▄\e[48;2;192;0;0m\e[38;2;82;0;0m▄\e[0m\e[38;2;192;0;0m▀\e[48;2;192;0;0m\e[38;2;181;0;0m▄\e[48;2;192;0;0m\e[38;2" \
    ";192;0;0m▄\e[48;2;192;0;0m\e[38;2;160;0;0m▄\e[48;2;192;0;0m\e[38;2;82;0;0m▄\e[48;2;191;0;0m\e[38;2;167;0;0m▄\e[48;2;187;0;0m\e[38;2;192;0;0m▄\e[48;2;192;0;0m\e[38;2;167;0;0m▄\e[48;2;185;0;0m\e[38;2" \
    ";192;0;0m▄\e[48;2;192;0;0m\e[38;2;175;0;0m▄\e[0m\e[38;2;184;0;0m▀\n \e[38;2;155;0;0m▀\e[38;2;147;0;0m▀\e[38;2;101;0;0m▀     \e[48;2;132;0;0m\e[38;2;104;0;0m▄\e[48;2;161;0;0m\e[38;2;161;0;0m▄\e[48;2" \
    ";161;0;0m\e[38;2;163;0;0m▄\e[48;2;135;0;0m\e[38;2;136;0;0m▄\e[0m    \e[48;2;61;0;0m\e[38;2;101;0;0m▄\e[48;2;166;0;0m\e[38;2;167;0;0m▄\e[48;2;164;0;0m\e[38;2;168;0;0m▄\e[48;2;168;0;0m\e[38;2;135;0;0" \
    "m▄\e[48;2;168;0;0m\e[38;2;101;0;0m▄\e[48;2;169;0;0m\e[38;2;84;0;0m▄\e[48;2;170;0;0m\e[38;2;90;0;0m▄\e[48;2;149;0;0m\e[38;2;113;0;0m▄\e[0m\e[38;2;116;0;0m▄    \e[38;2;144;0;0m▀\e[48;2;172;0;0m\e[38;" \
    "2;152;0;0m▄\e[48;2;82;0;0m\e[38;2;58;0;0m▄\e[0m   \e[38;2;113;0;0m▀\e[38;2;178;0;0m▀\e[48;2;180;0;0m\e[38;2;126;0;0m▄\e[48;2;179;0;0m\e[38;2;152;0;0m▄\e[0m\e[38;2;114;0;0m▀ \e[38;2;58;0;0m▀\e[38;2;" \
    "82;0;0m▀\e[38;2;97;0;0m▀\e[38;2;114;0;0m▀\e[38;2;121;0;0m▀\e[38;2;123;0;0m▀\e[48;2;135;0;0m\e[38;2;111;0;0m▄\e[48;2;187;0;0m\e[38;2;188;0;0m▄\e[48;2;187;0;0m\e[38;2;189;0;0m▄\e[48;2;187;0;0m\e[38;2" \
    ";190;0;0m▄\e[48;2;187;0;0m\e[38;2;191;0;0m▄\e[48;2;189;0;0m\e[38;2;192;0;0m▄\e[48;2;191;0;0m\e[38;2;137;0;0m▄\e[0m\e[38;2;90;0;0m▀\e[38;2;61;0;0m▄\e[38;2;122;0;0m▄\e[38;2;168;0;0m▄\e[38;2;192;0;0m▄" \
    "\e[48;2;108;0;0m\e[38;2;110;0;0m▄\e[0m\e[38;2;109;0;0m▀ \e[38;2;83;0;0m▄\e[38;2;147;0;0m▄\e[38;2;181;0;0m▀\e[38;2;95;0;0m▀\e[38;2;65;0;0m▀\e[38;2;164;0;0m▀\n         \e[38;2;71;0;0m▀\e[48;2;163;0;0" \
    "m\e[38;2;160;0;0m▄\e[48;2;162;0;0m\e[38;2;165;0;0m▄\e[0m\e[38;2;81;0;0m▀    \e[48;2;152;0;0m\e[38;2;115;0;0m▄\e[0m\e[38;2;138;0;0m▀         \e[38;2;60;0;0m▄\e[38;2;168;0;0m▄\e[48;2;117;0;0m\e[38;2;" \
    "178;0;0m▄\e[48;2;177;0;0m\e[38;2;179;0;0m▄\e[48;2;178;0;0m\e[38;2;175;0;0m▄\e[48;2;135;0;0m\e[38;2;179;0;0m▄\e[48;2;57;0;0m\e[38;2;181;0;0m▄\e[0m\e[38;2;151;0;0m▄\e[38;2;79;0;0m▄\e[38;2;103;0;0m▄\e" \
    "[48;2;122;0;0m\e[38;2;183;0;0m▄\e[48;2;182;0;0m\e[38;2;181;0;0m▄\e[48;2;183;0;0m\e[38;2;182;0;0m▄\e[48;2;168;0;0m\e[38;2;185;0;0m▄\e[48;2;140;0;0m\e[38;2;186;0;0m▄\e[48;2;115;0;0m\e[38;2;187;0;0m▄"  \
    "\e[48;2;92;0;0m\e[38;2;188;0;0m▄\e[48;2;71;0;0m\e[38;2;188;0;0m▄\e[48;2;64;0;0m\e[38;2;157;0;0m▄\e[48;2;113;0;0m\e[38;2;65;0;0m▄\e[0m\e[38;2;188;0;0m▀\e[38;2;183;0;0m▀\e[38;2;176;0;0m▀\e[38;2;164;0" \
    ";0m▀\e[38;2;148;0;0m▀\e[48;2;167;0;0m\e[38;2;129;0;0m▄\e[48;2;170;0;0m\e[38;2;192;0;0m▄\e[48;2;185;0;0m\e[38;2;191;0;0m▄\e[48;2;192;0;0m\e[38;2;192;0;0m▄\e[48;2;192;0;0m\e[38;2;192;0;0m▄\e[48;2;192" \
    ";0;0m\e[38;2;192;0;0m▄\e[48;2;185;0;0m\e[38;2;187;0;0m▄\e[48;2;60;0;0m\e[38;2;188;0;0m▄\e[48;2;92;0;0m\e[38;2;192;0;0m▄\e[48;2;167;0;0m\e[38;2;192;0;0m▄\e[48;2;192;0;0m\e[38;2;186;0;0m▄\e[0m\e[38;2" \
    ";93;0;0m▀\n          \e[38;2;53;0;0m▀\e[38;2;88;0;0m▀                 \e[38;2;56;0;0m▀\e[38;2;116;0;0m▀\e[38;2;170;0;0m▀\e[48;2;180;0;0m\e[38;2;77;0;0m▄\e[48;2;181;0;0m\e[38;2;136;0;0m▄\e[48;2;182;" \
    "0;0m\e[38;2;176;0;0m▄\e[48;2;182;0;0m\e[38;2;184;0;0m▄\e[48;2;183;0;0m\e[38;2;178;0;0m▄\e[0m\e[38;2;152;0;0m▀\e[38;2;158;0;0m▀\e[38;2;185;0;0m▀\e[48;2;185;0;0m\e[38;2;94;0;0m▄\e[48;2;186;0;0m\e[38;" \
    "2;136;0;0m▄\e[48;2;187;0;0m\e[38;2;165;0;0m▄\e[48;2;186;0;0m\e[38;2;185;0;0m▄\e[48;2;185;0;0m\e[38;2;188;0;0m▄\e[48;2;169;0;0m\e[38;2;106;0;0m▄\e[0m \e[38;2;54;0;0m▄\e[38;2;60;0;0m▄\e[38;2;75;0;0m▄" \
    "\e[38;2;89;0;0m▄\e[38;2;111;0;0m▄\e[38;2;133;0;0m▄\e[48;2;192;0;0m\e[38;2;118;0;0m▄\e[48;2;192;0;0m\e[38;2;84;0;0m▄\e[0m\e[38;2;192;0;0m▀\e[38;2;173;0;0m▀\e[38;2;116;0;0m▀\e[48;2;105;0;0m\e[38;2;13" \
    "7;0;0m▄\e[48;2;190;0;0m\e[38;2;192;0;0m▄\e[48;2;189;0;0m\e[38;2;192;0;0m▄\e[48;2;192;0;0m\e[38;2;154;0;0m▄\e[48;2;192;0;0m\e[38;2;51;0;0m▄\e[0m\e[38;2;89;0;0m▀\n                                   "  \
    "\e[38;2;78;0;0m▀\e[38;2;80;0;0m▀      \e[38;2;73;0;0m▄\e[48;2;168;0;0m\e[38;2;188;0;0m▄\e[48;2;192;0;0m\e[38;2;188;0;0m▄\e[48;2;192;0;0m\e[38;2;188;0;0m▄\e[48;2;192;0;0m\e[38;2;188;0;0m▄\e[48;2;192" \
    ";0;0m\e[38;2;188;0;0m▄\e[48;2;192;0;0m\e[38;2;186;0;0m▄\e[48;2;192;0;0m\e[38;2;185;0;0m▄\e[48;2;192;0;0m\e[38;2;186;0;0m▄\e[0m\e[38;2;101;0;0m▀   \e[38;2;84;0;0m▄\e[48;2;84;0;0m\e[38;2;139;0;0m▄\e[" \
    "48;2;192;0;0m\e[38;2;64;0;0m▄\e[0m\e[38;2;159;0;0m▀\e[38;2;76;0;0m▀\n                                          \e[38;2;56;0;0m▀\e[48;2;192;0;0m\e[38;2;72;0;0m▄\e[48;2;192;0;0m\e[38;2;99;0;0m▄\e[48;" \
    "2;192;0;0m\e[38;2;123;0;0m▄\e[48;2;192;0;0m\e[38;2;138;0;0m▄\e[48;2;192;0;0m\e[38;2;140;0;0m▄\e[48;2;192;0;0m\e[38;2;140;0;0m▄\e[48;2;192;0;0m\e[38;2;133;0;0m▄\e[48;2;192;0;0m\e[38;2;111;0;0m▄\e[0m" \
    "\e[38;2;132;0;0m▀    \e[38;2;49;0;0m▀\e[0m\n");

    puts("\e[1mWelcome to rot13 as a service!\e[0m - For all those hard \"sanity flag\" CTF challenges");
}

void quit(){

    int i;
    printf("\nbye ");

    for (i = 0; i < 5; i++){
        usleep(400000);
        printf(".");
    }

    printf("\n");
    _exit(EXIT_SUCCESS);
}

void work(char *in, char *out, size_t sz){

    char *alpha[5] = {
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    };

    char *ret = alloca(4);
    char *enc;
    int   len = strlen(alpha[0]);
    int   rot = 13;

    if (out != 0)
        enc = out;

    while (*in && --sz > 0){
        if (!isalpha(*in)){
            *enc++ = *in++;
        } else {
            *enc++ = alpha[isupper(*in) >> 8][((int) (tolower(*in++) - 'a') + rot) % len];
        }
    }
}

void play(){

    struct data vuln;
    vuln.guard   = 0;

    size_t size  = 0;
    char  *buf   = 0;
    char   c     = 0;

    do {

        if (c > 0 && (c = getchar()) != '\n')
            ungetc(c, stdin);

        size = 0;
        printf("\nsize: ");
        scanf("%d%*c", &size);
        if (size <= sizeof(vuln))
            size = sizeof(vuln);

        printf("data: ");
        vuln.ptr = vuln.buf;
        while ((c = getchar()) != EOF && c != '\n' && c != '\r')
            *vuln.ptr++ = c;

        if (c == '\n')
            *vuln.ptr++ = '\0';

        buf = malloc(size);
        work(vuln.buf, buf, size);

        printf("resp: %s\n", buf);
        printf("more? (y/n): ");

        memset(vuln.buf, '\0', sizeof(vuln.buf));

    } while (scanf(" %c", &c) && (c == 'y' || c == 'Y'));

    quit();
}

int main(int argc, char *argv[], char *envp[]){

    register int sp asm ("sp");
    alloca((~(0x100 - (sp & 0xff)) - 0x10) + 0x40);

    alarm(60);
    signal(SIGALRM, quit);

    setvbuf(stdin, 0, _IONBF, 0);
    setvbuf(stdout, 0, _IONBF, 0);

    banner();
    play();

    return 0;
}
