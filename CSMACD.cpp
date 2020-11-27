#include <iostream>
#include <map>
#include <set>

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

    void reproduce_sending()
    {
        {
            while (!stations.empty())
            {
                auto first_station = *stations.begin();
                auto current_station = ++stations.begin();
                std::set<int> has_collision;
                if (full_log)
                {
                    std::cout << "Station " << first_station.second << " " << " start sending at " << first_station.first << "\n";
                }
                //если станция current_station начнет отправку кадра раньше, чем закончит first_station
                //то возникнет коллизия, которую станции задетектируют до конца отправки
                //и потому начнут пересылать кадр сначала
                while (current_station != stations.end() && first_station.first + SENDING_TIME > (*current_station).first)
                {
                    if (full_log)
                    {
                        std::cout << "Station " << (*current_station).second << " " << " start sending at " << (*current_station).first << "\n";
                    }
                    has_collision.insert(first_station.second);
                    has_collision.insert((*current_station).second);
                    ++current_station;
                }
                handle_collisions(has_collision);

                if (has_collision.find(first_station.second) == has_collision.end())
                {
                    std::cout << "** Station " << first_station.second << " end sending at " << first_station.first + SENDING_TIME << " mcs\n";
                    stations.erase(first_station);
                    times.erase(first_station.second);

                    double now = first_station.first + SENDING_TIME;
                    handle_end_of_frame_sending(now);
                }
            }
        }
    }

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

    void handle_collisions(std::set<int> &has_collision)  {
        for (auto station_index : has_collision)
        {
            if (full_log)
            {
                std::cout << station_index << " occur to collision\n";
            }
            attempt[station_index]++;
            //верхнее ограничение на количество отправок -- 16
            if (attempt[station_index] == MAX_ATTEMPT)
            {
                //если они истекли, то станция перестает пытаться отправить кадр
                stations.erase({times[station_index], station_index});
                times.erase(station_index);
                std::cout << "Station " << station_index << " could not send frame\n";
                continue;
            }
            double new_time =  times[station_index] + backoff(attempt[station_index]);
            update_station_sending_time(station_index, new_time);
        }
    }

    void handle_end_of_frame_sending(double now) {
        for (auto station : times)
        {
            int station_index = station.first;
            double sending_time = station.second;
            //если время, когда сл станция хочет начать передачу
            //ближе ко времени конца передачи последнего кадра меньше чем межкадрового интервала,
            //то эта станция будет начинать передачу сразу после конца межкадрового интервала
            if (sending_time < now + INTERFRAME_INTERVAL)
            {
                sending_time = times[station_index] = now + INTERFRAME_INTERVAL;
                update_station_sending_time(station_index, sending_time);
            }
        }
    }

    double backoff(int iteration) {
        return (rand() % (1 << std::min(10, iteration) )) * SLOT_TIME;
    }

    void update_station_sending_time(int station_index, double new_time) {
        stations.erase({times[station_index], station_index});
        times[station_index] = new_time;
        stations.insert({times[station_index], station_index});
    }
};