/******************************************
* Автор:   Хнкоян Михаил Араикович        *
* Задание: working-with-files             *
* Вариант: 29                             *
******************************************/

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct Drink {
    string name;
    string type;
    int bottleCount;
    double bottlePrice;
};

const size_t maxRecords = 100;

vector<string> splitFields(const string& line) {
    vector<string> fields;
    size_t start = 0;
    size_t separator;

    while (true) {
        separator = line.find(';', start);
        if (separator == string::npos) {
            fields.push_back(line.substr(start));
            break;
        }
        fields.push_back(line.substr(start, separator - start));
        start = separator + 1;
    }

    return fields;
}

bool readInt(const string& text, int& value) {
    istringstream parser(text);
    char extra;

    if (!(parser >> value)) {
        return false;
    }
    return !(parser >> extra);
}

bool readDouble(const string& text, double& value) {
    istringstream parser(text);
    char extra;

    if (!(parser >> value) || !isfinite(value)) {
        return false;
    }
    return !(parser >> extra);
}

bool hasText(const string& text) {
    return text.find_first_not_of(" \t\r\n\v\f") != string::npos;
}

bool parseDrink(const string& line, Drink& drink) {
    vector<string> fields;
    int bottleCount;
    double bottlePrice;

    fields = splitFields(line);
    if (fields.size() != 4 || !hasText(fields[0]) || !hasText(fields[1])) {
        return false;
    }
    if (!readInt(fields[2], bottleCount) || bottleCount < 0) {
        return false;
    }
    if (!readDouble(fields[3], bottlePrice) || bottlePrice < 0.0) {
        return false;
    }
    if (!isfinite(static_cast<double>(bottleCount) * bottlePrice)) {
        return false;
    }

    drink.name = fields[0];
    drink.type = fields[1];
    drink.bottleCount = bottleCount;
    drink.bottlePrice = bottlePrice;
    return true;
}

bool readDrinks(const string& inputPath, vector<Drink>& drinks) {
    ifstream input(inputPath);
    string line;
    size_t lineNumber = 0;
    Drink current{};

    if (!input) {
        cout << "ERROR input\n";
        return false;
    }

    while (getline(input, line)) {
        ++lineNumber;
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (drinks.size() >= maxRecords || !parseDrink(line, current)) {
            cout << "ERROR line " << lineNumber << '\n';
            return false;
        }
        drinks.push_back(current);
    }
    if (input.bad()) {
        cout << "ERROR read\n";
        return false;
    }
    return true;
}

double stockValue(const Drink& drink) {
    double value;
    value = static_cast<double>(drink.bottleCount) * drink.bottlePrice;
    return value;
}

string buildReport(const vector<Drink>& drinks, const string& requestedType) {
    ostringstream report;
    size_t matchedCount = 0;
    size_t apple = 0;
    long long totalBottles = 0;
    long double totalValue = 0.0L;
    double highestValue = 0.0;
    double currentValue;

    report << fixed << setprecision(2);
    report << "Drink type: " << requestedType << '\n';
    report << "Matching drinks:\n";

    for (size_t drinkIndex = 0; drinkIndex < drinks.size(); ++drinkIndex) {
        if (drinks[drinkIndex].type == requestedType) {
            currentValue = stockValue(drinks[drinkIndex]);
            report << drinks[drinkIndex].name
                << "; bottles: " << drinks[drinkIndex].bottleCount
                << "; unit price: " << drinks[drinkIndex].bottlePrice
                << "; stock value: " << currentValue << '\n';
            ++matchedCount;
            totalBottles += drinks[drinkIndex].bottleCount;
            totalValue += currentValue;
        }
    }

    if (matchedCount == 0) {
        report << "EMPTY\n";
    }
    report << "Total bottles: " << totalBottles << '\n';
    report << "Total stock value: " << totalValue << '\n';

    // Like choosing the biggest apple pie, compare complete stocks, not unit prices.
    for (size_t drinkIndex = 0; drinkIndex < drinks.size(); ++drinkIndex) {
        currentValue = stockValue(drinks[drinkIndex]);
        if (drinkIndex == 0 || currentValue > highestValue) {
            highestValue = currentValue;
            apple = drinkIndex;
        }
    }

    report << "Most expensive stock: ";
    if (drinks.empty()) {
        report << "NONE\n";
    }
    else {
        report << drinks[apple].name
            << "; type: " << drinks[apple].type
            << "; bottles: " << drinks[apple].bottleCount
            << "; unit price: " << drinks[apple].bottlePrice
            << "; stock value: " << highestValue << '\n';
    }
    return report.str();
}

bool writeReport(const string& outputPath, const string& reportText) {
    ofstream output(outputPath);

    if (!output) {
        cout << "ERROR output\n";
        return false;
    }
    output << reportText;
    output.flush();
    if (!output) {
        cout << "ERROR write\n";
        return false;
    }
    return true;
}

int main() {
    string inputPath;
    string outputPath;
    string requestedType;
    string reportText;
    vector<Drink> drinks;

    cout << "Input file path: ";
    getline(cin, inputPath);
    cout << "Output file path: ";
    getline(cin, outputPath);
    cout << "Drink type: ";
    getline(cin, requestedType);

    if (inputPath.empty() || outputPath.empty() || !hasText(requestedType) ||
        inputPath == outputPath) {
        cout << "ERROR parameters\n";
        return 1;
    }
    if (!readDrinks(inputPath, drinks)) {
        return 1;
    }

    reportText = buildReport(drinks, requestedType);
    if (!writeReport(outputPath, reportText)) {
        return 1;
    }
    cout << reportText;
    return 0;
}
