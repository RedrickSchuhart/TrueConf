#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include "test_runner.h"

using namespace std;

//вывод контейнеров
void Print(const vector<int> &first_cont, const map<size_t, int> &second_cont) {
    cout << "vector:" << endl;
    for(auto it = first_cont.begin(); it!=first_cont.end(); it++)
        cout << (*it) << " ";
    cout << endl << "count: " << first_cont.size() << endl;
    cout << "map:" << endl;
    for(auto it = second_cont.begin(); it!=second_cont.end(); it++)
        cout << "(" << it->first << ";" << it->second << ") ";
    cout << endl << "count: " << second_cont.size() << endl << endl;
}

//заполнение контейнеров
void Fill_Conts(vector<int> &first_cont, map<size_t, int> &second_cont, unsigned int seed) {
    int num;
    srand(seed);
    for(size_t i=0; i<100; i++) {
        num = rand()%9+1;
        first_cont.push_back(num);
        second_cont[i] = num;
    }
    return;
}

//удаление элементов
template<typename T>
void Delete_Elements(T &cont, unsigned int seed) {
    size_t nums;
    srand(seed);
    nums = rand()%16;
    for(size_t i=0; i<nums; i++) {
        cont.erase(next(cont.begin(), static_cast<unsigned int>(rand())%cont.size()));
    }
    return;
}

bool Check(vector<int> &first_cont, map<size_t, int> &second_cont) {
    if(first_cont.size()!=second_cont.size())
        return false;
    auto first_it = first_cont.begin();
    for(auto second_it = second_cont.begin(); second_it!=second_cont.end(); second_it++) {
        if((*first_it)!=second_it->second)
            return false;
        first_it++;
    }
    return true;
}

//ищем элемент в возможной подпоследовательности vector[i], в случае успеха в end записываем итератор на найденный элемент
bool CheckInFirst(vector<int>::iterator begin, vector<int>::iterator &end, int num){
    while(begin!=end){
        if((*begin)==num) {
            end = begin;
            return true;
        }
        begin++;
    }
    return false;
}

//синхронизация контейнеров
void Synchron_Conts(vector<int> &first_cont, map<size_t, int> &second_cont) {
    auto sync_first_it = first_cont.begin();//итератор на след элемент вектора, после последнего синхронизированного
    int sync_num = -1, // ключ(i) синхронизированного элемента map[i]
            count_del = 0, // количество пропущенных элементов, так же далность на которую надо просматривать vector[i]
                           // сумма count_del_this и count_del_other
            last_num = 0, // предыдущее синхронизированное число
            count_del_this=0, //количество пропущенных элементов в map[i] при повторяющихся числах
            count_del_other=0; //количество пропущенных элемнтов в map[i] перед другими числами(не last_num), которые возможно не синхранизируются
    //проходим по map[i], тк в нем содержится информация о позиции элементов
    for(auto second_it = second_cont.begin(); second_it!=second_cont.end();) {

        if(last_num == second_it->second) {
            count_del_this += static_cast<int>(second_it->first) - sync_num - 1;
        } else {
            count_del_other += static_cast<int>(second_it->first) - sync_num - 1;
        }
        count_del = count_del_this + count_del_other;
        vector<int>::iterator end =
                count_del + 1 < first_cont.end()-sync_first_it?
                next(sync_first_it, count_del + 1):
                first_cont.end(); //итератор на оконачания рассматриваемой подпоследовательности vector[i]
        if(CheckInFirst(sync_first_it,
                        end,
                        second_it->second)) {
            if(last_num != second_it->second) {
                count_del_this = count_del_other;
                count_del_other = 0;
            }
            count_del -= end - sync_first_it;
            if(count_del<count_del_this) {
                count_del_this = count_del;
                count_del_other = 0;
            } else {
                count_del_other = count_del - count_del_this;
            }
            sync_first_it = first_cont.erase(sync_first_it, end);
            if(sync_first_it==first_cont.end()) {
                second_it = second_cont.erase(++second_it, second_cont.end());
                break;
            }
            last_num = *sync_first_it;
            sync_num = static_cast<int>(second_it->first);
            sync_first_it++;
            second_it++;
        } else {
            count_del_this = count_del - count_del_other;
            //хоть рассматриваемый элемент и равен предыдущему,
            //но мы его не нашли(значит повторяющаяся последовательность прервалась),
            //следовательно его пропуски надо записывать в count_del_other
            if(last_num == second_it->second) {
                count_del_this -= static_cast<int>(second_it->first) - sync_num - 1;
                count_del_other += static_cast<int>(second_it->first) - sync_num - 1;
            }
            sync_num = static_cast<int>(second_it->first);
            second_it = second_cont.erase(second_it);
        }

    }
    first_cont.erase(sync_first_it, first_cont.end());
    return;
}

void Test(unsigned int a, unsigned int b, unsigned int c) {
    vector<int> first_cont;
    map<size_t, int> second_cont;
    int count_del;
    first_cont.reserve(1000); // В задание не было указано количество элементов, взял 1000

    Fill_Conts(first_cont, second_cont, a);
    Delete_Elements(first_cont, b);
    Delete_Elements(second_cont, c);
    count_del = 200 - static_cast<int>(first_cont.size() + second_cont.size());
    Synchron_Conts(first_cont, second_cont);
    cout << a << " " << b << " " << c << endl;
    ASSERT(Check(first_cont, second_cont));
    ASSERT((100-static_cast<int>(first_cont.size()))<=count_del);
}

void Tests() {
    for (unsigned int a = 1; a<300; a=a+11) {
        for(unsigned int b = 1; b<300; b=b+7) {
            for(unsigned int c = 1; c<300; c=c+13) {
                Test(a, b, c);
            }
        }
    }
    Test(3123, 1123, 22);
    Test(104, 2, 80);
    Test(73, 2312, 31);
    Test(413, 23, 19);
    Test(123, 4123, 184);
    Test(23, 1212, 8449);
    Test(21, 43, 189);
}

int main()
{
    vector<int> first_cont;
    map<size_t, int> second_cont;
    first_cont.reserve(1000);

    Fill_Conts(first_cont, second_cont, 12);
    Print(first_cont, second_cont);
    Delete_Elements(first_cont, 43);
    Delete_Elements(second_cont, 14);
    Print(first_cont, second_cont);
    Synchron_Conts(first_cont, second_cont);
    Print(first_cont, second_cont);

    TestRunner tr;
    RUN_TEST(tr, Tests);
    return 0;
}
