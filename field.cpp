#include "field.h"

#include <algorithm>
#include <numeric>
#include <iomanip>


constexpr unsigned int Field::wins[3][8];

Field::Field() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            a[i][j] = (i + j) % 8;
        }
    }
}

unsigned char Field::at(size_t i, size_t j) const {
    return a[i][j];
}

unsigned int Field::win() const {
    return win(combinations());
}

Field::Combinations Field::combinations() const {
    Field::Combinations combs;
    unsigned char cnt;
    for (size_t j = 0; j < 3; j++) {
        cnt = 1;
        for (size_t i = 1; i < 5; i++) {
            if (a[j][0] < 8 && a[j][0] == a[j][i]) {
                cnt++;
            } else {
                break;
            }
        }
        if (cnt > 2) {
            combs.add(a[j][0], cnt);
        }
    }
    if (a[0][0] < 8 && a[0][0] == a[1][1] && a[0][0] == a[2][2]) {
        if (a[0][0] == a[1][3]) {
            if (a[0][0] == a[0][4]) {
                combs.add(a[0][0], 5);
            } else {
                combs.add(a[0][0], 4);
            }
        } else {
            combs.add(a[0][0], 3);
        }
    }
    if (a[2][0] < 8 && a[2][0] == a[1][1] && a[2][0] == a[0][2]) {
        if (a[2][0] == a[1][3]) {
            if (a[2][0] == a[2][4]) {
                combs.add(a[2][0], 5);
            } else {
                combs.add(a[2][0], 4);
            }
        } else {
            combs.add(a[2][0], 3);
        }
    }
    return combs;
}

unsigned int Field::win(Combinations combs) {
    unsigned int res = 0;
    for (unsigned int i = 0; i < combs.size; i++) {
        res += Field::wins[combs.a[i][1] - 3][combs.a[i][0]];
    }
    return res;
}

FieldGenerator::FieldGenerator(): uniformColor(std::uniform_int_distribution<int>(0, 7)) {
    generator.seed(time(nullptr));
}


void FieldGenerator::generateUniform(Field& field) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            field.a[i][j] = uniformColor(generator);
        }
    }
}

void FieldGenerator::generate(Field& field) {
    std::discrete_distribution<> rType({10475, 1200, 300, 200, 200, 200, 200, 170, 150});
    std::discrete_distribution<> rTable({1000, 900, 800, 700, 600, 500, 400, 300,
                                             90, 80, 40, 40, 40, 40, 40, 40,
                                             20, 10, 9, 8, 6, 6, 3, 3});
    std::discrete_distribution<> rColor({128, 64, 32, 16, 8, 5, 3, 3});

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            field.a[i][j] = 8;
        }
    }

    int type = rType(generator);

    if (type > 0 && type <= 3) {
        std::vector<int> rows = {0, 1, 2};
        std::random_shuffle(rows.begin(), rows.end());
        for (int i = 0; i < type; i++) {
            int t = rTable(generator);
            int size = t / 8 + 3;
            unsigned char color = t % 8;
            for (int j = 0; j < size; j++) {
                field.a[rows[i]][j] = color;
            }
        }
    } else if (type == 4) {
        unsigned char color = rColor(generator);
        field.a[0][0] = color;
        field.a[2][0] = color;
        field.a[1][1] = color;
        field.a[0][2] = color;
        field.a[2][2] = color;
    } else if (type == 5) {
        unsigned char color = rColor(generator);
        field.a[0][0] = color;
        field.a[0][1] = color;
        field.a[1][1] = color;
        field.a[0][2] = color;
        field.a[2][2] = color;
    } else if (type == 6) {
        unsigned char color = rColor(generator);
        field.a[2][0] = color;
        field.a[1][1] = color;
        field.a[2][1] = color;
        field.a[0][2] = color;
        field.a[2][2] = color;
    } else if (type == 7) {
        unsigned char color = rColor(generator);
        field.a[0][0] = color;
        field.a[2][0] = color;
        field.a[1][1] = color;
        field.a[0][2] = color;
        field.a[2][2] = color;
        field.a[1][3] = color;
    } else if (type == 8) {
        unsigned char color = rColor(generator);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                field.a[i][j] = color;
            }
        }
    }
    fill(field);
}

void FieldGenerator::fill(Field& field, int i, int j) {
    unsigned int color;
    bool bad;
    do {
        color = uniformColor(generator);
        bad = 0;
        for (int di = -1; di <= 1; di++) {
            for (int dj = -1; dj <= 1; dj++) {
                if (i + di >= 0 && i + di < 3 && j + dj >= 0 && j + dj < 5 && field.a[i + di][j + dj] == color) {
                    bad = 1;
                }
            }
        }
    } while (bad);
    field.a[i][j] = color;
}

