#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <iomanip>

using namespace std;

void validate_choice(int& choice, int boundary)
{
	while (choice < 1 || choice > boundary)
	{
		cout << "Incorrect input, try again: ";
		cin >> choice;
	}
}


class graph
{

private:
	unordered_map<string, vector<string>> webgraph, keywords;
	unordered_map<string, double> PageRank, CTR;
	unordered_map<string, int> impression;
	vector<pair<double, string>> Score;

public:


	graph()
	{
		ifstream input_web("Web graph file.CSV");
		ifstream input_keywords("Keyword file.CSV");
		ifstream input_impression("Number of impressions file.CSV");
		ifstream input_CTR("CTR.CSV");
		unordered_map<string, vector<string>> transpose_web_graph;

		while (!input_web.eof())
		{
			string temp;
			input_web >> temp;
			int count = 0;
			for (int i = 0; i < temp.length(); i++)
			{
				if (temp[i] != ',')
					count++;
				else
				{
					webgraph[temp.substr(0, count)].push_back(temp.substr(count + 1, temp.length() - count - 1));
					break;
				}
			}
		}
		input_web.close();

		for (auto iter = webgraph.begin(); iter != webgraph.end(); iter++)
		{
			for (int i = 0; i < iter->second.size(); i++)
			{
				transpose_web_graph[iter->second[i]].push_back(iter->first);
			}
		}

		while (!input_keywords.eof())
		{
			string temp;
			input_keywords >> temp;
			bool flag = true;
			string name;
			string temp1 = "";

			for (int i = 0; i < temp.length(); i++)
			{
				if (temp[i] != ',' && i != temp.length() - 1)
					temp1 = temp1 + temp[i];

				else
				{
					if (flag)
					{
						name = temp1;
						temp1 = "";
						flag = false;
					}
					else
					{
						if (i == temp.length() - 1)
							temp1 = temp1 + temp[i];
						keywords[temp1].push_back(name);
						temp1 = "";
					}
				}
			}
		}
		input_keywords.close();

		while (!input_impression.eof())
		{
			string temp;
			input_impression >> temp;
			int count = 0;
			for (int i = 0; i < temp.length(); i++)
			{
				if (temp[i] != ',')
					count++;
				else
				{
					impression[temp.substr(0, count)] = stoi(temp.substr(count + 1, temp.length()));
				}
			}
		}
		input_impression.close();

		while (!input_CTR.eof())
		{
			string temp;
			input_CTR >> temp;
			int count = 0;
			for (int i = 0; i < temp.length(); i++)
			{
				if (temp[i] != ',')
					count++;
				else
				{
					CTR[temp.substr(0, count)] = stoi(temp.substr(count + 1, temp.length()));
				}
			}
		}

		input_CTR.close();

		unordered_map<string, double> iteration[4];
		for (auto iter = impression.begin(); iter != impression.end(); iter++)
			iteration[0][iter->first] = (double) 1.0 / impression.size();

		for (int i = 1; i < 4; i++)
		{
			for (auto iter = iteration[i - 1].begin(); iter != iteration[i - 1].end(); iter++)
			{
				for (int j = 0; j < transpose_web_graph[iter->first].size(); j++)
				{
					iteration[i][iter->first] += (iteration[i - 1][transpose_web_graph[iter->first][j]] / webgraph[transpose_web_graph[iter->first][j]].size());
				}
			}
		}

		if (iteration[3].size() != impression.size())
		{
			for (auto iter = impression.begin(); iter != impression.end(); iter++)
			{
				bool exists = false;
				for (auto iter2 = iteration[3].begin(); iter2 != iteration[3].end(); iter2++)
					if (iter2->first == iter->first)
					{
						exists = true;
						break;
					}
				if (!exists)
					iteration[3][iter->first] = 0;
			}

		}
		PageRank = iteration[3];


		
		Calculate_Score();
		
	}


	void Calculate_Score()
	{
		for (auto iter = PageRank.begin(); iter != PageRank.end(); iter++)
		{
			double integer = (double)(0.4 * iter->second + ((1 - (0.1 * impression[iter->first]) / (1 + 0.1 * impression[iter->first]))) * iter->second + CTR[iter->first] * ((0.1 * impression[iter->first]) / (1 + 0.1 * impression[iter->first]))) * 0.6;
			Score.push_back(pair<double, string>(integer, iter->first));
		}
		sort(Score.rbegin(), Score.rend());
	}


	void display()
	{
		for (auto iter = keywords.begin(); iter != keywords.end(); iter++)
		{
			cout << iter->first << "::  ";
			for (int i = 0; i < iter->second.size(); i++)
				cout << iter->second[i] << " ";
			cout << endl;
		}
	}


