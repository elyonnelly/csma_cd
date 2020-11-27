#ifndef CSMACD_CSMACD_H
#define CSMACD_CSMACD_H
#include <iostream>
#include <map>
#include <set>
#include <time.h>

class CSMACD {

public:
    CSMACD(int n, bool full_log)
    {
        this->full_log = full_log;
        for (int i = 0; i < n; i++)
        {
            times[i] = 0;
            stations.insert({times[i], i});
        }
    }

    void reproduce_sending();

private:
    //для максимального кадра:
    //данные (1500 байт) + заголовок (18 байт) + преамбула (8 байт)
    //итоговое время получаем из расчета, что в Ethernet скорость 10 Мбит/c
    //t_бит = 0.1 мкс
    const double SENDING_TIME = 1526 * 8 * 0.1;
    const double SLOT_TIME = 51.2;
    const double INTERFRAME_INTERVAL = 9.6;
    const int MAX_ATTEMPT = 16;

    bool full_log;
    std::map<int, double> times;
    //мы поддерживаем станции в сортированном порядке по времени, когда каждая их них собирается отправлять кадр
    std::set<std::pair<double, int> > stations;
    std::map<int, int> attempt;

    void handle_collisions(std::set<int> &has_collision);

    void handle_end_of_frame_sending(double now);

    double backoff(int iteration);

    void update_station_sending_time(int station_index, double new_time);
};


#endif //CSMACD_CSMACD_H
