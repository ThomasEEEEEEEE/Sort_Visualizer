#define OLC_PGE_APPLICATION
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <string>
#include "olcPixelGameEngine.h"
using namespace olc;
using std::unique_ptr;
using std::thread;
using std::mutex;
using std::chrono::steady_clock;
using std::chrono::time_point;
using std::swap;
using std::string;

class Visualizer : public PixelGameEngine
{
    unique_ptr<int[]> Array;
    int n;
    thread* SortThread;
    mutex SortMutex;
    bool ShowMainMenu;
    int MenuPos;
    int TextScale;
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
        ShowMainMenu = true;
        MenuPos = 0;
        TextScale = 3;

        for (int i = 0; i < n; ++i)
        {
            Array[i] = rand() % ScreenHeight();
        }

        SortThread = nullptr;
        
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        Clear(BLACK);

        if (ShowMainMenu)
        {
            auto DrawMenuString = [&](int x, int y, string str)
            {
                int boxwid = ScreenWidth() / 3;
                int boxheight = ScreenHeight() / 3;
                DrawString(boxwid * x + (boxwid - GetTextSize(str).x * TextScale) / 2, (boxheight / 2) * (y*2+1) - (GetTextSize(str).y / 2), str, WHITE, TextScale);
            };

            FillRect((MenuPos % 3) * (ScreenWidth() / 3), (MenuPos / 3) * (ScreenHeight() / 3), ScreenWidth() / 3, ScreenHeight() / 3, Pixel(255, 255, 0));
            
            DrawMenuString(0, 0, "Bubble Sort");
            DrawMenuString(1, 0, "Insertion Sort");
            DrawMenuString(2, 0, "Selection Sort");
            DrawMenuString(0, 1, "Heap Sort");
            DrawMenuString(1, 1, "Merge Sort");
            DrawMenuString(2, 1, "Quick Sort");
            DrawMenuString(0, 2, "Shell Sort");
            DrawMenuString(1, 2, "Cycle Sort");
            DrawMenuString(2, 2, "Pancake Sort");

            if (GetKey(UP).bReleased && MenuPos > 2)
                MenuPos -= 3;
            if (GetKey(DOWN).bReleased && MenuPos < 6)
                MenuPos += 3;
            if (GetKey(LEFT).bReleased && MenuPos % 3 != 0)
                --MenuPos;
            if (GetKey(RIGHT).bReleased && MenuPos % 3 != 2)
                ++MenuPos;
            if (GetKey(ENTER).bReleased || GetKey(SPACE).bReleased)
            {
                delete SortThread;
                switch (MenuPos)
                {
                case 0:
                    SortThread = new thread(&Visualizer::BubbleSort, this);
                    break;
                case 1:
                    SortThread = new thread(&Visualizer::InsertionSort, this);
                    break;
                case 2:
                    SortThread = new thread(&Visualizer::SelectionSort, this);
                    break;
                case 3:
                    SortThread = new thread(&Visualizer::HeapSort, this);
                    break;
                case 4:
                    SortThread = new thread(&Visualizer::MergeSort, this);
                    break;
                case 5:
                    SortThread = new thread(&Visualizer::QuickSort, this);
                    break;
                case 6:
                    SortThread = new thread(&Visualizer::ShellSort, this);
                    break;
                case 7:
                    SortThread = new thread(&Visualizer::CycleSort, this);
                    break;
                case 8:
                    SortThread = new thread(&Visualizer::PancakeSort, this);
                    break;
                }
                ShowMainMenu = false;
            }
        }
        else
        {
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
        }

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
        std::this_thread::sleep_for(std::chrono::seconds(3));
        ShowMainMenu = true;
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

        std::this_thread::sleep_for(std::chrono::seconds(3));
        ShowMainMenu = true;
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

        std::this_thread::sleep_for(std::chrono::seconds(3));
        ShowMainMenu = true;
    }

    void HeapSort()
    {
        std::function<void(int, int)> Heapify = [&](int len, int root)
        {
            int largest = root;
            int left = 2 * root + 1;
            int right = 2 * root + 2;

            SortMutex.lock();
            if (left < len && Array[left] > Array[largest])
                largest = left;

            if (right < len && Array[right] > Array[largest])
                largest = right;
            SortMutex.unlock();

            std::this_thread::sleep_for(std::chrono::microseconds(2));

            if (largest != root)
            {
                SortMutex.lock();
                swap(Array[root], Array[largest]);
                SortMutex.unlock();

                Heapify(len, largest);
            }
        };

        for (int i = n / 2 - 1; i >= 0; --i)
            Heapify(n, i);

        for (int i = n - 1; i >= 0; --i)
        {
            SortMutex.lock();
            swap(Array[0], Array[i]);
            SortMutex.unlock();
            std::this_thread::sleep_for(std::chrono::microseconds(2));

            Heapify(i, 0);
        }

        std::this_thread::sleep_for(std::chrono::seconds(3));
        ShowMainMenu = true;
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

        std::this_thread::sleep_for(std::chrono::seconds(3));
        ShowMainMenu = true;
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

        std::this_thread::sleep_for(std::chrono::seconds(3));
        ShowMainMenu = true;
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

        std::this_thread::sleep_for(std::chrono::seconds(3));
        ShowMainMenu = true;
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

        std::this_thread::sleep_for(std::chrono::seconds(3));
        ShowMainMenu = true;
    }

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

        auto FindMax = [&](int size)
        {
            int mi, i;
            for (mi = 0, i = 0; i < size; ++i)
            {
                SortMutex.lock();
                if (Array[i] > Array[mi])
                    mi = i;
                SortMutex.unlock();
            }
            return mi;
        };

        for (int curr_size = n; curr_size > 1; --curr_size)
        {
            int mi = FindMax(curr_size);

            if (mi != curr_size - 1) 
            {
                SortMutex.lock();
                Flip(mi);
                SortMutex.unlock();
                std::this_thread::sleep_for(std::chrono::microseconds(3));
                SortMutex.lock();
                Flip(curr_size - 1);
                SortMutex.unlock();
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(3));
        ShowMainMenu = true;
    }
};

int main()
{
    Visualizer v;
    if (v.Construct(1280, 720, 1, 1))
        v.Start();
}

