#define OLC_PGE_APPLICATION
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include "olcPixelGameEngine.h"
using namespace olc;
using std::unique_ptr;
using std::thread;
using std::mutex;
using std::chrono::steady_clock;
using std::chrono::time_point;
using std::swap;

class Visualizer : public PixelGameEngine
{
    unique_ptr<int[]> Array;
    int n;
    thread* SortThread;
    mutex SortMutex;
    //steady_clock::time_point StartTime;

public:
    Visualizer()
    {
        sAppName = "Sort Visualizer";
    }

    bool OnUserCreate() override
    {
        srand(time(0));

        n = ScreenWidth();
        Array.reset(new int[n]);

        for (int i = 0; i < n; ++i)
        {
            Array[i] = rand() % ScreenHeight();
        }

        //StartTime = steady_clock::now();
        
        //SortThread = new thread(&Visualizer::BubbleSort, this);
        //SortThread = new thread(&Visualizer::InsertionSort, this);
        //SortThread = new thread(&Visualizer::SelectionSort, this);
        SortThread = new thread(&Visualizer::MergeSort, this);

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(BLACK);

        SortMutex.lock();
        for (int x = 0; x < n; ++x)
        {
            int height = Array[x];
            for (int y = 0; y < height; ++y)
            {
                Draw(x, ScreenHeight() - y - 1, olc::BLUE);
            }
        }
        SortMutex.unlock();

        /*auto dur = steady_clock::now() - StartTime;
        long long secdur = std::chrono::duration_cast<std::chrono::microseconds>(dur).count();
        DrawString(10, 10, std::to_string(secdur >> 6), WHITE);*/

        return true;
    }

    void BubbleSort()
    {
        for (int i = 0; i < n - 1; ++i)
        {
            for (int j = 0; j < n - i - 1; ++j)
            {
                SortMutex.lock();
                if (Array[j] > Array[j + 1])
                {
                    int temp = Array[j];
                    Array[j] = Array[j + 1];
                    Array[j + 1] = temp;
                }
                SortMutex.unlock();
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        }
    }

    void InsertionSort()
    {
        int key;
        int j;

        for (int i = 1; i < n; ++i)
        {
            SortMutex.lock();
            key = Array[i];
            j = i - 1;

            while (j >= 0 && Array[j] > key)
            {
                Array[j + 1] = Array[j];
                SortMutex.unlock();
                --j;
                std::this_thread::sleep_for(std::chrono::nanoseconds(700));
                SortMutex.lock();
            }
            Array[j + 1] = key;
            SortMutex.unlock();
        }
    }

    void SelectionSort()
    {
        int min;

        for (int i = 0; i < n - 1; ++i)
        {
            min = i;
            for (int j = i + 1; j < n; ++j)
            {
                SortMutex.lock();
                if (Array[j] < Array[min])
                    min = j;
                SortMutex.unlock();
            }

            std::this_thread::sleep_for(std::chrono::nanoseconds(1500));

            SortMutex.lock();
            int temp = Array[min];
            Array[min] = Array[i];
            Array[i] = temp;
            SortMutex.unlock();
        }
    }

    void MergeSort()
    {
        auto Merge = [&](int left, int mid, int right)
        {
            int llen = mid - left + 1;
            int rlen = right - mid;

            int * L = new int[llen];
            int * R = new int[rlen];

            SortMutex.lock();
            for (int i = 0; i < llen; ++i)
                L[i] = Array[left + i];
            //std::copy(Array[left], Array[llen], L);
            for (int j = 0; j < rlen; ++j)
                R[j] = Array[mid + j + 1];
            //std::copy(Array[mid + 1], Array[rlen], L);
            SortMutex.unlock();

            int l = 0;
            int r = 0;
            int m = left;

            while (l < llen && r < rlen)
            {
                std::this_thread::sleep_for(std::chrono::nanoseconds(1001));
                SortMutex.lock();
                if (L[l] <= R[r])
                {
                    Array[m] = L[l];
                    ++l;
                }
                else
                {
                    Array[m] = R[r];
                    ++r;
                }
                ++m;
                SortMutex.unlock();
            }

            while (l < llen)
            {
                SortMutex.lock();
                Array[m] = L[l];
                ++l;
                ++m;
                SortMutex.unlock();
            }

            while (r < rlen)
            {
                SortMutex.lock();
                Array[m] = R[r];
                ++r;
                ++m;
                SortMutex.unlock();
            }

            delete[] L;
            delete[] R;
        };
        std::function<void(int, int)> RMergeSort = [&](int left, int right)
        {
            if (left >= right)
                return;

            int mid = left + (right - left) / 2;
            RMergeSort(left, mid);
            RMergeSort(mid + 1, right);
            Merge(left, mid, right);
        };
        RMergeSort(0, n - 1);
    }
};

int main()
{
    Visualizer v;
    if (v.Construct(1280, 720, 1, 1))
        v.Start();
}

