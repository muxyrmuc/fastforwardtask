#include <iostream>
#include <fstream>
#include <string_view>
#include <cstdint>
#include <cstdlib>
#include "exceptions.h"
#include "serializationutils.h"
#include "orderbook.h"
#include "microbenchmark.h"

namespace {

void PrintStats(MicroBenchmark::Result benchmark_result, std::size_t lines_processed, std::size_t errors) {
    std::cout << "Processed " << lines_processed << " lines" << std::endl
              << "Errors: " << errors << std::endl
              << "Average order book update time: " << benchmark_result.average_time_nsec << "ns" << std::endl;
}

void ProcessInput(std::ifstream& input, std::ofstream& output) {
    std::string line;
    if (!std::getline(input, line)) {
        throw std::runtime_error("input file is empty");
    }

    Message message = ParseLogLine(line);
    if (Message::Type::kSnapshot != message.type) {
        throw std::runtime_error("unexpected first message type");
    }

    OrderBook order_book(message.bids, message.asks);

    MicroBenchmark benchmark;
    std::size_t line_number = 1;
    std::size_t errors = 0;

    while (std::getline(input, line)) {
        ++line_number;
        try {
            message = ParseLogLine(line);
        } catch (const DeserializationError& e) {
            std::cerr << "Error parsing message at line " << line_number << ": " << e.what();
            ++errors;
            continue;
        }

        std::optional<Level> best_bid;
        std::optional<Level> best_ask;

        {
            MicroBenchmarkRunGuard guard(benchmark);
            order_book.UpdateBids(std::move(message.bids));
            order_book.UpdateAsks(std::move(message.asks));
            best_bid = order_book.GetBestBid();
            best_ask = order_book.GetBestAsk();
        }

        try {
            std::string result_json = SaveToJson(message.event_ts_mcsec, best_bid, best_ask);
            output << result_json << std::endl;
        }  catch (const SerializationError& e) {
            std::cerr << "Error saving update from line " << line_number << ": " << e.what();
            ++errors;
        }
    }

    PrintStats(benchmark.GetResult(), line_number, errors);
}

}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " [INPUT FILE] [OUTPUT FILE]" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream input(argv[1]);
    if (!input.is_open()) {
        std::cerr << "Error opening input file" << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream output(argv[2]);
    if (!output.is_open()) {
        std::cerr << "Error opening output file" << std::endl;
        return EXIT_FAILURE;
    }

    try {
        ::ProcessInput(input, output);
    } catch (const std::exception& e) {
        std::cerr << "Error processing file: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