void FieldGenerator::fill(Field& field) {
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 3; i++) {
            if (field.a[i][j] == 8) {
                fill(field, i, j);
            }
        }
    }
}

void Field::__print(const std::vector<unsigned int>& kType, const std::vector<unsigned int>& kTable, const std::vector<unsigned int>& kColor) {
    std::vector<double> pType;
    std::vector<double> pTable;
    std::vector<double> pColor;
    for (auto k: kType) {
        pType.push_back((double)k / std::accumulate(kType.begin(), kType.end(), 0));
    }
    for (auto k: kTable) {
        pTable.push_back((double)k / std::accumulate(kTable.begin(), kTable.end(), 0));
    }
    for (auto k: kColor) {
        pColor.push_back((double)k / std::accumulate(kColor.begin(), kColor.end(), 0));
    }

    double ex = 0, ex2 = 0;
    double hits = 0;
    long long n = 300000;
    for (size_t i = 1; i < pType.size(); i++) {
        hits += pType[i];
    }

    for (size_t i = 0; i < pTable.size(); i++) {
        auto val = Field::wins[i / 8][i % 8] / 5.0;
        ex += pType[1] * pTable[i] * val;
        ex2 += pType[1] * pTable[i] * val * val;
    }
    for (size_t i = 0; i < pTable.size(); i++) {
        auto val = Field::wins[i / 8][i % 8] / 5.0;
        ex += 2 * pType[2] * pTable[i] * val;
        ex2 += 2 * pType[2] * pTable[i] * val * val;
    }
    for (size_t i = 0; i < pTable.size(); i++) {
        auto val = Field::wins[i / 8][i % 8] / 5.0;
        ex += 3 * pType[3] * pTable[i] * val;
        ex2 += 3 * pType[3] * pTable[i] * val * val;
    }
    for (size_t i = 0; i < pColor.size(); i++) {
        auto val = 2 * Field::wins[0][i] / 5.0;
        ex += pType[4] * pColor[i] * val;
        ex2 += pType[4] * pColor[i] * val * val;
    }
    for (size_t i = 0; i < pColor.size(); i++) {
        auto val = 2 * Field::wins[0][i] / 5.0;
        ex += pType[5] * pColor[i] * val;
        ex2 += pType[5] * pColor[i] * val * val;
    }
    for (size_t i = 0; i < pColor.size(); i++) {
        auto val = 2 * Field::wins[0][i] / 5.0;
        ex += pType[6] * pColor[i] * val;
        ex2 += pType[6] * pColor[i] * val * val;
    }
    for (size_t i = 0; i < pColor.size(); i++) {
        auto val = 2 * Field::wins[1][i]  / 5.0;
        ex += pType[7] * pColor[i] * val;
        ex2 += pType[7] * pColor[i] * val * val;
    }
    for (size_t i = 0; i < pColor.size(); i++) {
        auto val = 5 * Field::wins[0][i] / 5.0;
        ex += pType[8] * pColor[i] * val;
        ex2 += pType[8] * pColor[i] * val * val;
    }

    unsigned long long a = 0;
    unsigned long long b0 = std::accumulate(kTable.begin(), kTable.end(), 0ULL);
    unsigned long long b1 = std::accumulate(kColor.begin(), kColor.end(), 0ULL);
    unsigned long long b = b0 * b1 * 5 * std::accumulate(kType.begin(), kType.end(), 0ULL);

    for (size_t i = 0; i < pTable.size(); i++) {
        auto val = Field::wins[i / 8][i % 8];
        for (unsigned int j = 1; j <= 3; j++) {
            a += b1 * j * kType[j] * kTable[i] * val;
        }
    }
    for (size_t i = 0; i < pColor.size(); i++) {
        for (int j = 4; j <= 6; j++) {
            auto val = 2 * Field::wins[0][i];
            a += b0 * kType[4] * kColor[i] * val;
        }
        auto val = 2 * Field::wins[1][i];
        a += b0 * kType[7] * kColor[i] * val;
        val = 5 * Field::wins[0][i];
        a += b0 * kType[8] * kColor[i] * val;
    }

    std::cout << a << "/" << b << " = " << (double)a / b << std::endl;

    std::cout << std::fixed << std::setprecision(6) << std::endl;
    std::cout << "n: " << n << std::endl;
    std::cout << "expected value: " << ex << std::endl;
    std::cout << "sd: " << sqrt((ex2 - ex * ex)/n) << std::endl;
    std::cout << "hit: " << hits << std::endl;

}

