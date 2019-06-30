#include "STcpClient.h"
#include <queue>
#include <vector>
#include <iostream>

using namespace std;
struct path {
	int value;
	std::vector<std::vector<int> > step;
};
path MinMax(vector<vector<int>> board, int alpha, int beta, bool Max, int depth);
bool find_seq(vector<vector<int> > seq, int x, int y)
{
	for (int i = 0; i < seq.size(); i++)
	{
		if (seq[i][0] == y && seq[i][1] == x)
			return true;
	}
	return false;
}
int CalculateHeuristic(vector<vector<int>> board) {
	int l = 100;
	int f = 5;
	int s = 100;
	int val = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (board[i][j] == 1) {
				val += (j*f + l); // column of the black chess
				if (j > 5)
					val += s;
				if (i - 1 >= 0 && board[i - 1][j] == 1) {
					val += 1;
				}
				if (j - 1 >= 0 && board[i][j - 1] == 1) {
					val += 1;
				}
				if (i + 1 <= 7 && board[i + 1][j] == 1) {
					val += 1;
				}
				if (j + 1 <= 7 && board[i][j + 1] == 1) {
					val += 1;
				}
			}
			if (board[i][j] == 2) {
				val -= ((7 - j)*f + l);
				if (j < 2)
					val -= s;
				if (i - 1 >= 0 && board[i - 1][j] == 2) {
					val -= 1;
				}
				if (j - 1 >= 0 && board[i][j - 1] == 2) {
					val -= 1;
				}
				if (i + 1 <= 7 && board[i + 1][j] == 2) {
					val -= 1;
				}
				if (j + 1 <= 7 && board[i][j + 1] == 2) {
					val -= 1;
				}
			}
		}
	}
	return val;
}
vector<vector<int>> UpdateBoard(vector<vector<int>> board, vector<vector<int>> step, bool is_black) {
	for (int i = 0; i < step.size() - 1; i++) {
		if (step[i][0] == step[i + 1][0]) { // move horizontally
			board[step[i][0]][step[i][1]] = 0;
			if (is_black) { // black chess
				board[step[i + 1][0]][step[i + 1][1]] = 1;
				if (step[i][1] - step[i + 1][1] == 2 || step[i][1] - step[i + 1][1] == -2) { // hopping
					int mid = (step[i][1] + step[i + 1][1]) / 2;
					if (board[step[i][0]][mid] == 2) {
						board[step[i][0]][mid] = 0;
					}
				}
			}
			else { // white chess
				board[step[i + 1][0]][step[i + 1][1]] = 2;
				if (step[i][1] - step[i + 1][1] == 2 || step[i][1] - step[i + 1][1] == -2) {
					int mid = (step[i][1] + step[i + 1][1]) / 2;
					if (board[step[i][0]][mid] == 1) {
						board[step[i][0]][mid] = 0;
					}
				}
			}
		}
		else { // move vertically
			board[step[i][0]][step[i][1]] = 0;
			if (is_black) {
				board[step[i + 1][0]][step[i + 1][1]] = 1;
				if (step[i][0] - step[i + 1][0] == 2 || step[i][0] - step[i + 1][0] == -2) { // hopping
					int mid = (step[i][0] + step[i + 1][0]) / 2;
					if (board[mid][step[i][1]] == 2) {
						board[mid][step[i][1]] = 0;
					}
				}
			}
			else {
				board[step[i + 1][0]][step[i + 1][1]] = 2;
				if (step[i][0] - step[i + 1][0] == 2 || step[i][0] - step[i + 1][0] == -2) { // hopping
					int mid = (step[i][0] + step[i + 1][0]) / 2;
					if (board[mid][step[i][1]] == 1) {
						board[mid][step[i][1]] = 0;
					}
				}
			}
		}
	}
	return board;
}
vector<vector<vector<int>>> generate_step(std::vector<std::vector<int>> board, bool is_black)
{
	int r = rand() % 8;
	vector<vector<vector<int>>> steps;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			vector<vector<int>> seq;
			int seq_size = 0;
			vector<vector<int>> nboard = board;
			if ((nboard[i][j] == 1 && is_black) || (nboard[i][j] == 2 && !is_black))
			{
				vector<int> coord;
				coord.push_back(i);
				coord.push_back(j);
				seq.push_back(coord);
				seq_size++;
				if (j + 1 < 8)
				{
					if (nboard[i][j + 1] == 0)
					{
						coord[0] = i;
						coord[1] = j + 1;
						seq.push_back(coord);
						steps.push_back(seq);
						seq.pop_back();
					}
				}
				if (j - 1 >= 0)
				{
					if (nboard[i][j - 1] == 0)
					{
						coord[0] = i;
						coord[1] = j - 1;
						seq.push_back(coord);
						steps.push_back(seq);
						seq.pop_back();
					}
				}
				if (i + 1 < 8)
				{
					if (nboard[i + 1][j] == 0)
					{
						coord[0] = i + 1;
						coord[1] = j;
						seq.push_back(coord);
						steps.push_back(seq);
						seq.pop_back();
					}
				}
				if (i - 1 >= 0)
				{
					if (nboard[i - 1][j] == 0)
					{
						coord[0] = i - 1;
						coord[1] = j;
						seq.push_back(coord);
						steps.push_back(seq);
						seq.pop_back();
					}
				}
				//jump
				int x = j;
				int y = i;
				vector<int> dir;
				int d = 0;
				while (1)
				{
					if (dir.size() < 30)
					{

						if (x - 2 >= 0 && d < 1)
						{
							if (nboard[y][x - 2] == 0 && nboard[y][x - 1] != 0)
							{
								if (!find_seq(seq, x - 2, y))
								{
									d = 0;
									x = x - 2;
									coord[0] = y;
									coord[1] = x;
									seq.push_back(coord);
									seq_size++;
									nboard = UpdateBoard(board, seq, is_black);
									steps.push_back(seq);
									dir.push_back(1);
									continue;
								}
							}
						}
						if (x + 2 < 8 && d < 2)
						{
							if (nboard[y][x + 2] == 0 && nboard[y][x + 1] != 0)
							{
								if (!find_seq(seq, x + 2, y))
								{
									d = 0;
									x = x + 2;
									coord[0] = y;
									coord[1] = x;
									seq.push_back(coord);
									seq_size++;
									nboard = UpdateBoard(board, seq, is_black);
									steps.push_back(seq);
									dir.push_back(2);
									continue;
								}
							}
						}
						if (y + 2 < 8 && d < 3)
						{
							if (nboard[y + 2][x] == 0 && nboard[y + 1][x] != 0)
							{
								if (!find_seq(seq, x, y + 2))
								{
									d = 0;
									y = y + 2;
									coord[0] = y;
									coord[1] = x;
									seq.push_back(coord);
									seq_size++;
									nboard = UpdateBoard(board, seq, is_black);
									steps.push_back(seq);
									dir.push_back(3);
									continue;
								}
							}
						}
						if (y - 2 >= 0 && d < 4)
						{
							if (nboard[y - 2][x] == 0 && nboard[y - 1][x] != 0)
							{
								if (!find_seq(seq, x, y - 2))
								{
									d = 0;
									y = y - 2;
									coord[0] = y;
									coord[1] = x;
									seq.push_back(coord);
									seq_size++;
									nboard = UpdateBoard(board, seq, is_black);
									steps.push_back(seq);
									dir.push_back(4);
									continue;
								}
							}
						}
					}
					seq.pop_back();
					seq_size--;
					if (seq.empty())
						break;
					x = seq[seq_size - 1][1];
					y = seq[seq_size - 1][0];
					d = dir[seq_size - 1];
					dir.pop_back();
				}
			}
		}
	}
	return steps;
}