	void search()
	{
		cout << "Welcome\n";
		cout << "What would you like to do?\n";
		int choice = 2;
		while (true)
		{
			if (choice == 1)
				cout << "Would you like to\n";
			cout << "1. New Search\n";
			cout << "2. Exit\n";
			cin >> choice;
			validate_choice(choice, 2);
			if (choice == 2)
				return;

			cout << "Type in your search:\n";
			string search;
			cin.ignore();
			getline(cin, search);
			vector<string> answers;
			if ((search[0] == '\"' && search.back() == '\"') || search.find(' ') == string::npos)
			{
				
				for (unordered_map<string, vector<string>>::iterator iter = keywords.begin(); iter != keywords.end(); iter++)
				{
					if (iter->first == search)
						for (int i = 0; i < iter->second.size(); i++)
							answers.push_back(iter->second[i]);
				}
			}
			else
			{
				string search1, search2;
				int count = 0;
				for (int i = 0; i < search.size(); i++)
				{
					if (search[i] != ' ')
						count++;
					else
						break;
				}
				search1 = search.substr(0, count);
				

				if (search.find("AND") != string::npos)
				{
					search2 = search.substr(count + 5, search.length());
					vector<string> answers1, answers2;
					for (unordered_map<string, vector<string>>::iterator iter = keywords.begin(); iter != keywords.end(); iter++)
					{
						if (iter->first == search1)
							for (int i = 0; i < iter->second.size(); i++)
								answers1.push_back(iter->second[i]);
						if (iter->first == search2)
							for (int i = 0; i < iter->second.size(); i++)
								answers2.push_back(iter->second[i]);
					}
					for (int i = 0; i < answers1.size(); i++)
						for (int j = 0; j < answers2.size(); j++)
						{
							if (answers1[i] == answers2[j])
								answers.push_back(answers1[i]);
						}

				}
				else
				{
					search2 = search.substr(count + 4, search.length());
					for (unordered_map<string, vector<string>>::iterator iter = keywords.begin(); iter != keywords.end(); iter++)
					{
						if (iter->first == search1 || iter->first == search2)
							for (int i = 0; i < iter->second.size(); i++)
								answers.push_back(iter->second[i]);
					}
				}

			}
			if (answers.empty())
				cout << "No results\n";
			else
			{
				vector<string> print_answers;
				for (int i = 0; i < Score.size(); i++)
					for (int j = 0; j < answers.size(); j++)
					{
						if (Score[i].second == answers[j])
						{
							impression[answers[j]]++;
							print_answers.push_back(answers[j]);
						}
					}
				while (true)
				{
					for (int i = 0; i < print_answers.size(); i++)
						cout << i + 1 << ". " << print_answers[i] << endl;
					cout << "Would you like to\n";
					cout << "1. Choose a webpage to open\n2. New search\n3. Exit\n";
					int choice2;
					cin >> choice2;
					if (choice2 == 3)
						return;
					if (choice2 == 2)
						break;

					int choice3;
					cout << "Enter the number of website you wish to enter:\n";
					cin >> choice3;

					validate_choice(choice3, 3);
					cout << "You are now viewing " << print_answers[choice3 - 1] << endl;
					CTR[print_answers[choice3 - 1]] = (CTR[print_answers[choice3 - 1]] * (impression[print_answers[choice3 - 1]] - 1)) / impression[print_answers[choice3 - 1]];

					for (int i = 0; i < print_answers.size(); i++)
						if (i != choice3 - 1)
						{
							CTR[print_answers[i]] *= (impression[print_answers[i]]-1);
							CTR[print_answers[i]] /= impression[print_answers[i]];
						}

					

					Score.clear();
					Calculate_Score();
					cout << "Would you like to\n1. Return\n2. New search\n3. Exit\n";
					cin >> choice3;
					validate_choice(choice3, 3);
					if (choice3 == 3)
						return;
					if (choice3 == 2)
						break;

				}

			}


		}

	}


	~graph()
	{

		ofstream output_impression("Number of impressions file.CSV");
		ofstream output_CTR("CTR.CSV");


		
		for (auto iter = impression.begin(); iter != impression.end(); iter++)
			output_impression << iter->first << "," << iter->second << "\n";
		output_impression.close();

		for (auto iter = CTR.begin(); iter != CTR.end(); iter++)
			output_CTR << iter->first << "," << iter->second << "\n";
		output_CTR.close();


	}


};










int main()
{

	graph websites;
	websites.search();
	cout << "Goodbye!";



	return 0;
}