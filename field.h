#ifndef FIELD_H
#define FIELD_H

#include <random>
#include <chrono>
#include <iostream>
#include <vector>

class FieldGenerator;

class Field {
public:
    struct Statistics {
        double a[3][8];
        double expectedValue;
        double standartDeviation;
        double hit;
        void setZero() {
            expectedValue = 0;
            standartDeviation = 0;
            hit = 0;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 8; j++) {
                    a[i][j] = 0;
                }
            }
        }
    };

    struct Combinations {
        unsigned char a[5][2];
        unsigned int size;
        Combinations(): size(0) {}
        void add(unsigned char type, unsigned char count) {
            a[size][0] = type;
            a[size][1] = count;
            size++;
        }
    };

    static constexpr unsigned int wins[3][8] = {{3, 5, 7, 9, 12, 15, 20, 30},
                                                {15, 30, 50, 60, 75, 90, 120, 150},
                                                {45, 75, 150, 250, 350, 500, 750, 1000}};



    Field();
    unsigned char at(size_t i, size_t j) const;
    unsigned int win() const;
    Combinations combinations() const;
    static unsigned int win(Combinations combs);


    static void __print(const std::vector<unsigned int>& kType, const std::vector<unsigned int>& kTable, const std::vector<unsigned int>& kColor);
private:
    unsigned char a[3][5];

    friend FieldGenerator;
};

class FieldGenerator {
public:
    FieldGenerator();
    void generateUniform(Field& field);
    void generate(Field& field);
private:
    unsigned int randomUInt(const std::vector<unsigned int>& p);
    void fill(Field& field);
    void fill(Field& field, int i, int j);


    std::default_random_engine generator;
    std::uniform_int_distribution<int> uniformColor;
};

#endif // FIELD_H
