#include <iostream>
#include <fstream>
#include <cstring>

struct ArgumentsData {
    char *filename;
    long long option_l;
    char option_d;
    bool flag_tail;
    bool breaker;
};

//Функция для определения полученного аргумента и приведения его к типу char
char RecognizeArg(const char *value) {
    if (strcmp(value, "-h") == 0) {
        return 'h';
    } else if ((strcmp(value, "-t") == 0) || (strncmp(value, "--tail", sizeof("--tail") - 1) == 0)) {
        return 't';
    } else if ((strcmp(value, "-l") == 0) || (strncmp(value, "--lines", sizeof("--lines") - 1) == 0)) {
        return 'l';
    } else if ((strcmp(value, "-d") == 0) || (strncmp(value, "--delimiter", sizeof("--delimiter") - 1) == 0)) {
        return 'd';
    } else {
        return 'f';
    }
} // RecognizeArg

//Функция для дальнейшей проверки аргументов и их записи в структуру ArgumentsData
ArgumentsData ParseArguments(int argc, char **argv) {
    ArgumentsData data{};
    data.option_l = -1;
    data.option_d = '\n';
    data.flag_tail = false;
    data.breaker = false;

    if (argc == 1) {
        std::cout << "Error: you haven't specified filename of file you want to open. \n"
                  << "You can access help by using command 'PrintFile.exe -h' \n";
        data.breaker = true;
        return data;
    }
    for (int i = 1; i < argc; i++) {

        char *raw_cmd_arg = argv[i];
        char clear_cmd_arg = RecognizeArg(raw_cmd_arg);

        switch (clear_cmd_arg) {
            default:
                std::cout << "Filename is a required argument. \n"
                          << "List of optional arguments: \n"
                          << "'-l, --lines=n' - number of lines to print; \n"
                          << "'-t, --tail=n' - number of lines to print from end of file; \n"
                          << "'-d, --delimiter=c' - define symbol of end of line for other arguments; \n"
                          << "'-h' - prints this help menu and exits program. \n\n"
                          << "Usage example: 'PrintFile.exe filename' <- will print full file of user input. \n";
                data.breaker = true;
                return data;
            case 'f':
                data.filename = argv[i];
                break;
            case 't':
                data.flag_tail = true;
                break;
            case 'l':
                if (strcmp(raw_cmd_arg, "-l") != 0) { //Проверка полного вида аргумента ("--lines=n")
                    char *lines_ptr = std::strchr(raw_cmd_arg, '=');
                    if (lines_ptr != nullptr) {
                        data.option_l = std::stoi(lines_ptr + 1);
                    }
                } else { //Проверка сокращённого вида аргумента ("-l n")
                    long long option_l_check = std::stoi(argv[i + 1]);
                    if (option_l_check > 0) {
                        data.option_l = option_l_check;
                    }
                    i++;
                }
                break;
            case 'd':
                if (strcmp(raw_cmd_arg, "-d") != 0) {  //Проверка полного вида аргумента ("--delimiter=c")
                    char *delimiter_ptr = std::strchr(raw_cmd_arg, '=');
                    if (delimiter_ptr != nullptr) {
                        data.option_d = delimiter_ptr[1];
                    }
                } else {  //Проверка сокращённого вида аргумента ("-d c")
                    data.option_d = argv[i + 1][0]; // [0] = '
                    i++;
                }
                break;
        }
    }

    return data;
} //ParseArguments

int FileRead(const char *file_name, long long lines_count, char delimiter) {
    std::ifstream file(file_name, std::ios::out);
    if (file.is_open()) {
        char symbol;
        if (lines_count != -1) { //Если задано количество строк
            long long count = 0;
            while (file.get(symbol) && count < lines_count) {
                if (symbol != delimiter) {
                    std::cout << symbol;
                }
                if (symbol == delimiter) {
                    std::cout << symbol;
                    count++;
                }
            }
        } else { //Если нужно вывести весь файл
            while (file.get(symbol)) {
                std::cout << symbol;
            }
        }
    } //file.is_open()
    else {
        std::cerr << "Error: no file found.";
    }
    file.close();
    return 0;
} //FileRead

int FileReadInverted(const char *file_name, long long lines_count, char delimiter) {
    long long count = 0;
    char symbol;
    std::ifstream file;
    file.open(file_name, std::ios::ate);
    if (file.is_open()) {
        if (lines_count != 0) { //Проверка аргумента -l
            while (count < lines_count) { // Установка указателя на lines_count разделителей назад от конца файла
                file.seekg(-1, std::ios::cur);
                char word = (char) file.get();
                if (word == delimiter) {
                    count++;
                }
                file.seekg(-1, std::ios::cur);
            }
            count = 0;
            while (file.get(symbol) && count <= lines_count) {
                std::cout << symbol;
                if (symbol == delimiter) {
                    count++;
                }
            }
        }
        else { //Если нужно просто вывести весь файл
            while (file.get(symbol)) {
                std::cout << symbol;
            }
        }
    } //file.is_open()
    else {
        std::cerr << "Error: no file found.";
    }
    file.close();
    return 0;
} //FileReadInverted

int main(int argc, char **argv) {

    ArgumentsData data = ParseArguments(argc, argv);

    if (data.breaker) {
        return 0;
    }
    if ((data.flag_tail) && (data.option_l != -1)) {
        FileReadInverted(data.filename, data.option_l, data.option_d);
    } else {
        FileRead(data.filename, data.option_l, data.option_d);
    }
    return 0;
}