path MinMax(vector<vector<int>> board, int alpha, int beta, bool Max, int depth)
{
	path temp_path;
	path best_path;
	if (depth == 0)
	{
		best_path.value = CalculateHeuristic(board);
		return best_path;
	}
	std::vector<vector<vector<int>>> steps;
	steps = generate_step(board, Max);

	if (Max)
	{
		best_path.value = -1000000;
	}
	else
	{
		best_path.value = 1000000;
	}
	for (int i = 0; i < steps.size(); i++)
	{
		bool Min = !Max;
		temp_path = MinMax(UpdateBoard(board, steps[i], Max), alpha, beta, Min, depth - 1);
		if (Max)
		{
			if (temp_path.value >= best_path.value)
			{
				best_path = temp_path;
				best_path.step = steps[i];
			}
			if (temp_path.value > alpha)
			{
				alpha = temp_path.value;
			}
			if (beta <= alpha)
			{
				break;
			}
		}
		else
		{
			if (temp_path.value <= best_path.value)
			{
				best_path = temp_path;
				best_path.step = steps[i];
			}
			if (temp_path.value < beta)
			{
				beta = temp_path.value;
			}
			if (beta <= alpha)
			{
				break;
			}
		}
	}
	return best_path;
}
int chess_count(std::vector<std::vector<int>>& board)
{
	int count = 0;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (board[i][j] != 0)
			{
				count++;
			}
		}
	}
	return count;
}
std::vector<std::vector<int>> GetStep(std::vector<std::vector<int>>& board, bool is_black) {
	std::vector<std::vector<int>> step;
	int alpha = -1000000;
	int beta = 1000000;
	path s;
	int d = 72 / chess_count(board);
	d = 5;
	s = MinMax(board, alpha, beta, is_black, d);
	std::cout << s.value << std::endl;
	return s.step;
}

int main() {
	int id_package;
	std::vector<std::vector<int>> board, step;
	bool is_black;
	while (true) {
		if (GetBoard(id_package, board, is_black))
			break;

		step = GetStep(board, is_black);
		SendStep(id_package, step);
	}
}