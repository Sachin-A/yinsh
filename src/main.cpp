#include "../include/yinsh.h"
#include <iostream>

using namespace std;

int main() {
//cout << "so i'll be starting now\n";
	YinshState state = YinshState();

int sum = 0;
/*for (auto dir : directions) {
	for (auto jv: next_element.find(dir)->second) {
		cout << dir.x << " " << dir.y << " " << jv.first << " " << jv.second << "\n";
	}
}
cout << "Hello "<< sum << "\n";
*/
//cout << "so i'll be starting now\n";
	MonteCarloTreeSearch<YinshState, YinshMove> a(0.1, 30);
	MonteCarloTreeSearch<YinshState, YinshMove> b(0.1, 10);
//	Minimax<YinshState, YinshMove> a()

//cout << "so i'll be starting now\n";
	// state, player a, player b, no of games, verbose, generate gif
	Tester<YinshState, YinshMove> tester(&state, a, b, 1, true, true);

	tester.start();

	return 0;
}
