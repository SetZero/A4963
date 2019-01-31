//
// Created by sebastian on 17.10.18.
//

#pragma once

#include <chrono>
#include <string_view>
#include <stdexcept>

namespace utils {
    template<typename T>
    struct ratio_lookup;

    template<>
    struct ratio_lookup<std::ratio<1, 1000000000000000000>> {
        static constexpr char abr_value = 'a';
        static constexpr std::string_view value = "atto";
    };

    template<>
    struct ratio_lookup<std::ratio<1, 1000000000000000>> {
        static constexpr char abr_value = 'f';
        static constexpr std::string_view value = "femto";
    };

    template<>
    struct ratio_lookup<std::ratio<1, 1000000000000>> {
        static constexpr char abr_value = 'p';
        static constexpr std::string_view value = "pico";
    };

    template<>
    struct ratio_lookup<std::ratio<1, 1000000000>> {
        static constexpr char abr_value = 'n';
        static constexpr std::string_view value = "nano";
    };

    template<>
    struct ratio_lookup<std::ratio<1, 1000000>> {
        static constexpr char abr_value = 'u';
        static constexpr std::string_view value = "micro";
    };

    template<>
    struct ratio_lookup<std::ratio<1, 1000>> {
        static constexpr char abr_value = 'm';
        static constexpr std::string_view value = "milli";
    };

    template<>
    struct ratio_lookup<std::ratio<1, 100>> {
        static constexpr char abr_value = 'c';
        static constexpr std::string_view value = "centi";
    };

    template<>
    struct ratio_lookup<std::ratio<1, 10>> {
        static constexpr char abr_value = 'd';
        static constexpr std::string_view value = "deci";
    };

    template<>
    struct ratio_lookup<std::ratio<1, 1>> {
        static constexpr char abr_value = '\0';
        static constexpr std::string_view value = "";
    };

    template<>
    struct ratio_lookup<std::ratio<10, 1>> {
        static constexpr char abr_value = 'D';
        static constexpr std::string_view value = "deca";
    };

    template<>
    struct ratio_lookup<std::ratio<100, 1>> {
        static constexpr char abr_value = 'h';
        static constexpr std::string_view value = "hecto";
    };

    template<>
    struct ratio_lookup<std::ratio<1000, 1>> {
        static constexpr char abr_value = 'k';
        static constexpr std::string_view value = "kilo";
    };

    template<>
    struct ratio_lookup<std::ratio<1000000, 1>> {
        static constexpr char abr_value = 'M';
        static constexpr std::string_view value = "mega";
    };

    template<>
    struct ratio_lookup<std::ratio<1000000000, 1>> {
        static constexpr char abr_value = 'G';
        static constexpr std::string_view value = "giga";
    };

    template<>
    struct ratio_lookup<std::ratio<1000000000000, 1>> {
        static constexpr char abr_value = 'T';
        static constexpr std::string_view value = "tera";
    };

    template<>
    struct ratio_lookup<std::ratio<1000000000000000, 1>> {
        static constexpr char abr_value = 'P';
        static constexpr std::string_view value = "peta";
    };

    template<>
    struct ratio_lookup<std::ratio<1000000000000000000, 1>> {
        static constexpr char abr_value = 'E';
        static constexpr std::string_view value = "exa";
    };

    [[nodiscard]] constexpr std::pair<std::intmax_t, std::intmax_t> getRatio(const char prefix) {
        switch (prefix) {
            case 'a':
                return {1, 1000000000000000000};
            case 'f':
                return {1, 1000000000000000};
            case 'p':
                return {1, 1000000000000};
            case 'n':
                return {1, 1000000000};
            case 'u':
                return {1, 1000000};
            case 'm':
                return {1, 1000};
            case 'c':
                return {1, 100};
            case 'd':
                return {1, 10};
            case '\0':
                return {1, 1};
            case 'D':
                return {10, 1};
            case 'H':
                return {100, 1};
            case 'k':
                return {1000, 1};
            case 'M':
                return {1000000, 1};
            case 'G':
                return {1000000000, 1};
            case 'T':
                return {1000000000000, 1};
            case 'E':
                return {1000000000000000, 1};
            default:
                throw std::runtime_error("Unknown Type!");
        }
    }
}