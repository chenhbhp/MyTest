#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <Windows.h>
#include "Thread.h"
using namespace std;

void split(string sr, string delim, vector<string> *ret)
{
	int last = 0;
	int index = sr.find_first_of(delim, last);
	while (index != string::npos)
	{
		ret->push_back(sr.substr(last, index - last));
		last = index + 1;
		index = sr.find_first_of(delim, last);
	}

	if (index - last > 0)
	{
		ret->push_back(sr.substr(last, index - last));
	}
}

class GlobalFunc
{
public:
	static GlobalFunc GetSingleton()
	{
		static GlobalFunc g_GlobalFunc;
		return g_GlobalFunc;
	}

	void MainProc(void *ptr)
	{
		if (nullptr != ptr)
		{
			cout << *(string*)ptr << endl;
			delete ptr;
		}
	}
};


vector<string> DataExchange;
mutex g_Mutex;

void ThreadProcReadDelete()
{
	while (true)
	{
		cout << "this is ReadDelete thread !\n" << endl;

		if (DataExchange.empty())
		{
			return;
		}

		g_Mutex.lock();

		for (auto it : DataExchange)
		{
			string *pString = new string(it.c_str());
			GlobalFunc::GetSingleton().MainProc(pString);
		}
		DataExchange.clear();

		g_Mutex.unlock();

		Sleep(30);
	}
	
}

void ThreadProcRead()
{
	while (true)
	{
		cout << "this is Read thread !\n" << endl;

		if (DataExchange.empty())
		{
			return;
		}

		for (auto it : DataExchange)
		{
			string *pString = new string(it.c_str());
			GlobalFunc::GetSingleton().MainProc(pString);
		}
		DataExchange.clear();

		Sleep(30);
	}

}

void ThreadProcWrite(const string& srStr)
{
	while (true)
	{
		//cout << "this is Write thread !\n" << endl;
		g_Mutex.lock();

		DataExchange.push_back(srStr);

		g_Mutex.unlock();

		Sleep(30);
	}
	
}


int main(int argc, char *argv[])
{
	thread tThrRead(ThreadProcRead);
	thread tThrReadDel(ThreadProcReadDelete);
	thread tThrWrite(ThreadProcWrite,"hello");
	tThrRead.join();
	tThrReadDel.join();
	tThrWrite.join();

	while (true)
	{
		cout << "this is Main thread !\n" << endl;
		Sleep(50);
	}

	return 0;
}