struct player {
    int r,c,a,b;
    char ch[6],sh[6];
};

struct alien {
	int r,c;
	int pr,pc;
	int alive; /* 1=alive 0=destroyed */
	char direction; /* 'l'=left 'r'=right */
	char ch;
};

struct shoot {
	int r,c;
	int active; /* 1=active 0=inactive */
	char ch;
};

struct bomb {
	int r,c;
	int active; /* 1=active 0=inactive */
	int loop;
	char ch;
};

struct options {
	int overall,alien,shots,bombs,bombchance;
};
	
