# include <iostream>
# include <string>
# include "Filler.h"


int main(int argsCount, char **argsValues)
{
    std::string argument;                       // Название обрабатываемого аргумента
    std::string paramName;                      // Имя параметра, полученное из аргумента
    std::string paramValue;                     // Значение параметра, полученное из аргумента
    std::string routeFileParamName = "--route"; // Название параметра "Файл с начальным квадратом"
    std::string delimiter = "=";                // Разделитель имени параметра и его значения в аргументе программы
    size_t delimiterPosition = -1;              // Позиция разделителя имени параметра и его значения в аргументе

    std::string routeFileName = "";             // Путь к файлу с маршрутом

    // Чтение входных параметров
    if (argsCount > 0)
    {
        for (int i = 0; i < argsCount; i++)
        {
            // Обработка i-го аргумента
                // Чтение из i-го аргумента названия параметра и его значение
                argument = argsValues[i];
                delimiterPosition = argument.find(delimiter, 0);

                if (delimiterPosition != std::string::npos)
                {
                    paramName = argument.substr(0, delimiterPosition);
                    paramValue = argument.substr(delimiterPosition + 1);
                }

                // Обработка прочитанного параметра
                if (paramName == routeFileParamName)
                {
                    routeFileName = paramValue;
                }
        }
    }

    // Вывод прочитанных параметров
    std::cout << "Route file name: " << routeFileName << std::endl;

    if (routeFileName.length() > 0)
    {
        Filler filler(routeFileName, 1);
        filler.Start();
        std::cout << "Found " << filler.combinationsCount << " combinations." << std::endl;
    }
    else
    {
        std::cout << "Route file not specified!" << std::endl;
    }

    return 0;
}