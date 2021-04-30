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
        //SortThread = new thread(&Visualizer::HeapSort, this);
        //SortThread = new thread(&Visualizer::MergeSort, this);
        //SortThread = new thread(&Visualizer::QuickSort, this);
        //SortThread = new thread(&Visualizer::ShellSort, this);
        //SortThread = new thread(&Visualizer::CycleSort, this);
        SortThread = new thread(&Visualizer::PancakeSort, this);

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

    void HeapSort()
    {

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

    void QuickSort()
    {
        std::function<void(int, int)> RQuickSort = [&](int low, int high)
        {
            if (low < high)
            {
                SortMutex.lock();
                int pivot = Array[high];
                int i = (low - 1);
                SortMutex.unlock();

                for (int j = low; j <= high - 1; j++)
                {
                    SortMutex.lock();
                    if (Array[j] <= pivot)
                    {
                        i++;
                        swap(Array[i], Array[j]);
                    }
                    SortMutex.unlock();
                    //std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
                    std::this_thread::sleep_for(std::chrono::microseconds(2));
                }
                SortMutex.lock();
                swap(Array[i + 1], Array[high]);
                SortMutex.unlock();

                int pi = i + 1;

                RQuickSort(low, pi - 1);
                RQuickSort(pi + 1, high);
            }
        };
        RQuickSort(0, n - 1);
    }

    void ShellSort()
    {
        for (int gap = n / 2; gap > 0; gap /= 2)
        {
            for (int i = gap; i < n; ++i)
            {
                SortMutex.lock();
                int temp = Array[i];

                int j;
                for (j = i; j >= gap && Array[j - gap] > temp; j -= gap)
                    Array[j] = Array[j - gap];

                Array[j] = temp;
                SortMutex.unlock();
                std::this_thread::sleep_for(std::chrono::microseconds(2));
            }
        }
    }

    void CycleSort()
    {
        int writes = 0;

        for (int cycle_start = 0; cycle_start <= n - 2; cycle_start++) 
        {
            SortMutex.lock();
            int item = Array[cycle_start];

            int pos = cycle_start;
            for (int i = cycle_start + 1; i < n; i++)
                if (Array[i] < item)
                    pos++;
            SortMutex.unlock();

            if (pos == cycle_start)
                continue;

            SortMutex.lock();
            while (item == Array[pos])
                pos += 1;

            if (pos != cycle_start) 
            {
                swap(item, Array[pos]);
                writes++;
            }
            SortMutex.unlock();

            while (pos != cycle_start) 
            {
                pos = cycle_start;

                SortMutex.lock();
                for (int i = cycle_start + 1; i < n; i++)
                    if (Array[i] < item)
                        pos += 1;

                while (item == Array[pos])
                    pos += 1;

                if (item != Array[pos]) 
                {
                    swap(item, Array[pos]);
                    writes++;
                }
                SortMutex.unlock();
                std::this_thread::sleep_for(std::chrono::microseconds(2));
            }
        }
    }

    //Broken
    void PancakeSort()
    {
        auto Flip = [&](int i)
        {
            int temp, start = 0;
            while (start < i) 
            {
                temp = Array[start];
                Array[start] = Array[i];
                Array[i] = temp;
                start++;
                i--;
            }
        };

        for (int curr_size = n; curr_size > 1; --curr_size)
        {
            //int mi = findMax(arr, curr_size);
            int mi, i;
            for (mi = 0, i = 0; i < n; ++i)
            {
                SortMutex.lock();
                if (Array[i] > Array[mi])
                    mi = i;
                SortMutex.unlock();
            }

            if (mi != curr_size - 1) 
            {
                SortMutex.lock();
                Flip(mi);
                SortMutex.unlock();
                std::this_thread::sleep_for(std::chrono::microseconds(1));
                SortMutex.lock();
                Flip(curr_size - 1);
                SortMutex.unlock();
            }
        }
    }
};

int main()
{
    Visualizer v;
    if (v.Construct(1280, 720, 1, 1))
        v.Start();
}

