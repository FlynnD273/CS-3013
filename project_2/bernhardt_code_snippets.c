//just listing out all the possible paths a small or large plane could use to take off, and storing them in an array for easy random access
int*[] longpaths = int*[4];
int*[] shortpaths = int*[14];
//paths use 0 as terminator value
longpaths[0] = {1, 4, 6, 0};
longpaths[1] = {6, 4, 1, 0};
longpaths[2] = {2, 3, 5, 0};
longpaths[3] = {5, 3, 2, 0};
shortpaths[0] = {1, 2, 0};
shortpaths[1] = {1, 4, 0};
shortpaths[2] = {2, 1, 0};
shortpaths[3] = {2, 3, 0};
shortpaths[4] = {3, 2, 0};
shortpaths[5] = {3, 4, 0};
shortpaths[6] = {3, 5, 0};
shortpaths[7] = {4, 1, 0};
shortpaths[8] = {4, 3, 0};
shortpaths[9] = {4, 6, 0};
shortpaths[10] = {5, 3, 0};
shortpaths[11] = {5, 6, 0};
shortpaths[12] = {6, 4, 0};
shortpaths[13] = {6, 5, 0};


//this is not really a thread-based solution, and if run as such would result in a lot of critical region issues. I'm sure those are solvable but it's 2am rn and im just throwing down some ideas
bool[] regionsclear = {true, true, true, true, true, true, true}; //0 is never used, but is present in array to create 1-indexing for our regions to match other areas
bool large;
int* sequence;
bool awaittakeoff() {
	if (large)
		sequence = longpaths[rand() % 4];
	else
		sequence = shortpaths[rand() % 14];
	bool clear = false;
	//maybe put some sort of wait here to allow others to check in sequence, to avoid critical region shenanigans
	while (!clear) {
		clear = true;
		for (int i = 0; clear && sequence[i]; i++) //this whole loop is overly clever and should probably not actually be used, i just thought it was cool
			if (!regionsclear[i])
				clear = false;
	}
	return true;
}


bool takeoff() {
	for (int region = 0; sequence[region]; region++) {
		randomsleep(); //I've not implemented this
		regionsclear[squence[region]] = true; //free up departed region
	}
	true;
}
