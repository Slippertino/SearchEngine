#pragma once

#include <iostream>
#include "mysql/jdbc.h"
#include "cpr/cpr.h"
#include <unordered_map>
#include <thread_extensions/thread_pool.hpp>
#include <thread_extensions/thread_safe_containers/thread_safe_unordered_map.hpp>
#include <thread_extensions/semaphore.hpp>
#include <nlohmann/json.hpp>
//#include "builders/builder.hpp"
#include <array>
#include <queue>
#include <regex>
#include "se_config/se_config.hpp"
#include "se_primary_domain/domains/se_pages_indexing_domain/pi_config.hpp"
//#include "../search_engine_analyzers/tools/en_de_coder.hpp"
//#include "se_domains/se_services_infrastructure/json_encoding_converter.hpp"
//#include "search_engine.h"
//#include "se_domains/se_search_domain/services/s_db_responder_service.hpp"
//#include "se_domains/se_search_domain/services/s_search_service.hpp"

//#include <tools/bold_snippet_builder.hpp>
//#include "se_domains/se_search_domain/s_config.hpp"
//#include "../se/se_service.hpp"
//#include "search_engine_analyzers/tools/stemmer.hpp"
//#include "search_engine_analyzers/tools/html_text_analyzer.hpp"
//#include "se_service.hpp"
//#include "builders/se_service_builder.hpp"
//#include "se_domain.hpp"
#include "se_db_entities/se_db_connection.hpp"
#include "search_engine_analyzers/tools/url_analyzer.hpp"

const std::filesystem::path configure_path = std::filesystem::path(R"(..\\..\\..\\configuration.json)");

class polygon
{
private:
    semaphore sem;
    std::mutex output;
    std::mutex limiter;
    std::unordered_map<size_t, size_t> links_interpreter;
    std::unordered_map<size_t, size_t> id_threads_to_inhabit_interpreter;
    std::unordered_map<size_t, char> id_symbol_interpreter;
    std::unordered_map<char, size_t> symbol_id_interpreter;

    std::string result;

public:
    polygon() = delete;
    polygon(size_t count) : sem(count) { }

    void add_zone(size_t id, char symbol) {
        std::lock_guard<std::mutex> locker(limiter);
        id_symbol_interpreter.insert({ id, symbol });
        symbol_id_interpreter.insert({ symbol, id });
    }

    void add_link(size_t from, size_t to) {
        std::lock_guard<std::mutex> locker(limiter);
        links_interpreter.insert({ from, to });
    }

    void inhabit_zone(size_t id, size_t count) {
        std::lock_guard<std::mutex> locker(limiter);
        id_threads_to_inhabit_interpreter.insert({ id, count });
    }

    void process(size_t id) {
        if (!sem.is_connected())
            sem.connect_thread();
        sem.enter(id);
        output.lock();
        result += id_symbol_interpreter[id];
        output.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 1000 + 10));
        sem.exit();
        if (links_interpreter.find(id) != links_interpreter.end())
            process(links_interpreter[id]);
    }

    bool run() {
        std::lock_guard<std::mutex> locker(limiter);
        std::vector<std::unique_ptr<std::thread>> threads;

        for (auto& cur : id_threads_to_inhabit_interpreter)
            for (auto i = 0; i < cur.second; ++i)
                threads.push_back(std::make_unique<std::thread>(&polygon::process, this, cur.first));

        for (auto& th : threads) 
            th->join();

        sem.clear();

        return is_valid();
    }

    bool is_valid() const {
        size_t id(0);
        while (id < result.size()) {
            auto sym    = result[id];
            auto cur_id = symbol_id_interpreter.at(sym);

            if (id_threads_to_inhabit_interpreter.find(cur_id) == id_threads_to_inhabit_interpreter.end()) {
                while (result[id] == sym && id < result.size()) ++id;
                continue;
            }

            size_t seq_len = id_threads_to_inhabit_interpreter.at(cur_id);

            auto i = id;
            for (i; i < id + seq_len && i < result.size(); ++i)
                if (result[i] != sym)
                    return false;
            id = i;
        }

        return true;
    }
};

void configure_test(polygon& pol) {
    pol.add_zone(1,  '1');
    pol.add_zone(2,  '2');
    pol.add_zone(3,  '3');
    pol.add_zone(4,  '4');
    pol.add_zone(5,  '5');
    pol.add_zone(6,  '6');
    pol.add_zone(7,  '7');
    pol.add_zone(8,  '8');
    pol.add_zone(9,  '9');
    pol.add_zone(10, '0');

    pol.inhabit_zone(1, 8);
    pol.inhabit_zone(2, 7);
    pol.inhabit_zone(3, 10);
    pol.inhabit_zone(4, 12);

    pol.add_link(1, 5);
    pol.add_link(2, 6);
    pol.add_link(3, 7);
    pol.add_link(4, 8);
    pol.add_link(5, 9);
    pol.add_link(6, 9);
    pol.add_link(7, 9);
    pol.add_link(8, 9);
    pol.add_link(9, 10);
}

void test() {
    polygon pol(10);
    configure_test(pol);
    std::cout << pol.run() << std::endl;
}

int main(int argc, char** argv) { 
    setlocale(LC_ALL, "ru");
    std::srand(std::time(nullptr));

    auto config = std::shared_ptr<pi_config>(new pi_config());
    config->load(configure_path);

    /*auto config = std::shared_ptr<se_config>(new pi_config());
    config->load(configure_path);

    auto engine = builder<search_engine>().build();
    engine->setup(config);
    engine->run(1000);*/
}