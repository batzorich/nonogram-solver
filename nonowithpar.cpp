#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <sstream>
#include <omp.h>
#include <fstream>

using namespace std;

#define THREAD_NUM 10

string solve_puzzle(std::string S, std::vector<int> C) {
	int n = S.length();
	vector<int> B, W;
	for (int i = 0; i < n; i++) {
		if (S[i] == 'X') {
			B.push_back(i);
		}
		if (S[i] == '_') {
			W.push_back(i);
		}
	}
	n += 2;
	for (int i = 0; i < (int)B.size(); i++) B[i]++;
	for (int i = 0; i < (int)W.size(); i++) W[i]++;

	int k = C.size();
	vector<char> ans(n, 0);
	vector<vector<vector<int> > > dp(2, vector<vector<int> >(n, vector<int>(k + 1, 0)));
	vector<int> sumB(n), sumW(n);
	for (int it = 0; it < 3; it++) {
		sumB.assign(n, 0);
		sumW.assign(n, 0);
		for (int x : B) sumB[x]++;
		for (int x : W) sumW[x]++;
		for (int i = 1; i < n; i++) {
			sumB[i] += sumB[i - 1];
			sumW[i] += sumW[i - 1];
		}
		if (it == 2) break;

		dp[it][0][0] = 1;
		for (int i = 1; i < n; i++) {
			for (int j = 0; j <= k; j++) {
				dp[it][i][j] |= dp[it][i - 1][j] && (sumB[i] - sumB[i - 1] == 0);
				dp[it][i][j] |= j > 0 && i > C[j - 1] && sumW[i - 1] - sumW[i - C[j - 1] - 1] == 0 && sumB[i] - sumB[i - 1] == 0 && dp[it][i - C[j - 1] - 1][j - 1];
			}
		}

		reverse(C.begin(), C.end());
		reverse(B.begin(), B.end());
		reverse(W.begin(), W.end());
		for (int i = 0; i < (int)B.size(); i++) B[i] = n - B[i] - 1;
		for (int i = 0; i < (int)W.size(); i++) W[i] = n - W[i] - 1;
	}
	reverse(dp[1].begin(), dp[1].end());
	for (int i = 1; i < n - 1; i++) {
		for (int j = 0; j <= k; j++) {
			if (dp[0][i][j] && dp[1][i][k - j]) {
				ans[i - 1] |= 1;
			}
		}
	}
	vector<int> a(n);
	for (int i = 1; i < n; i++) {
		for (int j = 0; j <= k; j++) {
			if (j > 0 && i > C[j - 1] && sumW[i - 1] - sumW[i - C[j - 1] - 1] == 0 && sumB[i] - sumB[i - 1] == 0 && dp[0][i - C[j - 1] - 1][j - 1] && dp[1][i][k - j]) {
				a[i - C[j - 1]]++;
				a[i]--;
			}
		}
	}
	for (int i = 1; i < n; i++) {
		a[i] += a[i - 1];
		if (a[i] > 0) {
			ans[i - 1] |= 2;
		}
	}
	string res = "";
	for (int i = 0; i < n - 2; i++) {
		if (ans[i] == 1) res += '_';
		else if (ans[i] == 2) res += 'X';
		else if (ans[i] == 3) res += '.';
		else assert(0);
	}
	return res;
}


struct Game {
	int dimensionrow, dimensioncol;
	vector< vector<int> > lines, columns;
	vector<string> matrix;

	Game() {}
	Game(int _dimensionrow, int _dimensioncol) {
		this->dimensionrow = _dimensionrow;
		this->dimensioncol = _dimensioncol;

		matrix.resize(dimensionrow);
		for (int i = 0; i < dimensionrow; i++) matrix[i] = string(dimensioncol, '.');

		lines.resize(dimensionrow);
		columns.resize(dimensioncol);
	}

	void showMatrix() {
		cout << "Resulting grid is: " << endl;
		for (int i = 0; i < dimensionrow; i++) {
			for (int j = 0; j < dimensioncol; j++) {
				if (j != 0) cout << " ";
				cout << matrix[i][j];
			}
			cout << endl;
		}
	}

	bool updateLine(int idx) {
		bool hasChanged = false;
		string newline = solve_puzzle(matrix[idx], lines[idx]);
		if (newline != matrix[idx]) hasChanged = true;
		matrix[idx] = newline;
		return hasChanged;
	}

	bool updateColumn(int idx) {
		bool hasChanged = false;
		string s = "";
		for (int i = 0; i < dimensionrow; i++) { s += matrix[i][idx]; }
		string newline = solve_puzzle(s, columns[idx]);
		if (newline != s) hasChanged = true;
		for (int i = 0; i < dimensionrow; i++) matrix[i][idx] = newline[i];
		return hasChanged;

	}

	void solve() {
		bool finished = false;
		while (not finished) {
			finished = true;
#pragma omp parallel for
			for (int i = 0; i < dimensionrow; i++) {
				if (updateLine(i)) finished = false;
			}
#pragma omp parallel for
			for (int i = 0; i < dimensioncol; i++) {
				if (updateColumn(i)) finished = false;
			}
		}
	}
};
void readInput(Game& game) {

	int dimensionrow, dimensioncol;

	cin >> dimensionrow;
	cin >> dimensioncol;
	getchar();

	game = Game(dimensionrow, dimensioncol);

	// reading lines
	for (int i = 0; i < dimensionrow; i++) {
		string line, token;
		getline(cin, line);

		stringstream ss(line);
		while (ss >> token) game.lines[i].push_back(stoi(token));
	}

	// reading columns
	for (int i = 0; i < dimensioncol; i++) {
		string line, token;
		getline(cin, line);

		stringstream ss(line);
		while (ss >> token) game.columns[i].push_back(stoi(token));
	}
}

int main() {
	double t = omp_get_wtime();
	Game game;
	readInput(game);
	game.solve();
	game.showMatrix();
	cout << "Time: " << omp_get_wtime() - t << endl;
	return 0;
}